// Copyright (c) 2023 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once


#include <cstddef>
#include <cstdint>

#include <array>
#include <vector>

#include "../Internal/SimpleObj.hpp"
#include "../Internal/SimpleRlp.hpp"
#include "EventDescription.hpp"
#include "Receipt.hpp"
#include "Trie/Trie.hpp"


namespace EclipseMonitor
{
namespace Eth
{


class ReceiptsMgr
{
public: // static members


	using ReceiptListType = std::vector<Receipt>;


public:

	ReceiptsMgr(const SimpleObjects::ListBaseObj& receipts) :
		m_receipts(),
		m_rootHashBytes()
	{
		using _IntWriter = Internal::Rlp::EncodePrimitiveIntValue<
			uint64_t,
			Internal::Rlp::Endian::native,
			false
		>;
		using _KeyRlpWriter =
			Internal::Rlp::WriterBytesImpl<std::vector<uint8_t> >;

		m_receipts.reserve(receipts.size());

		Trie::PatriciaTrie trie;
		size_t i = 0;
		SimpleObjects::Bytes keyBigEndian;
		keyBigEndian.reserve(8); // size_t usually is at most 8 bytes
		for (const auto& receipt : receipts)
		{
			// 1. trie
			keyBigEndian.resize(0);
			_IntWriter::Encode(i, std::back_inserter(keyBigEndian));
			std::vector<uint8_t> keyRlp = _KeyRlpWriter::Write(keyBigEndian);
			trie.Put(keyRlp, receipt.AsBytes());

			// 2. receipt list
			m_receipts.emplace_back(Receipt::FromBytes(receipt.AsBytes()));

			++i;
		}

		m_rootHashBytes = trie.Hash();
	}


	const Internal::Obj::Bytes& GetRootHashBytes() const
	{
		return m_rootHashBytes;
	}


private:

	ReceiptListType m_receipts;
	Internal::Obj::Bytes m_rootHashBytes;

}; // class ReceiptsMgr


} // namespace Eth
} // namespace EclipseMonitor