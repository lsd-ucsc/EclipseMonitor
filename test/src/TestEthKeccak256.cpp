// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/EthKeccak256.hpp>

#include "EthHistHdr_0_100.hpp"

namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor;
using namespace EclipseMonitor_Test;

GTEST_TEST(TestEthKeccak256, CountTestFile)
{
	static auto tmp = ++EclipseMonitor_Test::g_numOfTestFile;
	(void)tmp;
}

GTEST_TEST(TestEthKeccak256, EmptyCalculation)
{
	std::vector<uint8_t> input;
	std::array<uint8_t, 32> expOut = {
		0xc5, 0xd2, 0x46, 0x01, 0x86, 0xf7, 0x23, 0x3c, 0x92, 0x7e,
		0x7d, 0xb2, 0xdc, 0xc7, 0x03, 0xc0, 0xe5, 0x00, 0xb6, 0x53,
		0xca, 0x82, 0x27, 0x3b, 0x7b, 0xfa, 0xd8, 0x04, 0x5d, 0x85,
		0xa4, 0x70
	};

	auto actOut = EthKeccak256(input);
	EXPECT_EQ(actOut, expOut);
}

GTEST_TEST(TestEthKeccak256, GenesisBlkCalculation)
{
	std::vector<uint8_t> input = GetEthHistHdr_0_100()[0];

	std::array<uint8_t, 32> expOut = {
		0xd4, 0xe5, 0x67, 0x40, 0xf8, 0x76, 0xae, 0xf8, 0xc0, 0x10,
		0xb8, 0x6a, 0x40, 0xd5, 0xf5, 0x67, 0x45, 0xa1, 0x18, 0xd0,
		0x90, 0x6a, 0x34, 0xe6, 0x9a, 0xec, 0x8c, 0x0d, 0xb1, 0xcb,
		0x8f, 0xa3
	};

	auto actOut = EthKeccak256(input);
	EXPECT_EQ(actOut, expOut);
}
