// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/Eth/SyncMsgMgr.hpp>

#include "BlockData.hpp"


namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor_Test;

using namespace EclipseMonitor;
using namespace EclipseMonitor::Eth;


namespace
{

class TestTimestamper : public TimestamperBase
{
public:
	TestTimestamper() :
		TimestamperBase()
	{}

	virtual ~TestTimestamper() = default;

	virtual TrustedTimestamp NowInSec() const override
	{
		return 0;
	}

}; // class TestTimestamper


class TestRandomGenerator : public RandomGeneratorBase
{
public:
	TestRandomGenerator(EventTopic nonce) :
		RandomGeneratorBase(),
		m_nonce(nonce)
	{}

	virtual ~TestRandomGenerator() = default;

	virtual void GenerateRandomBytes(uint8_t* buf, size_t len) const override
	{
		static constexpr size_t sk_nonceLen = std::tuple_size<SyncNonce>::value;
		if (len != sk_nonceLen)
		{
			throw std::invalid_argument("Not a valid nonce length");
		}
		std::memcpy(buf, m_nonce.data(), len);
	}

	EventTopic m_nonce;
}; // class TestRandomGenerator

} // namespace


GTEST_TEST(TestEthSyncMsgMgr, CountTestFile)
{
	static auto tmp = ++g_numOfTestFile;
	(void)tmp;
}


GTEST_TEST(TestEthSyncMsgMgr, SyncMsgEvent)
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


	MonitorId monitorId;
	monitorId.get_sessionID().resize(std::tuple_size<SessionID>::value);
	std::copy(
		sessionID.data(),
		sessionID.data() + monitorId.get_sessionID().size(),
		&(monitorId.get_sessionID()[0])
	);
	MonitorConfig monitorConfig = BuildEthereumMonitorConfig();


	auto receiptsMgrGetter =
		[&](BlockNumber) -> ReceiptsMgr
		{
			return ReceiptsMgr(receiptsB8628615.AsList());
		};


	std::unique_ptr<TestTimestamper> timestamper(new TestTimestamper());
	std::unique_ptr<TestRandomGenerator> randomGenerator(new TestRandomGenerator(nonce));
	std::shared_ptr<EventManager> eventMgr = std::make_shared<EventManager>();


	SyncMsgMgr syncMgr(
		monitorId,
		monitorConfig,
		*timestamper,
		*randomGenerator,
		decentSyncV2Addr,
		eventSignatureTopic,
		eventMgr
	);


	auto syncState = syncMgr.GetLastSyncState();


	EXPECT_FALSE(syncState->IsSynced());
	eventMgr->CheckEvents(
		headerMgr,
		receiptsMgrGetter
	);
	EXPECT_TRUE(syncState->IsSynced());


	EXPECT_EQ(eventMgr->GetNumOfListeners(), 0);


	// ----- Create a new SyncState ------

	syncState = syncMgr.NewSyncState(*timestamper, *randomGenerator);
	EXPECT_EQ(syncMgr.GetLastSyncState().get(), syncState.get());
	EXPECT_EQ(eventMgr->GetNumOfListeners(), 1);
	syncState = syncMgr.NewSyncState(*timestamper, *randomGenerator);
	EXPECT_EQ(syncMgr.GetLastSyncState().get(), syncState.get());
	EXPECT_EQ(eventMgr->GetNumOfListeners(), 1);



	EXPECT_FALSE(syncState->IsSynced());
	eventMgr->CheckEvents(
		headerMgr,
		receiptsMgrGetter
	);
	EXPECT_TRUE(syncState->IsSynced());


	EXPECT_EQ(eventMgr->GetNumOfListeners(), 0);
}


GTEST_TEST(TestEthSyncMsgMgr, NullEventMgr)
{
	// DecentSyncMsgV2 address = 0x74Be867FBD89bC3507F145b36ba76cd0B1bF4f1A
	const ContractAddr decentSyncV2Addr = {
		0X74U, 0XBEU, 0X86U, 0X7FU, 0XBDU, 0X89U, 0XBCU, 0X35U,
		0X07U, 0XF1U, 0X45U, 0XB3U, 0X6BU, 0XA7U, 0X6CU, 0XD0U,
		0XB1U, 0XBFU, 0X4FU, 0X1AU,
	};
	const std::string eventSignature = "SyncMsg(bytes16,bytes32)";
	const EventTopic eventSignatureTopic = Keccak256(eventSignature);
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

	MonitorId monitorId;
	monitorId.get_sessionID().resize(std::tuple_size<SessionID>::value);
	std::copy(
		sessionID.data(),
		sessionID.data() + monitorId.get_sessionID().size(),
		&(monitorId.get_sessionID()[0])
	);
	MonitorConfig monitorConfig = BuildEthereumMonitorConfig();

	std::unique_ptr<TestTimestamper> timestamper(new TestTimestamper());
	std::unique_ptr<TestRandomGenerator> randomGenerator(new TestRandomGenerator(nonce));
	std::shared_ptr<EventManager> eventMgr = std::make_shared<EventManager>();

	EXPECT_THROW(
		SyncMsgMgr syncMgr(
			monitorId,
			monitorConfig,
			*timestamper,
			*randomGenerator,
			decentSyncV2Addr,
			eventSignatureTopic,
			nullptr
		);,
		Exception
	);
}
