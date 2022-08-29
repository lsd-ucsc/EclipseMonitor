// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

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

	bool ValidateBlock(const EthHeaderMgr& header) const
	{
		Base::NonceType nonce;
		// TODO : validate the transaction and retrieve the nonce from the transaction
		return Base::ValidateMsg(nonce, header.GetTrustedTime());
	}

private:
	ContractAddrType m_contractAddr;
}; // class EthSyncMsgMgr

} // namespace EclipseMonitor
