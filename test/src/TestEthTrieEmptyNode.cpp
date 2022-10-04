// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/EthKeccak256.hpp>
#include <EclipseMonitor/Trie/TrieNode.hpp>
#include <SimpleRlp/SimpleRlp.hpp>

namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor_Test;

using namespace EclipseMonitor;
using namespace EclipseMonitor::Trie;
using namespace SimpleRlp;

GTEST_TEST(TestEthTrieEmptyNode, CountTestFile)
{
	static auto tmp = ++g_numOfTestFile;
	(void)tmp;
}

GTEST_TEST(TestEthTrieEmptyNode, EmptyRawTest)
{
	std::vector<uint8_t> rlp = WriteRlp(EmptyNode::EmptyNodeRaw());
	std::vector<uint8_t> expected = {0x80};
	EXPECT_EQ(rlp, expected);
}

GTEST_TEST(TestEthTrieEmptyNode, EmptyNodeTest)
{
	std::vector<uint8_t> emptyNodeRlp = WriteRlp(EmptyNode::EmptyNodeRaw());
	std::array<uint8_t, 32> emptyNodeHashArr = EthKeccak256(emptyNodeRlp);

	std::vector<uint8_t> emptyNodeHash(
		emptyNodeHashArr.begin(),
		emptyNodeHashArr.end()
	);

	EXPECT_EQ(EmptyNode::EmptyNodeHash().GetVal(), emptyNodeHash);
}
