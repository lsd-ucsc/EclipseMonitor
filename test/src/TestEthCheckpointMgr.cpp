// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/Eth/CheckpointMgr.hpp>

#include "EthHistHdr_0_100.hpp"

namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor_Test;

using namespace EclipseMonitor;
using namespace EclipseMonitor::Eth;

GTEST_TEST(TestEthCheckpointMgr, CountTestFile)
{
	static auto tmp = ++EclipseMonitor_Test::g_numOfTestFile;
	(void)tmp;
}

GTEST_TEST(TestEthCheckpointMgr, Normal_BootstrapI_Add)
{
	// Testing configurations
	static constexpr size_t testingChkptSize = 10;
	static constexpr size_t testingNumChkpt = 5;

	// testing sync state
	std::shared_ptr<SyncState> devSyncState =
		std::make_shared<SyncState>(SyncState::GetDevSyncState());

	// Expected results
	std::vector<Difficulty> expDiffMedian;
	for (size_t i = 0; i < testingNumChkpt; ++i)
	{
		std::vector<Difficulty> diffs;
		for (size_t j = 0; j < testingChkptSize; ++j)
		{
			HeaderMgr header(
				GetEthHistHdr_0_100()[(i * testingChkptSize) + j], 0);
			diffs.push_back(header.GetDiff());
		}
		auto mit = diffs.begin() + (diffs.size() / 2);
		std::nth_element(diffs.begin(), mit, diffs.end());
		expDiffMedian.push_back(*mit);
	}
	HeaderMgr expLastHeader(
				GetEthHistHdr_0_100()[
					(testingNumChkpt * testingChkptSize) - 1],
				0);

	// Testing variables
	EclipseMonitor::MonitorConfig mConf;
	mConf.get_checkpointSize() = testingChkptSize;
	size_t currChkptIdx = 0;
	std::unique_ptr<CheckpointMgr> chkptMgr;
	chkptMgr = SimpleObjects::Internal::make_unique<CheckpointMgr>(
		mConf,
		[&chkptMgr, &currChkptIdx, expDiffMedian](){
			auto actDiff = chkptMgr->GetDiffMedian();
			EXPECT_LT(currChkptIdx, expDiffMedian.size());
			if (currChkptIdx < expDiffMedian.size())
			{
				EXPECT_EQ(actDiff, expDiffMedian[currChkptIdx]);
			}
			auto numRange = chkptMgr->GetCheckpointBlkNumRange();
			EXPECT_EQ(numRange.first, currChkptIdx * testingChkptSize);
			EXPECT_EQ(
				numRange.second,
				((currChkptIdx + 1) * testingChkptSize) - 1);
			++currChkptIdx;
		});

	// Test
	EXPECT_TRUE(chkptMgr->IsEmpty());
	EXPECT_THROW(chkptMgr->GetLastHeader(), EclipseMonitor::Exception);
	EXPECT_THROW(chkptMgr->GetLastNode(), EclipseMonitor::Exception);
	for (size_t i = 0; i < testingNumChkpt; ++i)
	{
		for (size_t j = 0; j < testingChkptSize; ++j)
		{
			auto header = SimpleObjects::Internal::make_unique<HeaderMgr>(
				GetEthHistHdr_0_100()[(i * testingChkptSize) + j], 0);
			chkptMgr->AddHeader(std::move(header));
		}
	}

	EXPECT_EQ(currChkptIdx, testingNumChkpt);
	EXPECT_FALSE(chkptMgr->IsEmpty());
	EXPECT_NO_THROW(
		EXPECT_EQ(
			chkptMgr->GetLastHeader().GetRawHeader(),
			expLastHeader.GetRawHeader()
		)
	);
	EXPECT_THROW(chkptMgr->GetLastNode(), EclipseMonitor::Exception);

	EXPECT_NO_THROW(chkptMgr->EndBootstrapPhase(devSyncState));
	EXPECT_NO_THROW(
		EXPECT_EQ(
			chkptMgr->GetLastNode().GetHeader().GetRawHeader(),
			expLastHeader.GetRawHeader()
		);
	);
	EXPECT_NO_THROW(
		EXPECT_EQ(
			chkptMgr->GetLastHeader().GetRawHeader(),
			expLastHeader.GetRawHeader()
		)
	);
}

GTEST_TEST(TestEthCheckpointMgr, Normal_Runtime_Add)
{
	// Testing configurations
	static constexpr size_t testingChkptSize = 10;
	static constexpr size_t testingChkptStart = 2;
	static constexpr size_t testingChkptEnd = 7;

	// testing sync state
	std::shared_ptr<SyncState> devSyncState =
		std::make_shared<SyncState>(SyncState::GetDevSyncState());

	// Expected results
	std::vector<Difficulty> expDiffMedian;
	for (size_t i = testingChkptStart; i < testingChkptEnd; ++i)
	{
		std::vector<Difficulty> diffs;
		for (size_t j = 0; j < testingChkptSize; ++j)
		{
			HeaderMgr header(
				GetEthHistHdr_0_100()[(i * testingChkptSize) + j], 0);
			diffs.push_back(header.GetDiff());
		}
		auto mit = diffs.begin() + (diffs.size() / 2);
		std::nth_element(diffs.begin(), mit, diffs.end());
		expDiffMedian.push_back(*mit);
	}
	HeaderMgr expLastHeader(
				GetEthHistHdr_0_100()[
					(testingChkptEnd * testingChkptSize) - 1],
				0);

	// Testing variables
	EclipseMonitor::MonitorConfig mConf;
	mConf.get_checkpointSize() = testingChkptSize;
	size_t currChkptIdx = 0;
	std::unique_ptr<CheckpointMgr> chkptMgr;
	chkptMgr = SimpleObjects::Internal::make_unique<CheckpointMgr>(
		mConf,
		[&chkptMgr, &currChkptIdx, expDiffMedian](){
			if (currChkptIdx >= testingChkptStart)
			{
				auto adjChkptIdx = currChkptIdx - testingChkptStart;
				auto actDiff = chkptMgr->GetDiffMedian();
				EXPECT_LT(adjChkptIdx, expDiffMedian.size());
				if (adjChkptIdx < expDiffMedian.size())
				{
					EXPECT_EQ(actDiff, expDiffMedian[adjChkptIdx]);
				}
				HeaderMgr header(
					GetEthHistHdr_0_100()[
						((currChkptIdx + 1) * testingChkptSize) - 1],
					0
				);
				EXPECT_NO_THROW(
					EXPECT_EQ(
						chkptMgr->GetLastNode().GetHeader().GetRawHeader(),
						header.GetRawHeader()
					);
				);
			}
			auto numRange = chkptMgr->GetCheckpointBlkNumRange();
			EXPECT_EQ(numRange.first, currChkptIdx * testingChkptSize);
			EXPECT_EQ(
				numRange.second,
				((currChkptIdx + 1) * testingChkptSize) - 1);
			++currChkptIdx;
		});
	for (size_t i = 0; i < testingChkptStart; ++i)
	{
		for (size_t j = 0; j < testingChkptSize; ++j)
		{
			auto header = SimpleObjects::Internal::make_unique<HeaderMgr>(
				GetEthHistHdr_0_100()[(i * testingChkptSize) + j], 0);
			chkptMgr->AddHeader(std::move(header));
		}
	}
	chkptMgr->EndBootstrapPhase(devSyncState);

	// Test
	EXPECT_FALSE(chkptMgr->IsEmpty());
	for (size_t i = testingChkptStart; i < testingChkptEnd; ++i)
	{
		for (size_t j = 0; j < testingChkptSize; ++j)
		{
			auto header = SimpleObjects::Internal::make_unique<HeaderMgr>(
				GetEthHistHdr_0_100()[(i * testingChkptSize) + j], 0);
			auto node = SimpleObjects::Internal::make_unique<HeaderNode>(
				std::move(header),
				devSyncState
			);
			chkptMgr->AddNode(std::move(node));
		}
	}
	EXPECT_EQ(currChkptIdx, testingChkptEnd);
	EXPECT_NO_THROW(
		EXPECT_EQ(
			chkptMgr->GetLastNode().GetHeader().GetRawHeader(),
			expLastHeader.GetRawHeader()
		);
	);
}
