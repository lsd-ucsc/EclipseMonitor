// Copyright (c) 2022 Haofan Zheng, Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <cstdint>

#include <tuple>

#include "../Internal/SimpleObj.hpp"
#include "../Internal/SimpleRlp.hpp"
#include "../Exceptions.hpp"


namespace EclipseMonitor
{
namespace Eth
{


enum class TxnVersion : uint8_t
{
	Legacy     = 0,
	AccessList = 1,
	DynamicFee = 2,
}; // enum class TxnVersion


class TransactionMgr
{
public: // static members:

	static std::tuple<TxnVersion, Internal::Obj::Object> ParseTnx(
		const Internal::Obj::BytesBaseObj& rlpBytes
	)
	{
		TxnVersion version = TxnVersion::Legacy;
		Internal::Obj::Object txObj;

		uint8_t firstByte = rlpBytes[0];

		auto itBegin = rlpBytes.begin();
		auto itEnd = rlpBytes.end();
		size_t size = rlpBytes.size();

		if (firstByte == 0x01)
		{
			version = TxnVersion::AccessList;
			++itBegin;
			--size;
		}
		else if (firstByte == 0x02)
		{
			version = TxnVersion::DynamicFee;
			++itBegin;
			--size;
		}

		using _FrItType = typename Internal::Rlp::GeneralParser::IteratorType;
		txObj = Internal::Rlp::GeneralParser().Parse(
			_FrItType(itBegin.CopyPtr()),
			_FrItType(itEnd.CopyPtr()),
			size
		);

		return std::make_tuple(version, txObj);
	}


	static TransactionMgr FromBytes(
		const Internal::Obj::BytesBaseObj& rlpBytes
	)
	{
		TxnVersion ver;
		Internal::Obj::Object txnObj;
		std::tie(ver, txnObj) = ParseTnx(rlpBytes);

		return TransactionMgr(ver, std::move(txnObj));
	}


public:

	TransactionMgr(
		TxnVersion version,
		Internal::Obj::Object txnObj
	) :
		m_version(version),
		m_txnObj(std::move(txnObj)),
		m_txnBody(m_txnObj.AsList()),
		m_contractAddr(GetContractAddrRef(m_version, m_txnBody)),
		m_data(GetContractParamRef(m_version, m_txnBody))
	{}

	// LCOV_EXCL_START
	~TransactionMgr() = default;
	// LCOV_EXCL_STOP

	const Internal::Obj::BytesBaseObj& GetContractAddr() const
	{
		return m_contractAddr;
	}

	const Internal::Obj::BytesBaseObj& GetContactParams() const
	{
		return m_data;
	}


private: // static members:

	static Internal::Obj::BytesBaseObj& GetContractAddrRef(
		TxnVersion version,
		Internal::Obj::ListBaseObj& txnBody
	)
	{
		return
			(version == TxnVersion::Legacy     ? txnBody[3].AsBytes() :
			(version == TxnVersion::AccessList ? txnBody[4].AsBytes() :
			(version == TxnVersion::DynamicFee ? txnBody[5].AsBytes() :
				throw Exception("Invalid transaction version"))));
	}

	static Internal::Obj::BytesBaseObj& GetContractParamRef(
		TxnVersion version,
		Internal::Obj::ListBaseObj& txnBody
	)
	{
		return
			(version == TxnVersion::Legacy     ? txnBody[5].AsBytes() :
			(version == TxnVersion::AccessList ? txnBody[6].AsBytes() :
			(version == TxnVersion::DynamicFee ? txnBody[7].AsBytes() :
				throw Exception("Invalid transaction version"))));
	}

private:

	TxnVersion m_version;
	Internal::Obj::Object m_txnObj;
	Internal::Obj::ListBaseObj& m_txnBody;
	Internal::Obj::BytesBaseObj& m_contractAddr;
	Internal::Obj::BytesBaseObj& m_data;

}; // class TransactionMgr


} // namespace Eth
} // namespace EclipseMonitor