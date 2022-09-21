// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/EthValidator.hpp>

#include "EthHistHdr_0_100.hpp"
#include "EthHistHdr_Malformed.hpp"

namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor;
using namespace EclipseMonitor_Test;

GTEST_TEST(TestEthValidator, CountTestFile)
{
	static auto tmp = ++g_numOfTestFile;
	(void)tmp;
}

GTEST_TEST(TestEthValidator, Block0_1)
{
	std::vector<uint8_t> input0 = GetEthHistHdr_0_100()[0];
	EthHeaderMgr header0(input0, 0);

	std::vector<uint8_t> input1 = GetEthHistHdr_0_100()[1];
	EthHeaderMgr header1(input1, 0);

	std::unique_ptr<EthMainnetDAA> daa(new EthMainnetDAA);
	EthValidator validator(std::move(daa));
	EXPECT_TRUE(validator.CommonValidate(header0, header1, false));
}

GTEST_TEST(TestEthValidator, Block0_1_ErrNum)
{
	std::vector<uint8_t> input0 = GetEthHistHdr_0_100()[0];
	EthHeaderMgr header0(input0, 0);

	std::vector<uint8_t> input1 = GetEthHeaderBin_1_ErrNum();
	EthHeaderMgr header1(input1, 0);

	std::unique_ptr<EthMainnetDAA> daa(new EthMainnetDAA);
	EthValidator validator(std::move(daa));
	EXPECT_FALSE(validator.CommonValidate(header0, header1, false));
}

GTEST_TEST(TestEthValidator, Block0_1_ErrParentHash)
{
	std::vector<uint8_t> input0 = GetEthHistHdr_0_100()[0];
	EthHeaderMgr header0(input0, 0);

	std::vector<uint8_t> input1 = GetEthHeaderBin_1_ErrParentHash();
	EthHeaderMgr header1(input1, 0);

	std::unique_ptr<EthMainnetDAA> daa(new EthMainnetDAA);
	EthValidator validator(std::move(daa));
	EXPECT_FALSE(validator.CommonValidate(header0, header1, false));
}

GTEST_TEST(TestEthValidator, Block0_1_ErrDiffVal)
{
	std::vector<uint8_t> input0 = GetEthHistHdr_0_100()[0];
	EthHeaderMgr header0(input0, 0);

	std::vector<uint8_t> input1 = GetEthHeaderBin_1_ErrDiffVal();
	EthHeaderMgr header1(input1, 0);

	std::unique_ptr<EthMainnetDAA> daa(new EthMainnetDAA);
	EthValidator validator(std::move(daa));
	EXPECT_FALSE(validator.CommonValidate(header0, header1, false));
}
