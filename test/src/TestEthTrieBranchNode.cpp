// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/Trie/BranchNode.hpp>
#include <EclipseMonitor/Trie/LeafNode.hpp>

namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor_Test;

using namespace EclipseMonitor;
using namespace EclipseMonitor::Trie;
using namespace SimpleObjects;
using namespace SimpleRlp;

GTEST_TEST(TestEthTrieBranchNode, CountTestFile)
{
	static auto tmp = ++g_numOfTestFile;
	(void)tmp;
}

GTEST_TEST(TestEthTrieBranchNode, TestOneLeaf)
{
	std::vector<Nibble> nibbles = {5, 0, 6};
	SimpleObjects::Bytes val = {'c', 'o', 'i', 'n'};
	std::unique_ptr<NodeBase> leafNodeBase =
		LeafNode::NewLeafNodeFromNibbles(nibbles, val);
	std::unique_ptr<Node> leafNode =
		SimpleObjects::Internal::make_unique<Node>(std::move(leafNodeBase));

	BranchNode branchNode;
	branchNode.SetBranch(0, std::move(leafNode));
	branchNode.SetValue(SimpleObjects::Bytes({'v', 'e', 'r', 'b'}));

	std::vector<uint8_t> serialized = branchNode.Serialize();
	std::vector<uint8_t> expectedSerialized = {
		0xddU, 0xc8U, 0x82U, 0x35U, 0x06U, 0x84U, 0x63U, 0x6fU,
		0x69U, 0x6eU, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U,
		0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U,
		0x80U, 0x84U, 0x76U, 0x65U, 0x72U, 0x62U
	};
	EXPECT_EQ(serialized, expectedSerialized);

	SimpleObjects::Bytes hashed = branchNode.Hash();
	SimpleObjects::Bytes expectedHashed = {
		0xd7U, 0x57U, 0x70U, 0x9fU, 0x08U, 0xf7U, 0xa8U, 0x1dU,
		0xa6U, 0x4aU, 0x96U, 0x92U, 0x00U, 0xe5U, 0x9fU, 0xf7U,
		0xe6U, 0xcdU, 0x6bU, 0x06U, 0x67U, 0x4cU, 0x3fU, 0x66U,
		0x8cU, 0xe1U, 0x51U, 0xe8U, 0x42U, 0x98U, 0xaaU, 0x79U
	};
	EXPECT_EQ(hashed, expectedHashed);
}

GTEST_TEST(TestEthTrieBranchNode, TestTwoLeaves)
{
	std::vector<Nibble> nibbles1 = {5, 0, 6};
	SimpleObjects::Bytes val1 = {'c', 'o', 'i', 'n'};
	std::unique_ptr<NodeBase> leafNodeBase1 =
		LeafNode::NewLeafNodeFromNibbles(nibbles1, val1);
	std::unique_ptr<Node> leafNode1 =
		SimpleObjects::Internal::make_unique<Node>(std::move(leafNodeBase1));

	std::vector<Nibble> nibbles2 = {1, 2, 3, 4, 5};
	SimpleObjects::Bytes val2 = {'w', 'a', 't', 'e', 'r'};
	std::unique_ptr<NodeBase> leafNodeBase2 =
		LeafNode::NewLeafNodeFromNibbles(nibbles2, val2);
	std::unique_ptr<Node> leafNode2 =
		SimpleObjects::Internal::make_unique<Node>(std::move(leafNodeBase2));


	BranchNode branchNode;
	branchNode.SetBranch(0, std::move(leafNode1));
	branchNode.SetBranch(8, std::move(leafNode2));
	branchNode.SetValue(SimpleObjects::Bytes({'n', 'o', 'u', 'n'}));

	std::vector<uint8_t> serialized = branchNode.Serialize();
	std::vector<uint8_t> expectedSerialized = {
		0xe7U, 0xc8U, 0x82U, 0x35U, 0x06U, 0x84U, 0x63U, 0x6fU,
		0x69U, 0x6eU, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U,
		0x80U, 0xcaU, 0x83U, 0x31U, 0x23U, 0x45U, 0x85U, 0x77U,
		0x61U, 0x74U, 0x65U, 0x72U, 0x80U, 0x80U, 0x80U, 0x80U,
		0x80U, 0x80U, 0x80U, 0x84U, 0x6eU, 0x6fU, 0x75U, 0x6eU
	};
	EXPECT_EQ(serialized, expectedSerialized);

	SimpleObjects::Bytes hashed = branchNode.Hash();
	SimpleObjects::Bytes expectedHashed = {
		0xefU, 0x38U, 0xc7U, 0x40U, 0x5fU, 0x10U, 0x13U, 0x35U,
		0x1aU, 0xddU, 0x67U, 0xb0U, 0x60U, 0x16U, 0x55U, 0x1eU,
		0xffU, 0x32U, 0x5fU, 0xedU, 0x4dU, 0x8bU, 0xbbU, 0x28U,
		0x60U, 0xd3U, 0xdfU, 0x9eU, 0xc0U, 0x84U, 0xdcU, 0xf6U
	};
	EXPECT_EQ(hashed, expectedHashed);
}

GTEST_TEST(TestEthTrieBranchNode, TestNestedBranch)
{
	std::vector<Nibble> nibbles = {5, 0, 6};
	SimpleObjects::Bytes val = {'c', 'o', 'i', 'n'};
	std::unique_ptr<NodeBase> leafNodeBase =
		LeafNode::NewLeafNodeFromNibbles(nibbles, val);
	std::unique_ptr<Node> leafNode =
		SimpleObjects::Internal::make_unique<Node>(std::move(leafNodeBase));

	std::unique_ptr<BranchNode> branchNode1Ptr =
		SimpleObjects::Internal::make_unique<BranchNode>();
	branchNode1Ptr->SetBranch(0, std::move(leafNode));
	branchNode1Ptr->SetValue(SimpleObjects::Bytes({'v', 'e', 'r', 'b'}));
	std::unique_ptr<Node> branchNode1 =
		SimpleObjects::Internal::make_unique<Node>(std::move(branchNode1Ptr));

	BranchNode branchNode2;
	branchNode2.SetBranch(7, std::move(branchNode1));
	branchNode2.SetValue(SimpleObjects::Bytes({'n', 'o', 'u', 'n'}));

	std::vector<uint8_t> serialized = branchNode2.Serialize();
	std::vector<uint8_t> expectedSerialized = {
		0xf2U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U,
		0xddU, 0xc8U, 0x82U, 0x35U, 0x06U, 0x84U, 0x63U, 0x6fU,
		0x69U, 0x6eU, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U,
		0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U,
		0x80U, 0x84U, 0x76U, 0x65U, 0x72U, 0x62U, 0x80U, 0x80U,
		0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x84U, 0x6eU,
		0x6fU, 0x75U, 0x6eU
	};
	EXPECT_EQ(serialized, expectedSerialized);

	SimpleObjects::Bytes hashed = branchNode2.Hash();
	SimpleObjects::Bytes expectedHashed = {
		0x2bU, 0x88U, 0xddU, 0x6dU, 0x72U, 0x2fU, 0x39U, 0x8fU,
		0x92U, 0xb1U, 0x0dU, 0xa8U, 0x66U, 0xb8U, 0xa9U, 0xf9U,
		0xdfU, 0xb5U, 0xb8U, 0xecU, 0x6fU, 0x5fU, 0xa7U, 0x25U,
		0x0dU, 0xceU, 0x7cU, 0xe3U, 0xc0U, 0xfcU, 0x19U, 0xdaU
	};
	EXPECT_EQ(hashed, expectedHashed);
}
