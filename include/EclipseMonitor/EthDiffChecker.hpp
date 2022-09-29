// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <memory>

#include "EthHeaderMgr.hpp"
#include "EthCheckpointMgr.hpp"
#include "EthDataTypes.hpp"
#include "EthDAA.hpp"
#include "EthParams.hpp"
#include "MonitorReport.hpp"

namespace EclipseMonitor
{

class EthDiffCheckerBase
{
public:
	EthDiffCheckerBase() = default;

	virtual ~EthDiffCheckerBase() = default;

	virtual void UpdateDiffMin(const EthCheckpointMgr& chkpt) = 0;

	/**
	 * @brief Check the difficulty value (or anything equivalent) of the
	 *        current header
	 *
	 * @param parentHdr
	 * @param currentHdr
	 * @return true if the difficulty value is OK, otherwise false
	 */
	virtual bool CheckDifficulty(
		const EthHeaderMgr& parentHdr,
		const EthHeaderMgr& currentHdr) const = 0;

	/**
	 * @brief Check the estimated difficulty value (or anything equivalent)
	 *        based on the current time
	 *
	 * @param parentHdr
	 * @param currentHdr
	 * @return true if the difficulty value is OK, otherwise false
	 */
	virtual bool CheckEstDifficulty(
		const EthHeaderMgr& parentHdr,
		uint64_t currentTime) const = 0;

}; // class EthDiffCheckerBase


class EthPoWDiffChecker : public EthDiffCheckerBase
{
public: // static members:
	using Self = EthPoWDiffChecker;
	using Base = EthDiffCheckerBase;

	using DiffType = typename EthDiffTypeTrait::value_type;

public:
	EthPoWDiffChecker(
		const MonitorConfig& mConf,
		std::unique_ptr<EthDAABase> diffEstimator
	) :
		EthDiffCheckerBase(),
		m_minDiffPercent(mConf.get_minDiffPercent().GetVal()),
		m_maxWaitTime(mConf.get_maxWaitTime().GetVal()),
		m_diffEstimator(std::move(diffEstimator))
	{}

	virtual ~EthPoWDiffChecker() = default;

	virtual void UpdateDiffMin(const EthCheckpointMgr& chkpt) override
	{
		m_minDiff = (chkpt.GetDiffMedian() >> 7) * m_minDiffPercent;
	}

	virtual bool CheckDifficulty(
		const EthHeaderMgr& parentHdr,
		const EthHeaderMgr& currentHdr) const override
	{
		return
			// current header is received after the parent header
			(currentHdr.GetTrustedTime() > parentHdr.GetTrustedTime()) &&
			// current header is received within the max wait time
			((currentHdr.GetTrustedTime() - parentHdr.GetTrustedTime()) <=
				m_maxWaitTime) &&
			// current header's difficulty is greater than the min difficulty
			(currentHdr.GetDiff() >= m_minDiff);
	}

	virtual bool CheckEstDifficulty(
		const EthHeaderMgr& parentHdr,
		uint64_t currentTime) const override
	{
		auto deltaTime = currentTime - parentHdr.GetTrustedTime();
		auto estDiff = (*m_diffEstimator)(
			parentHdr.GetNumber(),
			parentHdr.GetTime(),
			parentHdr.GetDiff(),
			parentHdr.HasUncle(),
			parentHdr.GetNumber() + 1,
			currentTime
		);
		return
			// current header is received within the max wait time
			(deltaTime <= m_maxWaitTime) &&
			// estimated difficulty is greater than the min difficulty
			(estDiff >= m_minDiff);
	}


private:
	uint8_t m_minDiffPercent;
	DiffType m_minDiff;
	uint64_t m_maxWaitTime;

	std::unique_ptr<EthDAABase> m_diffEstimator;

}; // class EthPoWDiffChecker


template<typename _NetConfig>
class EthGenericDiffCheckerImpl : public EthDiffCheckerBase
{
public: // static members:
	using Self = EthGenericDiffCheckerImpl<_NetConfig>;
	using Base = EthDiffCheckerBase;

	using DiffType = typename EthDiffTypeTrait::value_type;

public:
	EthGenericDiffCheckerImpl(
		const MonitorConfig& mConf,
		std::unique_ptr<EthDAABase> diffEstimator
	) :
		Base(),
		m_powChecker(mConf, std::move(diffEstimator))
	{}

	virtual ~EthGenericDiffCheckerImpl() = default;


	virtual void UpdateDiffMin(const EthCheckpointMgr& chkpt) override
	{
		auto blkNumRange = chkpt.GetCheckpointBlkNumRange();
		if (blkNumRange.second < _NetConfig::GetParisBlkNum())
		{
			return m_powChecker.UpdateDiffMin(chkpt);
		}
		else
		{
			throw Exception("Not implemented yet");
		}
	}

	virtual bool CheckDifficulty(
		const EthHeaderMgr& parentHdr,
		const EthHeaderMgr& currentHdr) const override
	{
		if (currentHdr.GetNumber() < _NetConfig::GetParisBlkNum())
		{
			return m_powChecker.CheckDifficulty(parentHdr, currentHdr);
		}
		else
		{
			throw Exception("Not implemented yet");
		}
	}

	virtual bool CheckEstDifficulty(
		const EthHeaderMgr& parentHdr,
		uint64_t currentTime) const override
	{
		if (parentHdr.GetNumber() + 1 < _NetConfig::GetParisBlkNum())
		{
			return m_powChecker.CheckEstDifficulty(parentHdr, currentTime);
		}
		else
		{
			throw Exception("Not implemented yet");
		}
	}

private:

	EthPoWDiffChecker m_powChecker;
}; // class EthGenericDiffCheckerImpl


using EthDiffCheckerMainNet =
	EthGenericDiffCheckerImpl<EthMainnetConfig>;

} // namespace EclipseMonitor
