// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/Eth/BloomFilter.hpp>
#include <EclipseMonitor/Eth/DAA.hpp>
#include <EclipseMonitor/Eth/DiffChecker.hpp>

#include "EthHistHdr_0_100.hpp"

namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;

	class FixedDiffEstimator : public EclipseMonitor::Eth::DAABase
	{
	public:

		FixedDiffEstimator() = default;

		virtual ~FixedDiffEstimator() = default;

		virtual DiffType operator()(
			const EclipseMonitor::Eth::HeaderMgr&,
			const EclipseMonitor::Eth::HeaderMgr&
		) const override
		{
			return 0;
		}

	}; // class FixedDiffEstimator
}

using namespace EclipseMonitor_Test;

using namespace EclipseMonitor::Eth;

GTEST_TEST(TestEthDiffChecker, CountTestFile)
{
	static auto tmp = ++g_numOfTestFile;
	(void)tmp;
}


static std::vector<uint8_t> BuildHeader(
	typename BlkNumTypeTrait::value_type blkNum,
	typename DiffTypeTrait::value_type diffVal
)
{
	auto diffBytes = DiffTypeTrait::ToBytes(diffVal);
	auto blkNumBytes = DiffTypeTrait::ToBytes(blkNum);

	SimpleRlp::EthHeader rlpHdr;
	rlpHdr.get_Number() = blkNumBytes;
	rlpHdr.get_Difficulty() = diffBytes;
	rlpHdr.get_Sha3Uncles() = HeaderMgr::GetEmptyUncleHash();
	rlpHdr.get_LogsBloom().resize(BloomFilter::sk_bloomByteSize);

	return SimpleRlp::WriteRlp(rlpHdr);
}


GTEST_TEST(TestEthDiffChecker, TestHistBlocks)
{
	static constexpr size_t sk_checkpointSize = 10;
	static constexpr uint8_t sk_minDiffPercent = 103;
	static constexpr uint64_t sk_maxWaitTime = 400;

	static constexpr size_t sk_beginBlkNum = 50;
	static constexpr size_t sk_endBlkNum = 60;

	// Testing variables
	EclipseMonitor::MonitorConfig mConf;
	mConf.get_checkpointSize() = sk_checkpointSize;
	mConf.get_minDiffPercent() = sk_minDiffPercent;
	mConf.get_maxWaitTime() = sk_maxWaitTime;
	std::unique_ptr<DAABase> diffEstimator =
		SimpleObjects::Internal::make_unique<MainnetDAAEstimator>();
	std::unique_ptr<DAABase> fixedDiffEstimator =
		SimpleObjects::Internal::make_unique<FixedDiffEstimator>();
	DiffCheckerMainNet diffChecker(
		mConf,
		std::move(diffEstimator)
	);
	DiffCheckerMainNet diffCheckerFixedEst(
		mConf,
		std::move(fixedDiffEstimator)
	);
	typename DiffTypeTrait::value_type diffMedian = 0;
	std::unique_ptr<CheckpointMgr> chkptMgr;
	chkptMgr = SimpleObjects::Internal::make_unique<CheckpointMgr>(
		mConf,
		[&diffMedian, &diffChecker, &diffCheckerFixedEst, &chkptMgr](){
			diffChecker.OnChkptUpd(*chkptMgr);
			diffCheckerFixedEst.OnChkptUpd(*chkptMgr);
			diffMedian = chkptMgr->GetDiffMedian();
		}
	);
	for (size_t i = sk_beginBlkNum; i < sk_endBlkNum; ++i)
	{
		auto header = SimpleObjects::Internal::make_unique<HeaderMgr>(
			GetEthHistHdr_0_100()[i], 0);
		chkptMgr->AddHeader(std::move(header));
	}
	ASSERT_NE(diffMedian, 0);
	auto expDiffMin = (diffMedian >> 7) * sk_minDiffPercent;

	// ===== CheckDifficulty
	// OK header
	auto okHeaderBin = BuildHeader(15050001UL, expDiffMin + 1);
	auto okHeaderParent = HeaderMgr(okHeaderBin, 10);
	auto okHeader = HeaderMgr(okHeaderBin, 20);
	EXPECT_TRUE(
		diffChecker.CheckDifficulty(okHeaderParent, okHeader));

	// Wrong - trust time reversed
	EXPECT_FALSE(
		diffChecker.CheckDifficulty(okHeader, okHeaderParent));

	// Wrong - exceeds max wait time
	auto headerExcWait = HeaderMgr(okHeaderBin, 999999);
	EXPECT_FALSE(
		diffChecker.CheckDifficulty(okHeaderParent, headerExcWait));

	// Wrong - difficulty too low
	auto HeaderLowDiffBin = BuildHeader(15050002UL, expDiffMin - 1);
	auto headerLowDiff = HeaderMgr(HeaderLowDiffBin, 20);
	EXPECT_FALSE(
		diffChecker.CheckDifficulty(okHeaderParent, headerLowDiff));

	// ===== CheckEstDifficulty
	// OK header
	EXPECT_TRUE(
		diffChecker.CheckEstDifficulty(okHeaderParent, 15));

	// Wrong - exceeds max wait time
	EXPECT_FALSE(
		diffChecker.CheckEstDifficulty(okHeaderParent, 999999));

	// Wrong - difficulty too low
	EXPECT_FALSE(
		diffCheckerFixedEst.CheckEstDifficulty(okHeaderParent, 15));
}
