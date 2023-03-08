// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/Eth/Trie/Trie.hpp>

#include "EthTransactions.hpp"

namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor_Test;

using namespace EclipseMonitor::Eth::Trie;

// converts a uint64_t to byte vector
using IntWriter = SimpleRlp::EncodePrimitiveIntValue<
	uint64_t,
	SimpleRlp::Endian::little,
	false
>;

GTEST_TEST(TestEthTrieTxnRoot, CountTestFile)
{
	static auto tmp = ++g_numOfTestFile;
	(void)tmp;
}

GTEST_TEST(TestEthTrieTxnRoot, TestTransactionsRootBlock15415840)
{
	SimpleObjects::Bytes keyBytes;
	SimpleObjects::Bytes value;
	PatriciaTrie trie;

	const SimpleObjects::Bytes& expected = GetTransactionsRoot_15415840();
	const auto& txns = GetEthTransactions_15415840();

	// insert transactions
	for(uint64_t i = 0; i < txns.size(); i++)
	{
		keyBytes.resize(0);

		IntWriter::Encode(i, std::back_inserter(keyBytes));

		trie.Put(SimpleRlp::WriteRlp(keyBytes), txns[i].AsBytes());
	}

	EXPECT_EQ(expected, trie.Hash());
}


GTEST_TEST(TestEthTrieTxnRoot, TestTransactionsRootBlock15209997)
{
	SimpleObjects::Bytes keyBytes;
	SimpleObjects::Bytes value;
	PatriciaTrie trie;

	const SimpleObjects::Bytes& expected = GetTransactionsRoot_15209997();
	const auto& txns = GetEthTransactions_15209997();

	// insert transactions
	for(uint64_t i = 0; i < txns.size(); i++)
	{
		keyBytes.resize(0);

		IntWriter::Encode(i, std::back_inserter(keyBytes));

		trie.Put(SimpleRlp::WriteRlp(keyBytes), txns[i].AsBytes());
	}
	EXPECT_EQ(expected, trie.Hash());
}
