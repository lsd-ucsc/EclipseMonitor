// Copyright (c) 2024 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.


#include <gtest/gtest.h>

#include <EclipseMonitor/Eth/Address.hpp>

#include "Common.hpp"


namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor_Test;
using namespace EclipseMonitor::Eth;


GTEST_TEST(TestEthAddress, CountTestFile)
{
	static auto tmp = ++EclipseMonitor_Test::g_numOfTestFile;
	(void)tmp;
}


/**
 * @brief
 *
 * @param addrHexStr this string should avoid the prefix "0x"
 * @param prefix
 */
static void TestAddressParseAndString(
	const std::string& addrHexStr,
	const std::string& prefix = "0x"
)
{
	// parse without the prefix
	Address addr(addrHexStr);
	auto generatedStr = addr.ToString("");
	EXPECT_EQ(generatedStr, addrHexStr);

	// parse again with the prefix
	Address addr2(generatedStr);
	auto generatedStr2 = addr2.ToString(prefix);
	EXPECT_EQ(generatedStr2, prefix + addrHexStr);

	// two `Address` instances should be equal
	EXPECT_EQ(addr, addr2);
	EXPECT_FALSE(addr != addr2);

	// test the copy constructor
	Address copied(addr);
	EXPECT_EQ(copied, addr);

	// test the move constructor
	Address moved(std::move(copied));
	EXPECT_EQ(moved, addr);
}


GTEST_TEST(TestEthAddress, ParseAndString)
{
	TestAddressParseAndString("010EEE07C4020148D96F80CEd0EE4D129a267D20");
	TestAddressParseAndString("453272C49Dd5b2343Fef13EAdb746E083fB36411");
	TestAddressParseAndString("653E2Bb1258edA29c2F348e88de7F936af8E32C3");
	TestAddressParseAndString("359E745B64498408F11e2811c7376c745084C80f");
	TestAddressParseAndString("9Baa87097A3C3Ff7Fb6428baa2930a031A1Ea4dF");
	TestAddressParseAndString("Dbc12BE0FB8059040b275fe35D6C0c44e420436E");
	TestAddressParseAndString("2bE4803127CD97Abb65F1bE319fA18b6A5567C77");
	TestAddressParseAndString("B39c2ecB0BC4Fa3e75e4Adcb3A59B8cb46AEc16c");
	TestAddressParseAndString("7Bc655F54f53c5ae0aac55d19CCe245368f518AB");
	TestAddressParseAndString("786d53fCc2ac73F3ac8aC21a1E03c0c1bDC70Ad3");

	// string with incorrect length
	EXPECT_THROW_MSG(
		TestAddressParseAndString(""),
		EclipseMonitor::Exception,
		"The given ETH address hex string is of incorrect length"
	);
	EXPECT_THROW_MSG(
		TestAddressParseAndString("0"),
		EclipseMonitor::Exception,
		"The given ETH address hex string is of incorrect length"
	);
	EXPECT_THROW_MSG(
		TestAddressParseAndString("0x"),
		EclipseMonitor::Exception,
		"The given ETH address hex string is of incorrect length"
	);
	EXPECT_THROW_MSG(
		TestAddressParseAndString("786d53fCc2"),
		EclipseMonitor::Exception,
		"The given ETH address hex string is of incorrect length"
	);
	EXPECT_THROW_MSG(
		TestAddressParseAndString("786d53fCc2ac73F3ac8aC21a1E03c0c1bDC70Ad3786d53fCc2"),
		EclipseMonitor::Exception,
		"The given ETH address hex string is of incorrect length"
	);

	// string containing invalid characters
	EXPECT_THROW_MSG(
		TestAddressParseAndString("786d53fCc2ac73F3ac8HC21a1E03c0c1bDC70Ad3"),
		std::invalid_argument,
		"Invalid hex character"
	);
}

