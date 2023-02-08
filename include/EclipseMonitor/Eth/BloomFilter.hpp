// Copyright (c) 2023 Tuan Tran, Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once


#include <vector>

#include "../Exceptions.hpp"
#include "../Internal/SimpleObj.hpp"
#include "Keccak256.hpp"


namespace EclipseMonitor
{
namespace Eth
{

class BloomFilter
{
public: // static members:

	static constexpr size_t sk_bloomBitSize = 2048;
	static constexpr size_t sk_bloomByteSize = sk_bloomBitSize / 8;

public:

	BloomFilter(const Internal::Obj::Bytes& bloomBytes) :
		m_bloomBeginPtr(CheckBloomBytes(bloomBytes))
	{}


	~BloomFilter() = default;


	bool CheckBloomBits(
		const std::array<uint8_t, 32>& hashedData
	) const
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

		bool inBloom = (
			(m_bloomBeginPtr[i1] & v1) &&
			(m_bloomBeginPtr[i2] & v2) &&
			(m_bloomBeginPtr[i3] & v3)
		);

		return inBloom;
	}


	template< typename... _ArgTs >
	bool IsHashInBloom(const _ArgTs&... hashes) const
	{
		for (const auto& hash : { hashes... })
		{
			if (!CheckBloomBits(hash))
			{
				return false;
			}
		}

		return true;
	}


	template< typename... _ArgTs >
	bool IsEventInBloom(const _ArgTs&... eventData) const
	{
		return IsHashInBloom(
			Keccak256(eventData)...
		);
	}


private:


	static const uint8_t* CheckBloomBytes(
		const Internal::Obj::Bytes& bloomBytes
	)
	{
		if (bloomBytes.size() != sk_bloomByteSize)
		{
			throw Exception("Invalid bloom bytes size");
		}

		return bloomBytes.data();
	}


	const uint8_t* m_bloomBeginPtr;

}; // class BloomFilter

} // namespace Eth
} // namespace EclipseMonitor
