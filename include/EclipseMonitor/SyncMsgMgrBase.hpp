// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <cstdint>

#include <array>
#include <atomic>
#include <memory>

#include "DataTypes.hpp"
#include "Exceptions.hpp"
#include "MonitorReport.hpp"
#include "PlatformInterfaces.hpp"

namespace EclipseMonitor
{

class SyncState
{
public:

	SyncState(
		TrustedTimestamp maxWaitTime,
		const TimestamperBase& timestamper,
		const RandomGeneratorBase& randGen
	) :
		m_maxWaitTime(maxWaitTime),
		m_genTime(),
		m_nonce(),
		m_isSynced(false)
	{
		// generate a timestamp for this nonce
		m_genTime = timestamper.NowInSec();
		// generate a random nonce
		randGen.GenerateRandomBytes(m_nonce.data(), m_nonce.size());
	}

	~SyncState() = default;

	void SetSynced(TrustedTimestamp recvTime)
	{
		TrustedTimestamp deltaT = recvTime - m_genTime;
		// ensure the sync msg is received within the max wait time
		if (deltaT <= m_maxWaitTime)
		{
			m_isSynced.store(true);
		}
	}

	bool IsSynced() const
	{
		return m_isSynced.load();
	}

	const SyncNonce& GetNonce() const
	{
		return m_nonce;
	}

private:

	TrustedTimestamp m_maxWaitTime;
	TrustedTimestamp m_genTime;
	SyncNonce m_nonce;
	std::atomic_bool m_isSynced;
}; // class SyncState

class SyncMsgMgrBase
{
public: // static members:

#if __cplusplus < 202002L
	using AtomicSyncStateType = std::shared_ptr<SyncState>;
#else
	using AtomicSyncStateType = std::atomic<std::shared_ptr<SyncState> >;
#endif // __cplusplus < 202002L

public:

	/**
	 * @brief Construct a new sync message manager object
	 *
	 * @param mId    reference to the monitor ID
	 * @param mConf  Configuration of the monitor
	 * @param timestamper reference to the timestamper
	 * @param randGen     reference to the random generator
	 */
	SyncMsgMgrBase(
		const MonitorId& mId,
		const MonitorConfig& mConf,
		const TimestamperBase& timestamper,
		const RandomGeneratorBase& randGen
	) :
		m_maxWaitTime(mConf.get_syncMaxWaitTime().GetVal()),
		m_sessId(GetSessIdFromVec(mId.get_sessionID().GetVal())),
		m_lastSyncState(BuildSyncState(timestamper, randGen))
	{}

	// LCOV_EXCL_START
	virtual ~SyncMsgMgrBase() = default;
	// LCOV_EXCL_STOP

	std::shared_ptr<SyncState> GetLastSyncState()
	{
		return AtomicGetSyncState();
	}

	const SessionID& GetSessionID() const
	{
		return m_sessId;
	}

	virtual std::shared_ptr<SyncState> NewSyncState(
		const TimestamperBase& timestamper,
		const RandomGeneratorBase& randGen
	) = 0;

protected:

	std::shared_ptr<SyncState> BuildSyncState(
		const TimestamperBase& timestamper,
		const RandomGeneratorBase& randGen
	)
	{
		return std::make_shared<SyncState>(
			m_maxWaitTime,
			timestamper,
			randGen
		);
	}

	std::shared_ptr<SyncState> AtomicGetSyncState()
	{
#if __cplusplus < 202002L
		return std::atomic_load(&m_lastSyncState);
#else
		return m_lastSyncState.load();
#endif // __cplusplus < 202002L
	}

	void AtomicSetSyncState(std::shared_ptr<SyncState> syncState)
	{
#if __cplusplus < 202002L
		std::atomic_store(&m_lastSyncState, syncState);
#else
		m_lastSyncState.store(syncState);
#endif // __cplusplus < 202002L
	}

private: // helper functions:

	static SessionID GetSessIdFromVec(
		const std::vector<uint8_t>& sessIdVec
	)
	{
		if (sessIdVec.size() != std::tuple_size<SessionID>::value)
		{
			throw Exception("Invalid session ID size");
		}
		SessionID sessId;
		std::copy(
			sessIdVec.begin(),
			sessIdVec.end(),
			sessId.begin()
		);
		return sessId;
	}

private:

	const TrustedTimestamp m_maxWaitTime;
	const SessionID m_sessId;
	AtomicSyncStateType m_lastSyncState;
}; // class SyncMsgMgrBase

} // namespace EclipseMonitor
