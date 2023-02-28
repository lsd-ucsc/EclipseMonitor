// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <functional>
#include <memory>
#include <unordered_map>

#include "../EclipseMonitorBase.hpp"

#include "CheckpointMgr.hpp"
#include "DiffChecker.hpp"
#include "HeaderMgr.hpp"
#include "Validator.hpp"

namespace EclipseMonitor
{
namespace Eth
{


class EclipseMonitor : public EclipseMonitorBase
{
public: // Static members

	using Self = EclipseMonitor;
	using Base = EclipseMonitorBase;

	using OnHeaderConfCallback = std::function<void(const HeaderMgr&)>;
	using NodeLookUpMap =
		std::unordered_map<Internal::Obj::Bytes, HeaderNode*>;

public:

	EclipseMonitor(
		const MonitorConfig& conf,
		const std::string& chainName,
		TimestamperType timestamper,
		OnHeaderConfCallback onHeaderConfirmed,
		std::unique_ptr<ValidatorBase> validator,
		std::unique_ptr<DiffCheckerBase> diffChecker
	) :
		EclipseMonitorBase(conf, chainName, std::move(timestamper)),
		m_onHeaderConfirmed(onHeaderConfirmed),
		m_checkpoint(conf, [this](){
			this->OnCheckpointComplete();
		}),
		m_validator(std::move(validator)),
		m_diffChecker(std::move(diffChecker))
	{}

	virtual ~EclipseMonitor()
	{}

	virtual void Update(const std::vector<uint8_t>& hdrBinary) override
	{
		// 1. check current phase
		if (Base::GetPhase() == Phases::BootstrapI)
		{
			UpdateOnBootstrapI(hdrBinary);
		}
		// all other phase will be treated like the runtime phase
		else
		{
			UpdateOnRuntime(hdrBinary);
		}

		if (Base::GetPhase() != Phases::BootstrapI)
		{
			RuntimeMaintenance();
		}
	}

	virtual void EndBootstrapI() override
	{
		// 1. notify checkpoint manager so it will create the dummy node
		//    for the last header
		m_checkpoint.EndBootstrapPhase();

		// 2. update active nodes so we will use it as the starting
		//    point to add the following children
		auto lastNodePtr = m_checkpoint.GetLastNodePtr();
		const auto& lastHeader = lastNodePtr->GetHeader();
		m_offlineNodes[lastHeader.GetHashObj()] = lastNodePtr;

		// 3. notify the base class that we're entering the next phase
		Base::EndBootstrapI();
	}

protected:

	void UpdateOnBootstrapI(const std::vector<uint8_t>& hdrBinary)
	{
		// We're loading blocks before the latest checkpoint

		std::unique_ptr<HeaderMgr> header =
			Internal::Obj::Internal::make_unique<HeaderMgr>(
				hdrBinary, 0);

		// 1 check if this is the genesis (very first) block
		if (m_checkpoint.IsEmpty())
		{
			// a. it is the genesis block
			// 1.a. update the monitor security state
			Base::GetMonitorSecState().get_genesisHash() = header->GetHashObj();
		}
		else
		{
			// b. it is not the genesis block
			// 1.b validate the block
			if (!m_validator->CommonValidate(
					m_checkpoint.GetLastHeader(),
					false,
					*header,
					false
				)
			)
			{
				throw Exception(
					"The given block failed common validation");
			}
		}

		// Add the header to the checkpoint
		m_checkpoint.AddHeader(std::move(header));
		// !!! NOTE: header is invalid after this point !!!
	}

	void UpdateOnRuntime(const std::vector<uint8_t>& hdrBinary)
	{
		std::unique_ptr<HeaderMgr> header =
			Internal::Obj::Internal::make_unique<HeaderMgr>(
				hdrBinary,
				Base::GetTimestamper().NowInSec()
			);

		// Check offline nodes map first
		if (m_offlineNodes.size() > 0)
		{
			auto offNoIt = m_offlineNodes.find(
				header->GetRawHeader().get_ParentHash()
			);
			if (offNoIt != m_offlineNodes.end())
			{
				// we found the parent node
				HeaderNode* parentNode = offNoIt->second;
				UpdateOnRuntimeAddChild(
					parentNode,
					false,
					std::move(header)
				);
				// !!! NOTE: header is invalid after this point !!!
			}
		}

		// if we didn't find the parent node in the offline nodes map,
		// check the active nodes map then
		if (header != nullptr)
		{
			auto actNoIt = m_activeNodes.find(
				header->GetRawHeader().get_ParentHash()
			);
			if (actNoIt != m_activeNodes.end())
			{
				// we found the parent node
				HeaderNode* parentNode = actNoIt->second;
				UpdateOnRuntimeAddChild(
					parentNode,
					true,
					std::move(header)
				);
				// !!! NOTE: header is invalid after this point !!!
			}
		}
	}

	void RuntimeMaintenance()
	{
		// 1. check for new checkpoint candidates
		HeaderNode* lastChptNode = m_checkpoint.GetLastNodePtr();
		auto confirmedChild = lastChptNode->ReleaseChildHasNDesc(
			static_cast<size_t>(
				Base::GetMonitorConfig().get_checkpointSize().GetVal()
			)
		);
		if (confirmedChild != nullptr)
		{
			// we found a new checkpoint candidate

			// both last node and confirmed child are not active anymore
			if (m_offlineNodes.size() > 0)
			{
				m_offlineNodes.erase(lastChptNode->GetHeader().GetHashObj());
				m_offlineNodes.erase(confirmedChild->GetHeader().GetHashObj());
			}
			m_activeNodes.erase(lastChptNode->GetHeader().GetHashObj());
			m_activeNodes.erase(confirmedChild->GetHeader().GetHashObj());

			// add to checkpoint
			m_checkpoint.AddNode(std::move(confirmedChild));
		}

		// 2. check for expired active nodes
		auto now = Base::GetTimestamper().NowInSec();
		for (auto nodePtr : m_activeNodes)
		{
			if (
				!m_diffChecker->CheckEstDifficulty(
					nodePtr.second->GetHeader(),
					now
				)
			)
			{
				// the node is expired
				m_activeNodes.erase(nodePtr.first);
			}
		}
	}

private:

	void UpdateOnRuntimeAddChild(
		HeaderNode* parentNode,
		bool isParentNodeLive,
		std::unique_ptr<HeaderMgr> header
	)
	{
		// common validation
		bool isNewNodeLive = false; // TODO
		bool validateRes = m_validator->CommonValidate(
			parentNode->GetHeader(),
			isParentNodeLive,
			*header,
			isNewNodeLive
		);

		// check difficulty
		bool diffRes = false;
		if (validateRes)
		{
			diffRes = m_diffChecker->CheckDifficulty(
				parentNode->GetHeader(),
				*header
			);
		}

		// if both check passed, add it to the parent node
		if (validateRes && diffRes)
		{
			auto hashObj = header->GetHashObj();

			// add the header to the parent node
			HeaderNode* node =
				parentNode->AddChild(std::move(header));
			// !!! NOTE: header is invalid after this point !!!

			// add this node also to the active nodes
			if (isNewNodeLive)
			{
				m_activeNodes.insert(std::make_pair(hashObj, node));
			}
			else
			{
				m_offlineNodes.insert(std::make_pair(hashObj, node));
			}
		}
	}

	void OnCheckpointComplete()
	{
		// 1. update the monitor security state about latest checkpoint
		const auto& lastHeader = m_checkpoint.GetLastHeader();
		Base::GetMonitorSecState().get_checkpointHash() =
			lastHeader.GetHashObj();

		// 2. update the difficulty checker
		m_diffChecker->OnChkptUpd(m_checkpoint);

		// on confirmed header callback
		m_checkpoint.IterateCurrWindow(
			[this](const HeaderMgr& header)
			{
				m_onHeaderConfirmed(header);
			}
		);
	}

private:

	OnHeaderConfCallback m_onHeaderConfirmed;

	CheckpointMgr m_checkpoint;
	std::unique_ptr<ValidatorBase> m_validator;
	std::unique_ptr<DiffCheckerBase> m_diffChecker;

	// runtime & forks
	// TODO sync manager
	NodeLookUpMap m_offlineNodes;
	NodeLookUpMap m_activeNodes;


}; // class EclipseMonitor


} // namespace Eth
} // namespace EclipseMonitor
