// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/Eth/SyncMsgMgr.hpp>

namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor_Test;

using namespace EclipseMonitor::Eth;

GTEST_TEST(TestEthSyncMsgMgr, CountTestFile)
{
	static auto tmp = ++g_numOfTestFile;
	(void)tmp;
}
