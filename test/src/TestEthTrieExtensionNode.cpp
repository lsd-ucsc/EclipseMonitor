// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/Eth/Trie/BranchNode.hpp>
#include <EclipseMonitor/Eth/Trie/ExtensionNode.hpp>
#include <EclipseMonitor/Eth/Trie/LeafNode.hpp>
#include <EclipseMonitor/Eth/Trie/Nibbles.hpp>

namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor_Test;

using namespace EclipseMonitor::Eth::Trie;

GTEST_TEST(TestEthTrieExtensionNode, CountTestFile)
{
	static auto tmp = ++g_numOfTestFile;
	(void)tmp;
}

GTEST_TEST(TestEthTrieExtensionNode, SingleExtensionTest)
{
	std::vector<Nibble> nibbles = {5, 0, 6};
	SimpleObjects::Bytes val = {'c', 'o', 'i', 'n'};
	std::unique_ptr<LeafNode> leafBase =
		LeafNode::NewLeafNodeFromNibbles(nibbles, val);
	std::unique_ptr<Node> leaf =
		SimpleObjects::Internal::make_unique<Node>(std::move(leafBase));

	std::unique_ptr<BranchNode> branchNodeBase =
		SimpleObjects::Internal::make_unique<BranchNode>();
	branchNodeBase->SetBranch(0, std::move(leaf));
	branchNodeBase->SetValue(SimpleObjects::Bytes({'v', 'e', 'r', 'b'}));
	std::unique_ptr<Node> branchNode =
		SimpleObjects::Internal::make_unique<Node>(std::move(branchNodeBase));

	std::vector<uint8_t> nibbleBytes = {1, 2, 3, 4};
	std::vector<Nibble> nibs = NibbleHelper::FromBytes(nibbleBytes);
	ExtensionNode extensionNode(std::move(nibs), std::move(branchNode));

	std::vector<uint8_t> serialized = extensionNode.Serialize();
	std::vector<uint8_t> expectedSerialized = {
		0xe4U, 0x85U, 0x00U, 0x01U, 0x02U, 0x03U, 0x04U, 0xddU,
		0xc8U, 0x82U, 0x35U, 0x06U, 0x84U, 0x63U, 0x6fU, 0x69U,
		0x6eU, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U,
		0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U,
		0x84U, 0x76U, 0x65U, 0x72U, 0x62U
	};
	EXPECT_EQ(serialized, expectedSerialized);

	SimpleObjects::Bytes hashed = extensionNode.Hash();
	SimpleObjects::Bytes expectedHashed = {
		0x64U, 0xd6U, 0x7cU, 0x53U, 0x18U, 0xa7U, 0x14U, 0xd0U,
		0x8dU, 0xe6U, 0x95U, 0x8cU, 0x0eU, 0x63U, 0xa0U, 0x55U,
		0x22U, 0x64U, 0x2fU, 0x3fU, 0x10U, 0x87U, 0xc6U, 0xfdU,
		0x68U, 0xa9U, 0x78U, 0x37U, 0xf2U, 0x03U, 0xd3U, 0x59U
	};
	EXPECT_EQ(hashed, expectedHashed);
}

GTEST_TEST(TestEthTrieExtensionNode, NestedExtensionTest)
{
	std::vector<Nibble> nibbles1 = {1, 1, 1};
	SimpleObjects::Bytes val1 = {'a'};
	std::unique_ptr<LeafNode> leaf1Base =
		LeafNode::NewLeafNodeFromNibbles(nibbles1, val1);
	std::unique_ptr<Node> leaf1 =
		SimpleObjects::Internal::make_unique<Node>(std::move(leaf1Base));

	std::unique_ptr<BranchNode> branchNode1Base =
		SimpleObjects::Internal::make_unique<BranchNode>();
	branchNode1Base->SetBranch(0, std::move(leaf1));
	branchNode1Base->SetValue(SimpleObjects::Bytes({'b'}));
	std::unique_ptr<Node> branchNode1 =
		SimpleObjects::Internal::make_unique<Node>(std::move(branchNode1Base));

	std::vector<uint8_t> nibbleBytes1 = {1, 2};
	std::vector<Nibble> nibs1 = NibbleHelper::FromBytes(nibbleBytes1);
	std::unique_ptr<ExtensionNode> extensionNode1Base =
		SimpleObjects::Internal::make_unique<ExtensionNode>(
			std::move(nibs1),
			std::move(branchNode1)
		);
	std::unique_ptr<Node> extensionNode1 =
		SimpleObjects::Internal::make_unique<Node>(
			std::move(extensionNode1Base)
		);

	std::vector<Nibble> nibbles2 = {7, 7, 7};
	SimpleObjects::Bytes val2 = {'d'};
	std::unique_ptr<LeafNode> leaf2Base =
		LeafNode::NewLeafNodeFromNibbles(nibbles2, val2);
	std::unique_ptr<Node> leaf2 =
		SimpleObjects::Internal::make_unique<Node>(std::move(leaf2Base));

	std::unique_ptr<BranchNode> branchNode2Base =
		SimpleObjects::Internal::make_unique<BranchNode>();
	branchNode2Base->SetBranch(4, std::move(extensionNode1));
	branchNode2Base->SetBranch(8, std::move(leaf2));
	branchNode2Base->SetValue(SimpleObjects::Bytes({'e'}));
	std::unique_ptr<Node> branchNode2 =
		SimpleObjects::Internal::make_unique<Node>(std::move(branchNode2Base));

	std::vector<uint8_t> nibbleBytes2 = {3, 4};
	std::vector<Nibble> nibs2 = NibbleHelper::FromBytes(nibbleBytes2);
	ExtensionNode extensionNode2(std::move(nibs2), std::move(branchNode2));

	std::vector<uint8_t> serialized = extensionNode2.Serialize();
	std::vector<uint8_t> expectedSerialized = {
		0xe5U, 0x83U, 0x00U, 0x03U, 0x04U, 0xa0U, 0x7aU, 0xaeU,
		0xf9U, 0x11U, 0x2bU, 0xdbU, 0xbeU, 0xf7U, 0x7eU, 0x21U,
		0xa1U, 0xfcU, 0x94U, 0x8fU, 0x0cU, 0x3eU, 0x3aU, 0xb7U,
		0xaaU, 0xd9U, 0xdaU, 0x79U, 0x67U, 0xbcU, 0xdbU, 0xe5U,
		0x7aU, 0x93U, 0xe7U, 0x41U, 0x03U, 0xd2U
	};
	EXPECT_EQ(serialized, expectedSerialized);

	SimpleObjects::Bytes hashed = extensionNode2.Hash();
	SimpleObjects::Bytes expectedHashed = {
		0x1fU, 0xcbU, 0x0eU, 0xd9U, 0x47U, 0xaaU, 0x72U, 0x1eU,
		0x9aU, 0x85U, 0xadU, 0x53U, 0x2bU, 0xf2U, 0x06U, 0x96U,
		0xaaU, 0x88U, 0x8fU, 0x97U, 0xb8U, 0x89U, 0xe2U, 0x12U,
		0x32U, 0x91U, 0x46U, 0xddU, 0x95U, 0x85U, 0x95U, 0x16U
	};
	EXPECT_EQ(hashed, expectedHashed);
}
