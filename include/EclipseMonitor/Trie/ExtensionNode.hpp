// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include "../EthKeccak256.hpp"
#include "Nibbles.hpp"
#include "TrieNode.hpp"


namespace EclipseMonitor
{

namespace Trie
{

class ExtensionNode : public NodeBase
{
public:

	ExtensionNode(std::vector<Nibble>&& otherPath,
				  std::unique_ptr<Node> next)
	{
		Path = std::move(otherPath);
		Next = std::move(next);
	}

	std::vector<uint8_t> Serialize()
	{
		NodeBase* BasePtr = static_cast<NodeBase*>(this);

		return NodeHelper::Serialize(BasePtr);
	}

	virtual NodeType GetNodeType() const override
	{
		return NodeType::Extension;
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

		std::unique_ptr<NodeBase>& nextBasePtr = Next->GetNodeBase();
		std::vector<uint8_t> serialized = NodeHelper::Serialize(nextBasePtr.get());
		if (serialized.size() >= 32)
		{
			hashes[1] = nextBasePtr->Hash();
		}
		else
		{
			hashes[1] = nextBasePtr->Raw();
		}

		return hashes;
	}

	std::vector<Nibble>& GetPath()
	{
		return Path;
	}

	std::unique_ptr<Node>& GetNext()
	{
		return Next;
	}

private:

	std::vector<Nibble> Path;
	std::unique_ptr<Node> Next;
}; // class ExtensionNode

} // namespace Extension
} // namespace EclipseMonitor
