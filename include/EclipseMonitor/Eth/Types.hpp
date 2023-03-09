// Copyright (c) 2023 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once


#include <cstdint>

#include <array>


namespace EclipseMonitor
{
namespace Eth
{


using ContractAddrType = std::array<uint8_t, 20>;
using EventTopicType = std::array<uint8_t, 32>;


} // namespace Eth
} // namespace EclipseMonitor
