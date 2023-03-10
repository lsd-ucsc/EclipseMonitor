// Copyright (c) 2023 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.


#include <gtest/gtest.h>

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


GTEST_TEST(TestEthEventManager, TestSyncMsg)
{
	using _BlockNum = typename HeaderMgr::BlkNumType;

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
	// const EventTopic sessionID = {
	// 	0X52U, 0XFDU, 0XFCU, 0X07U, 0X21U, 0X82U, 0X65U, 0X4FU,
	// 	0X16U, 0X3FU, 0X5FU, 0X0FU, 0X9AU, 0X62U, 0X1DU, 0X72U,
	// 	0X00U, 0X00U, 0X00U, 0X00U, 0X00U, 0X00U, 0X00U, 0X00U,
	// 	0X00U, 0X00U, 0X00U, 0X00U, 0X00U, 0X00U, 0X00U, 0X00U,
	// };
	// // nonce = 0x9566c74d10037c4d7bbb0407d1e2c64981855ad8681d0d86d1e91e00167939cb
	// const EventTopic nonce = {
	// 	0X95U, 0X66U, 0XC7U, 0X4DU, 0X10U, 0X03U, 0X7CU, 0X4DU,
	// 	0X7BU, 0XBBU, 0X04U, 0X07U, 0XD1U, 0XE2U, 0XC6U, 0X49U,
	// 	0X81U, 0X85U, 0X5AU, 0XD8U, 0X68U, 0X1DU, 0X0DU, 0X86U,
	// 	0XD1U, 0XE9U, 0X1EU, 0X00U, 0X16U, 0X79U, 0X39U, 0XCBU,
	// };

	bool isEventFound = false;
	EventCallbackId eventCallbackIdInCB = 0;
	EventDescription eventDesc(
		decentSyncV1Addr,
		std::vector<EventTopic>({ eventSignatureTopic, }), //{  nonce, sessionID, }
		[&](
			const HeaderMgr&,
			const ReceiptLogEntry&,
			EventCallbackId eventCallbackId
		) -> void
		{
			eventCallbackIdInCB = eventCallbackId;
			isEventFound = true;
		}
	);

	ReceiptsMgr receiptsMgr(receiptsB8569169.AsList());

	EventManager eventMgr;
	auto eventCallbackIdLit = eventMgr.Listen(std::move(eventDesc));

	auto receiptsMgrGetter =
		[&](_BlockNum) -> ReceiptsMgr
		{
			return std::move(receiptsMgr);
		};

	eventMgr.CheckEvents(
		headerMgr,
		receiptsMgrGetter
	);

	EXPECT_TRUE(isEventFound);
	EXPECT_EQ(eventCallbackIdLit, eventCallbackIdInCB);
}
