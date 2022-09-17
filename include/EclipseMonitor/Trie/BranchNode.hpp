// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include "EclipseMonitor/EthKeccak256.hpp"
#include "EclipseMonitor/Trie/Nibbles.hpp"
#include "EclipseMonitor/Trie/TrieNode.hpp"
#include "SimpleObjects/Internal/make_unique.hpp"

using namespace SimpleObjects;

namespace EclipseMonitor
{

namespace Branch
{

class BranchNode : public TrieNode::NodeBase
{
public:

    BranchNode()
    {
        NodeHasValue = false;
        Branches.resize(16);
    }

    bool HasValue()
    {
        return NodeHasValue;
    }

    void SetBranch(const Nibble& nibble,
                   std::unique_ptr<TrieNode::NodeBase> other)
    {
        int nibbleInt = static_cast<int>(nibble);
        Branches[nibbleInt] = std::move(other);
    }

    void RemoveBranch(const Nibble& nibble)
    {
        int nibbleInt = static_cast<int>(nibble);
        Branches[nibbleInt].reset();
    }

    void SetValue(SimpleObjects::Bytes otherValue)
    {
        NodeHasValue = true;
        Value = std::move(otherValue);
    }

    void RemoveValue()
    {
        NodeHasValue = false;
        Value.resize(0);

    }

    std::vector<uint8_t> Serialize()
    {
        TrieNode::NodeBase* BasePtr = static_cast<TrieNode::NodeBase*>(this);
        return TrieNode::Node::Serialize(BasePtr);
    }

    virtual SimpleObjects::Bytes Hash() override
    {
        std::vector<uint8_t> serialized = Serialize();
        std::array<uint8_t, 32> hashed = EthKeccak256(serialized);

        return SimpleObjects::Bytes(hashed.begin(), hashed.end());
    }

    virtual SimpleObjects::List Raw() override
    {
        SimpleObjects::List hashes;
        hashes.resize(NumNodes + 1);

        for (uint8_t i = 0; i < NumNodes; i++)
        {
            if (!Branches[i])
            {
                hashes[i] = TrieNode::EmptyNode::EmptyNodeRaw();
            }
            else
            {
                TrieNode::NodeBase* NodePtr = Branches[i].get();
                std::vector<uint8_t> serialized = TrieNode::Node::Serialize(NodePtr);

                if (serialized.size() >= 32)
                {
                    hashes[i] = NodePtr->Hash();
                }
                else
                {
                    hashes[i] = NodePtr->Raw();
                }
            }
        }

        hashes[NumNodes] = Value;
        return hashes;
    }

    std::unique_ptr<TrieNode::NodeBase>& GetBranch(Nibble nibble)
    {
        int nibbleInt = static_cast<int>(nibble);
        return Branches[nibbleInt];
    }

private:

    bool NodeHasValue;
    const uint8_t NumNodes = 16;

    std::vector<std::unique_ptr<TrieNode::NodeBase>> Branches;
    SimpleObjects::Bytes Value;
}; // class BranchNode

} // namespace Branch
} // namespace EclipseMonitor