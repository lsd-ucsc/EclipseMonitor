// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include "../EthKeccak256.hpp"
#include "../Trie/Nibbles.hpp"
#include "../Trie/TrieNode.hpp"
#include "SimpleObjects/Internal/make_unique.hpp"

namespace EclipseMonitor
{

namespace Trie
{

class BranchNode : public NodeBase
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
				   std::unique_ptr<Node> other)
	{
		int nibbleInt = static_cast<int>(nibble);
		Branches[nibbleInt] = std::move(other);
	}

	std::unique_ptr<Node>& GetBranch(Nibble nibble)
	{
		int nibbleInt = static_cast<int>(nibble);
		return Branches[nibbleInt];
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


	std::vector <uint8_t> Serialize()
	{
		NodeBase* BasePtr = static_cast<NodeBase*>(this);
		return NodeHelper::Serialize(BasePtr);
	}

	virtual NodeType GetNodeType() const override
	{
		return NodeType::Branch;
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
				hashes[i] = EmptyNode::EmptyNodeRaw();
			}
			else
			{
				// Node* NodePtr = Branches[i].get();
				std::unique_ptr<NodeBase>& NodeBasePtr = Branches[i]->GetNodeBase();
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

		hashes[NumNodes] = Value;
		return hashes;
	}

private:

	bool NodeHasValue;
	const uint8_t NumNodes = 16;

	std::vector<std::unique_ptr<Node>> Branches;
	SimpleObjects::Bytes Value;
}; // class BranchNode

} // namespace Branch
} // namespace EclipseMonitor
