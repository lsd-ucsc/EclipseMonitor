// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <cstdint>

#include <memory>
#include <string>
#include <vector>

#include "DataTypes.hpp"
#include "MonitorReport.hpp"
#include "PlatformInterfaces.hpp"

namespace EclipseMonitor
{

enum class Phases
{
	BootstrapI,
	BootstrapII,
	Sync,
	Runtime,
}; // enum class Phases


class EclipseMonitorBase
{
public: // Static members:

	using TimestamperType = std::unique_ptr<TimestamperBase>;
	using RandomGeneratorType = std::unique_ptr<RandomGeneratorBase>;

public:

	EclipseMonitorBase(
		const MonitorConfig& conf,
		const std::string& chainName,
		TimestamperType timestamper,
		RandomGeneratorType randGen
	) :
		m_mConfig(conf),
		m_mId(),
		m_mSecState(),
		m_phase(Phases::BootstrapI),
		m_timestamper(std::move(timestamper)),
		m_randGen(std::move(randGen))
	{
		m_mSecState.get_chainName() = chainName;

		// generate a random session ID
		SessionID tmpId;
		m_randGen->GenerateRandomBytes(tmpId.data(), tmpId.size());
		std::vector<uint8_t> tmpIdVec(tmpId.begin(), tmpId.end());
		m_mId.get_sessionID() = std::move(tmpIdVec);
	}

	virtual ~EclipseMonitorBase() = default;

	/**
	 * @brief Re-generate the sync nonce and the timestamp that marks
	 *        the beginning of the sync process
	 *        NOTE: a call to this function is needed at the beginning of EACH
	 *        sync process, even the first one, otherwise, the beginning
	 *        timestamp will be zero and the sync process will be failed
	 *        eventually.
	 *
	 */
	// virtual std::vector<uint8_t> ResetSyncNonce()
	// {
	// 	m_syncStartTime = m_timestamper.NowInSec();

	// 	// TODO: generate nonce
	// }

	const MonitorConfig& GetMonitorConfig() const
	{
		return m_mConfig;
	}

	const MonitorSecState& GetMonitorSecState() const
	{
		return m_mSecState;
	}

	virtual void Update(const std::vector<uint8_t>& hdrBinary) = 0;

	virtual void EndBootstrapI()
	{
		m_phase = Phases::BootstrapII;
	}

	Phases GetPhase() const
	{
		return m_phase;
	}

protected:

	TimestamperBase& GetTimestamper()
	{
		return *m_timestamper;
	}

	MonitorSecState& GetMonitorSecState()
	{
		return m_mSecState;
	}

	// void FinishBootstrapII()
	// {
	// 	m_phase = Phases::Sync;
	// }

	// void FinishSync()
	// {
	// 	m_phase = Phases::Runtime;
	// }

private:

	MonitorConfig       m_mConfig;
	MonitorId           m_mId;
	MonitorSecState     m_mSecState;
	Phases              m_phase;
	TimestamperType     m_timestamper;
	RandomGeneratorType m_randGen;

}; // class EclipseMonitorBase

} // namespace EclipseMonitor
