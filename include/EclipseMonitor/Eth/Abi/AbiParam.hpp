// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <string>

#include <SimpleObjects/Internal/make_unique.hpp>
#include <SimpleRlp/SimpleRlp.hpp>

#include "AbiParamType.hpp"

namespace EclipseMonitor
{
namespace Eth
{
namespace Abi
{

class AbiParam
{
public:
	AbiParam(
		std::unique_ptr<AbiParamType> type,
		Internal::Obj::Object input
	) :
		m_abiparamtype(std::move(type)),
		m_input(input)
	{}

	std::unique_ptr<AbiParamType>& GetAbiParamType()
	{
		return m_abiparamtype;
	}

	Internal::Obj::Object& GetInput()
	{
		return m_input;
	}

private:
	std::unique_ptr<AbiParamType> m_abiparamtype;
	Internal::Obj::Object m_input;
};

} // namespace Abi
} // namespace Eth
} // namespace EclipseMonitor
