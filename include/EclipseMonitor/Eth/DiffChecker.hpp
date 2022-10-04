// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <memory>

#include "../MonitorReport.hpp"

#include "CheckpointMgr.hpp"
#include "DAA.hpp"
#include "DataTypes.hpp"
#include "HeaderMgr.hpp"
#include "Params.hpp"

namespace EclipseMonitor
{
namespace Eth
{


class DiffCheckerBase
{
public:
	DiffCheckerBase() = default;

	// LCOV_EXCL_START
	virtual ~DiffCheckerBase() = default;
	// LCOV_EXCL_STOP

	virtual void UpdateDiffMin(const CheckpointMgr& chkpt) = 0;

	/**
	 * @brief Check the difficulty value (or anything equivalent) of the
	 *        current header
	 *
	 * @param parentHdr
	 * @param currentHdr
	 * @return true if the difficulty value is OK, otherwise false
	 */
	virtual bool CheckDifficulty(
		const HeaderMgr& parentHdr,
		const HeaderMgr& currentHdr) const = 0;

	/**
	 * @brief Check the estimated difficulty value (or anything equivalent)
	 *        based on the current time
	 *
	 * @param parentHdr
	 * @param currentHdr
	 * @return true if the difficulty value is OK, otherwise false
	 */
	virtual bool CheckEstDifficulty(
		const HeaderMgr& parentHdr,
		uint64_t currentTime) const = 0;

}; // class DiffCheckerBase


class PoWDiffChecker : public DiffCheckerBase
{
public: // static members:
	using Self = PoWDiffChecker;
	using Base = DiffCheckerBase;

	using DiffType = typename DiffTypeTrait::value_type;

public:
	PoWDiffChecker(
		const MonitorConfig& mConf,
		std::unique_ptr<DAABase> diffEstimator
	) :
		DiffCheckerBase(),
		m_minDiffPercent(mConf.get_minDiffPercent().GetVal()),
		m_maxWaitTime(mConf.get_maxWaitTime().GetVal()),
		m_diffEstimator(std::move(diffEstimator))
	{}

	// LCOV_EXCL_START
	virtual ~PoWDiffChecker() = default;
	// LCOV_EXCL_STOP

	virtual void UpdateDiffMin(const CheckpointMgr& chkpt) override
	{
		m_minDiff = (chkpt.GetDiffMedian() >> 7) * m_minDiffPercent;
	}

	virtual bool CheckDifficulty(
		const HeaderMgr& parentHdr,
		const HeaderMgr& currentHdr) const override
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
		const HeaderMgr& parentHdr,
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

	std::unique_ptr<DAABase> m_diffEstimator;

}; // class PoWDiffChecker


template<typename _NetConfig>
class GenericDiffCheckerImpl : public DiffCheckerBase
{
public: // static members:
	using Self = GenericDiffCheckerImpl<_NetConfig>;
	using Base = DiffCheckerBase;

	using DiffType = typename DiffTypeTrait::value_type;

public:
	GenericDiffCheckerImpl(
		const MonitorConfig& mConf,
		std::unique_ptr<DAABase> diffEstimator
	) :
		Base(),
		m_powChecker(mConf, std::move(diffEstimator))
	{}

	// LCOV_EXCL_START
	virtual ~GenericDiffCheckerImpl() = default;
	// LCOV_EXCL_STOP

	virtual void UpdateDiffMin(const CheckpointMgr& chkpt) override
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
		const HeaderMgr& parentHdr,
		const HeaderMgr& currentHdr) const override
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
		const HeaderMgr& parentHdr,
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

	PoWDiffChecker m_powChecker;
}; // class GenericDiffCheckerImpl


using DiffCheckerMainNet =
	GenericDiffCheckerImpl<MainnetConfig>;


} // namespace Eth
} // namespace EclipseMonitor
