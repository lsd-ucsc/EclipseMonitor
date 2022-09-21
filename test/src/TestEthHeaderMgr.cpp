// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/EthHeaderMgr.hpp>

#include "EthHistHdr_0_100.hpp"

namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor;
using namespace EclipseMonitor_Test;

GTEST_TEST(TestEthHeaderMgr, CountTestFile)
{
	static auto tmp = ++g_numOfTestFile;
	(void)tmp;
}

GTEST_TEST(TestEthHeaderMgr, Header_0)
{
	std::vector<uint8_t> input = GetEthHistHdr_0_100()[0];
	uint64_t trustedTime = 1234567890;
	EthHeaderMgr header(input, trustedTime);

	// Some data in raw header
	auto expUncleHash = SimpleObjects::Bytes({
		0X1DU, 0XCCU, 0X4DU, 0XE8U, 0XDEU, 0XC7U, 0X5DU, 0X7AU, 0XABU, 0X85U,
		0XB5U, 0X67U, 0XB6U, 0XCCU, 0XD4U, 0X1AU, 0XD3U, 0X12U, 0X45U, 0X1BU,
		0X94U, 0X8AU, 0X74U, 0X13U, 0XF0U, 0XA1U, 0X42U, 0XFDU, 0X40U, 0XD4U,
		0X93U, 0X47U,
	});
	EXPECT_EQ(
		header.GetRawHeader().get_Sha3Uncles(),
		expUncleHash);

	// trusted timestamp
	EXPECT_EQ(header.GetTrustedTime(), trustedTime);

	// Hash of the header itself
	auto expHash = std::array<uint8_t, 32>({
		0xd4, 0xe5, 0x67, 0x40, 0xf8, 0x76, 0xae, 0xf8, 0xc0, 0x10,
		0xb8, 0x6a, 0x40, 0xd5, 0xf5, 0x67, 0x45, 0xa1, 0x18, 0xd0,
		0x90, 0x6a, 0x34, 0xe6, 0x9a, 0xec, 0x8c, 0x0d, 0xb1, 0xcb,
		0x8f, 0xa3
	});
	EXPECT_EQ(
		header.GetHash(),
		expHash);
	auto expHashObj = SimpleObjects::Bytes(expHash.begin(), expHash.end());
	EXPECT_EQ(
		header.GetHashObj(),
		expHashObj);

	// Block number
	EXPECT_EQ(
		header.GetNumber(),
		typename EthHeaderMgr::BlkNumType(0));

	// Timestamp
	EXPECT_EQ(
		header.GetTime(),
		typename EthHeaderMgr::TimeType(0));

	// Difficulty
	EXPECT_EQ(
		header.GetDiff(),
		typename EthHeaderMgr::DiffType(17179869184ULL));
}
