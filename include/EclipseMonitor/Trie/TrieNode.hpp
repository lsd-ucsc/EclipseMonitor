// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include "../Internal/SimpleRlp.hpp"

namespace EclipseMonitor
{

namespace Trie
{

/**
 * @brief Enum for different node types
 *
 */
enum class NodeType
{
	Leaf,
	Branch,
	Extension,
	Null
}; // enum class NodeType


class NodeBase
{
public:

	NodeBase() = default;

	// LCOV_EXCL_START
	virtual ~NodeBase() = default;
	// LCOV_EXCL_STOP

	virtual NodeType GetNodeType() const = 0;

	virtual SimpleObjects::Bytes Hash() = 0;

	virtual SimpleObjects::List Raw() = 0;
}; // class NodeBase


class Node
{
public:
	Node() :
		m_node(nullptr)
	{}

	Node(std::unique_ptr<NodeBase> nodeBase) :
		m_node(std::move(nodeBase))
	{}

	// LCOV_EXCL_START
	~Node() = default;
	// LCOV_EXCL_STOP

	void SetNode(std::unique_ptr<NodeBase> nodeBase)
	{
		m_node.reset();
		m_node = std::move(nodeBase);
	}

	NodeType GetNodeType() const
	{
		if (m_node)
		{
			return m_node->GetNodeType();
		}
		else
		{
			return NodeType::Null;
		}
	}

	std::unique_ptr<NodeBase>& GetNodeBase()
	{
		return m_node;
	}

private:
	std::unique_ptr<NodeBase> m_node;

}; // class Node


struct EmptyNode
{
	static bool IsEmptyNode(const NodeBase* node)
	{
		return node == nullptr;
	}

	static SimpleObjects::Bytes EmptyNodeRaw()
	{
		return SimpleObjects::Bytes();
	}

	static SimpleObjects::Bytes EmptyNodeHash()
	{
		// https://github.com/ethereum/go-ethereum/blob/master/trie/trie.go#L32
		SimpleObjects::Bytes emptyTrieHash = {
			0x56, 0xe8, 0x1f, 0x17, 0x1b, 0xcc, 0x55, 0xa6,
			0xff, 0x83, 0x45, 0xe6, 0x92, 0xc0, 0xf8, 0x6e,
			0x5b, 0x48, 0xe0, 0x1b, 0x99, 0x6c, 0xad, 0xc0,
			0x01, 0x62, 0x2f, 0xb5, 0xe3, 0x63, 0xb4, 0x21
		};
		return emptyTrieHash;
	}

}; // struct EmptyNode


struct NodeHelper
{
	static std::vector<uint8_t> Serialize(NodeBase* node)
	{
		SimpleObjects::Object raw;
		if (EmptyNode::IsEmptyNode(node))
		{
			raw = EmptyNode::EmptyNodeRaw();
		}
		else
		{
			raw = node->Raw();
		}

		Internal::Rlp::OutputContainerType rlp = Internal::Rlp::WriteRlp(raw);
		return rlp;
	}
}; // struct NodeHelper

} // namespace Trie
} // namespace EclipseMonitor
