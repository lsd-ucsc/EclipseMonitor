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
#include "AbiParamType.hpp"
#include "AbiInputParser.hpp"

namespace EclipseMonitor
{
namespace Eth
{
namespace Abi
{

class AbiParamParser
{
public:
	AbiParamParser(std::vector<std::unique_ptr<AbiParam>> params) :
		m_params(std::move(params))
	{}

	static AbiParamParser ParseParams(
		const std::string& paramStr,
		Internal::Obj::Bytes& inputBytes,
		bool functionHeader
	)
	{
		auto paramTypes = AbiParamType::ParseParamType(paramStr);
		auto abiParams = AbiInputParser::ParseInput(
				std::move(paramTypes), inputBytes, functionHeader);

		return AbiParamParser(std::move(abiParams));
	}

	std::vector<std::unique_ptr<AbiParam>>& GetParams()
	{
		return m_params;
	}

private:
	std::vector<std::unique_ptr<AbiParam>> m_params;

};

} // namespace Abi
} // namespace Eth
} // namespace EclipseMonitor
