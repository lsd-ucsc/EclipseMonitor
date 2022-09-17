// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <typeinfo>

#include "EclipseMonitor/Trie/BranchNode.hpp"
#include "EclipseMonitor/Trie/ExtensionNode.hpp"
#include "EclipseMonitor/Trie/LeafNode.hpp"
#include "EclipseMonitor/Trie/Nibbles.hpp"
#include "EclipseMonitor/Trie/TrieNode.hpp"

using namespace SimpleObjects;

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
class Trie
{
public:

    Trie() = default;

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
        if (TrieNode::EmptyNode::IsEmptyNode(Root.get()))
        {
            return TrieNode::EmptyNode::EmptyNodeHash();
        }
        return Root->Hash();
    }

    void Put(const SimpleObjects::Bytes& key,
             const SimpleObjects::Bytes& value)
    {
        std::vector<Nibble> nibbles = NibbleHelper::FromBytes(key.GetVal());
        PutKey(Root, nibbles, value);
    }

    void PutKey(std::unique_ptr<TrieNode::NodeBase>& Node,
                std::vector<Nibble> nibbles,
                const SimpleObjects::Bytes& value)
    {
        // empty node, create a new leaf
        if (TrieNode::EmptyNode::IsEmptyNode(Node.get()))
        {
            auto leaf = Leaf::LeafNode::NewLeafNodeFromNibbles(nibbles, value);
            Node = std::move(leaf);

            return;
        }

        // leaf node, convert to Extension node, add new branch with new leaf
        if (typeid(*Node) == typeid(Leaf::LeafNode))
        {
            Leaf::LeafNode* leaf = static_cast<Leaf::LeafNode *>(Node.get());

            const std::vector<Nibble> &leafPath = leaf->GetPath();
            uint8_t matched = NibbleHelper::PrefixMatchedLen(nibbles, leafPath);

            if (matched == nibbles.size() && matched == leafPath.size())
            {
                // replace leaf with new value
                auto newLeaf = Leaf::LeafNode::NewLeafNodeFromNibbles(leafPath, value);
                Node.reset();
                Node = std::move(newLeaf);

                return;
            }

            auto branch = SimpleObjects::Internal::make_unique<Branch::BranchNode>();

            // set the branch value
            if (matched == leafPath.size())
            {
                SimpleObjects::Bytes leafValue = leaf->GetValue();
                branch->SetValue(leafValue);
            }

            if (matched == nibbles.size())
            {
                branch->SetValue(value);
            }

            // assign LeafNode to branch
            if (matched < leafPath.size())
            {
                Nibble branchNibble(leafPath[matched]);
                std::vector<Nibble> leafNibbles(leafPath.begin() + matched + 1, leafPath.end());
                auto newLeaf = Leaf::LeafNode::NewLeafNodeFromNibbles(leafNibbles, leaf->GetValue());
                branch->SetBranch(branchNibble, std::move(newLeaf));
            }

            if (matched < nibbles.size())
            {
                Nibble branchNibble(nibbles[matched]);
                std::vector<Nibble> leafNibbles(nibbles.begin() + matched + 1, nibbles.end());
                auto newLeaf = Leaf::LeafNode::NewLeafNodeFromNibbles(leafNibbles, value);
                branch->SetBranch(branchNibble, std::move(newLeaf));
            }

            // if some Nibbles match, make branch part of an ExtensionNode
            if (matched > 0)
            {
                std::vector<Nibble> sharedNibbles(leafPath.begin(), leafPath.begin() + matched);
                auto extension = SimpleObjects::Internal::make_unique<Extension::ExtensionNode>(std::move(sharedNibbles),
                                                                                                std::move(branch));
                Node.reset();
                Node = std::move(extension);
            }
            else
            {
                Node.reset();
                Node = std::move(branch);
            }
            return;
        } // end leaf node

        // branch node, update value if nibbles are empty, otherwise update nibble branch
        if (typeid(*Node) == typeid(Branch::BranchNode))
        {
            Branch::BranchNode* branch = static_cast<Branch::BranchNode*>(Node.get());

            if (nibbles.size() == 0)
            {
                branch->SetValue(value);
                return;
            }

            Nibble branchNibble(nibbles[0]);
            std::vector<Nibble> remaining(nibbles.begin() + 1, nibbles.end());
            std::unique_ptr<TrieNode::NodeBase>& branchNode = branch->GetBranch(branchNibble);
            PutKey(branchNode, remaining, value);

            return;
        } // end branch node

        if (typeid(*Node) == typeid(Extension::ExtensionNode))
        {
            Extension::ExtensionNode* extension = static_cast<Extension::ExtensionNode *>(Node.get());

            std::vector<Nibble> extensionPath = extension->GetPath();
            uint8_t matched = NibbleHelper::PrefixMatchedLen(nibbles, extensionPath);

            if (matched < extensionPath.size())
            {
                std::vector<Nibble> sharedNibbles(extensionPath.begin(), extensionPath.begin() + matched);
                Nibble branchNibble(extensionPath[matched]);
                std::vector<Nibble> remaining(extensionPath.begin() + matched + 1, extensionPath.end());

                Nibble nodeBranchNibble(nibbles[matched]);
                std::vector<Nibble> nodeLeafNibbles(nibbles.begin() + matched + 1, nibbles.end());

                auto branch = SimpleObjects::Internal::make_unique<Branch::BranchNode>();
                std::unique_ptr<TrieNode::NodeBase> nextNode = std::move(extension->GetNext());

                if (remaining.size() == 0)
                {
                    branch->SetBranch(branchNibble, std::move(nextNode));
                }
                else
                {
                    auto newExtension = SimpleObjects::Internal::make_unique<Extension::ExtensionNode>(std::move(remaining),
                                                                                                       std::move(nextNode));
                    branch->SetBranch(branchNibble, std::move(newExtension));
                }

                auto remainingLeaf = Leaf::LeafNode::NewLeafNodeFromNibbles(nodeLeafNibbles, value);
                branch->SetBranch(nodeBranchNibble, std::move(remainingLeaf));

                Node.reset();
                if (sharedNibbles.size() == 0)
                {
                    Node = std::move(branch);
                }
                else
                {
                    auto newExtension = SimpleObjects::Internal::make_unique<Extension::ExtensionNode>(std::move(sharedNibbles),
                                                                                                       std::move(branch));
                    Node = std::move(newExtension);
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

    std::unique_ptr<TrieNode::NodeBase> Root;
}; // end class Trie

} // namespace Trie
} // namespace SimpleObjects