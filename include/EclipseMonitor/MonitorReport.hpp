// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <cstdint>

#include <array>

/**
 * @brief Eclipse Monitor Secure Version Number (SVN) - Upper 1 byte
 *
 */
#define ECLIPSEMONITOR_SVN_UPPER 0x01U
/**
 * @brief Eclipse Monitor Secure Version Number (SVN) - Lower 2 bytes
 *
 */
#define ECLIPSEMONITOR_SVN_LOWER 0x0000U

namespace EclipseMonitor
{

/**
 * @brief Data used to identify the eclipse monitor instance, and it's
 *        not critical to security considerations
 *
 */
struct MonitorId
{
	using SessionIdType = std::array<uint8_t, 16>;

	/**
	 * @brief The 128-bit session ID (randomly generated) used to identify
	 *        a specific monitor process runtime
	 *
	 */
	std::array<uint8_t, 16> m_sid;

}; // struct MonitorId


/**
 * @brief Configurations of eclipse monitor, and it's *critical* to
 *        security considerations
 *
 */
struct MonitorConfig
{
	/**
	 * @brief The number of blocks in each checkpoint window
	 *
	 */
	uint64_t m_checkpointSize;

	/**
	 * @brief The "percentage" value used to calculate the minimum difficulty
	 *        value. In order to calculate the minimum difficulty value in a
	 *        integer value system, it's using the formula
	 *        "diff_min = (diff_median >> 7) * m_minDiffPercent", which is
	 *        approximately "diff_min = diff_median * (m_minDiffPercent / 128)"
	 *
	 */
	uint8_t  m_minDiffPercent;

	/**
	 * @brief The maximum time (in seconds) to wait for a new block. After this
	 *        time has passed since the creation of a block, that block will
	 *        stop to accept any new block to be its direct child, and it
	 *        will be marked as inactive.
	 *
	 */
	uint64_t m_maxWaitTime;

	/**
	 * @brief The maximum time (in seconds) to wait for the sync message to be
	 *        published on the blockchain. If the sync message is detected after
	 *        this time, the sync process is unsuccessful, and a re-sync will be
	 *        needed.
	 *
	 */
	uint64_t m_syncMaxWaitTime;

}; // struct MonitorConfig


inline MonitorConfig BuildDefaultMonitorConfig()
{
	MonitorConfig conf;
	conf.m_checkpointSize  = 430;
	conf.m_minDiffPercent  = 103; // which is around 80%
	conf.m_maxWaitTime     = 400;
	conf.m_syncMaxWaitTime = 13;

	return conf;
}

} // namespace EclipseMonitor
