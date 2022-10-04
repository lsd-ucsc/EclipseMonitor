// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/Eth/DataTypes.hpp>


namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor_Test;

using namespace EclipseMonitor::Eth;


GTEST_TEST(TestEthDataTypes, CountTestFile)
{
	static auto tmp = ++g_numOfTestFile;
	(void)tmp;
}

GTEST_TEST(TestEthDataTypes, BlkNum)
{
	{
		SimpleRlp::BytesObjType input = { };
		auto output = BlkNumTypeTrait::FromBytes(input);
		EXPECT_EQ(output, 0ULL);
	}
	{
		SimpleRlp::BytesObjType input = { 0x01U, };
		auto output = BlkNumTypeTrait::FromBytes(input);
		EXPECT_EQ(output, 1ULL);
	}
}

GTEST_TEST(TestEthDataTypes, TimeStamp)
{
	{
		SimpleRlp::BytesObjType input = { };
		auto output = TimeTypeTrait::FromBytes(input);
		EXPECT_EQ(output, 0ULL);
	}
	{
		SimpleRlp::BytesObjType input = { 0X55U, 0XBAU, 0X42U, 0X24U, };
		auto output = TimeTypeTrait::FromBytes(input);
		EXPECT_EQ(output, 1438269988ULL);
	}
}

GTEST_TEST(TestEthDataTypes, DiffVal)
{
	{
		SimpleRlp::BytesObjType input = { 0X04U, 0X00U, 0X00U, 0X00U, 0X00U, };
		auto output = DiffTypeTrait::FromBytes(input);
		EXPECT_EQ(output, 17179869184ULL);
	}
	{
		SimpleRlp::BytesObjType input = { 0X03U, 0XFFU, 0X80U, 0X00U, 0X00U, };
		auto output = DiffTypeTrait::FromBytes(input);
		EXPECT_EQ(output, 17171480576ULL);
	}
}
