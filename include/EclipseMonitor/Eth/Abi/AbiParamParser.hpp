// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <SimpleObjects/Internal/make_unique.hpp>

#include "../../Exceptions.hpp"
#include "AbiParam.hpp"

namespace EclipseMonitor
{
namespace Eth
{
namespace Abi
{

class AbiParamParser
{
public:
	AbiParamParser() = default;

	// static void ParseParams(
	// 	const std::string& paramStr,
	// 	Internal::Obj::Bytes& input
	// )
	// {
	// 	// std::vector<std::unique_ptr<AbiParam>> params;

	// }

};

} // namespace Abi
} // namespace Eth
} // namespace EclipseMonitor
