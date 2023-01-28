// Copyright (c) 2023 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <cstdint>
#include <vector>

namespace EclipseMonitor_Test
{

const std::vector<uint8_t>& GetEthHeader_15001871();
const std::vector<uint8_t>& GetEthHeader_TestBlock_1();
const std::vector<uint8_t>& GetEthHeader_TestBlock_2();

} // namespace EclipseMonitor_Test
