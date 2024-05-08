// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/Eth/HeaderMgr.hpp>
#include <EclipseMonitor/Eth/TransactionsMgr.hpp>
#include <EclipseMonitor/Eth/Trie/Trie.hpp>

#include "BlockData.hpp"


namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}


using namespace EclipseMonitor_Test;

using namespace EclipseMonitor::Eth;


GTEST_TEST(TestEthTransactionsMgr, CountTestFile)
{
	static auto tmp = ++g_numOfTestFile;
	(void)tmp;
}


GTEST_TEST(TestEthTransactionsMgr, Block15415840)
{
	const auto headerB15415840 =
		BlockData::ReadBinary(
			BlockData::GetRlpFilePath("mainnet_b_15415840.header")
		);
	const auto txnB15415840 =
		BlockData::ReadRlp("mainnet_b_15415840.txns");

	const HeaderMgr headerMgr(headerB15415840, 0);

	TransactionsMgr mgr(txnB15415840.AsList());

	EXPECT_EQ(
		mgr.GetRootHashBytes(),
		headerMgr.GetRawHeader().get_TransactionsRoot()
	);
}


GTEST_TEST(TestEthTransactionsMgr, Block15209997)
{
	const auto headerB15209997 =
		BlockData::ReadBinary(
			BlockData::GetRlpFilePath("mainnet_b_15209997.header")
		);
	const auto txnB15209997 =
		BlockData::ReadRlp("mainnet_b_15209997.txns");

	const HeaderMgr headerMgr(headerB15209997, 0);

	TransactionsMgr mgr(txnB15209997.AsList());

	EXPECT_EQ(
		mgr.GetRootHashBytes(),
		headerMgr.GetRawHeader().get_TransactionsRoot()
	);
}
