// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>
#include <string>

#include <EclipseMonitor/Eth/Abi/AbiParam.hpp>
#include <EclipseMonitor/Eth/Abi/AbiParamParser.hpp>

namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor_Test;
using namespace EclipseMonitor::Eth::Abi;

GTEST_TEST(TestEthAbiParamTypeParser, CountTestFile)
{
	static auto tmp = ++g_numOfTestFile;
	(void)tmp;
}

GTEST_TEST(TestEthAbiParamTypeParser, ParseSingleUint256)
{
	std::string funcSig = "function foo(uint64 num) public";
	auto params = AbiParamType::ParseParamType(funcSig);

	AbiParamType funcParam = *params[0];
	EXPECT_EQ(funcParam.GetType(), ParamType::Uint64);
	EXPECT_EQ(funcParam.IsStatic(), true);
	EXPECT_EQ(funcParam.IsArray(), false);
}

GTEST_TEST(TestEthAbiParamTypeParser, ParseSingleBool)
{
	std::string funcSig = "function bar(bool isFoo) public";
	auto params = AbiParamType::ParseParamType(funcSig);

	AbiParamType funcParam = *params[0];
	EXPECT_EQ(funcParam.GetType(), ParamType::Bool);
	EXPECT_EQ(funcParam.IsStatic(), true);
	EXPECT_EQ(funcParam.IsArray(), false);
}

GTEST_TEST(TestEthAbiParamTypeParser, ParseSingleBytes)
{
	std::string funcSig = "function baz(bytes data) public";
	auto params = AbiParamType::ParseParamType(funcSig);

	AbiParamType funcParam = *params[0];
	EXPECT_EQ(funcParam.GetType(), ParamType::Bytes);
	EXPECT_EQ(funcParam.IsStatic(), false);
	EXPECT_EQ(funcParam.IsArray(), false);
}

GTEST_TEST(TestEthAbiParamTypeParser, ParseSingleBytes32)
{
	std::string funcSig = "function alpha(bytes32 data) public";
	auto params = AbiParamType::ParseParamType(funcSig);

	AbiParamType funcParam = *params[0];
	EXPECT_EQ(funcParam.GetType(), ParamType::Bytes32);
	EXPECT_EQ(funcParam.IsStatic(), true);
	EXPECT_EQ(funcParam.IsArray(), false);
}

GTEST_TEST(TestEthAbiParamTypeParser, ParseMultipleUint)
{
	std::string funcSig = "function foo(uint64 num1, uint64 num2) public";
	auto params = AbiParamType::ParseParamType(funcSig);

	AbiParamType funcParam1 = *params[0];
	EXPECT_EQ(funcParam1.GetType(), ParamType::Uint64);
	EXPECT_EQ(funcParam1.IsStatic(), true);
	EXPECT_EQ(funcParam1.IsArray(), false);

	AbiParamType funcParam2 = *params[1];
	EXPECT_EQ(funcParam2.GetType(), ParamType::Uint64);
	EXPECT_EQ(funcParam2.IsStatic(), true);
	EXPECT_EQ(funcParam2.IsArray(), false);
}

GTEST_TEST(TestEthAbiParamTypeParser, ParseMultipleBool)
{
	std::string funcSig = "function bar(bool isFoo, bool isBar) public";
	auto params = AbiParamType::ParseParamType(funcSig);

	AbiParamType funcParam1 = *params[0];
	EXPECT_EQ(funcParam1.GetType(), ParamType::Bool);
	EXPECT_EQ(funcParam1.IsStatic(), true);
	EXPECT_EQ(funcParam1.IsArray(), false);

	AbiParamType funcParam2 = *params[1];
	EXPECT_EQ(funcParam2.GetType(), ParamType::Bool);
	EXPECT_EQ(funcParam2.IsStatic(), true);
	EXPECT_EQ(funcParam2.IsArray(), false);
}

GTEST_TEST(TestEthAbiParamTypeParser, ParseMultipleBytes)
{
	std::string funcSig = "function baz(bytes data1, bytes data2) public";
	auto params = AbiParamType::ParseParamType(funcSig);

	AbiParamType funcParam1 = *params[0];
	EXPECT_EQ(funcParam1.GetType(), ParamType::Bytes);
	EXPECT_EQ(funcParam1.IsStatic(), false);
	EXPECT_EQ(funcParam1.IsArray(), false);

	AbiParamType funcParam2 = *params[1];
	EXPECT_EQ(funcParam2.GetType(), ParamType::Bytes);
	EXPECT_EQ(funcParam2.IsStatic(), false);
	EXPECT_EQ(funcParam2.IsArray(), false);
}

GTEST_TEST(TestEthAbiParamTypeParser, ParseMultipleBytes32)
{
	std::string funcSig = "function alpha(bytes32 data1, bytes32 data2) public";
	auto params = AbiParamType::ParseParamType(funcSig);

	AbiParamType funcParam1 = *params[0];
	EXPECT_EQ(funcParam1.GetType(), ParamType::Bytes32);
	EXPECT_EQ(funcParam1.IsStatic(), true);
	EXPECT_EQ(funcParam1.IsArray(), false);

	AbiParamType funcParam2 = *params[1];
	EXPECT_EQ(funcParam2.GetType(), ParamType::Bytes32);
	EXPECT_EQ(funcParam2.IsStatic(), true);
	EXPECT_EQ(funcParam2.IsArray(), false);
}

GTEST_TEST(TestEthAbiParamTypeParser, ParseMultipleMixedType)
{
	std::string funcSig =
	"function alpha(uint64 num, bool isFoo, bytes data, bytes32 data2) public";
	auto params = AbiParamType::ParseParamType(funcSig);

	AbiParamType funcParam1 = *params[0];
	EXPECT_EQ(funcParam1.GetType(), ParamType::Uint64);
	EXPECT_EQ(funcParam1.IsStatic(), true);
	EXPECT_EQ(funcParam1.IsArray(), false);

	AbiParamType funcParam2 = *params[1];
	EXPECT_EQ(funcParam2.GetType(), ParamType::Bool);
	EXPECT_EQ(funcParam2.IsStatic(), true);
	EXPECT_EQ(funcParam2.IsArray(), false);

	AbiParamType funcParam3 = *params[2];
	EXPECT_EQ(funcParam3.GetType(), ParamType::Bytes);
	EXPECT_EQ(funcParam3.IsStatic(), false);
	EXPECT_EQ(funcParam3.IsArray(), false);

	AbiParamType funcParam4 = *params[3];
	EXPECT_EQ(funcParam4.GetType(), ParamType::Bytes32);
	EXPECT_EQ(funcParam4.IsStatic(), true);
	EXPECT_EQ(funcParam4.IsArray(), false);
}
