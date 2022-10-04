// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <array>

#include <gtest/gtest.h>

#include <EclipseMonitor/Eth/HeaderMgr.hpp>
#include <EclipseMonitor/MonitorReport.hpp>

#include "EthHistHdr_0_100.hpp"

namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor;
using namespace EclipseMonitor_Test;

GTEST_TEST(TestMonitorReport, CountTestFile)
{
	static auto tmp = ++g_numOfTestFile;
	(void)tmp;
}

GTEST_TEST(TestMonitorReport, MonitorId)
{
	{
		auto testInput = SimpleObjects::Bytes({
			0x00U, 0x01U, 0x02U, 0x03U, 0x04U, 0x05U, 0x06U, 0x07U,
			0x00U, 0x01U, 0x02U, 0x03U, 0x04U, 0x05U, 0x06U, 0x07U,
		});

		MonitorId mId;
		mId.get_sessionID() = testInput;
		EXPECT_EQ(mId.get_sessionID(), testInput);

		testInput[0] = 0x01U;
		EXPECT_NE(mId.get_sessionID(), testInput);
	}
}

GTEST_TEST(TestMonitorReport, MonitorConfig)
{
	{
		MonitorConfig mConf = BuildDefaultMonitorConfig();
		EXPECT_EQ(mConf.get_checkpointSize().GetVal(),  430);
		EXPECT_EQ(mConf.get_minDiffPercent().GetVal(),  103);
		EXPECT_EQ(mConf.get_maxWaitTime().GetVal(),     400);
		EXPECT_EQ(mConf.get_syncMaxWaitTime().GetVal(), 13);
	}
}

GTEST_TEST(TestMonitorReport, MonitorSecState)
{
	{
		auto header00Bin = GetEthHistHdr_0_100()[0];
		Eth::HeaderMgr header00 = Eth::HeaderMgr(header00Bin, 0);
		auto header01Bin = GetEthHistHdr_0_100()[1];
		Eth::HeaderMgr header01 = Eth::HeaderMgr(header01Bin, 0);

		MonitorSecState mSecState;
		mSecState.get_chainName()      = "ethereum";
		mSecState.get_genesisHash()    =
			header00.GetRawHeader().get_ParentHash();
		mSecState.get_checkpointIter() = 12345;
		mSecState.get_checkpointHash() =
			header01.GetRawHeader().get_ParentHash();

		EXPECT_EQ(mSecState.get_chainName(), "ethereum");
		EXPECT_EQ(
			mSecState.get_genesisHash(),
			header00.GetRawHeader().get_ParentHash());
		EXPECT_EQ(mSecState.get_checkpointIter().GetVal(), 12345);
		EXPECT_EQ(
			mSecState.get_checkpointHash(),
			header01.GetRawHeader().get_ParentHash());
	}
}
