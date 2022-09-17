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

namespace Leaf
{

class LeafNode : public TrieNode::NodeBase
{
public:

    LeafNode(std::vector<Nibble> otherPath,
             SimpleObjects::Bytes otherValue)
    {
        Path = std::move(otherPath);
        Value = std::move(otherValue);
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
        std::vector<Nibble> prefixedPath = NibbleHelper::ToPrefixed(Path, true);
        std::vector<uint8_t> pathBytes = NibbleHelper::ToBytes(prefixedPath);

        SimpleObjects::Bytes pathBytesObject(std::move(pathBytes));
        SimpleObjects::List raw = {pathBytesObject, Value};
        return raw;
    }

    static std::unique_ptr<LeafNode> NewLeafNodeFromNibbles(const std::vector<Nibble>& nibbles,
                                                            const SimpleObjects::Bytes& value)
    {
        return SimpleObjects::Internal::make_unique<LeafNode>(nibbles, value);
    }

    static std::unique_ptr<LeafNode> NewLeafNodeFromBytes(const std::vector<uint8_t>& key,
                                                          const SimpleObjects::Bytes& value)
    {
        std::vector<Nibble> nibbles = NibbleHelper::FromBytes(key);
        return NewLeafNodeFromNibbles(nibbles, value);
    }

    std::vector<Nibble> const& GetPath()
    {
        return Path;
    }

    SimpleObjects::Bytes& GetValue()
    {
        return Value;
    }

private:

    std::vector<Nibble> Path;
    SimpleObjects::Bytes Value;
}; // class LeafNode

} // namespace Leaf
} // namespace EclipseMonitor