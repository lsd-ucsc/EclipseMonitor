// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "EthHistHdr_Malformed.hpp"

#include "EthHistHdr_0_100.hpp"

static std::vector<uint8_t> BuildEthHeaderBin_1_ErrNum()
{
	std::vector<uint8_t> inst = EclipseMonitor_Test::GetEthHistHdr_0_100()[1];
	inst[454] = 0x02U; // The single byte of the number
	return inst;
} // BuildEthHeaderBin_1_ErrNum()

const std::vector<uint8_t>& EclipseMonitor_Test::GetEthHeaderBin_1_ErrNum()
{
	static const std::vector<uint8_t> inst = BuildEthHeaderBin_1_ErrNum();

	return inst;
} // GetEthHeaderBin_1_ErrNum()

static std::vector<uint8_t> BuildEthHeaderBin_1_ErrParentHash()
{
	std::vector<uint8_t> inst = EclipseMonitor_Test::GetEthHistHdr_0_100()[1];
	inst[4] = 0x00U; // first byte of the hash
	return inst;
} // BuildEthHeaderBin_1_ErrParentHash()

const std::vector<uint8_t>& EclipseMonitor_Test::GetEthHeaderBin_1_ErrParentHash()
{
	static const std::vector<uint8_t> inst = BuildEthHeaderBin_1_ErrParentHash();

	return inst;
} // GetEthHeaderBin_1_ErrParentHash()

static std::vector<uint8_t> BuildEthHeaderBin_1_ErrDiffVal()
{
	std::vector<uint8_t> inst = EclipseMonitor_Test::GetEthHistHdr_0_100()[1];
	inst[453] = 0x01U; // last byte of diff val
	return inst;
} // BuildEthHeaderBin_1_ErrDiffVal()

const std::vector<uint8_t>& EclipseMonitor_Test::GetEthHeaderBin_1_ErrDiffVal()
{
	static const std::vector<uint8_t> inst = BuildEthHeaderBin_1_ErrDiffVal();

	return inst;
} // GetEthHeaderBin_1_ErrDiffVal()
