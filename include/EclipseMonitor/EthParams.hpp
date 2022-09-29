// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include "EthDataTypes.hpp"

namespace EclipseMonitor
{

struct EthParams
{
	// Reference: https://github.com/ethereum/go-ethereum/blob/master/consensus/ethash/difficulty.go
	// Reference: https://github.com/ethereum/go-ethereum/blob/master/params/protocol_params.go

	static constexpr int GetDifficultyBoundDivisorBitShift()
	{
		return 11;
	}

	static constexpr uint64_t GetMinimumDifficulty()
	{
		return 131072;
	}

	static constexpr uint64_t GetDurationLimit()
	{
		return 13;
	}

}; // struct EthParams


struct EthMainnetConfig
{
public: // static members:

	using BlkNumType = typename EthBlkNumTypeTrait::value_type;

	static const BlkNumType& GetParisBlkNum()
	{
		// https://github.com/ethereum/execution-specs/blob/master/network-upgrades/mainnet-upgrades/paris.md
		static const BlkNumType blkNum(15537394UL);
		return blkNum;
	}

	static const BlkNumType& GetGrayGlacierBlkNum()
	{
		static const BlkNumType blkNum(15050000UL);
		return blkNum;
	}

	static const BlkNumType& GetArrowGlacierBlkNum()
	{
		static const BlkNumType blkNum(13773000UL);
		return blkNum;
	}

	static const BlkNumType& GetLondonBlkNum()
	{
		static const BlkNumType blkNum(12965000UL);
		return blkNum;
	}

	static const BlkNumType& GetMuirGlacierBlkNum()
	{
		static const BlkNumType blkNum(9200000UL);
		return blkNum;
	}

	static const BlkNumType& GetConstantinopleBlkNum()
	{
		static const BlkNumType blkNum(7280000UL);
		return blkNum;
	}

	static const BlkNumType& GetByzantiumBlkNum()
	{
		static const BlkNumType blkNum(4370000UL);
		return blkNum;
	}

	static const BlkNumType& GetHomesteadBlkNum()
	{
		static const BlkNumType blkNum(1150000UL);
		return blkNum;
	}
}; // struct EthMainnetConfig

} // namespace EclipseMonitor
