// Copyright (c) 2023 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.


#include <gtest/gtest.h>

#include <EclipseMonitor/Eth/AbiParser.hpp>
#include <EclipseMonitor/Eth/EventManager.hpp>

#include "BlockData.hpp"


namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor_Test;

using namespace EclipseMonitor::Eth;


GTEST_TEST(TestEthEventManager, CountTestFile)
{
	static auto tmp = ++g_numOfTestFile;
	(void)tmp;
}


GTEST_TEST(TestEthEventManager, ListenCancelAndSize)
{
	// DecentSyncMsgV1 address = 0xe22ee57780ba71e1edd18e066c5b8a2a93bdc3ae
	const ContractAddr decentSyncV1Addr = {
		0XE2U, 0X2EU, 0XE5U, 0X77U, 0X80U, 0XBAU, 0X71U, 0XE1U,
		0XEDU, 0XD1U, 0X8EU, 0X06U, 0X6CU, 0X5BU, 0X8AU, 0X2AU,
		0X93U, 0XBDU, 0XC3U, 0XAEU,
	};

	EventDescription eventDesc(
		decentSyncV1Addr,
		std::vector<EventTopic>(),
		[&](
			const HeaderMgr&,
			const ReceiptLogEntry&,
			EventCallbackId
		) -> void
		{}
	);

	EventManager eventMgr;
	EXPECT_EQ(eventMgr.GetNumOfListeners(), 0);

	auto eventCallbackIdRet = eventMgr.Listen(std::move(eventDesc));
	EXPECT_EQ(eventMgr.GetNumOfListeners(), 1);

	eventMgr.Cancel(eventCallbackIdRet);
	EXPECT_EQ(eventMgr.GetNumOfListeners(), 0);

	eventMgr.Cancel(eventCallbackIdRet);
	EXPECT_EQ(eventMgr.GetNumOfListeners(), 0);
}


GTEST_TEST(TestEthEventManager, TestDecentSyncMsgV1)
{
	const auto headerB8569169 =
		BlockData::ReadBinary(
			BlockData::GetRlpFilePath("testnet_b_8569169.header")
		);
	const auto receiptsB8569169 =
		BlockData::ReadRlp("testnet_b_8569169.receipts");

	const HeaderMgr headerMgr(headerB8569169, 0);

	// DecentSyncMsgV1 address = 0xe22ee57780ba71e1edd18e066c5b8a2a93bdc3ae
	const ContractAddr decentSyncV1Addr = {
		0XE2U, 0X2EU, 0XE5U, 0X77U, 0X80U, 0XBAU, 0X71U, 0XE1U,
		0XEDU, 0XD1U, 0X8EU, 0X06U, 0X6CU, 0X5BU, 0X8AU, 0X2AU,
		0X93U, 0XBDU, 0XC3U, 0XAEU,
	};
	const std::string eventSignature = "SyncMsg(bytes16,bytes32)";
	const EventTopic eventSignatureTopic = Keccak256(eventSignature);
	// expected signature topic is c237b918200c043ce93bb9d4b7674b0a476e905ab54e0c20d0287547305c3a98
	const EventTopic eventSignatureTopicExp = {
		0XC2U, 0X37U, 0XB9U, 0X18U, 0X20U, 0X0CU, 0X04U, 0X3CU,
		0XE9U, 0X3BU, 0XB9U, 0XD4U, 0XB7U, 0X67U, 0X4BU, 0X0AU,
		0X47U, 0X6EU, 0X90U, 0X5AU, 0XB5U, 0X4EU, 0X0CU, 0X20U,
		0XD0U, 0X28U, 0X75U, 0X47U, 0X30U, 0X5CU, 0X3AU, 0X98U,
	};
	ASSERT_EQ(eventSignatureTopic, eventSignatureTopicExp);
	// sessionID = 0x52fdfc072182654f163f5f0f9a621d72
	const EventTopic sessionID = {
		0X52U, 0XFDU, 0XFCU, 0X07U, 0X21U, 0X82U, 0X65U, 0X4FU,
		0X16U, 0X3FU, 0X5FU, 0X0FU, 0X9AU, 0X62U, 0X1DU, 0X72U,
		0X00U, 0X00U, 0X00U, 0X00U, 0X00U, 0X00U, 0X00U, 0X00U,
		0X00U, 0X00U, 0X00U, 0X00U, 0X00U, 0X00U, 0X00U, 0X00U,
	};
	// nonce = 0x9566c74d10037c4d7bbb0407d1e2c64981855ad8681d0d86d1e91e00167939cb
	const EventTopic nonce = {
		0X95U, 0X66U, 0XC7U, 0X4DU, 0X10U, 0X03U, 0X7CU, 0X4DU,
		0X7BU, 0XBBU, 0X04U, 0X07U, 0XD1U, 0XE2U, 0XC6U, 0X49U,
		0X81U, 0X85U, 0X5AU, 0XD8U, 0X68U, 0X1DU, 0X0DU, 0X86U,
		0XD1U, 0XE9U, 0X1EU, 0X00U, 0X16U, 0X79U, 0X39U, 0XCBU,
	};

	bool isEventFound = false;
	EventCallbackId eventCallbackIdRet = 0;
	EventDescription eventDesc(
		decentSyncV1Addr,
		std::vector<EventTopic>({ eventSignatureTopic, }), //{ sessionID, nonce,  }
		[&](
			const HeaderMgr&,
			const ReceiptLogEntry& logEntry,
			EventCallbackId eventCallbackId
		) -> void
		{
			std::vector<uint8_t> sessIDandNonce;
			sessIDandNonce.reserve(sessionID.size() + nonce.size());
			sessIDandNonce.insert(
				sessIDandNonce.end(), sessionID.begin(), sessionID.end()
			);
			sessIDandNonce.insert(
				sessIDandNonce.end(), nonce.begin(), nonce.end()
			);

			ASSERT_EQ(logEntry.m_topics.size(), 1);
			EXPECT_EQ(logEntry.m_topics[0], eventSignatureTopic);
			EXPECT_EQ(logEntry.m_logData, sessIDandNonce);
			EXPECT_EQ(eventCallbackIdRet, eventCallbackId);
			isEventFound = true;

			using _AbiSessionID = AbiParser<
				SimpleObjects::ObjCategory::Bytes,
				AbiSize<16>
			>;
			using _AbiNonce = AbiParser<
				SimpleObjects::ObjCategory::Bytes,
				AbiSize<32>
			>;

			std::vector<uint8_t> abiSessID;
			std::vector<uint8_t> abiNonce;
			auto abiBegin = logEntry.m_logData.begin();
			auto abiEnd = logEntry.m_logData.end();

			std::tie(abiSessID, abiBegin) = _AbiSessionID().ToPrimitive(abiBegin, abiEnd, abiBegin);
			std::tie(abiNonce, abiBegin) = _AbiNonce().ToPrimitive(abiBegin, abiEnd, abiBegin);

			EXPECT_EQ(
				abiSessID,
				std::vector<uint8_t>(sessionID.begin(), sessionID.begin() + 16)
			);
			EXPECT_EQ(
				abiNonce,
				std::vector<uint8_t>(nonce.begin(), nonce.end())
			);
			EXPECT_EQ(abiBegin, abiEnd);

			// std::string hex = "eventSignatureTopic: ";
			// SimpleObjects::Internal::BytesToHEX<true, char>(
			// 	std::back_inserter(hex),
			// 	logEntry.m_topics[0].begin(),
			// 	logEntry.m_topics[0].end()
			// );
			// hex += ", LogData: ";
			// SimpleObjects::Internal::BytesToHEX<true, char>(
			// 	std::back_inserter(hex),
			// 	logEntry.m_logData.begin(),
			// 	logEntry.m_logData.end()
			// );
			// std::cout << hex << std::endl;
		}
	);

	ReceiptsMgr receiptsMgr(receiptsB8569169.AsList());

	EventManager eventMgr;
	eventCallbackIdRet = eventMgr.Listen(std::move(eventDesc));

	auto receiptsMgrGetter =
		[&](BlockNumber) -> ReceiptsMgr
		{
			return std::move(receiptsMgr);
		};

	eventMgr.CheckEvents(
		headerMgr,
		receiptsMgrGetter
	);

	EXPECT_TRUE(isEventFound);
}


GTEST_TEST(TestEthEventManager, TestDecentSyncMsgV2)
{
	const auto headerB8628615 =
		BlockData::ReadBinary(
			BlockData::GetRlpFilePath("testnet_b_8628615.header")
		);
	const auto receiptsB8628615 =
		BlockData::ReadRlp("testnet_b_8628615.receipts");

	const HeaderMgr headerMgr(headerB8628615, 0);

	// DecentSyncMsgV2 address = 0x74Be867FBD89bC3507F145b36ba76cd0B1bF4f1A
	const ContractAddr decentSyncV2Addr = {
		0X74U, 0XBEU, 0X86U, 0X7FU, 0XBDU, 0X89U, 0XBCU, 0X35U,
		0X07U, 0XF1U, 0X45U, 0XB3U, 0X6BU, 0XA7U, 0X6CU, 0XD0U,
		0XB1U, 0XBFU, 0X4FU, 0X1AU,
	};
	const std::string eventSignature = "SyncMsg(bytes16,bytes32)";
	const EventTopic eventSignatureTopic = Keccak256(eventSignature);
	// expected signature topic is c237b918200c043ce93bb9d4b7674b0a476e905ab54e0c20d0287547305c3a98
	const EventTopic eventSignatureTopicExp = {
		0XC2U, 0X37U, 0XB9U, 0X18U, 0X20U, 0X0CU, 0X04U, 0X3CU,
		0XE9U, 0X3BU, 0XB9U, 0XD4U, 0XB7U, 0X67U, 0X4BU, 0X0AU,
		0X47U, 0X6EU, 0X90U, 0X5AU, 0XB5U, 0X4EU, 0X0CU, 0X20U,
		0XD0U, 0X28U, 0X75U, 0X47U, 0X30U, 0X5CU, 0X3AU, 0X98U,
	};
	ASSERT_EQ(eventSignatureTopic, eventSignatureTopicExp);
	// sessionID = 0x52fdfc072182654f163f5f0f9a621d72
	const EventTopic sessionID = {
		0X52U, 0XFDU, 0XFCU, 0X07U, 0X21U, 0X82U, 0X65U, 0X4FU,
		0X16U, 0X3FU, 0X5FU, 0X0FU, 0X9AU, 0X62U, 0X1DU, 0X72U,
		0X00U, 0X00U, 0X00U, 0X00U, 0X00U, 0X00U, 0X00U, 0X00U,
		0X00U, 0X00U, 0X00U, 0X00U, 0X00U, 0X00U, 0X00U, 0X00U,
	};
	// nonce = 0x9566c74d10037c4d7bbb0407d1e2c64981855ad8681d0d86d1e91e00167939cb
	const EventTopic nonce = {
		0X95U, 0X66U, 0XC7U, 0X4DU, 0X10U, 0X03U, 0X7CU, 0X4DU,
		0X7BU, 0XBBU, 0X04U, 0X07U, 0XD1U, 0XE2U, 0XC6U, 0X49U,
		0X81U, 0X85U, 0X5AU, 0XD8U, 0X68U, 0X1DU, 0X0DU, 0X86U,
		0XD1U, 0XE9U, 0X1EU, 0X00U, 0X16U, 0X79U, 0X39U, 0XCBU,
	};

	bool isEventFound = false;
	EventCallbackId eventCallbackIdRet = 0;
	EventDescription eventDesc(
		decentSyncV2Addr,
		std::vector<EventTopic>({ eventSignatureTopic, sessionID, nonce, }),
		[&](
			const HeaderMgr&,
			const ReceiptLogEntry& logEntry,
			EventCallbackId eventCallbackId
		) -> void
		{
			ASSERT_EQ(logEntry.m_topics.size(), 3);
			EXPECT_EQ(logEntry.m_topics[0], eventSignatureTopic);
			EXPECT_EQ(logEntry.m_topics[1], sessionID);
			EXPECT_EQ(logEntry.m_topics[2], nonce);
			EXPECT_EQ(eventCallbackIdRet, eventCallbackId);
			isEventFound = true;
		}
	);

	ReceiptsMgr receiptsMgr(receiptsB8628615.AsList());

	EventManager eventMgr;
	eventCallbackIdRet = eventMgr.Listen(std::move(eventDesc));

	auto receiptsMgrGetter =
		[&](BlockNumber) -> ReceiptsMgr
		{
			return std::move(receiptsMgr);
		};

	eventMgr.CheckEvents(
		headerMgr,
		receiptsMgrGetter
	);

	EXPECT_TRUE(isEventFound);
}
