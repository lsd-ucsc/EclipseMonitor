// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/EthKeccak256.hpp>
#include <EclipseMonitor/Trie/TrieNode.hpp>
#include <SimpleRlp/SimpleRlp.hpp>

using namespace EclipseMonitor;
using namespace SimpleRlp;

GTEST_TEST(TestEmptyNode, EmptyRawTest)
{
    std::vector<uint8_t> rlp = WriteRlp(TrieNode::EmptyNode::EmptyNodeRaw());
    std::vector<uint8_t> expected = {0x80};
    EXPECT_EQ(rlp, expected);
}

GTEST_TEST(TestEmptyNode, EmptyNodeTest)
{
    std::vector<uint8_t> emptyNodeRlp = WriteRlp(TrieNode::EmptyNode::EmptyNodeRaw());
    std::array<uint8_t, 32> emptyNodeHashArr = EthKeccak256(emptyNodeRlp);

    std::vector<uint8_t> emptyNodeHash;
    emptyNodeHash.reserve(32);
    std::move(emptyNodeHashArr.begin(), emptyNodeHashArr.end(), std::back_inserter(emptyNodeHash));

    EXPECT_EQ(TrieNode::EmptyNode::EmptyNodeHash().GetVal(), emptyNodeHash);
}