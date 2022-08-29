// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <cstdint>

#include <array>

#include "MonitorReport.hpp"

namespace EclipseMonitor
{

class SyncMsgMgr
{
public: // static members:
	using NonceType = std::array<uint8_t, 32>;
	using sesIdType = typename MonitorId::SessionIdType;

public:
	/**
	 * @brief Construct a new sync message manager object
	 *
	 * @param mId reference to the monitor ID
	 * @param mConf Configuration of the monitor
	 * @param time current timestamp, used to mark the time when the nonce
	 *             is generated
	 */
	SyncMsgMgr(const MonitorId& mId, const MonitorConfig& mConf, uint64_t time) :
		m_mId(mId),
		m_mConf(mConf),
		m_nonce(), // TODO - Haofan: generate random nonce
		m_time(time)
	{}

	virtual ~SyncMsgMgr() = default;

	const sesIdType& GetSessionId() const
	{
		return m_mId.m_sid;
	}

	const NonceType& GetNonce() const
	{
		return m_nonce;
	}

	uint64_t GetTimestamp() const
	{
		return m_time;
	}

protected:

	/**
	 * @brief Validate the message published on chain
	 *
	 * @param inNonce   The nonce found in the block transaction
	 * @param recvTime  The timestamp when the block is received
	 * @return true if the message is validate, otherwise, false, and a re-sync
	 *         process is needed
	 */
	bool ValidateMsg(const NonceType& inNonce, uint64_t recvTime) const
	{
		return (
			(inNonce == m_nonce) && // Nonces are match
			(recvTime >= m_time) && // recv time is after generate time
			((recvTime - m_time) <= m_mConf.m_syncMaxWaitTime) // time elapsed within limit
		);
	}

private:
	const MonitorId&        m_mId;   // <- reference to the monitor ID, containing the session ID
	const MonitorConfig&    m_mConf; // <- reference to configuration of the monitor, containing the m_syncMaxWaitTime
	NonceType               m_nonce; // <- 256-bit nonce
	uint64_t                m_time;  // <- The timestamp when the nonce is generated
}; // class SyncMsgMgr

} // namespace EclipseMonitor
