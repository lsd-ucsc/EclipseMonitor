// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include "EclipseMonitor/EthKeccak256.hpp"
#include "EclipseMonitor/Trie/Nibbles.hpp"
#include "EclipseMonitor/Trie/TrieNode.hpp"

using namespace SimpleObjects;

namespace EclipseMonitor
{

namespace Extension
{

class ExtensionNode : public TrieNode::NodeBase
{
public:

    ExtensionNode(std::vector<Nibble>&& otherPath,
                  std::unique_ptr<TrieNode::NodeBase> next)
    {
        Path = std::move(otherPath);
        Next = std::move(next);
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
        hashes.resize(2);

        std::vector<Nibble> prefixedPath = NibbleHelper::ToPrefixed(Path, false);
        std::vector<uint8_t> pathBytes = NibbleHelper::ToBytes(prefixedPath);
        SimpleObjects::Bytes pathBytesObject(std::move(pathBytes));
        hashes[0] = pathBytesObject;

        std::vector<uint8_t> serialized = TrieNode::Node::Serialize(Next.get());
        if (serialized.size() >= 32)
        {
            hashes[1] = Next->Hash();
        }
        else
        {
            hashes[1] = Next->Raw();
        }

        return hashes;
    }

    std::vector<Nibble>& GetPath()
    {
        return Path;
    }

    std::unique_ptr<TrieNode::NodeBase>& GetNext()
    {
        return Next;
    }

private:

    std::vector<Nibble> Path;
    std::unique_ptr<TrieNode::NodeBase> Next;
}; // class ExtensionNode

} // namespace Extension
} // namespace EclipseMonitor