// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <cstdint>

#include <vector>

namespace EclipseMonitor_Test
{

const std::vector<uint8_t>& GetEthHeaderBin_1_ErrNum();

const std::vector<uint8_t>& GetEthHeaderBin_1_ErrParentHash();

const std::vector<uint8_t>& GetEthHeaderBin_1_ErrDiffVal();

} // namespace EclipseMonitor_Test
