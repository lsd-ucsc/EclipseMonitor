// Copyright 2023 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/Eth/Bloom.hpp>
#include <EclipseMonitor/Eth/Keccak256.hpp>
#include <SimpleObjects/SimpleObjects.hpp>
#include <SimpleRlp/SimpleRlp.hpp>

#include "EthHeader.hpp"

namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor::Eth;
using namespace EclipseMonitor_Test;
using namespace SimpleRlp;

GTEST_TEST(TestEthBloom, CountTestFile)
{
	static auto tmp = ++g_numOfTestFile;
	(void)tmp;
}

void PrintBytes(const SimpleObjects::Bytes& bytes)
{
	for (auto b : bytes)
	{
		std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b << " ";
	}
	std::cout << std::endl;
	std::cout << (uint)bytes[254] << std::endl;
}

GTEST_TEST(TestEthBloom, EthBloom_15001871)
{
	std::vector<uint8_t> headerRlp = GetEthHeader_15001871();
	BloomFilter bloom(headerRlp);

	std::vector<uint8_t> address = {
		0xDAU, 0xC1U, 0x7FU, 0x95U, 0x8DU, 0x2EU, 0xE5U, 0x23U,
		0xA2U, 0x20U, 0x62U, 0x06U, 0x99U, 0x45U, 0x97U, 0xC1U,
		0x3DU, 0x83U, 0x1EU, 0xC7U
	};
	std::array<uint8_t, 32> hashedAddr = Keccak256(address);

	std::vector<uint8_t> topic = {
		0xDDU, 0xF2U, 0x52U, 0xADU, 0x1BU, 0xE2U, 0xC8U, 0x9BU,
		0x69U, 0xC2U, 0xB0U, 0x68U, 0xFCU, 0x37U, 0x8DU, 0xAAU,
		0x95U, 0x2BU, 0xA7U, 0xF1U, 0x63U, 0xC4U, 0xA1U, 0x16U,
		0x28U, 0xF5U, 0x5AU, 0x4DU, 0xF5U, 0x23U, 0xB3U, 0xEFU
	};

	std::array<uint8_t, 32> hashedTopic = Keccak256(topic);

	EXPECT_TRUE(bloom.IsEventInBloom(hashedAddr, hashedTopic));
}


GTEST_TEST(TestEthBloom, EthBloom_TestBlock_1)
{
	std::vector<uint8_t> headerRlp = GetEthHeader_TestBlock_1();
	BloomFilter bloom(headerRlp);

	std::vector<uint8_t> address = {
		0xD8U, 0xF7U, 0xFFU, 0x92U, 0x2CU, 0x3DU, 0x60U, 0x27U,
		0x28U, 0x50U, 0x26U, 0x95U, 0xBCU, 0xD2U, 0x83U, 0x8DU,
		0x5CU, 0x80U, 0x84U, 0x61U
	};
	std::array<uint8_t, 32> hashedAddr = Keccak256(address);

	std::vector<uint8_t> topic = {
		0x6EU, 0x76U, 0xFBU, 0x4CU, 0x77U, 0x25U, 0x60U, 0x06U,
		0xD9U, 0xC3U, 0x8EU, 0xC7U, 0xD8U, 0x2BU, 0x45U, 0xA8U,
		0xC8U, 0xF3U, 0xC2U, 0x7BU, 0x1DU, 0x67U, 0x66U, 0xFFU,
		0xFCU, 0x42U, 0xDFU, 0xB8U, 0xDEU, 0x68U, 0x44U, 0x92U
	};

	std::array<uint8_t, 32> hashedTopic = Keccak256(topic);

	EXPECT_TRUE(bloom.IsEventInBloom(hashedAddr, hashedTopic));

	topic = {
		0xDDU, 0xF2U, 0x52U, 0xADU, 0x1BU, 0xE2U, 0xC8U, 0x9BU,
		0x69U, 0xC2U, 0xB0U, 0x68U, 0xFCU, 0x37U, 0x8DU, 0xAAU,
		0x95U, 0x2BU, 0xA7U, 0xF1U, 0x63U, 0xC4U, 0xA1U, 0x16U,
		0x28U, 0xF5U, 0x5AU, 0x4DU, 0xF5U, 0x23U, 0xB3U, 0xEFU
	};

	hashedTopic = Keccak256(topic);
	EXPECT_FALSE(bloom.IsEventInBloom(hashedAddr, hashedTopic));
}