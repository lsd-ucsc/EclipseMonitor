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

} // namespace EclipseMonitor
