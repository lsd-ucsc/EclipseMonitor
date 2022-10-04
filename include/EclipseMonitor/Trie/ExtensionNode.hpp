// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include "../Internal/SimpleObj.hpp"

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

	ExtensionNode(
		std::vector<Nibble>&& otherPath,
		std::unique_ptr<Node> next
	) :
		m_path(std::move(otherPath)),
		m_next(std::move(next))
	{}

	// LCOV_EXCL_START
	virtual ~ExtensionNode() = default;
	// LCOV_EXCL_STOP

	std::vector<uint8_t> Serialize()
	{
		return NodeHelper::Serialize(this);
	}

	virtual NodeType GetNodeType() const override
	{
		return NodeType::Extension;
	}

	virtual Internal::Obj::Bytes Hash() override
	{
		std::vector<uint8_t> serialized = Serialize();
		std::array<uint8_t, 32> hashed = EthKeccak256(serialized);

		return Internal::Obj::Bytes(hashed.begin(), hashed.end());
	}

	virtual Internal::Obj::List Raw() override
	{
		Internal::Obj::List hashes;
		hashes.resize(2);

		std::vector<Nibble> prefixedPath =
			NibbleHelper::ToPrefixed(m_path, false);
		std::vector<uint8_t> pathBytes = NibbleHelper::ToBytes(prefixedPath);
		Internal::Obj::Bytes pathBytesObject(std::move(pathBytes));
		hashes[0] = pathBytesObject;

		std::unique_ptr<NodeBase>& nextBasePtr = m_next->GetNodeBase();
		std::vector<uint8_t> serialized =
			NodeHelper::Serialize(nextBasePtr.get());
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
		return m_path;
	}

	std::unique_ptr<Node>& GetNext()
	{
		return m_next;
	}

private:

	std::vector<Nibble> m_path;
	std::unique_ptr<Node> m_next;

}; // class ExtensionNode

} // namespace Trie
} // namespace EclipseMonitor
