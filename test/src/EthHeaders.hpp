// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <cstdint>

#include <array>

namespace EclipseMonitor_Test
{

const std::array<uint8_t, 535>& GetEthHeaderBin_0();

const std::array<uint8_t, 532>& GetEthHeaderBin_1();

const std::array<uint8_t, 532>& GetEthHeaderBin_1_ErrNum();

const std::array<uint8_t, 532>& GetEthHeaderBin_1_ErrParentHash();

const std::array<uint8_t, 532>& GetEthHeaderBin_1_ErrDiffVal();

} // namespace EclipseMonitor_Test
