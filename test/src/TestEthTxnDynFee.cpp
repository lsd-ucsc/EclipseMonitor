// Copyright (c) 2024 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.


#include <gtest/gtest.h>

#include <EclipseMonitor/Eth/Transaction/DynamicFee.hpp>
#include <EclipseMonitor/Exceptions.hpp>

#include <SimpleObjects/Codec/Hex.hpp>

#include "Common.hpp"


namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor_Test;

using namespace EclipseMonitor::Eth;


GTEST_TEST(TestEthTxnDynFee, CountTestFile)
{
	static auto tmp = ++EclipseMonitor_Test::g_numOfTestFile;
	(void)tmp;
}


GTEST_TEST(TestEthTxnDynFee, ValidateAccessList)
{
	{
		Transaction::AccessListObj accessListEmpty;
		EXPECT_NO_THROW(Transaction::ValidateAccessList(accessListEmpty));
	}

	{
		Transaction::AccessListObj accessListEmptyStKeys = {
			SimpleObjects::List({
				// address
				SimpleObjects::Bytes({
					0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
					0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
					0x00U, 0x00U, 0x00U, 0x01U,
				}),
				// storage keys
				SimpleObjects::List()
			})
		};
		EXPECT_NO_THROW(Transaction::ValidateAccessList(accessListEmptyStKeys));
	}

	{
		Transaction::AccessListObj accessList = {
			SimpleObjects::List({
				// address
				SimpleObjects::Bytes({
					0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
					0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
					0x00U, 0x00U, 0x00U, 0x01U,
				}),
				// storage keys
				SimpleObjects::List({
					SimpleObjects::Bytes({
						0x01U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
						0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
						0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
						0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
					})
				})
			})
		};
		EXPECT_NO_THROW(Transaction::ValidateAccessList(accessList));
	}

	{
		Transaction::AccessListObj accessListInvalidAddr = {
			SimpleObjects::List({
				// address
				SimpleObjects::String("0000000000000000000000000000000000000001"),
				// storage keys
				SimpleObjects::List()
			})
		};
		EXPECT_THROW_MSG(
			Transaction::ValidateAccessList(accessListInvalidAddr),
			EclipseMonitor::Exception,
			"Invalid access list address type"
		);
	}

	{
		Transaction::AccessListObj accessListInvalidAddr = {
			SimpleObjects::List({
				// address
				SimpleObjects::Bytes({
					0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
					0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x01U,
				}),
				// storage keys
				SimpleObjects::List()
			})
		};
		EXPECT_THROW_MSG(
			Transaction::ValidateAccessList(accessListInvalidAddr),
			EclipseMonitor::Exception,
			"Invalid access list address size"
		);
	}

	{
		Transaction::AccessListObj accessListInvalidTuple = {
			SimpleObjects::List({
				// address
				SimpleObjects::Bytes({
					0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
					0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
					0x00U, 0x00U, 0x00U, 0x01U,
				}),
			})
		};
		EXPECT_THROW_MSG(
			Transaction::ValidateAccessList(accessListInvalidTuple),
			EclipseMonitor::Exception,
			"Invalid access list tuple size"
		);
	}

	{
		Transaction::AccessListObj accessListInvalidKeys = {
			SimpleObjects::List({
				// address
				SimpleObjects::Bytes({
					0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
					0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
					0x00U, 0x00U, 0x00U, 0x01U,
				}),
				// storage keys
				SimpleObjects::String("0100000000000000000000000000000000000000000000000000000000000000"),
			})
		};
		EXPECT_THROW_MSG(
			Transaction::ValidateAccessList(accessListInvalidKeys),
			EclipseMonitor::Exception,
			"Invalid access list storage keys type"
		);
	}

	{
		Transaction::AccessListObj accessListInvalidKey = {
			SimpleObjects::List({
				// address
				SimpleObjects::Bytes({
					0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
					0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
					0x00U, 0x00U, 0x00U, 0x01U,
				}),
				// storage keys
				SimpleObjects::List({
					SimpleObjects::String("0100000000000000000000000000000000000000000000000000000000000000"),
				})
			})
		};
		EXPECT_THROW_MSG(
			Transaction::ValidateAccessList(accessListInvalidKey),
			EclipseMonitor::Exception,
			"Invalid access list storage key type"
		);
	}

	{
		Transaction::AccessListObj accessListInvalidKey = {
			SimpleObjects::List({
				// address
				SimpleObjects::Bytes({
					0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
					0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
					0x00U, 0x00U, 0x00U, 0x01U,
				}),
				// storage keys
				SimpleObjects::List({
					SimpleObjects::Bytes({
						0x01U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
						0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
						0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
					})
				})
			})
		};
		EXPECT_THROW_MSG(
			Transaction::ValidateAccessList(accessListInvalidKey),
			EclipseMonitor::Exception,
			"Invalid access list storage key size"
		);
	}
}


GTEST_TEST(TestEthTxnDynFee, DeEncoding)
{
	{
		uint64_t chainID = 1900ULL;
		uint64_t nonce = 34ULL;
		uint64_t maxPriorFeePerGas = 2000000000ULL;
		uint64_t maxFeePerGas = 2000000000ULL;
		uint64_t gasLimit = 100000ULL;
		SimpleObjects::Bytes destination = {
			// 0x09616C3d61b3331fc4109a9E41a8BDB7d9776609
			0x09U, 0x61U, 0x6cU, 0x3dU, 0x61U, 0xb3U, 0x33U, 0x1fU,
			0xc4U, 0x10U, 0x9aU, 0x9eU, 0x41U, 0xa8U, 0xbdU, 0xb7U,
			0xd9U, 0x77U, 0x66U, 0x09U,
		};
		uint64_t amount = 100000000000000ULL;
		SimpleObjects::Bytes blkData = {
			// 0x616263646566
			0x61U, 0x62U, 0x63U, 0x64U, 0x65U, 0x66U,
		};
		Transaction::AccessListObj accessList = {
			SimpleObjects::List({
				// address
				SimpleObjects::Bytes({
					0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
					0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
					0x00U, 0x00U, 0x00U, 0x01U,
				}),
				// storage keys
				SimpleObjects::List({
					SimpleObjects::Bytes({
						0x01U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
						0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
						0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
						0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
					})
				})
			})
		};

		Transaction::DynFee dynFeeTxn;
		dynFeeTxn.SetChainID(chainID);
		dynFeeTxn.SetNonce(nonce);
		dynFeeTxn.SetMaxPriorFeePerGas(maxPriorFeePerGas);
		dynFeeTxn.SetMaxFeePerGas(maxFeePerGas);
		dynFeeTxn.SetGasLimit(gasLimit);
		dynFeeTxn.get_Destination() = destination;
		dynFeeTxn.SetAmount(amount);
		dynFeeTxn.get_Data() = blkData;
		dynFeeTxn.get_AccessList() = accessList;

		EXPECT_EQ(dynFeeTxn.GetChainID(), chainID);
		EXPECT_EQ(dynFeeTxn.GetNonce(), nonce);
		EXPECT_EQ(dynFeeTxn.GetMaxPriorFeePerGas(), maxPriorFeePerGas);
		EXPECT_EQ(dynFeeTxn.GetMaxFeePerGas(), maxFeePerGas);
		EXPECT_EQ(dynFeeTxn.GetGasLimit(), gasLimit);
		EXPECT_EQ(dynFeeTxn.get_Destination(), destination);
		EXPECT_EQ(dynFeeTxn.GetAmount(), amount);
		EXPECT_EQ(dynFeeTxn.get_Data(), blkData);
		EXPECT_EQ(dynFeeTxn.get_AccessList(), accessList);

		std::vector<uint8_t> expRlp = {
			// f86f82076c2284773594008477359400830186a09409616c3d61b3331fc4109a
			0xf8U, 0x6fU, 0x82U, 0x07U, 0x6cU, 0x22U, 0x84U, 0x77U,
			0x35U, 0x94U, 0x00U, 0x84U, 0x77U, 0x35U, 0x94U, 0x00U,
			0x83U, 0x01U, 0x86U, 0xa0U, 0x94U, 0x09U, 0x61U, 0x6cU,
			0x3dU, 0x61U, 0xb3U, 0x33U, 0x1fU, 0xc4U, 0x10U, 0x9aU,
			// 9e41a8bdb7d9776609865af3107a400086616263646566f838f7940000000000
			0x9eU, 0x41U, 0xa8U, 0xbdU, 0xb7U, 0xd9U, 0x77U, 0x66U,
			0x09U, 0x86U, 0x5aU, 0xf3U, 0x10U, 0x7aU, 0x40U, 0x00U,
			0x86U, 0x61U, 0x62U, 0x63U, 0x64U, 0x65U, 0x66U, 0xf8U,
			0x38U, 0xf7U, 0x94U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			// 000000000000000000000000000001e1a0010000000000000000000000000000
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x01U, 0xe1U,
			0xa0U, 0x01U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			// 0000000000000000000000000000000000
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U,
		};
		std::cout << "Expected RLP:  " <<
			SimpleObjects::Codec::Hex::Encode<std::string>(expRlp) << std::endl;

		auto rlp = dynFeeTxn.RlpSerialize();
		std::cout << "Generated RLP: " <<
			SimpleObjects::Codec::Hex::Encode<std::string>(rlp) << std::endl;

		EXPECT_EQ(rlp, expRlp);

		auto rlpDecoded = Transaction::DynFee::FromRlp(rlp);

		EXPECT_EQ(rlpDecoded.GetChainID(), chainID);
		EXPECT_EQ(rlpDecoded.GetNonce(), nonce);
		EXPECT_EQ(rlpDecoded.GetMaxPriorFeePerGas(), maxPriorFeePerGas);
		EXPECT_EQ(rlpDecoded.GetMaxFeePerGas(), maxFeePerGas);
		EXPECT_EQ(rlpDecoded.GetGasLimit(), gasLimit);
		EXPECT_EQ(rlpDecoded.get_Destination(), destination);
		EXPECT_EQ(rlpDecoded.GetAmount(), amount);
		EXPECT_EQ(rlpDecoded.get_Data(), blkData);
		EXPECT_EQ(rlpDecoded.get_AccessList(), accessList);

		std::array<uint8_t, 32> expHash = {
			// d385a3379c2fbd2ccdda2cb84fa1202cfd0635ba0e422a1921ccf8361b24465c
			0xd3U, 0x85U, 0xa3U, 0x37U, 0x9cU, 0x2fU, 0xbdU, 0x2cU,
			0xcdU, 0xdaU, 0x2cU, 0xb8U, 0x4fU, 0xa1U, 0x20U, 0x2cU,
			0xfdU, 0x06U, 0x35U, 0xbaU, 0x0eU, 0x42U, 0x2aU, 0x19U,
			0x21U, 0xccU, 0xf8U, 0x36U, 0x1bU, 0x24U, 0x46U, 0x5cU,
		};
		auto hash = dynFeeTxn.Hash();
		EXPECT_EQ(hash, rlpDecoded.Hash());
		EXPECT_EQ(hash, expHash);
	}
}

