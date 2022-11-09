// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <string>

#include <SimpleObjects/Internal/make_unique.hpp>

#include "../../Exceptions.hpp"

namespace EclipseMonitor
{

namespace Eth
{

namespace Abi
{

enum class ParamType : uint8_t
{
	Uint64  = 0,
	Bool = 1,
	Bytes = 2,
	Bytes32 = 3
}; // enum class ParamType

class AbiParamType
{
public:
	AbiParamType(ParamType type, bool isStatic, bool isArray) :
		m_type(type),
		m_isStatic(isStatic),
		m_isArray(isArray)
	{}



	static std::unique_ptr<AbiParamType> ParseParamString(const std::string& param)
	{
		ParamType type;
		bool isStatic = true;
		bool isArray = false;

		if (param.find("uint") != std::string::npos)
		{
			type = ParamType::Uint64;
		}
		else if (param.find("bool") != std::string::npos)
		{
			type = ParamType::Bool;
		}
		else if (param.find("bytes32") != std::string::npos)
		{
			type = ParamType::Bytes32;
		}
		else if (param.find("bytes") != std::string::npos)
		{
			type = ParamType::Bytes;
			isStatic = false;
		}
		else
		{
			throw Exception("Unknown param type");
		}

		if (param.find("[") != std::string::npos)
		{
			isStatic = false;
			isArray = true;
		}

		return SimpleObjects::Internal::make_unique<AbiParamType>(type, isStatic, isArray);
	}


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

	static std::vector<std::unique_ptr<AbiParamType>> ParseParamType(
		const std::string& paramStr
	)
	{
		int openBraceIndex = paramStr.find("(");
		int closeBraceIndex = paramStr.find(")");
		std::string delimitedParams = paramStr.substr(
			openBraceIndex + 1, closeBraceIndex - openBraceIndex);

		std::vector<std::string> params = SplitParams(delimitedParams, ',');
		std::vector<std::unique_ptr<AbiParamType>> parsedParams;
		for (const std::string& param : params)
		{
			std::unique_ptr<AbiParamType> abiParam = ParseParamString(param);
			parsedParams.push_back(std::move(abiParam));
		}
		return parsedParams;
	}

	ParamType& GetType()
	{
		return m_type;
	}

	bool& IsStatic()
	{
		return m_isStatic;
	}

	bool& IsArray()
	{
		return m_isArray;
	}

private:
	ParamType m_type;
	bool m_isStatic;
	bool m_isArray;

}; // class AbiParam






} // namespace Abi
} // namespace Eth
} // namespace EclipseMonitor