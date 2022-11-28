// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/Eth/Keccak256.hpp>
#include <EclipseMonitor/Eth/ReceiptMgr.hpp>
#include <SimpleObjects/SimpleObjects.hpp>

#include "EthReceipt.hpp"

namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor_Test;
using namespace EclipseMonitor::Eth;

GTEST_TEST(TestEthReceiptMgr, CountTestFile)
{
	static auto tmp = ++g_numOfTestFile;
	(void)tmp;
}

GTEST_TEST(TestEthReceiptMgr, LegacyReceipt_15415840)
{
	const SimpleObjects::Bytes& receiptBytes = LegacyReceipt_15415840();
	ReceiptMgr mgr = ReceiptMgr::FromBytes(receiptBytes);

	const SimpleObjects::ListBaseObj& receiptBody = mgr.GetReceiptBody();
	EXPECT_EQ(receiptBody.size(), 4);

	const SimpleObjects::ListBaseObj& receiptLogs = mgr.GetReceiptLogs();
	SimpleObjects::List expectedLogs = {};

	EXPECT_EQ(receiptLogs, expectedLogs);
}

GTEST_TEST(TestEthReceiptMgr, AccessListReceipt_15415840)
{
	const SimpleObjects::Bytes& receiptBytes = AccessListReceipt_15415840();
	ReceiptMgr mgr = ReceiptMgr::FromBytes(receiptBytes);

	const SimpleObjects::ListBaseObj& receiptBody = mgr.GetReceiptBody();
	EXPECT_EQ(receiptBody.size(), 4);

	const SimpleObjects::ListBaseObj& receiptLogs = mgr.GetReceiptLogs();
	EXPECT_EQ(receiptLogs.size(), 12);

	const SimpleObjects::ListBaseObj& logIndex11 = receiptLogs[11].AsList();

	const SimpleObjects::BytesBaseObj& logIndex11Address =
		logIndex11[0].AsBytes();

	SimpleObjects::Bytes expectedLogIndex11Address = {
		0x29, 0x0a, 0x6a, 0x74, 0x60, 0xb3, 0x08, 0xee,
		0x3f, 0x19, 0x02, 0x3d, 0x2d, 0x00, 0xde, 0x60,
		0x4b, 0xcf, 0x5b, 0x42
	};
	EXPECT_EQ(logIndex11Address, expectedLogIndex11Address);

	const SimpleObjects::ListBaseObj& logIndex11Topics = logIndex11[1].AsList();
	EXPECT_EQ(logIndex11Topics.size(), 3);

	const SimpleObjects::BytesBaseObj& logIndex11Data = logIndex11[2].AsBytes();
	SimpleObjects::Bytes expectedLogIndex11Data = {
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x38U,
		0xcaU, 0x28U, 0x12U, 0xbeU, 0x66U, 0xffU, 0x53U, 0xa5U,
		0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU,
		0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU,
		0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU,
		0xf8U, 0xd0U, 0x95U, 0x1bU, 0x18U, 0x1aU, 0x01U, 0x66U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x05U, 0xb3U, 0x1fU, 0xbfU,
		0xbaU, 0xd4U, 0x84U, 0x83U, 0x0bU, 0xcbU, 0xe9U, 0x38U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0xbdU, 0x13U,
		0x92U, 0x7bU, 0xb8U, 0xbbU, 0x27U, 0x0dU, 0x9eU, 0xf1U,
		0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU,
		0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU,
		0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU,
		0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xfeU, 0xd6U, 0xbcU
	};

	EXPECT_EQ(logIndex11Data, expectedLogIndex11Data);

}

GTEST_TEST(TestEthReceiptMgr, DynamicFeeReceipt_15415840)
{
	const SimpleObjects::Bytes& receiptBytes = DynamicFeeReceipt_15415840();
	ReceiptMgr mgr = ReceiptMgr::FromBytes(receiptBytes);

	const SimpleObjects::ListBaseObj& receiptBody = mgr.GetReceiptBody();
	EXPECT_EQ(receiptBody.size(), 4);

	const SimpleObjects::ListBaseObj& receiptLogs = mgr.GetReceiptLogs();
	EXPECT_EQ(receiptLogs.size(), 1);

	const SimpleObjects::ListBaseObj& logEntry = receiptLogs[0].AsList();

	const SimpleObjects::BytesBaseObj& logEntryAddress = logEntry[0].AsBytes();
	SimpleObjects::Bytes expectedLogEntryAddress = {
		0x9fU, 0x8fU, 0x72U, 0xaaU, 0x93U, 0x04U, 0xc8U, 0xb5U,
		0x93U, 0xd5U, 0x55U, 0xf1U, 0x2eU, 0xf6U, 0x58U, 0x9cU,
		0xc3U, 0xa5U, 0x79U, 0xa2U
	};

	EXPECT_EQ(logEntryAddress, expectedLogEntryAddress);

	const SimpleObjects::ListBaseObj& logEntryTopics = logEntry[1].AsList();
	EXPECT_EQ(logEntryTopics.size(), 3);

	const SimpleObjects::BytesBaseObj& logEntryData = logEntry[2].AsBytes();
	SimpleObjects::Bytes expectedData = {
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x37U, 0x3bU, 0xccU, 0xe9U, 0xbeU, 0x0eU, 0x00U, 0x00U
	};
	EXPECT_EQ(logEntryData, expectedData);
}


GTEST_TEST(TestEthReceiptMgr, TopicTest)
{
	std::string event("Revoke(bytes32)");
	std::vector<uint8_t> eventBytes(event.begin(), event.end());
	std::array<uint8_t, 32> hashedEvent = Keccak256(eventBytes);
	SimpleObjects::Bytes bytesObject(hashedEvent.begin(), hashedEvent.end());

	SimpleObjects::Bytes expected = {
		0x22U, 0xb3U, 0xf5U, 0x49U, 0x5eU, 0xf9U, 0xc5U, 0x6aU,
		0xd6U, 0xe2U, 0x70U, 0xe8U, 0xc8U, 0x4fU, 0x1aU, 0x2fU,
		0x0eU, 0x0bU, 0xd8U, 0x3cU, 0xcbU, 0x69U, 0x88U, 0xa1U,
		0x84U, 0x23U, 0x14U, 0x72U, 0x7aU, 0x44U, 0x1dU, 0x5cU
	};

	EXPECT_EQ(bytesObject, expected);

}

GTEST_TEST(TestEThReceiptMgr, EventTest1)
{
	const SimpleObjects::Bytes& receiptBytes = TestReceipt1();
	ReceiptMgr mgr = ReceiptMgr::FromBytes(receiptBytes);

	std::string event("Revoke(bytes32)");
	std::vector<uint8_t> eventStringBytes(event.begin(), event.end());
	std::array<uint8_t, 32> hashedEvent = Keccak256(eventStringBytes);
	SimpleObjects::Bytes eventBytes(hashedEvent.begin(), hashedEvent.end());

	SimpleObjects::Bytes contractAddress = {
		0xe3U, 0x3bU, 0x30U, 0xcdU, 0xe1U, 0xaaU, 0xe0U, 0x64U,
		0x09U, 0xe8U, 0xaaU, 0xdfU, 0xa5U, 0xb5U, 0xfcU, 0xfcU,
		0x8dU, 0x8dU, 0xdaU, 0x7cU
	};

	bool eventEmitted;
	SimpleObjects::Bytes eventData;

	std::tie(eventEmitted, eventData) =
		mgr.IsEventEmitted(contractAddress, eventBytes);

	EXPECT_TRUE(eventEmitted);

	SimpleObjects::Bytes expectedEventData = {
		0x9fU, 0x8fU, 0x72U, 0xaaU, 0x93U, 0x04U, 0xc8U, 0xb5U,
		0x93U, 0xd5U, 0x55U, 0xf1U, 0x2eU, 0xf6U, 0x58U, 0x9cU,
		0xc3U, 0xa5U, 0x79U, 0xa2U, 0x9fU, 0x8fU, 0x72U, 0xaaU,
		0x93U, 0x04U, 0xc8U, 0xb5U, 0x93U, 0xd5U, 0x9fU, 0x8fU
	};

	EXPECT_EQ(eventData, expectedEventData);
}

GTEST_TEST(TestEThReceiptMgr, EventTest2)
{
	const SimpleObjects::Bytes& receiptBytes = TestReceipt2();
	ReceiptMgr mgr = ReceiptMgr::FromBytes(receiptBytes);

	std::string event("Revoke(bytes32,uint64,bool)");
	std::vector<uint8_t> eventStringBytes(event.begin(), event.end());
	std::array<uint8_t, 32> hashedEvent = Keccak256(eventStringBytes);
	SimpleObjects::Bytes eventBytes(hashedEvent.begin(), hashedEvent.end());

	SimpleObjects::Bytes contractAddress = {
		0x4cU, 0x66U, 0x08U, 0xcdU, 0xe7U, 0x5eU, 0x62U, 0x20U,
		0x82U, 0x8eU, 0x3fU, 0x37U, 0x61U, 0x66U, 0x3bU, 0xdbU,
		0x86U, 0x05U, 0x38U, 0xe6U
	};

	bool eventEmitted;
	SimpleObjects::Bytes eventData;

	std::tie(eventEmitted, eventData) =
		mgr.IsEventEmitted(contractAddress, eventBytes);

	EXPECT_TRUE(eventEmitted);
}

GTEST_TEST(TestEThReceiptMgr, EventTest3)
{
	const SimpleObjects::Bytes& receiptBytes = TestReceipt3();
	ReceiptMgr mgr = ReceiptMgr::FromBytes(receiptBytes);

	std::string event("Revoke(bytes[])");
	std::vector<uint8_t> eventStringBytes(event.begin(), event.end());
	std::array<uint8_t, 32> hashedEvent = Keccak256(eventStringBytes);
	SimpleObjects::Bytes eventBytes(hashedEvent.begin(), hashedEvent.end());

	SimpleObjects::Bytes contractAddress = {
		0x51U, 0x3aU, 0xafU, 0xe1U, 0x14U, 0x37U, 0x20U, 0x77U,
		0x19U, 0x45U, 0xbcU, 0x3aU, 0x43U, 0x0dU, 0x7fU, 0x41U,
		0x71U, 0xf2U, 0xd2U, 0x48U
	};

	bool eventEmitted;
	SimpleObjects::Bytes eventData;

	std::tie(eventEmitted, eventData) =
		mgr.IsEventEmitted(contractAddress, eventBytes);

	EXPECT_TRUE(eventEmitted);
}
