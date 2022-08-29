// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/EthValidator.hpp>

#include "EthHeaders.hpp"

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
	std::vector<uint8_t> input0(
		GetEthHeaderBin_0().begin(),
		GetEthHeaderBin_0().end());
	EthHeaderMgr header0(input0, 0);

	std::vector<uint8_t> input1(
		GetEthHeaderBin_1().begin(),
		GetEthHeaderBin_1().end());
	EthHeaderMgr header1(input1, 0);

	std::unique_ptr<EthMainnetDAA> daa(new EthMainnetDAA);
	EthValidator validator(std::move(daa));
	EXPECT_TRUE(validator.CommonValidate(header0, header1));
}

GTEST_TEST(TestEthValidator, Block0_1_ErrNum)
{
	std::vector<uint8_t> input0(
		GetEthHeaderBin_0().begin(),
		GetEthHeaderBin_0().end());
	EthHeaderMgr header0(input0, 0);

	std::vector<uint8_t> input1(
		GetEthHeaderBin_1_ErrNum().begin(),
		GetEthHeaderBin_1_ErrNum().end());
	EthHeaderMgr header1(input1, 0);

	std::unique_ptr<EthMainnetDAA> daa(new EthMainnetDAA);
	EthValidator validator(std::move(daa));
	EXPECT_FALSE(validator.CommonValidate(header0, header1));
}

GTEST_TEST(TestEthValidator, Block0_1_ErrParentHash)
{
	std::vector<uint8_t> input0(
		GetEthHeaderBin_0().begin(),
		GetEthHeaderBin_0().end());
	EthHeaderMgr header0(input0, 0);

	std::vector<uint8_t> input1(
		GetEthHeaderBin_1_ErrParentHash().begin(),
		GetEthHeaderBin_1_ErrParentHash().end());
	EthHeaderMgr header1(input1, 0);

	std::unique_ptr<EthMainnetDAA> daa(new EthMainnetDAA);
	EthValidator validator(std::move(daa));
	EXPECT_FALSE(validator.CommonValidate(header0, header1));
}

GTEST_TEST(TestEthValidator, Block0_1_ErrDiffVal)
{
	std::vector<uint8_t> input0(
		GetEthHeaderBin_0().begin(),
		GetEthHeaderBin_0().end());
	EthHeaderMgr header0(input0, 0);

	std::vector<uint8_t> input1(
		GetEthHeaderBin_1_ErrDiffVal().begin(),
		GetEthHeaderBin_1_ErrDiffVal().end());
	EthHeaderMgr header1(input1, 0);

	std::unique_ptr<EthMainnetDAA> daa(new EthMainnetDAA);
	EthValidator validator(std::move(daa));
	EXPECT_FALSE(validator.CommonValidate(header0, header1));
}
