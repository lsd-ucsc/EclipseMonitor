// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/EthCheckpointMgr.hpp>

#include "EthHistHdr_0_100.hpp"

namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor;
using namespace EclipseMonitor_Test;

GTEST_TEST(TestCheckpointMgr, CountTestFile)
{
	static auto tmp = ++EclipseMonitor_Test::g_numOfTestFile;
	(void)tmp;
}

GTEST_TEST(TestCheckpointMgr, Normal_BootstrapI_Add)
{
	// Testing configurations
	static constexpr size_t testingChkptSize = 10;
	static constexpr size_t testingNumChkpt = 5;

	// Expected results
	std::vector<typename EthDiffTypeTrait::value_type> expDiffMedian;
	for (size_t i = 0; i < testingNumChkpt; ++i)
	{
		std::vector<typename EthDiffTypeTrait::value_type> diffs;
		for (size_t j = 0; j < testingChkptSize; ++j)
		{
			EthHeaderMgr header(
				GetEthHistHdr_0_100()[(i * testingChkptSize) + j], 0);
			diffs.push_back(header.GetDiff());
		}
		auto mit = diffs.begin() + (diffs.size() / 2);
		std::nth_element(diffs.begin(), mit, diffs.end());
		expDiffMedian.push_back(*mit);
	}
	EthHeaderMgr expLastHeader(
				GetEthHistHdr_0_100()[
					(testingNumChkpt * testingChkptSize) - 1],
				0);

	// Testing variables
	MonitorConfig mConf;
	mConf.get_checkpointSize() = testingChkptSize;
	size_t currChkptIdx = 0;
	std::unique_ptr<EthCheckpointMgr> chkptMgr;
	chkptMgr = SimpleObjects::Internal::make_unique<EthCheckpointMgr>(
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
	EXPECT_THROW(chkptMgr->GetLastHeader(), Exception);
	EXPECT_THROW(chkptMgr->GetLastNode(), Exception);
	for (size_t i = 0; i < testingNumChkpt; ++i)
	{
		for (size_t j = 0; j < testingChkptSize; ++j)
		{
			auto header = SimpleObjects::Internal::make_unique<EthHeaderMgr>(
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
	EXPECT_THROW(chkptMgr->GetLastNode(), Exception);

	EXPECT_NO_THROW(chkptMgr->EndBootstrapPhase());
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

GTEST_TEST(TestCheckpointMgr, Normal_Runtime_Add)
{
	// Testing configurations
	static constexpr size_t testingChkptSize = 10;
	static constexpr size_t testingChkptStart = 2;
	static constexpr size_t testingChkptEnd = 7;

	// Expected results
	std::vector<typename EthDiffTypeTrait::value_type> expDiffMedian;
	for (size_t i = testingChkptStart; i < testingChkptEnd; ++i)
	{
		std::vector<typename EthDiffTypeTrait::value_type> diffs;
		for (size_t j = 0; j < testingChkptSize; ++j)
		{
			EthHeaderMgr header(
				GetEthHistHdr_0_100()[(i * testingChkptSize) + j], 0);
			diffs.push_back(header.GetDiff());
		}
		auto mit = diffs.begin() + (diffs.size() / 2);
		std::nth_element(diffs.begin(), mit, diffs.end());
		expDiffMedian.push_back(*mit);
	}
	EthHeaderMgr expLastHeader(
				GetEthHistHdr_0_100()[
					(testingChkptEnd * testingChkptSize) - 1],
				0);

	// Testing variables
	MonitorConfig mConf;
	mConf.get_checkpointSize() = testingChkptSize;
	size_t currChkptIdx = 0;
	std::unique_ptr<EthCheckpointMgr> chkptMgr;
	chkptMgr = SimpleObjects::Internal::make_unique<EthCheckpointMgr>(
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
				EthHeaderMgr header(
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
			auto header = SimpleObjects::Internal::make_unique<EthHeaderMgr>(
				GetEthHistHdr_0_100()[(i * testingChkptSize) + j], 0);
			chkptMgr->AddHeader(std::move(header));
		}
	}
	chkptMgr->EndBootstrapPhase();

	// Test
	EXPECT_FALSE(chkptMgr->IsEmpty());
	for (size_t i = testingChkptStart; i < testingChkptEnd; ++i)
	{
		for (size_t j = 0; j < testingChkptSize; ++j)
		{
			auto header = SimpleObjects::Internal::make_unique<EthHeaderMgr>(
				GetEthHistHdr_0_100()[(i * testingChkptSize) + j], 0);
			auto node = SimpleObjects::Internal::make_unique<EthHeaderNode>(
				std::move(header));
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
