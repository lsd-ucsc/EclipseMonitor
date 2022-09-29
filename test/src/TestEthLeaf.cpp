// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/Trie/LeafNode.hpp>

using namespace EclipseMonitor;
using namespace SimpleObjects;
using namespace SimpleRlp;
using namespace Trie;

GTEST_TEST(TestLeafNode, RawTest1)
{
	std::vector<uint8_t> nibbleBytes = {1};
	SimpleObjects::Bytes val = {};
	std::unique_ptr<LeafNode> leafNode = LeafNode::NewLeafNodeFromBytes(nibbleBytes, val);

	SimpleObjects::List raw = leafNode->Raw();
	SimpleObjects::List expected = {
		SimpleObjects::Bytes({0x20U, 0x01U}),
		SimpleObjects::Bytes()
	};
	EXPECT_EQ(raw[0], expected[0]);
	EXPECT_EQ(raw[1], expected[1]);
}

GTEST_TEST(TestLeafNode, RawTest2)
{
	std::vector<uint8_t> nibbleBytes = {1};
	SimpleObjects::Bytes val = {'h', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd'};
	std::unique_ptr<LeafNode> leafNode = LeafNode::NewLeafNodeFromBytes(nibbleBytes, val);

	SimpleObjects::List raw = leafNode->Raw();
	SimpleObjects::List expected = {
		SimpleObjects::Bytes({0x20U, 0x01U}),
		SimpleObjects::Bytes({0x68U, 0x65U, 0x6cU, 0x6cU,
							  0x6fU, 0x2cU, 0x20U, 0x77U,
							  0x6fU, 0x72U, 0x6cU, 0x64U})
	};
	EXPECT_EQ(raw[0], expected[0]);
	EXPECT_EQ(raw[1], expected[1]);
}

GTEST_TEST(TestLeafNode, RawTest3)
{
	std::vector<uint8_t> nibbleBytes = {1, 2, 3, 4, 5};
	SimpleObjects::Bytes val = {'h', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd'};
	std::unique_ptr<LeafNode> leafNode = LeafNode::NewLeafNodeFromBytes(nibbleBytes, val);

	SimpleObjects::List raw = leafNode->Raw();
	SimpleObjects::List expected = {
		SimpleObjects::Bytes({0x20U, 0x01U, 0x02U, 0x03U,
							  0x04U, 0x05U}),
		SimpleObjects::Bytes({0x68U, 0x65U, 0x6cU, 0x6cU,
							  0x6fU, 0x2cU, 0x20U, 0x77U,
							  0x6fU, 0x72U, 0x6cU, 0x64U})
	};
	EXPECT_EQ(raw[0], expected[0]);
	EXPECT_EQ(raw[1], expected[1]);
}

GTEST_TEST(TestLeafNode, HashTest1)
{
	std::vector<Nibble> nibbles = {5, 0, 6};
	SimpleObjects::Bytes val = {'c', 'o', 'i', 'n'};

	std::unique_ptr<LeafNode> leafNode = LeafNode::NewLeafNodeFromNibbles(nibbles, val);
	SimpleObjects::Bytes hash = leafNode->Hash();

	SimpleObjects::Bytes expected = {
		0xc3U, 0x7eU, 0xc9U, 0x85U, 0xb7U, 0xa8U, 0x8cU, 0x2cU,
		0x62U, 0xbeU, 0xb2U, 0x68U, 0x75U, 0x0eU, 0xfeU, 0x65U,
		0x7cU, 0x36U, 0xa5U, 0x85U, 0xbeU, 0xb4U, 0x35U, 0xebU,
		0x9fU, 0x43U, 0xb8U, 0x39U, 0x84U, 0x66U, 0x82U, 0xceU
	};

	EXPECT_EQ(hash, expected);
}

GTEST_TEST(TestLeafNode, HashTest2)
{
	std::vector<uint8_t> nibbleBytes = {1, 2, 3, 4};
	SimpleObjects::Bytes val = {'v', 'e', 'r', 'b'};
	std::unique_ptr<LeafNode> leafNode = LeafNode::NewLeafNodeFromBytes(nibbleBytes, val);
	SimpleObjects::Bytes hash = leafNode->Hash();

	SimpleObjects::Bytes expected = {
		0x2bU, 0xafU, 0xd1U, 0xeeU, 0xf5U, 0x8eU, 0x87U, 0x07U,
		0x56U, 0x9bU, 0x7cU, 0x70U, 0xebU, 0x2fU, 0x91U, 0x68U,
		0x31U, 0x36U, 0x91U, 0x06U, 0x06U, 0xbaU, 0x7eU, 0x31U,
		0xd0U, 0x75U, 0x72U, 0xb8U, 0xb6U, 0x7bU, 0xf5U, 0xc6U
	};

	EXPECT_EQ(hash, expected);
}
