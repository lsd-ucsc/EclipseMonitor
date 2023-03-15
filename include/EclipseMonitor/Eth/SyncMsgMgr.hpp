// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <mutex>

#include "../SyncMsgMgrBase.hpp"

#include "DataTypes.hpp"
#include "EventManager.hpp"
#include "HeaderMgr.hpp"

namespace EclipseMonitor
{
namespace Eth
{


class SyncMsgMgr : public SyncMsgMgrBase
{
public: // static member

	using Self = SyncMsgMgr;
	using Base = SyncMsgMgrBase;

public:

	/**
	 * @brief Construct a new sync message manager object
	 *
	 * @param mId    reference to the monitor ID
	 * @param mConf  Configuration of the monitor
	 * @param timestamper reference to the timestamper
	 * @param randGen     reference to the random generator
	 * @param syncContractAddr Ethereum contract address for the sync message
	 */
	SyncMsgMgr(
		const MonitorId& mId,
		const MonitorConfig& mConf,
		const TimestamperBase& timestamper,
		const RandomGeneratorBase& randGen,
		const ContractAddr& syncContractAddr,
		const EventTopic& eventSign,
		std::shared_ptr<EventManager> eventMgr
	) :
		Base(mId, mConf, timestamper, randGen),
		m_syncStateMutex(),
		m_syncContractAddr(syncContractAddr),
		m_eventSign(eventSign),
		m_eventMgr(eventMgr),
		m_syncEventCallbackId(RegisterSyncEvent_Locked(Base::GetLastSyncState()))
	{}


	// LCOV_EXCL_START
	virtual ~SyncMsgMgr() = default;
	// LCOV_EXCL_STOP


	virtual std::shared_ptr<SyncState> NewSyncState(
		const TimestamperBase& timestamper,
		const RandomGeneratorBase& randGen
	) override
	{
		std::lock_guard<std::mutex> lock(m_syncStateMutex);

		auto eventMgr = m_eventMgr.lock();
		if (eventMgr)
		{
			eventMgr->Cancel(m_syncEventCallbackId);
		}

		auto state = Base::BuildSyncState(timestamper, randGen);
		m_syncEventCallbackId = RegisterSyncEvent_Locked(state);

		Base::AtomicSetSyncState(state);

		return state;
	}


protected: // helper functions:


	EventCallbackId RegisterSyncEvent_Locked(
		std::shared_ptr<SyncState> syncState
	)
	{
		static_assert(
			std::tuple_size<EventTopic>::value >=
				std::tuple_size<SessionID>::value,
			"SessionID must be able to fit in EventTopic"
		);

		const auto& baseSessID = Base::GetSessionID();
		EventTopic sessionID;
		sessionID.fill(0);
		std::copy(
			baseSessID.begin(),
			baseSessID.end(),
			sessionID.begin()
		);

		std::weak_ptr<EventManager> weakEventMgr = m_eventMgr;

		EventDescription eventDesc(
			m_syncContractAddr,
			std::vector<EventTopic>({
				m_eventSign, sessionID, syncState->GetNonce()
			}),
			[syncState, weakEventMgr](
				const HeaderMgr& headerMgr,
				const ReceiptLogEntry&,
				EventCallbackId cbID
			) -> void
			{
				if (!syncState->IsSynced())
				{
					syncState->SetSynced(headerMgr.GetTrustedTime());
				}

				auto eventMgr = weakEventMgr.lock();
				if (eventMgr)
				{
					eventMgr->Cancel(cbID);
				}
			}
		);

		auto eventMgr = m_eventMgr.lock();
		if (eventMgr)
		{
			return eventMgr->Listen(std::move(eventDesc));
		}
		throw Exception("Event manager is not available");
	}


private:
	std::mutex m_syncStateMutex;
	ContractAddr m_syncContractAddr;
	EventTopic m_eventSign;
	std::weak_ptr<EventManager> m_eventMgr;
	EventCallbackId m_syncEventCallbackId;
}; // class SyncMsgMgr


} // namespace Eth
} // namespace EclipseMonitor
