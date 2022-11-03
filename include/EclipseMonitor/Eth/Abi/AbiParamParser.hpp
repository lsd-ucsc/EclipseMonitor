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

#include "AbiParam.hpp"
#include "../../Exceptions.hpp"

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

	static std::vector<std::string> SplitParams(
		std::string paramStr, char delimiter
	)
	{
		std::istringstream ss(paramStr);
		std::string token;

		std::vector<std::string> params;
		while (std::getline(ss, token, delimiter))
		{
			params.push_back(token);
		}
		return params;
	}

	static std::vector<std::unique_ptr<AbiParam>> ParseParams(
		const std::string& paramStr
	)
	{
		int openBraceIndex = paramStr.find("(");
		int closeBraceIndex = paramStr.find(")");
		std::string delimitedParams = paramStr.substr(
			openBraceIndex + 1, closeBraceIndex - openBraceIndex);

		std::vector<std::string> params = SplitParams(delimitedParams, ',');
		std::vector<std::unique_ptr<AbiParam>> parsedParams;
		for (const std::string& param : params)
		{
			std::unique_ptr<AbiParam> abiParam = AbiParam::ParseParam(param);
			parsedParams.push_back(std::move(abiParam));
		}
		return parsedParams;
	}

};

} // namespace Abi
} // namespace Eth
} // namespace EclipseMonitor