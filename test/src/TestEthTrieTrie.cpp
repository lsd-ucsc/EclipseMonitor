// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/Trie/LeafNode.hpp>
#include <EclipseMonitor/Trie/Trie.hpp>

namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor_Test;

using namespace EclipseMonitor;
using namespace EclipseMonitor::Trie;
using namespace SimpleObjects;
using namespace SimpleRlp;

GTEST_TEST(TestEthTrieTrie, CountTestFile)
{
	static auto tmp = ++g_numOfTestFile;
	(void)tmp;
}

GTEST_TEST(TestEthTrieTrie, TestPutSingleKey)
{
	Trie::PatriciaTrie trie;

	SimpleObjects::Bytes emptyHash = EmptyNode::EmptyNodeHash();
	EXPECT_EQ(emptyHash, trie.Hash());

	SimpleObjects::Bytes key = {1, 2, 3, 4};
	SimpleObjects::Bytes val = {'h', 'e', 'l', 'l', 'o'};
	trie.Put(key, val);

	std::vector<Nibble> nibbles = NibbleHelper::FromBytes(key.GetVal());
	LeafNode leafNode(nibbles, val);
	EXPECT_EQ(leafNode.Hash(), trie.Hash());
}

GTEST_TEST(TestEthTrieTrie, TestPutLeafShorter)
{
	Trie::PatriciaTrie trie;

	SimpleObjects::Bytes key1 = {1, 2, 3, 4};
	SimpleObjects::Bytes val1 = {'h', 'e', 'l', 'l', 'o'};
	trie.Put(key1, val1);

	SimpleObjects::Bytes key2 = {1, 2, 3};
	SimpleObjects::Bytes val2 = {'w', 'o', 'r', 'l', 'd'};
	trie.Put(key2, val2);

	SimpleObjects::Bytes hash = trie.Hash();
	std::vector<uint8_t> expected = {
		0xc2U, 0xe8U, 0x98U, 0x86U, 0xfdU, 0xaeU, 0x0fU, 0x43U,
		0x62U, 0x27U, 0xcbU, 0xa5U, 0x55U, 0xc4U, 0x49U, 0xa0U,
		0xbdU, 0xadU, 0x66U, 0xf9U, 0x6aU, 0x21U, 0xe3U, 0xb3U,
		0x07U, 0xcaU, 0x89U, 0x7bU, 0x99U, 0x49U, 0xc2U, 0x0fU
	};
	EXPECT_EQ(expected, hash.GetVal());

	std::vector<Nibble> leafNibbles = {4};
	std::unique_ptr<LeafNode> leafBase =
		LeafNode::NewLeafNodeFromNibbles(leafNibbles, val1);
	std::unique_ptr<Node> leaf =
		SimpleObjects::Internal::make_unique<Node>(std::move(leafBase));

	std::unique_ptr<BranchNode> branchBase =
		SimpleObjects::Internal::make_unique<BranchNode>();
	branchBase->SetBranch(0, std::move(leaf));
	branchBase->SetValue(val2);
	std::unique_ptr<Node> branch =
		SimpleObjects::Internal::make_unique<Node>(std::move(branchBase));

	std::vector<Nibble> extNibbles = {0, 1, 0, 2, 0, 3};
	ExtensionNode extensionNode(std::move(extNibbles), std::move(branch));
	hash = extensionNode.Hash();
	EXPECT_EQ(expected, hash.GetVal());
}

GTEST_TEST(TestEthTrieTrie, PutLeafAllMatched)
{
	Trie::PatriciaTrie trie;

	SimpleObjects::Bytes key1 = {1, 2, 3, 4};
	SimpleObjects::Bytes val1 = {'h', 'e', 'l', 'l', 'o'};
	trie.Put(key1, val1);

	SimpleObjects::Bytes key2 = {1, 2, 3, 4};
	SimpleObjects::Bytes val2 = {'w', 'o', 'r', 'l', 'd'};
	trie.Put(key2, val2);

	std::unique_ptr<LeafNode> leaf =
		LeafNode::NewLeafNodeFromBytes(key1.GetVal(), val2);
	EXPECT_EQ(leaf->Hash(), trie.Hash());
}

GTEST_TEST(TestEthTrieTrie, PutLeafMore)
{
	Trie::PatriciaTrie trie;

	SimpleObjects::Bytes key1 = {1, 2, 3, 4};
	SimpleObjects::Bytes val1 = {'h', 'e', 'l', 'l', 'o'};
	trie.Put(key1, val1);

	SimpleObjects::Bytes key2 = {1, 2, 3, 4, 5, 6};
	SimpleObjects::Bytes val2 = {'w', 'o', 'r', 'l', 'd'};
	trie.Put(key2, val2);

	std::vector<Nibble> leafNibbles = {5, 0, 6};
	std::unique_ptr<LeafNode> leafBase =
		LeafNode::NewLeafNodeFromNibbles(leafNibbles, val2);
	std::unique_ptr<Node> leaf =
		SimpleObjects::Internal::make_unique<Node>(std::move(leafBase));


	std::unique_ptr<BranchNode> branchBase =
		SimpleObjects::Internal::make_unique<BranchNode>();
	branchBase->SetValue(val1);
	branchBase->SetBranch(0, std::move(leaf));
	std::unique_ptr<Node> branch =
		SimpleObjects::Internal::make_unique<Node>(std::move(branchBase));

	std::vector<Nibble> extNibbles = {0, 1, 0, 2, 0, 3, 0, 4};
	std::unique_ptr<ExtensionNode> extension =
		SimpleObjects::Internal::make_unique<ExtensionNode>(
			std::move(extNibbles),
			std::move(branch)
		);
	EXPECT_EQ(extension->Hash(), trie.Hash());
}

GTEST_TEST(TestEthTrieTrie, TestPutMultipleKeys)
{
	Trie::PatriciaTrie trie;

	SimpleObjects::Bytes key1 = {1};
	SimpleObjects::Bytes val1 = {'a'};
	trie.Put(key1, val1);

	SimpleObjects::Bytes key2 = {1, 1};
	SimpleObjects::Bytes val2 = {'a', 'b'};
	trie.Put(key2, val2);

	SimpleObjects::Bytes key3 = {1, 1, 2};
	SimpleObjects::Bytes val3 = {'a', 'b', 'c'};
	trie.Put(key3, val3);

	std::vector<uint8_t> expected = {
		0x33U, 0xb2U, 0xefU, 0x44U, 0x78U, 0x4aU, 0x9bU, 0x74U,
		0x57U, 0xb4U, 0xd3U, 0xd0U, 0xf1U, 0x50U, 0x53U, 0xa8U,
		0x4aU, 0xe1U, 0x30U, 0x44U, 0xb9U, 0xe7U, 0x04U, 0xd8U,
		0x5dU, 0xbdU, 0x79U, 0x70U, 0xbaU, 0x9cU, 0xf8U, 0xc5U
	};
	EXPECT_EQ(expected, trie.Hash().GetVal());

	SimpleObjects::Bytes key4 = {1, 1, 3};
	SimpleObjects::Bytes val4 = {'a', 'b', 'c'};
	trie.Put(key4, val4);

	expected = {
		0x71U, 0x0cU, 0x96U, 0xfcU, 0x68U, 0xe7U, 0x82U, 0x2eU,
		0x3dU, 0x65U, 0x9aU, 0xa9U, 0xc9U, 0xb1U, 0x2fU, 0x9eU,
		0xbaU, 0xa4U, 0x85U, 0xe3U, 0x19U, 0x27U, 0x2dU, 0xafU,
		0xd1U, 0x0cU, 0xceU, 0x8fU, 0x2aU, 0xf4U, 0xd8U, 0xb8U
	};
	EXPECT_EQ(expected, trie.Hash().GetVal());


	SimpleObjects::Bytes key5 = {1, 1, 10};
	SimpleObjects::Bytes val5 = {'d'};
	trie.Put(key5, val5);

	SimpleObjects::Bytes key6 = {1, 1, 11};
	SimpleObjects::Bytes val6 = {'e'};
	trie.Put(key6, val6);

	SimpleObjects::Bytes key7 = {1, 1, 18};
	SimpleObjects::Bytes val7 = {'f'};
	trie.Put(key7, val7);

	SimpleObjects::Bytes key8 = {1, 1, 19};
	SimpleObjects::Bytes val8 = {'g'};
	trie.Put(key8, val8);

	expected = {
		0x5cU, 0xb3U, 0xbcU, 0x23U, 0x96U, 0x5fU, 0x40U, 0x31U,
		0x32U, 0x1aU, 0x8bU, 0xe1U, 0x9eU, 0x18U, 0xe6U, 0xdfU,
		0x1eU, 0xb3U, 0x08U, 0xe4U, 0x5dU, 0x06U, 0xf1U, 0xafU,
		0x71U, 0x41U, 0x6dU, 0x49U, 0xf0U, 0x91U, 0x5cU, 0x57U
	};
	EXPECT_EQ(expected, trie.Hash().GetVal());
}
