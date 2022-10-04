// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once


#include <SimpleObjects/Internal/make_unique.hpp>

#include "../Internal/SimpleObj.hpp"

#include "../EthKeccak256.hpp"

#include "Nibbles.hpp"
#include "TrieNode.hpp"

namespace EclipseMonitor
{

namespace Trie
{

class LeafNode : public NodeBase
{
public: // static members:

	static std::unique_ptr<LeafNode> NewLeafNodeFromNibbles(
		const std::vector<Nibble>& nibbles,
		const Internal::Obj::Bytes& value
	)
	{
		return Internal::Obj::Internal::make_unique<LeafNode>(nibbles, value);
	}

	static std::unique_ptr<LeafNode> NewLeafNodeFromBytes(
		const std::vector <uint8_t>& key,
		const Internal::Obj::Bytes& value
	)
	{
		std::vector<Nibble> nibbles = NibbleHelper::FromBytes(key);
		return NewLeafNodeFromNibbles(nibbles, value);
	}

public:

	LeafNode(
		std::vector<Nibble> otherPath, // TODO[Tuan]: make this type consistent with other child classes
		Internal::Obj::Bytes otherValue
	) :
		m_path(std::move(otherPath)),
		m_value(std::move(otherValue))
	{}

	// LCOV_EXCL_START
	virtual ~LeafNode() = default;
	// LCOV_EXCL_STOP

	std::vector<uint8_t> Serialize()
	{
		NodeBase* BasePtr = static_cast<NodeBase*>(this);
		return NodeHelper::Serialize(BasePtr);
	}

	virtual NodeType GetNodeType() const override
	{
		return NodeType::Leaf;
	}

	virtual Internal::Obj::Bytes Hash() override
	{
		std::vector<uint8_t> serialized = Serialize();
		std::array<uint8_t, 32> hashed = EthKeccak256(serialized);
		return Internal::Obj::Bytes(hashed.begin(), hashed.end());
	}

	virtual Internal::Obj::List Raw() override
	{
		std::vector<Nibble> prefixedPath =
			NibbleHelper::ToPrefixed(m_path, true);
		std::vector<uint8_t> pathBytes = NibbleHelper::ToBytes(prefixedPath);

		Internal::Obj::Bytes pathBytesObject(std::move(pathBytes));
		Internal::Obj::List raw = {pathBytesObject, m_value};
		return raw;
	}

	std::vector<Nibble> const& GetPath()
	{
		return m_path;
	}

	Internal::Obj::Bytes& GetValue()
	{
		return m_value;
	}

private:

	std::vector<Nibble> m_path;
	Internal::Obj::Bytes m_value;

}; // class LeafNode

} // namespace Trie
} // namespace EclipseMonitor
