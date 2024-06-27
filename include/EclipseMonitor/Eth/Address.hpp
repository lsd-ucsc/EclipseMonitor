// Copyright (c) 2024 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once


#include <string>

#include <SimpleObjects/Codec/Hex.hpp>

#include "../Exceptions.hpp"
#include "../Internal/SimpleObj.hpp"
#include "DataTypes.hpp"
#include "Keccak256.hpp"


namespace EclipseMonitor
{
namespace Eth
{


class Address
{
public: // static members:

	using value_type = ContractAddr;

	static constexpr size_t sk_sizeBytes = 20;

public:

	Address(const value_type& addr) :
		m_addr(addr)
	{}

	explicit Address(const std::string& addr) :
		m_addr()
	{
		size_t expLen = sk_sizeBytes * 2;
		auto begin = addr.begin();

		// check if the string begins with "0x"
		if (
			(addr.size() >= 2) && // at least 2 characters
			((addr[0] == '0') && (addr[1] == 'x')) // starts with "0x"
		)
		{
			begin += 2;
			expLen += 2;
		}

		// check if the string is of the correct length
		if (addr.size() != expLen)
		{
			throw Exception(
				"The given ETH address hex string is of incorrect length"
			);
		}

		Internal::Obj::Codec::Hex::Decode(m_addr.begin(), begin, addr.end());
	}

	Address(const Address& addr) :
		m_addr(addr.m_addr)
	{}

	Address(Address&& addr) :
		m_addr(std::move(addr.m_addr))
	{}

	~Address() = default;

	bool operator==(const Address& addr) const
	{
		return m_addr == addr.m_addr;
	}

	bool operator!=(const Address& addr) const
	{
		return m_addr != addr.m_addr;
	}

	std::string ToString(const std::string& prefix = "0x") const
	{
		// std::array<uint8_t, 20> should generate a hex string of length 40
		std::string hexLower =
			Internal::Obj::Codec::Hex::Encode<std::string>(m_addr, "");
		// std::array<uint8_t, 20> should generate a hex string of length 40
		std::string hexUpper =
			Internal::Obj::Codec::HEX::Encode<std::string>(m_addr, "");

		// the checksummed address that is going to be generated
		std::string checksummed = prefix;

		// The result of a 256-bit hash should have 32 bytes
		auto addrHash = Keccak256(hexLower);

		for (size_t i = 0; i < hexLower.size(); ++i)
		{
			// if `i` is even, the nibble is the higher 4-bit of the byte
			//     e.g., (0 % 2) = 0, (2 % 2) = 0, (4 % 2) = 0, ...
			//     1 - (i % 2) = 1 - 0 = 1
			//     (1 - (i % 2)) * 4 = 1 * 4 = 4
			// if `i` is odd, the nibble is the lower 4-bit of the byte
			//     e.g., (1 % 2) = 1, (3 % 2) = 1, (5 % 2) = 1, ...
			//     1 - (i % 2) = 1 - 1 = 0
			//     (1 - (i % 2)) * 4 = 0 * 4 = 0
			uint8_t rightShift = (1 - (i % 2)) * 4;
			uint8_t hashByte   = addrHash[i / 2];
			uint8_t hashNibble = (hashByte >> rightShift) & 0x0FU;

			// if the nibble is greater than 7, the hex should be upper case
			// otherwise, the hex should be lower case
			char hexCh = hashNibble > 7 ? hexUpper[i] : hexLower[i];

			checksummed.push_back(hexCh);
		}

		return checksummed;
	}


private: // members:

	value_type m_addr;
}; // class Address


} // namespace Eth
} // namespace EclipseMonitor

