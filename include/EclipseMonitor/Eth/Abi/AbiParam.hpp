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

class AbiParam
{
public:
	AbiParam(ParamType type, bool isStatic, bool isArray) :
		m_type(type),
		m_isStatic(isStatic),
		m_isArray(isArray)
	{}

	static std::unique_ptr<AbiParam> ParseParam(const std::string& param)
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

		return SimpleObjects::Internal::make_unique<AbiParam>(type, isStatic, isArray);
	}

	ParamType const& GetType()
	{
		return m_type;
	}

	bool const& IsStatic()
	{
		return m_isStatic;
	}

	bool const& IsArray()
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