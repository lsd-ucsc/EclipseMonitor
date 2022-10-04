// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <SimpleObjects/Internal/make_unique.hpp>

#include "../../Internal/SimpleObj.hpp"
#include "../../EthKeccak256.hpp"

#include "Nibbles.hpp"
#include "TrieNode.hpp"

namespace EclipseMonitor
{
namespace Eth
{
namespace Trie
{

class BranchNode : public NodeBase
{
public: // static members:

	static constexpr uint8_t sk_numNodes = 16;

public:

	BranchNode() :
		m_nodeHasValue(false),
		m_branches(16), // TODO[Tuan]: where does this number come from?
		m_value()
	{}

	// LCOV_EXCL_START
	virtual ~BranchNode() = default;
	// LCOV_EXCL_STOP

	bool HasValue()
	{
		return m_nodeHasValue;
	}

	void SetBranch(
		const Nibble& nibble,
		std::unique_ptr<Node> other
	)
	{
		int nibbleInt = static_cast<int>(nibble);
		m_branches[nibbleInt] = std::move(other);
	}

	std::unique_ptr<Node>& GetBranch(Nibble nibble)
	{
		int nibbleInt = static_cast<int>(nibble);
		return m_branches[nibbleInt];
	}

	void RemoveBranch(const Nibble& nibble)
	{
		int nibbleInt = static_cast<int>(nibble);
		m_branches[nibbleInt].reset();
	}

	void SetValue(Internal::Obj::Bytes otherValue)
	{
		m_nodeHasValue = true;
		m_value = std::move(otherValue);
	}

	void RemoveValue()
	{
		m_nodeHasValue = false;
		m_value.resize(0);

	}

	std::vector <uint8_t> Serialize()
	{
		NodeBase* BasePtr = static_cast<NodeBase*>(this);
		return NodeHelper::Serialize(BasePtr);
	}

	virtual NodeType GetNodeType() const override
	{
		return NodeType::Branch;
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
		hashes.resize(sk_numNodes + 1);

		for (uint8_t i = 0; i < sk_numNodes; i++)
		{
			if (!m_branches[i])
			{
				hashes[i] = EmptyNode::EmptyNodeRaw();
			}
			else
			{
				// Node* NodePtr = m_branches[i].get();
				std::unique_ptr<NodeBase>& NodeBasePtr = m_branches[i]->GetNodeBase();
				NodeBase* NodePtr = NodeBasePtr.get();
				std::vector<uint8_t> serialized = NodeHelper::Serialize(NodePtr);

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

		hashes[sk_numNodes] = m_value;
		return hashes;
	}

private:

	bool m_nodeHasValue;
	std::vector<std::unique_ptr<Node> > m_branches;
	Internal::Obj::Bytes m_value;

}; // class BranchNode

} // namespace Trie
} // namespace Eth
} // namespace EclipseMonitor
