// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <algorithm>

#include "Internal/SimpleObj.hpp"
#include "Internal/SimpleRlp.hpp"

#include "EthDataTypes.hpp"
#include "EthKeccak256.hpp"

namespace EclipseMonitor
{

class EthHeaderMgr
{
public: // static member

	using RawHeaderType = Internal::Rlp::EthHeader;
	using RawHeaderParser = Internal::Rlp::EthHeaderParser;
	using BlkNumType = typename EthBlkNumTypeTrait::value_type;
	using TimeType = typename EthTimeTypeTrait::value_type;
	using DiffType = typename EthDiffTypeTrait::value_type;

	using BytesObjType = Internal::Rlp::BytesObjType;

	static const BytesObjType& GetEmptyUncleHash()
	{
		static const BytesObjType inst({
			0X1DU, 0XCCU, 0X4DU, 0XE8U, 0XDEU, 0XC7U, 0X5DU, 0X7AU, 0XABU, 0X85U,
			0XB5U, 0X67U, 0XB6U, 0XCCU, 0XD4U, 0X1AU, 0XD3U, 0X12U, 0X45U, 0X1BU,
			0X94U, 0X8AU, 0X74U, 0X13U, 0XF0U, 0XA1U, 0X42U, 0XFDU, 0X40U, 0XD4U,
			0X93U, 0X47U,
		});
		return inst;
	}

public:

	EthHeaderMgr(const std::vector<uint8_t>& rawBinary, uint64_t trustedTime) :
		m_rawHeader(RawHeaderParser().Parse(rawBinary)),
		m_trustedTime(trustedTime),
		m_hash(EthKeccak256(rawBinary)),
		m_hashObj(m_hash.begin(), m_hash.end()),
		m_blkNum(EthBlkNumTypeTrait::FromBytes(m_rawHeader.get_Number())),
		m_time(EthTimeTypeTrait::FromBytes(m_rawHeader.get_Timestamp())),
		m_diff(EthDiffTypeTrait::FromBytes(m_rawHeader.get_Difficulty())),
		m_hasUncle(m_rawHeader.get_Sha3Uncles() != GetEmptyUncleHash())
	{}

	~EthHeaderMgr() = default;

	const RawHeaderType& GetRawHeader() const
	{
		return m_rawHeader;
	}

	uint64_t GetTrustedTime() const
	{
		return m_trustedTime;
	}

	const std::array<uint8_t, 32>& GetHash() const
	{
		return m_hash;
	}

	const Internal::Obj::Bytes& GetHashObj() const
	{
		return m_hashObj;
	}

	const BlkNumType& GetNumber() const
	{
		return m_blkNum;
	}

	const TimeType& GetTime() const
	{
		return m_time;
	}

	const DiffType& GetDiff() const
	{
		return m_diff;
	}

	bool HasUncle() const
	{
		return m_hasUncle;
	}

private:

	RawHeaderType m_rawHeader;
	uint64_t m_trustedTime;
	std::array<uint8_t, 32> m_hash;
	Internal::Obj::Bytes m_hashObj;
	BlkNumType m_blkNum;
	TimeType m_time;
	DiffType m_diff;
	bool m_hasUncle;
}; // class EthHeaderMgr

} // namespace EclipseMonitor
