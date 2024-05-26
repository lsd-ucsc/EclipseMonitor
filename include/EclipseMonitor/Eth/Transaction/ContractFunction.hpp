// Copyright (c) 2024 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once


#include <cstdint>

#include <string>
#include <tuple>
#include <vector>

#include "../../Internal/SimpleObj.hpp"
#include "../AbiCommon.hpp"
#include "../DataTypes.hpp"
#include "../Keccak256.hpp"
#include "DynamicFee.hpp"


namespace EclipseMonitor
{
namespace Eth
{
namespace Transaction
{

class ContractFuncBase
{
public:

	ContractFuncBase(ContractAddr contractAddr, std::string functionName) :
		m_contractAddr(contractAddr),
		m_functionName(functionName)
	{}

	ContractFuncBase(const ContractFuncBase&) = delete;

	ContractFuncBase(ContractFuncBase&& other) :
		m_contractAddr(std::move(other.m_contractAddr)),
		m_functionName(std::move(other.m_functionName))
	{}

	~ContractFuncBase() = default;

	ContractFuncBase& operator=(const ContractFuncBase&) = delete;

	ContractFuncBase& operator=(ContractFuncBase&& other)
	{
		if (this != &other)
		{
			m_contractAddr = std::move(other.m_contractAddr);
			m_functionName = std::move(other.m_functionName);
		}
		return *this;
	}

	const ContractAddr& GetContractAddr() const
	{
		return m_contractAddr;
	}

	const std::string& GetFuncName() const
	{
		return m_functionName;
	}

	template<typename _AbiWriter>
	std::vector<uint8_t> GetFuncSelector(_AbiWriter writer) const
	{
		std::string funcNameAndType =
			m_functionName + writer.GetTypeName();
		std::array<uint8_t, 32> funcSelectorHash = Keccak256(funcNameAndType);

		// the function selector is the first 4 bytes of the hash
		return {
			funcSelectorHash[0], funcSelectorHash[1],
			funcSelectorHash[2], funcSelectorHash[3]
		};
	}

	DynFee BuildTxn(
		const std::vector<uint8_t>& data
	) const
	{
		Transaction::DynFee dynFeeTxn;
		dynFeeTxn.get_Destination() =
			Internal::Obj::Bytes(m_contractAddr.begin(), m_contractAddr.end());
		dynFeeTxn.get_Data() = Internal::Obj::Bytes(data);
		return dynFeeTxn;
	}

protected:

	ContractAddr m_contractAddr;
	std::string m_functionName;

}; // class ContractFuncBase

template<typename _AbiWriter>
class ContractFuncStaticDef : public ContractFuncBase
{
public: // static members:

	using AbiWriter = _AbiWriter;

	using Base = ContractFuncBase;

public:
	ContractFuncStaticDef(ContractAddr contractAddr, std::string functionName) :
		Base(contractAddr, functionName),
		m_abiWriter()
	{}

	ContractFuncStaticDef(const ContractFuncStaticDef&) = delete;

	ContractFuncStaticDef(ContractFuncStaticDef&& other) :
		Base(std::forward<Base>(other)),
		m_abiWriter(std::move(other.m_abiWriter))
	{}

	~ContractFuncStaticDef() = default;

	ContractFuncStaticDef& operator=(const ContractFuncStaticDef&) = delete;

	ContractFuncStaticDef& operator=(ContractFuncStaticDef&& rhs)
	{
		Base::operator=(std::forward<Base>(rhs));
		if (this != &rhs)
		{
			m_abiWriter = std::move(rhs.m_abiWriter);
		}
		return *this;
	}

	const AbiWriter& GetAbiWriter() const
	{
		return m_abiWriter;
	}

	std::vector<uint8_t> GetFuncSelector() const
	{
		return Base::GetFuncSelector(m_abiWriter);
	}

	template<typename... _Args>
	DynFee CallByTxn(_Args&&... args) const
	{
		std::vector<uint8_t> data = GetFuncSelector();

		std::tuple<_Args&&...> argsTuple(std::forward<_Args>(args)...);

		if (m_abiWriter.IsDynamicType())
		{
			// the parameter tuple is dynamic, so the data is in the tail

			size_t sizeNeeded = m_abiWriter.GetNumTailChunks(argsTuple) *
				AbiCodecConst::sk_chunkSize();
			data.reserve(data.size() + sizeNeeded);
			// the data might be relocated, so we MUST get the iterator here
			auto destIt =
				Internal::Obj::ToOutIt<uint8_t>(std::back_inserter(data));

			m_abiWriter.WriteTail(destIt, argsTuple);
		}
		else
		{
			// the parameter tuple is static, so the data is in the head

			size_t sizeNeeded = m_abiWriter.GetNumHeadChunks() *
				AbiCodecConst::sk_chunkSize();
			data.reserve(data.size() + sizeNeeded);
			// the data might be relocated, so we MUST get the iterator here
			auto destIt =
				Internal::Obj::ToOutIt<uint8_t>(std::back_inserter(data));

			m_abiWriter.WriteHead(destIt, argsTuple, 0);
		}
		return Base::BuildTxn(data);
	}

private:

	AbiWriter m_abiWriter;

}; // class ContractFunction


} // namespace Transaction
} // namespace Eth
} // namespace EclipseMonitor

