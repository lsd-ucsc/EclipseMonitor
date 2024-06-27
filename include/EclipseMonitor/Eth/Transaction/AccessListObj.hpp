// Copyright (c) 2024 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once


#include <tuple>

#include "../../Exceptions.hpp"
#include "../../Internal/SimpleObj.hpp"
#include "../../Internal/SimpleRlp.hpp"


namespace EclipseMonitor
{
namespace Eth
{
namespace Transaction
{


using StorageKeys = Internal::Obj::ListT<Internal::Obj::Bytes>;


/**
 * @brief Validate the given bytes is a smart contract (or wallet) address.
 *        It mainly checks the length of the given bytes is 20.
 *
 * @param addr The address bytes to validate.
 */
inline void ValidateContractAddr(
	const Internal::Obj::BytesBaseObj& addr
)
{
	if (addr.size() != 20)
	{
		throw Exception("Invalid access list address size");
	}
}


/**
 * @brief Validate the given storage keys list that comes from a access list.
 *        It mainly checks the length of the given by is 32.
 *
 * @param storageKey The storage key bytes to validate.
 */
inline void ValidateAccessListStorageKey(
	const Internal::Obj::BytesBaseObj& storageKey
)
{
	if (storageKey.size() != 32)
	{
		throw Exception("Invalid access list storage key size");
	}
}


inline void ValidateAccessListStorageKeys(const StorageKeys& storageKeys)
{
	for (const auto& storageKey : storageKeys)
	{
		ValidateAccessListStorageKey(storageKey);
	}
}


/**
 * @brief The access list object should contain two elements:
 *        1. address: bytes
 *        2. storageKeys : list[bytes]
 *        reference: https://github.com/ethereum/eth-account/blob/8478a86a8d235acba0a33fcae5804887473c72de/eth_account/account.py#L666
 *
 */
using AccessListItemTupleCore = std::tuple<
	// 01.
	std::pair<Internal::Obj::StrKey<SIMOBJ_KSTR("Address")>,
		Internal::Obj::Bytes>,
	// 02.
	std::pair<Internal::Obj::StrKey<SIMOBJ_KSTR("StorageKeys")>,
		StorageKeys>
>;


using StorageKeyParser = Internal::Rlp::ListParserT<
	Internal::Rlp::BytesParser,
	Internal::Rlp::FailingParser<
		Internal::Rlp::InputContainerType,
		Internal::Rlp::ByteValType,
		Internal::Rlp::RlpEncTypeCat::List,
		Internal::Rlp::BytesObjType
	>,
	StorageKeys
>;


using AccessListItemParserTupleCore = std::tuple<
	// 01.
	std::pair<Internal::Obj::StrKey<SIMOBJ_KSTR("Address")>,
		Internal::Rlp::BytesParser>,
	// 02.
	std::pair<Internal::Obj::StrKey<SIMOBJ_KSTR("StorageKeys")>,
		StorageKeyParser>
>;


class AccessListItem :
	public Internal::Obj::StaticDict<AccessListItemTupleCore>
{
public: // static members:

	using Self = AccessListItem;
	using Base = Internal::Obj::StaticDict<AccessListItemTupleCore>;

public:

	using Base::Base;

	void Validate() const
	{
		// Validate the address
		ValidateContractAddr(get_Address());

		// validate the storage keys
		ValidateAccessListStorageKeys(get_StorageKeys());
	}

	// 01. Address
	typename Base::template GetRef<Internal::Obj::StrKey<SIMOBJ_KSTR("Address")> >
	get_Address()
	{
		return Base::template get<Internal::Obj::StrKey<SIMOBJ_KSTR("Address")> >();
	}

	typename Base::template GetConstRef<Internal::Obj::StrKey<SIMOBJ_KSTR("Address")> >
	get_Address() const
	{
		return Base::template get<Internal::Obj::StrKey<SIMOBJ_KSTR("Address")> >();
	}

	// 02. StorageKeys
	typename Base::template GetRef<Internal::Obj::StrKey<SIMOBJ_KSTR("StorageKeys")> >
	get_StorageKeys()
	{
		return Base::template get<Internal::Obj::StrKey<SIMOBJ_KSTR("StorageKeys")> >();
	}

	typename Base::template GetConstRef<Internal::Obj::StrKey<SIMOBJ_KSTR("StorageKeys")> >
	get_StorageKeys() const
	{
		return Base::template get<Internal::Obj::StrKey<SIMOBJ_KSTR("StorageKeys")> >();
	}

}; // class AccessListItem


using AccessListItemParser = Internal::Rlp::StaticDictParserT<
	AccessListItemParserTupleCore,
	false,
	false,
	AccessListItem
>;


using AccessListObj = Internal::Obj::ListT<AccessListItem>;


using AccessListObjParser = Internal::Rlp::ListParserT<
	Internal::Rlp::FailingParser<
		Internal::Rlp::InputContainerType,
		Internal::Rlp::ByteValType,
		Internal::Rlp::RlpEncTypeCat::Bytes,
		AccessListItem
	>,
	AccessListItemParser,
	AccessListObj
>;


inline void ValidateAccessListObj(const AccessListObj& accessList)
{
	for (const auto& accessListItem : accessList)
	{
		accessListItem.Validate();
	}
}


} // namespace Transaction
} // namespace Eth
} // namespace EclipseMonitor

