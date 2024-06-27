// Copyright (c) 2024 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once


#include <cstdint>

#include <string>
#include <tuple>
#include <type_traits>

#include "../Internal/SimpleObj.hpp"
#include "../Exceptions.hpp"

#include "AbiCommon.hpp"


namespace EclipseMonitor
{
namespace Eth
{


// ==========
// Constant values
// ==========


struct AbiCodecConst
{
	static constexpr size_t sk_chunkSize() noexcept
	{
		return 32;
	}
}; // struct AbiCodecConst


// ==========
// Alias types for convenience
// ==========


using AbiUInt8 =
	std::integral_constant<
		Internal::Obj::RealNumType,
		Internal::Obj::RealNumType::UInt8
	>;
using AbiUInt64 =
	std::integral_constant<
		Internal::Obj::RealNumType,
		Internal::Obj::RealNumType::UInt64
	>;

template<size_t _Size>
using AbiSize = std::integral_constant<size_t, _Size>;


// ==========
// Utilities
// ==========


namespace EthInternal
{


template<
	Internal::Obj::RealNumType _RealNumType,
	typename _PrimitiveType
>
struct RealNumTypeTraitsPrimitiveImpl
{
	using Primitive = _PrimitiveType;

	static constexpr size_t sk_consumedSize() noexcept
	{
		return sizeof(Primitive);
	}
}; // struct RealNumTypeTraitsPrimitiveImpl


template<typename _T>
inline constexpr _T AbiCeilingDiv(_T a, _T b) noexcept
{
	return (a + (b - 1)) / b;
}


inline constexpr size_t AbiWithinChunkSize(size_t size)
{
	return size <= AbiCodecConst::sk_chunkSize() ?
		size :
		throw Exception("ABI parser - bytes type is too large");
}


template<Internal::Obj::RealNumType _RealNumType>
struct RealNumTypeTraits;


template<>
struct RealNumTypeTraits<Internal::Obj::RealNumType::UInt8> :
	public RealNumTypeTraitsPrimitiveImpl<
		Internal::Obj::RealNumType::UInt8,
		uint8_t
	>
{
	static Primitive FromRealNumBase(const Internal::Obj::RealNumBaseObj& val)
	{
		return val.AsCppUInt8();
	}

	static const char* GetTypeName() { return "uint8"; }
}; // struct RealNumTypeTraits<Internal::Obj::RealNumType::UInt8>


template<>
struct RealNumTypeTraits<Internal::Obj::RealNumType::UInt16> :
	public RealNumTypeTraitsPrimitiveImpl<
		Internal::Obj::RealNumType::UInt16,
		uint16_t
	>
{
	static const char* GetTypeName() { return "uint16"; }
}; // struct RealNumTypeTraits<Internal::Obj::RealNumType::UInt16>


template<>
struct RealNumTypeTraits<Internal::Obj::RealNumType::UInt32> :
	public RealNumTypeTraitsPrimitiveImpl<
		Internal::Obj::RealNumType::UInt32,
		uint32_t
	>
{
	static Primitive FromRealNumBase(const Internal::Obj::RealNumBaseObj& val)
	{
		return val.AsCppUInt32();
	}
	static const char* GetTypeName() { return "uint32"; }
}; // struct RealNumTypeTraits<Internal::Obj::RealNumType::UInt32>


template<>
struct RealNumTypeTraits<Internal::Obj::RealNumType::UInt64> :
	public RealNumTypeTraitsPrimitiveImpl<
		Internal::Obj::RealNumType::UInt64,
		uint64_t
	>
{
	static Primitive FromRealNumBase(const Internal::Obj::RealNumBaseObj& val)
	{
		return val.AsCppUInt64();
	}
	static const char* GetTypeName() { return "uint64"; }
}; // struct RealNumTypeTraits<Internal::Obj::RealNumType::UInt64>


} // namespace EthInternal


// ==========
// Codec Basis
// ==========


namespace EthInternal
{


template<
	Internal::Obj::ObjCategory _DataType,
	typename... _Args
>
struct AbiCodecImpl;


// ==========
// AbiCodecImpl for integer types
// ==========


template<Internal::Obj::RealNumType _RealNumType>
struct AbiCodecImpl<
	Internal::Obj::ObjCategory::Integer,
	std::integral_constant<Internal::Obj::RealNumType, _RealNumType>
>
{
	using RealNumTraits = RealNumTypeTraits<_RealNumType>;

	static constexpr size_t sk_consumedSize = RealNumTraits::sk_consumedSize();

	static_assert(
		sk_consumedSize <= AbiCodecConst::sk_chunkSize(),
		"ABI parser - integer type is too large"
	);

	static constexpr size_t sk_leadPadSize =
		AbiCodecConst::sk_chunkSize() - sk_consumedSize;

	using Primitive = typename RealNumTraits::Primitive;

	using IsDynamic = std::false_type;
	static constexpr bool sk_isDynamic = IsDynamic::value;
	bool IsDynamicType() const noexcept { return sk_isDynamic; }

	static const char* GetTypeName() { return RealNumTraits::GetTypeName(); }

}; // struct AbiCodecImpl<Internal::Obj::ObjCategory::Integer, RealNumType>


// ==========
// AbiCodecImpl for bool types
// ==========


template<>
struct AbiCodecImpl<
	Internal::Obj::ObjCategory::Bool
> :
	public AbiCodecImpl<
		Internal::Obj::ObjCategory::Integer,
		AbiUInt8
	>
{
	using Base = AbiCodecImpl<
		Internal::Obj::ObjCategory::Integer,
		AbiUInt8
	>;
	using Self = AbiCodecImpl<Internal::Obj::ObjCategory::Bool>;

	static constexpr size_t sk_consumedSize = Base::sk_consumedSize;

	static constexpr size_t sk_leadPadSize = Base::sk_leadPadSize;

	using Primitive = bool;

	using IsDynamic = std::false_type;
	static constexpr bool sk_isDynamic = IsDynamic::value;
	bool IsDynamicType() const noexcept { return sk_isDynamic; }

	static const char* GetTypeName() { return "bool"; }

}; // struct AbiCodecImpl<Internal::Obj::ObjCategory::Bool>


// ==========
// AbiCodecImpl for bytes<M> types
// ==========


template<>
struct AbiCodecImpl<
	Internal::Obj::ObjCategory::Bytes,
	std::false_type // IsDynamic? - false
>
{

	using Primitive = std::vector<uint8_t>;

	using IsDynamic = std::false_type;
	static constexpr bool sk_isDynamic = IsDynamic::value;
	bool IsDynamicType() const noexcept { return sk_isDynamic; }

	static std::string GetTypeName(size_t size)
	{
		return std::string("bytes") + std::to_string(size);
	}

}; // struct AbiCodecImpl<Internal::Obj::ObjCategory::Bytes, false>


// ==========
// AbiCodecImpl for bytes types
// ==========


template<>
struct AbiCodecImpl<
	Internal::Obj::ObjCategory::Bytes,
	std::true_type // IsDynamic? - true
>
{

	using Primitive = std::vector<uint8_t>;

	using IsDynamic = std::true_type;
	static constexpr bool sk_isDynamic = IsDynamic::value;
	bool IsDynamicType() const noexcept { return sk_isDynamic; }

	static const char* GetTypeName() { return "bytes"; }

}; // struct AbiCodecImpl<Internal::Obj::ObjCategory::Bytes, true>


// ==========
// AbiCodecImpl for T[k] types, where T is static type
// ==========


template<typename _ItemCodec>
struct AbiCodecImpl<
	Internal::Obj::ObjCategory::List,
	_ItemCodec,
	std::false_type, // IsLenDynamic? - false
	std::false_type  // IsItemDynamic? - false
>
{
	using ItemCodec = _ItemCodec;
	using ItemPrimitive = typename ItemCodec::Primitive;
	using Primitive = std::vector<ItemPrimitive>;

	static_assert(
		!ItemCodec::sk_isDynamic,
		"ABI parser - The internal item should be a static type"
	);

	using IsDynamic = std::false_type;
	static constexpr bool sk_isDynamic = IsDynamic::value;
	bool IsDynamicType() const noexcept { return sk_isDynamic; }

}; // struct AbiCodecImpl<Internal::Obj::ObjCategory::List, _ItemCodec, false, false>


// ==========
// AbiCodecImpl for T[k] types, where T is dynamic type
// ==========


template<typename _ItemCodec>
struct AbiCodecImpl<
	Internal::Obj::ObjCategory::List,
	_ItemCodec,
	std::false_type, // IsLenDynamic? - false
	std::true_type  // IsItemDynamic? - true
>
{
	using ItemCodec = _ItemCodec;
	using ItemPrimitive = typename ItemCodec::Primitive;
	using Primitive = std::vector<ItemPrimitive>;

	static_assert(
		ItemCodec::sk_isDynamic,
		"ABI parser - The internal item should be a dynamic type"
	);

	using IsDynamic = std::true_type;
	static constexpr bool sk_isDynamic = IsDynamic::value;
	bool IsDynamicType() const noexcept { return sk_isDynamic; }

}; // struct AbiCodecImpl<Internal::Obj::ObjCategory::List, _ItemCodec, false, true>


// ==========
// AbiCodecImpl for T[] types, where T is static type
// ==========


/**
 * @brief The codec implementation for a list with dynamic length.
 *        NOTE: T[] is a dynamic type, no matter what T is
 *
 * @tparam _ItemCodec The codec for the internal item
 * @tparam _IsItemDyn The dynamic-property of the internal item
 */
template<typename _ItemCodec, typename _IsItemDyn>
struct AbiCodecDynLenListImpl
{
	using ItemCodec = _ItemCodec;
	using ItemPrimitive = typename ItemCodec::Primitive;
	using Primitive = std::vector<ItemPrimitive>;

	static_assert(
		(
			(_IsItemDyn::value == ItemCodec::sk_isDynamic) &&
			std::is_same<_IsItemDyn, typename ItemCodec::IsDynamic>::value
		),
		"ABI parser - The dynamic-property of the internal item doesn't match"
	);

	using IsDynamic = std::true_type;
	static constexpr bool sk_isDynamic = IsDynamic::value;
	bool IsDynamicType() const noexcept { return sk_isDynamic; }

}; // struct AbiCodecDynLenListImpl


template<typename _ItemCodec>
struct AbiCodecImpl<
	Internal::Obj::ObjCategory::List,
	_ItemCodec,
	std::true_type, // IsLenDynamic? - true
	std::false_type // IsItemDynamic? - false
> :
	public AbiCodecDynLenListImpl<_ItemCodec, std::false_type>
{}; // struct AbiCodecImpl<Internal::Obj::ObjCategory::List, _ItemCodec, true, false>


// ==========
// AbiCodecImpl for T[] types, where T is dynamic type
// ==========


template<typename _ItemCodec>
struct AbiCodecImpl<
	Internal::Obj::ObjCategory::List,
	_ItemCodec,
	std::true_type, // IsLenDynamic? - true
	std::true_type  // IsItemDynamic? - true
> :
	public AbiCodecDynLenListImpl<_ItemCodec, std::true_type>
{}; // struct AbiCodecImpl<Internal::Obj::ObjCategory::List, _ItemCodec, true, true>


} // namespace EthInternal


} // namespace Eth
} // namespace EclipseMonitor

