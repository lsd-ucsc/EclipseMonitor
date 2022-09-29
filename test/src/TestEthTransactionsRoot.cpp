// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/Trie/Trie.hpp>

#include "EthTransactions.hpp"

using namespace EclipseMonitor;
using namespace EclipseMonitor_Test;
using namespace SimpleObjects;
using namespace SimpleRlp;
using namespace Trie;

// converts a uint64_t to byte vector
using IntWriter = SimpleRlp::Internal::EncodeSizeValue<Endian::little, false>;

GTEST_TEST(TestTransactionsRoot, TestTransactionsRootBlock15415840)
{
	std::vector<uint8_t> key;
	SimpleObjects::Bytes keyBytes;
	SimpleObjects::Bytes encodedKey;
	SimpleObjects::Bytes value;
	Trie::PatriciaTrie trie;

	const SimpleObjects::Bytes& expected = GetTransactionsRoot_15415840();
	const std::vector<SimpleObjects::Bytes>& txns = GetEthTransactions_15415840();

	// insert first transaction separately
	if(txns.size() > 0)
	{
		keyBytes = SimpleObjects::Bytes({0x80U});
		value = txns[0];
		trie.Put(keyBytes, value);
	}

	// insert remaining transactions
	for(uint64_t i = 1; i < txns.size(); i++)
	{
		key.clear();

		IntWriter::Encode(key, i);
		keyBytes = SimpleObjects::Bytes(key);
		encodedKey = SimpleObjects::Bytes(SimpleRlp::WriteRlp(keyBytes));

		value = txns[i];
		trie.Put(encodedKey, value);
	}

	EXPECT_EQ(expected, trie.Hash());
}


GTEST_TEST(TestTransactionsRoot, TestTransactionsRootBlock15209997)
{
	std::vector<uint8_t> key;
	SimpleObjects::Bytes keyBytes;
	SimpleObjects::Bytes encodedKey;
	SimpleObjects::Bytes value;
	Trie::PatriciaTrie trie;

	const SimpleObjects::Bytes& expected = GetTransactionsRoot_15209997();
	const std::vector<SimpleObjects::Bytes>& txns = GetEthTransactions_15209997();

	// insert first transaction separately
	if(txns.size() > 0)
	{
		keyBytes = SimpleObjects::Bytes({0x80U});
		value = txns[0];
		trie.Put(keyBytes, value);
	}

	// insert remaining transactions
	for(uint64_t i = 1; i < txns.size(); i++)
	{
		key.clear();

		IntWriter::Encode(key, i);
		keyBytes = SimpleObjects::Bytes(key);
		encodedKey = SimpleObjects::Bytes(SimpleRlp::WriteRlp(keyBytes));

		value = txns[i];
		trie.Put(encodedKey, value);
	}
	EXPECT_EQ(expected, trie.Hash());
}
