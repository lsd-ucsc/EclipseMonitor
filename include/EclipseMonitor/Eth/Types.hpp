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


using ContractAddr = std::array<uint8_t, 20>;

using EventTopic = std::array<uint8_t, 32>;

using EventCallbackId = std::uintptr_t;


} // namespace Eth
} // namespace EclipseMonitor
