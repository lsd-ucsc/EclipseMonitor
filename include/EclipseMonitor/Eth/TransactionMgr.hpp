// Copyright (c) 2022 Haofan Zheng, Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <cstdint>
#include <tuple>

#include "../Internal/SimpleObj.hpp"
#include "../Internal/SimpleRlp.hpp"


namespace EclipseMonitor
{
namespace Eth
{


enum class TxnVersion : uint8_t
{
	Legacy  = 0,
	AccessList = 1,
	DynamicFee = 2,
}; // enum class TnxVersion


class TransactionMgr
{
public: // static members:

	static std::tuple<TxnVersion, Internal::Obj::Object> ParseTnx(
		const Internal::Obj::Bytes& rlpBytes
	)
	{
		TxnVersion version;
		uint8_t firstByte = rlpBytes[0];
		Internal::Obj::Bytes innerRlp;
		if (firstByte == 0x01)
		{
			version = TxnVersion::AccessList;
			innerRlp = Internal::Obj::Bytes(rlpBytes.data() + 1, rlpBytes.data() + rlpBytes.size());
		}
		else if (firstByte == 0x02)
		{
			version = TxnVersion::DynamicFee;
			innerRlp = Internal::Obj::Bytes(rlpBytes.data() + 1, rlpBytes.data() + rlpBytes.size());
		}
		else
		{
			version = TxnVersion::Legacy;
			innerRlp = rlpBytes;
		}

		Internal::Obj::Object txObj
			= Internal::Rlp::ParseRlp(innerRlp.GetVal());
		return std::make_tuple(version, txObj);
	}


	static TransactionMgr FromBytes(
		const Internal::Obj::Bytes& rlpBytes
	)
	{
		TxnVersion ver;
		Internal::Obj::Object txnObj;
		std::tie(ver, txnObj) = ParseTnx(rlpBytes);

		Internal::Obj::ListBaseObj &txnListObj = txnObj.AsList();

		return TransactionMgr(ver, txnObj, txnListObj);
	}


public:

	TransactionMgr(
		TxnVersion version,
		const Internal::Obj::Object& txnObj,
		Internal::Obj::ListBaseObj& txnListObj
	) :
		m_version(version),
		m_txnObj(txnObj),
		m_txnBody(txnListObj)
	{
		if (m_version == TxnVersion::Legacy)
		{
			Internal::Obj::BytesBaseObj& addr = m_txnBody[3].AsBytes();
			m_contractAddr = Internal::Obj::Bytes(addr.data(), addr.data() + addr.size());

			Internal::Obj::BytesBaseObj& data = m_txnBody[5].AsBytes();
			m_data = Internal::Obj::Bytes(data.data(), data.data() + data.size());
		}
		else if (m_version == TxnVersion::AccessList)
		{
			Internal::Obj::BytesBaseObj& addr = m_txnBody[4].AsBytes();
			m_contractAddr = Internal::Obj::Bytes(addr.data(), addr.data() + addr.size());

			Internal::Obj::BytesBaseObj& data = m_txnBody[6].AsBytes();
			m_data = Internal::Obj::Bytes(data.data(), data.data() + data.size());
		}
		else if (m_version == TxnVersion::DynamicFee)
		{
			Internal::Obj::BytesBaseObj& addr = m_txnBody[5].AsBytes();
			m_contractAddr = Internal::Obj::Bytes(addr.data(), addr.data() + addr.size());

			Internal::Obj::BytesBaseObj& data = m_txnBody[7].AsBytes();
			m_data = Internal::Obj::Bytes(data.data(), data.data() + data.size());
		}
	};

	// LCOV_EXCL_START
	~TransactionMgr() = default;
	// LCOV_EXCL_STOP

	const Internal::Obj::Bytes& GetContractAddr() const
	{
		return m_contractAddr;
	}

	const Internal::Obj::Bytes& GetContactParams() const
	{
		return m_data;
	}

private:
	TxnVersion m_version;
	Internal::Obj::Bytes m_contractAddr;
	Internal::Obj::Bytes m_data;
	Internal::Obj::Object m_txnObj;
	Internal::Obj::ListBaseObj& m_txnBody;
};


} // namespace Eth
} // namespace EclipseMonitor