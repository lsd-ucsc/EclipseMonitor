// Copyright (c) 2023 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <vector>

#include "../Internal/SimpleObj.hpp"
#include "../Internal/SimpleRlp.hpp"


namespace EclipseMonitor
{
namespace Eth
{

class BloomFilter
{

public:
	using RawHeaderType = Internal::Rlp::EthHeader;
	using RawHeaderParser = Internal::Rlp::EthHeaderParser;

	bool CheckBloomBits(
		const std::array<uint8_t, 32>& hashedData
	)
	{
		// Adapted from: https://github.com/noxx3xxon/evm-by-example
		uint8_t v1 = (1 << (hashedData[1] & 0x7));
		uint8_t v2 = (1 << (hashedData[3] & 0x7));
		uint8_t v3 = (1 << (hashedData[5] & 0x7));

		uint16_t i1 = hashedData[0] << 8 | hashedData[1];
		i1 = (i1 & 0x7FF) >> 3;
		i1 = (256 - i1 - 1);

		uint16_t i2 = hashedData[2] << 8 | hashedData[3];
		i2 = (i2 & 0x7FF) >> 3;
		i2 = (256 - i2 - 1);

		uint16_t i3 = hashedData[4] << 8 | hashedData[5];
		i3 = (i3 & 0x7FF) >> 3;
		i3 = (256 - i3 - 1);

		bool inBloom = (m_bloom[i1] & v1) && (m_bloom[i2] & v2) && (m_bloom[i3] & v3);

		return inBloom;
	}

	template< typename... Arguments >
	bool IsEventInBloom(const Arguments&... eventData)
	{
		for (const auto& data : { eventData... })
		{
			if (!CheckBloomBits(data))
			{
				return false;
			}
		}

		return true;
	}


public:
	BloomFilter(const std::vector<uint8_t>& headerRlp) :
		m_rawHeader(RawHeaderParser().Parse(headerRlp)),
		m_bloom(
			m_rawHeader.get_LogsBloom().begin(),
			m_rawHeader.get_LogsBloom().end()
		)
	{}

	const RawHeaderType& GetRawHeader() const
	{
		return m_rawHeader;
	}

	const Internal::Obj::Bytes& GetBloom() const
	{
		return m_bloom;
	}

private:
	RawHeaderType m_rawHeader;
	Internal::Obj::Bytes m_bloom;
};

} // namespace Eth
} // namespace EclipseMonitor
