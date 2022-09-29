// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <typeinfo>

#include "BranchNode.hpp"
#include "ExtensionNode.hpp"
#include "LeafNode.hpp"
#include "Nibbles.hpp"
#include "TrieNode.hpp"

namespace EclipseMonitor
{

namespace Trie
{

/**
 * This Trie implementation is based on a simplified version of Ethereum's
 * Patricia Merkle Trie.
 *
 * source: https://github.com/zhangchiqing/merkle-patricia-trie
 *
 * */
class PatriciaTrie
{
public:

	PatriciaTrie() = default;

	void Reset()
	{
		Root.reset();
	}

	/**
	 * calculates the root hash of the trie. For transactions and receipts, this function
	 * would produce the "transactionsRoot" and "receiptsRoot", respectively.
	 *
	 * */
	SimpleObjects::Bytes Hash()
	{
		if (!Root)
		{
			return EmptyNode::EmptyNodeHash();
		}
		std::unique_ptr<NodeBase>& rootBase = Root->GetNodeBase();
		return rootBase->Hash();
	}

	void Put(const SimpleObjects::Bytes& key,
			 const SimpleObjects::Bytes& value)
	{
		std::vector<Nibble> nibbles = NibbleHelper::FromBytes(key.GetVal());
		PutKey(Root, nibbles, value);
	}

	void PutKey(std::unique_ptr<Node>& node,
				std::vector<Nibble> nibbles,
				const SimpleObjects::Bytes& value)
	{
		// empty node, create a new leaf
		if (!node)
		{
			std::unique_ptr<NodeBase> leafBase = LeafNode::NewLeafNodeFromNibbles(nibbles, value);
			std::unique_ptr<Node> leaf = SimpleObjects::Internal::make_unique<Node>(std::move(leafBase));
			node = std::move(leaf);

			return;
		}

		// std::unique_ptr<NodeBase>& nodeBase = node->GetNodeBase();
		NodeType nodeType = node->GetNodeType();

		// leaf node, convert to Extension node, add new branch with new leaf
		if (nodeType == NodeType::Leaf)
		{

			std::unique_ptr<NodeBase>& nodeBase = node->GetNodeBase();
			LeafNode* leaf = static_cast<LeafNode*>(nodeBase.get());


			const std::vector<Nibble>& leafPath = leaf->GetPath();

			uint8_t matched = NibbleHelper::PrefixMatchedLen(nibbles, leafPath);

			if (matched == nibbles.size() && matched == leafPath.size())
			{
				// replace leaf with new value
				auto newLeafBase = LeafNode::NewLeafNodeFromNibbles(leafPath, value);
				std::unique_ptr<Node> newLeaf = SimpleObjects::Internal::make_unique<Node>(std::move(newLeafBase));
				node.reset();
				node = std::move(newLeaf);

				return;
			}

			auto branchBase = SimpleObjects::Internal::make_unique<BranchNode>();

			// set the branch value
			if (matched == leafPath.size())
			{
				SimpleObjects::Bytes leafValue = leaf->GetValue();
				branchBase->SetValue(leafValue);
			}

			if (matched == nibbles.size())
			{
				branchBase->SetValue(value);
			}

			// assign LeafNode to branch
			if (matched < leafPath.size())
			{
				Nibble branchNibble(leafPath[matched]);
				std::vector<Nibble> leafNibbles(leafPath.begin() + matched + 1, leafPath.end());
				auto newLeafBase = LeafNode::NewLeafNodeFromNibbles(leafNibbles, leaf->GetValue());
				std::unique_ptr<Node> newLeaf = SimpleObjects::Internal::make_unique<Node>(std::move(newLeafBase));
				branchBase->SetBranch(branchNibble, std::move(newLeaf));
			}


			if (matched < nibbles.size())
			{
				Nibble branchNibble(nibbles[matched]);
				std::vector<Nibble> leafNibbles(nibbles.begin() + matched + 1, nibbles.end());
				auto newLeafBase = LeafNode::NewLeafNodeFromNibbles(leafNibbles, value);
				std::unique_ptr<Node> newLeaf = SimpleObjects::Internal::make_unique<Node>(std::move(newLeafBase));
				branchBase->SetBranch(branchNibble, std::move(newLeaf));
			}

			std::unique_ptr<Node> branch = SimpleObjects::Internal::make_unique<Node>(std::move(branchBase));

			// if some Nibbles match, make branch part of an ExtensionNode
			if (matched > 0)
			{
				std::vector<Nibble> sharedNibbles(leafPath.begin(), leafPath.begin() + matched);
				std::unique_ptr<ExtensionNode> extensionBase = SimpleObjects::Internal::make_unique<ExtensionNode>(
					std::move(sharedNibbles),
					std::move(branch));
				std::unique_ptr <Node> extension = SimpleObjects::Internal::make_unique<Node>(std::move(extensionBase));

				node.reset();
				node = std::move(extension);
			}
			else
			{
				node.reset();
				node = std::move(branch);
			}
			return;
		} // end leaf node

		// branch node, update value if nibbles are empty, otherwise update nibble branch
		if (nodeType == NodeType::Branch)
		{
			BranchNode* branch = static_cast<BranchNode*>(node->GetNodeBase().get());

			if (nibbles.size() == 0)
			{
				branch->SetValue(value);
				return;
			}

			Nibble branchNibble(nibbles[0]);
			std::vector<Nibble> remaining(nibbles.begin() + 1, nibbles.end());
			std::unique_ptr<Node>& branchNode = branch->GetBranch(branchNibble);
			PutKey(branchNode, remaining, value);

			return;
		} // end branch node

		if (nodeType == NodeType::Extension)
		{
			ExtensionNode* extension = static_cast<ExtensionNode*>(node->GetNodeBase().get());

			std::vector<Nibble> extensionPath = extension->GetPath();
			uint8_t matched = NibbleHelper::PrefixMatchedLen(nibbles, extensionPath);

			if (matched < extensionPath.size())
			{
				std::vector<Nibble> sharedNibbles(extensionPath.begin(), extensionPath.begin() + matched);
				Nibble branchNibble(extensionPath[matched]);
				std::vector<Nibble> remaining(extensionPath.begin() + matched + 1, extensionPath.end());

				Nibble nodeBranchNibble(nibbles[matched]);
				std::vector<Nibble> nodeLeafNibbles(nibbles.begin() + matched + 1, nibbles.end());

				auto branchBase = SimpleObjects::Internal::make_unique<BranchNode>();
				std::unique_ptr<Node> nextNode = std::move(extension->GetNext());

				if (remaining.size() == 0)
				{
					branchBase->SetBranch(branchNibble, std::move(nextNode));
				}
				else
				{
					auto newExtensionBase = SimpleObjects::Internal::make_unique<ExtensionNode>(std::move(remaining),
																								std::move(nextNode));
					std::unique_ptr<Node> newExtension = SimpleObjects::Internal::make_unique<Node>(
						std::move(newExtensionBase));
					branchBase->SetBranch(branchNibble, std::move(newExtension));
				}

				auto remainingLeafBase = LeafNode::NewLeafNodeFromNibbles(nodeLeafNibbles, value);
				std::unique_ptr<Node> remainingLeaf = SimpleObjects::Internal::make_unique<Node>(
					std::move(remainingLeafBase));
				branchBase->SetBranch(nodeBranchNibble, std::move(remainingLeaf));

				std::unique_ptr<Node> branch = SimpleObjects::Internal::make_unique<Node>(std::move(branchBase));
				node.reset();

				if (sharedNibbles.size() == 0)
				{
					node = std::move(branch);
				}
				else
				{
					auto newExtensionBase = SimpleObjects::Internal::make_unique<ExtensionNode>(
						std::move(sharedNibbles),
						std::move(branch));
					std::unique_ptr<Node> newExtension = SimpleObjects::Internal::make_unique<Node>(
						std::move(newExtensionBase));
					node = std::move(newExtension);
				}
				return;
			}
			std::vector<Nibble> remaining(nibbles.begin() + matched, nibbles.end());
			PutKey(extension->GetNext(), remaining, value);

			return;
		} // end extension node
		return;
	} // end PutKey()

private:

	std::unique_ptr<Node> Root;
}; // end class Trie

} // namespace Trie
} // namespace SimpleObjects
