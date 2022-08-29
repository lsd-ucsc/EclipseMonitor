// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <vector>

#include "Internal/SimpleObj.hpp"

#include "SyncMsgMgr.hpp"
#include "EthHeaderMgr.hpp"

namespace EclipseMonitor
{

class EthSyncMsgMgr : public SyncMsgMgr
{
public: // static member

	using Self = EthSyncMsgMgr;
	using Base = SyncMsgMgr;

	using ContractAddrType = std::array<uint8_t, 32>;

public:
	/**
	 * @brief Construct a new sync message manager object
	 *
	 * @param mId reference to the monitor ID
	 * @param mConf Configuration of the monitor
	 * @param time current timestamp, used to mark the time when the nonce
	 *             is generated
	 * @param contractAddr Ethereum contract address of the dummy smart contract
	 */
	EthSyncMsgMgr(
		const MonitorId& mId,
		const MonitorConfig& mConf,
		uint64_t time,
		ContractAddrType contractAddr) :
		SyncMsgMgr(mId, mConf, time),
		m_contractAddr(contractAddr)
	{}

	virtual ~EthSyncMsgMgr() = default;

	bool ValidateBlock(
		const EthHeaderMgr& header,
		const std::vector<std::vector<uint8_t> >& txBinList) const
	{
		Base::NonceType nonce;

		for (const auto& txBin : txBinList)
		{
			const Internal::Obj::BaseObj& tx = Internal::Rlp::ParseRlp(txBin);

			// TODO : validate the transaction and retrieve the nonce from the transaction
			if (tx.GetCategory() == Internal::Obj::ObjCategory::List)
			{
				// Legacy transaction
				// Skip or validate
				return Base::ValidateMsg(nonce, header.GetTrustedTime());
			}
			else if (tx.GetCategory() == Internal::Obj::ObjCategory::Bytes)
			{
				// EIP-2718 transaction
				const auto& outerRlp = tx.AsBytes();

				uint8_t firstByte = outerRlp[0];

				std::vector<uint8_t> innerRlp(
					outerRlp.data() + 1,
					outerRlp.data() + outerRlp.size());
				auto innerTx = Internal::Rlp::ParseRlp(innerRlp);

				if (firstByte == 0x01)
				{
					// EIP-2930 transaction
					// Skip or validate
					return Base::ValidateMsg(nonce, header.GetTrustedTime());
				}
				else if (firstByte == 0x02)
				{
					// EIP-1559 transaction
					return Base::ValidateMsg(nonce, header.GetTrustedTime());
				}
			}
		}

		return false;
	}

private:
	ContractAddrType m_contractAddr;
}; // class EthSyncMsgMgr

} // namespace EclipseMonitor
