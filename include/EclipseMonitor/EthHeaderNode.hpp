// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <memory>
#include <vector>

#include "Internal/SimpleObj.hpp"

#include "Exceptions.hpp"
#include "EthHeaderMgr.hpp"

namespace EclipseMonitor
{

class EthHeaderNode;

class EthHeaderNode
{
public: // static members:
	struct ChildInfo
	{
		/**
		 * @brief Number of descendants that *this* child has
		 *
		 */
		size_t m_numOfDesc;
		std::unique_ptr<EthHeaderNode> m_child;

		ChildInfo(
			size_t numOfDesc,
			std::unique_ptr<EthHeaderNode> child) :
			m_numOfDesc(numOfDesc),
			m_child(std::move(child))
		{}
	};

public:
	EthHeaderNode(std::unique_ptr<EthHeaderMgr> header) :
		m_children(),
		m_parent(nullptr),
		m_header(std::move(header)),
		m_isLive(false)
	{}

	~EthHeaderNode() = default;

	EthHeaderNode* AddChild(std::unique_ptr<EthHeaderMgr> childHeader)
	{
		auto child = Internal::Obj::Internal::make_unique<EthHeaderNode>(
			std::move(childHeader));
		EthHeaderNode* childPtr = child.get();

		// if parent is a live node (passed the sync phase),
		// then so do its children
		child->m_isLive = m_isLive;

		// link child's parent
		child->m_parent = this;
		// append child to children list AND set number of descendants to 0
		m_children.emplace_back(0, std::move(child));
		// !!! NOTE: child is invalid after this point !!!

		// Inform all ancestors that a new descendants has been added
		if (m_parent != nullptr)
		{
			m_parent->AddDescCount(this);
		}

		return childPtr;
	}

	const EthHeaderMgr& GetHeader() const
	{
		if (m_header == nullptr)
		{
			throw Exception("Header is already released");
		}
		return *m_header;
	}

	std::unique_ptr<EthHeaderMgr> ReleaseHeader()
	{
		return std::move(m_header);
	}

	std::unique_ptr<EthHeaderNode> ReleaseChildHasNDesc(
		size_t numOfDesc)
	{
		// find the child in the children list
		auto it = std::find_if(m_children.begin(), m_children.end(),
			[&](const ChildInfo& childInfo) -> bool
			{
				return childInfo.m_numOfDesc >= numOfDesc;
			});

		if (it == m_children.end())
		{
			return nullptr;
		}

		// remove the child from the children list
		std::unique_ptr<EthHeaderNode> child = std::move(it->m_child);
		m_children.erase(it);

		return child;
	}

	size_t GetNumOfChildren() const
	{
		return m_children.size();
	}

protected:

	void AddDescCount(EthHeaderNode* childPtr)
	{
		// ensure that the child is a child of this node
		if (childPtr->m_parent != this)
		{
			throw Exception(
				"The given node pointer is not a child of this node");
		}

		// find the child in the children list
		auto it = std::find_if(m_children.begin(), m_children.end(),
			[&](const ChildInfo& childInfo) -> bool
			{
				return childInfo.m_child.get() == childPtr;
			});

		if (it == m_children.end())
		{
			// This should never happen, unless we have implementation error
			throw Exception(
				"The given node pointer is not a child of this node");
		}

		// increment the number of descendants
		++(it->m_numOfDesc);

		// Inform all ancestors that a new descendants has been added
		if (m_parent != nullptr)
		{
			m_parent->AddDescCount(this);
		}
	}


private:
	std::vector<ChildInfo> m_children;
	EthHeaderNode* m_parent;
	std::unique_ptr<EthHeaderMgr> m_header;
	bool m_isLive;

}; // class EthHeaderNode

} // namespace EclipseMonitor
