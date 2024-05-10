// Copyright (c) 2024 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once


#include <tuple>

#include "../../Exceptions.hpp"
#include "../../Internal/SimpleObj.hpp"
#include "../../Internal/SimpleRlp.hpp"

#include "../DataTypes.hpp"
#include "../Keccak256.hpp"


namespace EclipseMonitor
{
namespace Eth
{
namespace Transaction
{


using AccessListObj = Internal::Obj::List;



inline void ValidateContractAddr(
	const Internal::Obj::BytesBaseObj& addr
)
{
	if (addr.size() != 20)
	{
		throw Exception("Invalid access list address size");
	}
}


inline void ValidateAccessListStorageKey(
	const Internal::Obj::BytesBaseObj& storageKey
)
{
	if (storageKey.size() != 32)
	{
		throw Exception("Invalid access list storage key size");
	}
}


inline void ValidateAccessListStorageKeys(
	const Internal::Obj::ListBaseObj& storageKeys
)
{
	for (const auto& storageKey : storageKeys)
	{
		if (storageKey.GetCategory() != Internal::Obj::ObjCategory::Bytes)
		{
			throw Exception("Invalid access list storage key type");
		}
		ValidateAccessListStorageKey(storageKey.AsBytes());
	}
}


/**
 * @brief validate the tuples in the access list object; each tuple should
 *        contain two elements: address(bytes) and storageKeys(list[bytes])
 *        reference: https://github.com/ethereum/eth-account/blob/8478a86a8d235acba0a33fcae5804887473c72de/eth_account/account.py#L666
 *
 * @param accessTuple the tuple to be validated
 */
inline void ValidateAccessListTuple(const Internal::Obj::ListBaseObj& accessTuple)
{
	if (accessTuple.size() != 2)
	{
		throw Exception("Invalid access list tuple size");
	}

	if (accessTuple[0].GetCategory() != Internal::Obj::ObjCategory::Bytes)
	{
		throw Exception("Invalid access list address type");
	}
	ValidateContractAddr(accessTuple[0].AsBytes());

	if (accessTuple[1].GetCategory() != Internal::Obj::ObjCategory::List)
	{
		throw Exception("Invalid access list storage keys type");
	}
	ValidateAccessListStorageKeys(accessTuple[1].AsList());
}


/**
 * @brief validate the access list object, which supposed to be a list of
 *        tuple[bytes, list[bytes]]
 *
 * @param accessList the access list object to be validated
 */
inline void ValidateAccessList(const AccessListObj& accessList)
{
	for (const auto& tuple : accessList)
	{
		if (tuple.GetCategory() != Internal::Obj::ObjCategory::List)
		{
			throw Exception("Invalid access list tuple type");
		}
		ValidateAccessListTuple(tuple.AsList());
	}
}


/**
 * @brief fields in a dynamic fee transaction object
 *        which are: chain_id, nonce, max_priority_fee_per_gas, max_fee_per_gas,
 *                   gas_limit, destination, amount, data, access_list
 *        source: https://github.com/ethereum/EIPs/blob/master/EIPS/eip-1559.md
 *
 */
using DynFeeTupleCore = std::tuple<
	// 01.
	std::pair<Internal::Obj::StrKey<SIMOBJ_KSTR("ChainID")>,
		Internal::Obj::Bytes>,
	// 02.
	std::pair<Internal::Obj::StrKey<SIMOBJ_KSTR("Nonce")>,
		Internal::Obj::Bytes>,
	// 03.
	std::pair<Internal::Obj::StrKey<SIMOBJ_KSTR("MaxPriorFeePerGas")>,
		Internal::Obj::Bytes>,
	// 04.
	std::pair<Internal::Obj::StrKey<SIMOBJ_KSTR("MaxFeePerGas")>,
		Internal::Obj::Bytes>,
	// 05.
	std::pair<Internal::Obj::StrKey<SIMOBJ_KSTR("GasLimit")>,
		Internal::Obj::Bytes>,
	// 06.
	std::pair<Internal::Obj::StrKey<SIMOBJ_KSTR("Destination")>,
		Internal::Obj::Bytes>,
	// 07.
	std::pair<Internal::Obj::StrKey<SIMOBJ_KSTR("Amount")>,
		Internal::Obj::Bytes>,
	// 08.
	std::pair<Internal::Obj::StrKey<SIMOBJ_KSTR("Data")>,
		Internal::Obj::Bytes>,
	// 09.
	std::pair<Internal::Obj::StrKey<SIMOBJ_KSTR("AccessList")>,
		AccessListObj>
	>;


using DynFeeParserTupleCore = std::tuple<
	// 01.
	std::pair<Internal::Obj::StrKey<SIMOBJ_KSTR("ChainID")>,
		Internal::Rlp::BytesParser>,
	// 02.
	std::pair<Internal::Obj::StrKey<SIMOBJ_KSTR("Nonce")>,
		Internal::Rlp::BytesParser>,
	// 03.
	std::pair<Internal::Obj::StrKey<SIMOBJ_KSTR("MaxPriorFeePerGas")>,
		Internal::Rlp::BytesParser>,
	// 04.
	std::pair<Internal::Obj::StrKey<SIMOBJ_KSTR("MaxFeePerGas")>,
		Internal::Rlp::BytesParser>,
	// 05.
	std::pair<Internal::Obj::StrKey<SIMOBJ_KSTR("GasLimit")>,
		Internal::Rlp::BytesParser>,
	// 06.
	std::pair<Internal::Obj::StrKey<SIMOBJ_KSTR("Destination")>,
		Internal::Rlp::BytesParser>,
	// 07.
	std::pair<Internal::Obj::StrKey<SIMOBJ_KSTR("Amount")>,
		Internal::Rlp::BytesParser>,
	// 08.
	std::pair<Internal::Obj::StrKey<SIMOBJ_KSTR("Data")>,
		Internal::Rlp::BytesParser>,
	// 09.
	std::pair<Internal::Obj::StrKey<SIMOBJ_KSTR("AccessList")>,
		Internal::Rlp::ListParser>
>;


class DynFee :
	public Internal::Obj::StaticDict<DynFeeTupleCore>
{
public: // static members:

	using Self = DynFee;
	using Base = Internal::Obj::StaticDict<DynFeeTupleCore>;

	static Self FromRlp(const Internal::Rlp::InputContainerType& rlp);

public:

	using Base::Base;

	Internal::Rlp::OutputContainerType RlpSerialize() const
	{
		// validate the access list object
		ValidateAccessList(get_AccessList());

		// validate the destination address
		ValidateContractAddr(get_Destination());

		return Internal::Rlp::WriterGeneric::Write(*this);
	}

	/**
	 * @brief Get the type flag, which is 0x02 for dynamic fee transaction
	 *
	 * @return the type flag in a byte
	 */
	uint8_t GetTypeFlag() const
	{
		return 0x02U;
	}

	std::array<uint8_t, 32> Hash() const
	{
		auto rlp = RlpSerialize();

		// prepend the type flag
		rlp.insert(rlp.begin(), GetTypeFlag());

		return Keccak256(rlp);
	}

	// 01. ChainID
	typename Base::template GetRef<Internal::Obj::StrKey<SIMOBJ_KSTR("ChainID")> >
	get_ChainID()
	{
		return Base::template get<Internal::Obj::StrKey<SIMOBJ_KSTR("ChainID")> >();
	}

	typename Base::template GetConstRef<Internal::Obj::StrKey<SIMOBJ_KSTR("ChainID")> >
	get_ChainID() const
	{
		return Base::template get<Internal::Obj::StrKey<SIMOBJ_KSTR("ChainID")> >();
	}

	void SetChainID(uint64_t chainID)
	{
		get_ChainID() = PrimitiveTypeTrait<uint64_t>::ToBytes(chainID);
	}

	uint64_t GetChainID() const
	{
		return PrimitiveTypeTrait<uint64_t>::FromBytes(get_ChainID());
	}

	// 02. Nonce
	typename Base::template GetRef<Internal::Obj::StrKey<SIMOBJ_KSTR("Nonce")> >
	get_Nonce()
	{
		return Base::template get<Internal::Obj::StrKey<SIMOBJ_KSTR("Nonce")> >();
	}

	typename Base::template GetConstRef<Internal::Obj::StrKey<SIMOBJ_KSTR("Nonce")> >
	get_Nonce() const
	{
		return Base::template get<Internal::Obj::StrKey<SIMOBJ_KSTR("Nonce")> >();
	}

	void SetNonce(uint64_t nonce)
	{
		get_Nonce() = PrimitiveTypeTrait<uint64_t>::ToBytes(nonce);
	}

	uint64_t GetNonce() const
	{
		return PrimitiveTypeTrait<uint64_t>::FromBytes(get_Nonce());
	}

	// 03. MaxPriorFeePerGas
	typename Base::template GetRef<Internal::Obj::StrKey<SIMOBJ_KSTR("MaxPriorFeePerGas")> >
	get_MaxPriorFeePerGas()
	{
		return Base::template get<Internal::Obj::StrKey<SIMOBJ_KSTR("MaxPriorFeePerGas")> >();
	}

	typename Base::template GetConstRef<Internal::Obj::StrKey<SIMOBJ_KSTR("MaxPriorFeePerGas")> >
	get_MaxPriorFeePerGas() const
	{
		return Base::template get<Internal::Obj::StrKey<SIMOBJ_KSTR("MaxPriorFeePerGas")> >();
	}

	void SetMaxPriorFeePerGas(uint64_t maxPriorFeePerGas)
	{
		get_MaxPriorFeePerGas() = PrimitiveTypeTrait<uint64_t>::ToBytes(maxPriorFeePerGas);
	}

	uint64_t GetMaxPriorFeePerGas() const
	{
		return PrimitiveTypeTrait<uint64_t>::FromBytes(get_MaxPriorFeePerGas());
	}

	// 04. MaxFeePerGas
	typename Base::template GetRef<Internal::Obj::StrKey<SIMOBJ_KSTR("MaxFeePerGas")> >
	get_MaxFeePerGas()
	{
		return Base::template get<Internal::Obj::StrKey<SIMOBJ_KSTR("MaxFeePerGas")> >();
	}

	typename Base::template GetConstRef<Internal::Obj::StrKey<SIMOBJ_KSTR("MaxFeePerGas")> >
	get_MaxFeePerGas() const
	{
		return Base::template get<Internal::Obj::StrKey<SIMOBJ_KSTR("MaxFeePerGas")> >();
	}

	void SetMaxFeePerGas(uint64_t maxFeePerGas)
	{
		get_MaxFeePerGas() = PrimitiveTypeTrait<uint64_t>::ToBytes(maxFeePerGas);
	}

	uint64_t GetMaxFeePerGas() const
	{
		return PrimitiveTypeTrait<uint64_t>::FromBytes(get_MaxFeePerGas());
	}

	// 05. GasLimit
	typename Base::template GetRef<Internal::Obj::StrKey<SIMOBJ_KSTR("GasLimit")> >
	get_GasLimit()
	{
		return Base::template get<Internal::Obj::StrKey<SIMOBJ_KSTR("GasLimit")> >();
	}

	typename Base::template GetConstRef<Internal::Obj::StrKey<SIMOBJ_KSTR("GasLimit")> >
	get_GasLimit() const
	{
		return Base::template get<Internal::Obj::StrKey<SIMOBJ_KSTR("GasLimit")> >();
	}

	void SetGasLimit(uint64_t gasLimit)
	{
		get_GasLimit() = PrimitiveTypeTrait<uint64_t>::ToBytes(gasLimit);
	}

	uint64_t GetGasLimit() const
	{
		return PrimitiveTypeTrait<uint64_t>::FromBytes(get_GasLimit());
	}

	// 06. Destination
	typename Base::template GetRef<Internal::Obj::StrKey<SIMOBJ_KSTR("Destination")> >
	get_Destination()
	{
		return Base::template get<Internal::Obj::StrKey<SIMOBJ_KSTR("Destination")> >();
	}

	typename Base::template GetConstRef<Internal::Obj::StrKey<SIMOBJ_KSTR("Destination")> >
	get_Destination() const
	{
		return Base::template get<Internal::Obj::StrKey<SIMOBJ_KSTR("Destination")> >();
	}

	// 07. Amount
	typename Base::template GetRef<Internal::Obj::StrKey<SIMOBJ_KSTR("Amount")> >
	get_Amount()
	{
		return Base::template get<Internal::Obj::StrKey<SIMOBJ_KSTR("Amount")> >();
	}

	typename Base::template GetConstRef<Internal::Obj::StrKey<SIMOBJ_KSTR("Amount")> >
	get_Amount() const
	{
		return Base::template get<Internal::Obj::StrKey<SIMOBJ_KSTR("Amount")> >();
	}

	void SetAmount(uint64_t amount)
	{
		get_Amount() = PrimitiveTypeTrait<uint64_t>::ToBytes(amount);
	}

	uint64_t GetAmount() const
	{
		return PrimitiveTypeTrait<uint64_t>::FromBytes(get_Amount());
	}

	// 08. Data
	typename Base::template GetRef<Internal::Obj::StrKey<SIMOBJ_KSTR("Data")> >
	get_Data()
	{
		return Base::template get<Internal::Obj::StrKey<SIMOBJ_KSTR("Data")> >();
	}

	typename Base::template GetConstRef<Internal::Obj::StrKey<SIMOBJ_KSTR("Data")> >
	get_Data() const
	{
		return Base::template get<Internal::Obj::StrKey<SIMOBJ_KSTR("Data")> >();
	}

	// 09. AccessList
	typename Base::template GetRef<Internal::Obj::StrKey<SIMOBJ_KSTR("AccessList")> >
	get_AccessList()
	{
		return Base::template get<Internal::Obj::StrKey<SIMOBJ_KSTR("AccessList")> >();
	}

	typename Base::template GetConstRef<Internal::Obj::StrKey<SIMOBJ_KSTR("AccessList")> >
	get_AccessList() const
	{
		return Base::template get<Internal::Obj::StrKey<SIMOBJ_KSTR("AccessList")> >();
	}

}; // class DynFee



using DynFeeParser = Internal::Rlp::StaticDictParserT<
	DynFeeParserTupleCore,
	false,
	false,
	DynFee
>;


inline DynFee::Self DynFee::FromRlp(const Internal::Rlp::InputContainerType& rlp)
{
	return DynFeeParser().Parse(rlp);
}


} // namespace Transaction
} // namespace Eth
} // namespace EclipseMonitor

