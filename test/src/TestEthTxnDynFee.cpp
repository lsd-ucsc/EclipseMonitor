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


static Transaction::AccessListObj GetSampleAcList_1Addr_1Key()
{
	Transaction::AccessListItem accessListItem;
	accessListItem.get_Address() = SimpleObjects::Bytes({
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x01U,
	});
	accessListItem.get_StorageKeys().push_back(SimpleObjects::Bytes({
		0x01U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
	}));
	Transaction::AccessListObj accessList = {
		accessListItem
	};
	return accessList;
}


static Transaction::AccessListObj GetSampleAcList_1Addr_0Key()
{
	Transaction::AccessListItem accessListItem;
	accessListItem.get_Address() = SimpleObjects::Bytes({
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x01U,
	});
	Transaction::AccessListObj accessList = {
		accessListItem
	};
	return accessList;
}


static Transaction::AccessListObj GetSampleAcList_0Addr_0Key()
{
	Transaction::AccessListObj accessList;
	return accessList;
}


static Transaction::AccessListObj GetSampleAcList_1LongAddr()
{
	Transaction::AccessListItem accessListItem;
	accessListItem.get_Address() = SimpleObjects::Bytes({
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x01U,
	});
	Transaction::AccessListObj accessList = {
		accessListItem
	};
	return accessList;
}


static Transaction::AccessListObj GetSampleAcList_1Addr_1LongKey()
{
	Transaction::AccessListItem accessListItem;
	accessListItem.get_Address() = SimpleObjects::Bytes({
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x01U,
	});
	accessListItem.get_StorageKeys().push_back(SimpleObjects::Bytes({
		0x01U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
	}));
	Transaction::AccessListObj accessList = {
		accessListItem
	};
	return accessList;
}


GTEST_TEST(TestEthTxnDynFee, ValidateAccessList)
{
	{
		Transaction::AccessListObj accessListEmpty =
			GetSampleAcList_0Addr_0Key();
		EXPECT_NO_THROW(
			Transaction::ValidateAccessListObj(accessListEmpty)
		);
	}

	{
		Transaction::AccessListObj accessListEmptyStKeys =
			GetSampleAcList_1Addr_0Key();
		EXPECT_NO_THROW(
			Transaction::ValidateAccessListObj(accessListEmptyStKeys)
		);
	}

	{
		Transaction::AccessListObj accessListEmptyStKeys =
			GetSampleAcList_1Addr_1Key();
		EXPECT_NO_THROW(
			Transaction::ValidateAccessListObj(accessListEmptyStKeys)
		);
	}

	{
		Transaction::AccessListObj accessListInvalidAddr =
			GetSampleAcList_1LongAddr();
		EXPECT_THROW_MSG(
			Transaction::ValidateAccessListObj(accessListInvalidAddr),
			EclipseMonitor::Exception,
			"Invalid access list address size"
		);
	}

	{
		Transaction::AccessListObj accessListInvalidKey =
			GetSampleAcList_1Addr_1LongKey();
		EXPECT_THROW_MSG(
			Transaction::ValidateAccessListObj(accessListInvalidKey),
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
		Transaction::AccessListObj accessList = GetSampleAcList_1Addr_1Key();

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

		auto rlp = dynFeeTxn.RlpSerializeUnsigned();
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


	{
		uint64_t chainID = 634ULL;
		uint64_t nonce = 814370ULL;
		uint64_t maxPriorFeePerGas = 98765432154321ULL;
		uint64_t maxFeePerGas = 987654321ULL;
		uint64_t gasLimit = 123456ULL;
		SimpleObjects::Bytes destination = {
			// 0x09616C3d61b3331fc4109a9E41a8BDB7d9776609
			0x09U, 0x61U, 0x6cU, 0x3dU, 0x61U, 0xb3U, 0x33U, 0x1fU,
			0xc4U, 0x10U, 0x9aU, 0x9eU, 0x41U, 0xa8U, 0xbdU, 0xb7U,
			0xd9U, 0x77U, 0x66U, 0x09U,
		};
		uint64_t amount = 28957ULL;
		SimpleObjects::Bytes blkData = {
			// 0x9879ab123d274ef5
			0x98U, 0x79U, 0xabU, 0x12U, 0x3dU, 0x27U, 0x4eU, 0xf5U,
		};
		Transaction::AccessListObj accessList = GetSampleAcList_1Addr_0Key();

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
			// f85082027a830c6d228659d39e7fe8d1843ade68b18301e2409409616c3d61b3
			0xf8U, 0x50U, 0x82U, 0x02U, 0x7aU, 0x83U, 0x0cU, 0x6dU,
			0x22U, 0x86U, 0x59U, 0xd3U, 0x9eU, 0x7fU, 0xe8U, 0xd1U,
			0x84U, 0x3aU, 0xdeU, 0x68U, 0xb1U, 0x83U, 0x01U, 0xe2U,
			0x40U, 0x94U, 0x09U, 0x61U, 0x6cU, 0x3dU, 0x61U, 0xb3U,
			// 331fc4109a9e41a8bdb7d977660982711d889879ab123d274ef5d7d694000000
			0x33U, 0x1fU, 0xc4U, 0x10U, 0x9aU, 0x9eU, 0x41U, 0xa8U,
			0xbdU, 0xb7U, 0xd9U, 0x77U, 0x66U, 0x09U, 0x82U, 0x71U,
			0x1dU, 0x88U, 0x98U, 0x79U, 0xabU, 0x12U, 0x3dU, 0x27U,
			0x4eU, 0xf5U, 0xd7U, 0xd6U, 0x94U, 0x00U, 0x00U, 0x00U,
			// 0000000000000000000000000000000001c0
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x01U, 0xc0U,
		};
		std::cout << "Expected RLP:  " <<
			SimpleObjects::Codec::Hex::Encode<std::string>(expRlp) << std::endl;

		auto rlp = dynFeeTxn.RlpSerializeUnsigned();
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
			// ac8785206b5b48c55ec9ccd796282dad8d8669cb91dde26e4ce424ccbdcbd0e0
			0xacU, 0x87U, 0x85U, 0x20U, 0x6bU, 0x5bU, 0x48U, 0xc5U,
			0x5eU, 0xc9U, 0xccU, 0xd7U, 0x96U, 0x28U, 0x2dU, 0xadU,
			0x8dU, 0x86U, 0x69U, 0xcbU, 0x91U, 0xddU, 0xe2U, 0x6eU,
			0x4cU, 0xe4U, 0x24U, 0xccU, 0xbdU, 0xcbU, 0xd0U, 0xe0U,
		};
		auto hash = dynFeeTxn.Hash();
		EXPECT_EQ(hash, rlpDecoded.Hash());
		EXPECT_EQ(hash, expHash);
	}


	{
		uint64_t chainID = 3948ULL;
		uint64_t nonce = 25169ULL;
		uint64_t maxPriorFeePerGas = 2956132109347ULL;
		uint64_t maxFeePerGas = 14067925928ULL;
		uint64_t gasLimit = 2563498ULL;
		SimpleObjects::Bytes destination = {
			// 0x09616C3d61b3331fc4109a9E41a8BDB7d9776609
			0x09U, 0x61U, 0x6cU, 0x3dU, 0x61U, 0xb3U, 0x33U, 0x1fU,
			0xc4U, 0x10U, 0x9aU, 0x9eU, 0x41U, 0xa8U, 0xbdU, 0xb7U,
			0xd9U, 0x77U, 0x66U, 0x09U,
		};
		uint64_t amount = 1532891978124ULL;
		SimpleObjects::Bytes blkData = {
			// 0x5789a7b3fe8d
			0x57U, 0x89U, 0xa7U, 0xb3U, 0xfeU, 0x8dU,
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

		EXPECT_EQ(dynFeeTxn.GetChainID(), chainID);
		EXPECT_EQ(dynFeeTxn.GetNonce(), nonce);
		EXPECT_EQ(dynFeeTxn.GetMaxPriorFeePerGas(), maxPriorFeePerGas);
		EXPECT_EQ(dynFeeTxn.GetMaxFeePerGas(), maxFeePerGas);
		EXPECT_EQ(dynFeeTxn.GetGasLimit(), gasLimit);
		EXPECT_EQ(dynFeeTxn.get_Destination(), destination);
		EXPECT_EQ(dynFeeTxn.GetAmount(), amount);
		EXPECT_EQ(dynFeeTxn.get_Data(), blkData);
		EXPECT_EQ(dynFeeTxn.get_AccessList(), GetSampleAcList_0Addr_0Key());

		std::vector<uint8_t> expRlp = {
			// f83b820f6c8262518602b047344c238503468383a883271daa9409616c3d61b3
			0xf8U, 0x3bU, 0x82U, 0x0fU, 0x6cU, 0x82U, 0x62U, 0x51U,
			0x86U, 0x02U, 0xb0U, 0x47U, 0x34U, 0x4cU, 0x23U, 0x85U,
			0x03U, 0x46U, 0x83U, 0x83U, 0xa8U, 0x83U, 0x27U, 0x1dU,
			0xaaU, 0x94U, 0x09U, 0x61U, 0x6cU, 0x3dU, 0x61U, 0xb3U,
			// 331fc4109a9e41a8bdb7d9776609860164e77b598c865789a7b3fe8dc0
			0x33U, 0x1fU, 0xc4U, 0x10U, 0x9aU, 0x9eU, 0x41U, 0xa8U,
			0xbdU, 0xb7U, 0xd9U, 0x77U, 0x66U, 0x09U, 0x86U, 0x01U,
			0x64U, 0xe7U, 0x7bU, 0x59U, 0x8cU, 0x86U, 0x57U, 0x89U,
			0xa7U, 0xb3U, 0xfeU, 0x8dU, 0xc0U,
		};
		std::cout << "Expected RLP:  " <<
			SimpleObjects::Codec::Hex::Encode<std::string>(expRlp) << std::endl;

		auto rlp = dynFeeTxn.RlpSerializeUnsigned();
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
		EXPECT_EQ(rlpDecoded.get_AccessList(), GetSampleAcList_0Addr_0Key());

		std::array<uint8_t, 32> expHash = {
			// 26eaa1e0f350d89325edfd64b52dfd955fcdc56b85b157c50191d7abcf2f579a
			0x26U, 0xeaU, 0xa1U, 0xe0U, 0xf3U, 0x50U, 0xd8U, 0x93U,
			0x25U, 0xedU, 0xfdU, 0x64U, 0xb5U, 0x2dU, 0xfdU, 0x95U,
			0x5fU, 0xcdU, 0xc5U, 0x6bU, 0x85U, 0xb1U, 0x57U, 0xc5U,
			0x01U, 0x91U, 0xd7U, 0xabU, 0xcfU, 0x2fU, 0x57U, 0x9aU,
		};
		auto hash = dynFeeTxn.Hash();
		EXPECT_EQ(hash, rlpDecoded.Hash());
		EXPECT_EQ(hash, expHash);
	}
}

