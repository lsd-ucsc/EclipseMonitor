// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>
#include <string>
#include <EclipseMonitor/Eth/Abi/AbiParam.hpp>

namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}


using namespace EclipseMonitor_Test;
using namespace EclipseMonitor::Eth::Abi;

GTEST_TEST(TestEthAbiParam, CountTestFile)
{
	static auto tmp = ++g_numOfTestFile;
	(void)tmp;
}


GTEST_TEST(TestEthAbiParam, ParseUint64)
{
	std::string paramStr = "uint64";
	auto param = AbiParamType::ParseParamString(paramStr);

	EXPECT_EQ(param->GetType(), ParamType::Uint64);
	EXPECT_EQ(param->IsStatic(), true);
}

GTEST_TEST(TestEthAbiParam, ParseBool)
{
	std::string paramStr = "bool";
	auto param = AbiParamType::ParseParamString(paramStr);

	EXPECT_EQ(param->GetType(), ParamType::Bool);
	EXPECT_EQ(param->IsStatic(), true);
}

GTEST_TEST(TestEthAbiParam, ParseBytes)
{
	std::string paramStr = "bytes";
	auto param = AbiParamType::ParseParamString(paramStr);

	EXPECT_EQ(param->GetType(), ParamType::Bytes);
	EXPECT_EQ(param->IsStatic(), false);
}

GTEST_TEST(TestEthAbiParam, ParseBytes32)
{
	std::string paramStr = "bytes32";
	auto param = AbiParamType::ParseParamString(paramStr);

	EXPECT_EQ(param->GetType(), ParamType::Bytes32);
	EXPECT_EQ(param->IsStatic(), true);
}

GTEST_TEST(TestEthAbiParam, ParseUint256Arr)
{
	std::string paramStr = "uint64[]";
	auto param = AbiParamType::ParseParamString(paramStr);

	EXPECT_EQ(param->GetType(), ParamType::Uint64);
	EXPECT_EQ(param->IsStatic(), false);
}

GTEST_TEST(TestEthAbiParam, ParseBoolArr)
{
	std::string paramStr = "bool[]";
	auto param = AbiParamType::ParseParamString(paramStr);

	EXPECT_EQ(param->GetType(), ParamType::Bool);
	EXPECT_EQ(param->IsStatic(), false);
}

GTEST_TEST(TestEthAbiParam, ParseBytes32Arr)
{
	std::string paramStr = "bytes32[]";
	auto param = AbiParamType::ParseParamString(paramStr);

	EXPECT_EQ(param->GetType(), ParamType::Bytes32);
	EXPECT_EQ(param->IsStatic(), false);
}