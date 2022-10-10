// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma  once

#include <vector>

#include "EclipseMonitor/Internal/SimpleObj.hpp"

using namespace SimpleObjects;

namespace EclipseMonitor_Test
{

const std::vector <SimpleObjects::Bytes>& GetEthReceipts_15415840();

const SimpleObjects::Bytes& GetReceiptsRoot_15415840();

const std::vector <SimpleObjects::Bytes>& GetEthReceipts_15209997();

const SimpleObjects::Bytes& GetReceiptsRoot_15209997();

}