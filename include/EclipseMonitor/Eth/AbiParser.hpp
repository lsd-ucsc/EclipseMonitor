// Copyright (c) 2022 Haofan Zheng, Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once


#include <cstdint>

#include <tuple>
#include <type_traits>
#include <vector>

#include <SimpleObjects/BasicDefs.hpp>
#include <SimpleRlp/RlpDecoding.hpp>

#include "../Internal/SimpleObj.hpp"
#include "../Internal/SimpleRlp.hpp"
#include "../Exceptions.hpp"


namespace EclipseMonitor
{
namespace Eth
{


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


template<Internal::Obj::RealNumType _RealNumType>
struct RealNumTypeTraits;


template<>
struct RealNumTypeTraits<Internal::Obj::RealNumType::UInt8> :
	public RealNumTypeTraitsPrimitiveImpl<
		Internal::Obj::RealNumType::UInt8,
		uint8_t
	>
{}; // struct RealNumTypeTraits<Internal::Obj::RealNumType::UInt8>


template<>
struct RealNumTypeTraits<Internal::Obj::RealNumType::UInt16> :
	public RealNumTypeTraitsPrimitiveImpl<
		Internal::Obj::RealNumType::UInt16,
		uint16_t
	>
{}; // struct RealNumTypeTraits<Internal::Obj::RealNumType::UInt16>


template<>
struct RealNumTypeTraits<Internal::Obj::RealNumType::UInt32> :
	public RealNumTypeTraitsPrimitiveImpl<
		Internal::Obj::RealNumType::UInt32,
		uint32_t
	>
{}; // struct RealNumTypeTraits<Internal::Obj::RealNumType::UInt32>


template<>
struct RealNumTypeTraits<Internal::Obj::RealNumType::UInt64> :
	public RealNumTypeTraitsPrimitiveImpl<
		Internal::Obj::RealNumType::UInt64,
		uint64_t
	>
{}; // struct RealNumTypeTraits<Internal::Obj::RealNumType::UInt64>


template<bool _CheckVal, typename _It>
inline _It AbiParserSkipPadding(size_t skipLen, _It begin, _It end)
{
	for (size_t i = 0; i < skipLen; ++i)
	{
		if (begin == end)
		{
			throw Exception("ABI parser - unexpected end of input");
		}
		if (_CheckVal && (*begin != 0))
		{
			throw Exception(
				"ABI parser - there are non-zero bytes in skipped bytes"
			);
		}
		++begin;
	}
	return begin;
}


template<typename _SrcIt, typename _DestIt>
inline _SrcIt AbiParserCopyBytes(
	size_t len,
	_SrcIt begin,
	_SrcIt end,
	_DestIt dest
)
{
	for (size_t i = 0; i < len; ++i)
	{
		if (begin == end)
		{
			throw Exception("ABI parser - unexpected end of input");
		}
		*(dest++) = *(begin++);
	}
	return begin;
}


} // namespace EthInternal


struct AbiParserConst
{
	static constexpr size_t sk_chunkSize() noexcept
	{
		return 32;
	}
}; // struct AbiParserConst


template<
	Internal::Obj::ObjCategory _DataType
>
struct AbiParser;


template<>
struct AbiParser<Internal::Obj::ObjCategory::Integer>
{

	template<Internal::Obj::RealNumType _RealNumType>
	using RealNumTraits = EthInternal::RealNumTypeTraits<_RealNumType>;

	template<
		Internal::Obj::RealNumType _RealNumType,
		typename _ItType
	>
	static
	std::tuple<
		typename RealNumTraits<_RealNumType>::Primitive,
		_ItType
	>
	ToPrimitive(_ItType begin, _ItType end)
	{
		using Primitive = typename RealNumTraits<_RealNumType>::Primitive;

		static constexpr size_t sk_consumedSize =
			RealNumTraits<_RealNumType>::sk_consumedSize();
		static_assert(
			sk_consumedSize <= AbiParserConst::sk_chunkSize(),
			"ABI parser - integer type is too large"
		);
		static constexpr size_t sk_skipLeadSize =
			AbiParserConst::sk_chunkSize() - sk_consumedSize;


		// Skip the leading zero bytes that are larger than the target type
		begin = EthInternal::AbiParserSkipPadding<true>(
			sk_skipLeadSize,
			begin,
			end
		);


		// Parse the integer to target type
		auto inFunc = [&begin, &end]()
		{
			return begin != end ?
				*(begin++) :
				throw Exception("ABI parser - unexpected end of input");
		};
		Primitive res = Internal::Rlp::ParsePrimitiveIntValue<
			Primitive,
			Internal::Rlp::Endian::native
		>::Parse(sk_consumedSize, inFunc);

		return std::make_tuple(res, begin);
	}

}; // struct AbiParser<Internal::Obj::ObjCategory::Integer>


template<>
struct AbiParser<Internal::Obj::ObjCategory::Bool>
{
	template<typename _ItType>
	static std::tuple<bool, _ItType>
	ToPrimitive(_ItType begin, _ItType end)
	{
		uint8_t valInt = 0;
		std::tie(valInt, begin) = AbiParser<Internal::Obj::ObjCategory::Integer>::
			ToPrimitive<Internal::Obj::RealNumType::UInt8>(begin, end);

		bool valBool =
			(valInt == 1 ? true  :
			(valInt == 0 ? false :
				throw Exception("ABI parser - invalid bool value")));

		return std::make_tuple(valBool, begin);
	}
}; // struct AbiParser<Internal::Obj::ObjCategory::Bool>


template<>
struct AbiParser<Internal::Obj::ObjCategory::Bytes>
{

	/**
	 * @brief Parse bytes with static length (0 < len <= 32)
	 */
	template<size_t _Len, typename _ItType>
	static std::tuple<std::vector<uint8_t>, _ItType>
	ToPrimitive(_ItType begin, _ItType end)
	{
		static_assert(
			_Len <= AbiParserConst::sk_chunkSize(),
			"ABI parser - bytes type is too large"
		);

		return ToPrimitiveImpl(_Len, begin, end);
	}

	/**
	 * @brief Parse bytes with dynamic length (0 < len <= 32)
	 */
	template<typename _ItType>
	static std::tuple<std::vector<uint8_t>, _ItType>
	ToPrimitive(_ItType begin, _ItType end)
	{
		// first, parse the length of the bytes
		uint64_t len = 0;
		std::tie(len, begin) = AbiParser<Internal::Obj::ObjCategory::Integer>::
			ToPrimitive<Internal::Obj::RealNumType::UInt64>(begin, end);

		size_t numChunk =
			(
				static_cast<size_t>(len) +
				(AbiParserConst::sk_chunkSize() - 1)
			) /
			AbiParserConst::sk_chunkSize();

		size_t paddingSize =
			(numChunk * AbiParserConst::sk_chunkSize()) - len;

		std::vector<uint8_t> res;
		res.reserve(len);

		// Copy the bytes
		begin = EthInternal::AbiParserCopyBytes(
			len,
			begin,
			end,
			std::back_inserter(res)
		);

		// Skip the padding bytes
		begin = EthInternal::AbiParserSkipPadding<true>(
			paddingSize,
			begin,
			end
		);

		return std::make_tuple(res, begin);
	}

private:

	/**
	 * @brief Parse bytes with static length (0 < len <= 32)
	 *        NOTE: check `len` before calling this function
	 */
	template<typename _ItType>
	static std::tuple<std::vector<uint8_t>, _ItType>
	ToPrimitiveImpl(size_t len, _ItType begin, _ItType end)
	{
		std::vector<uint8_t> res;
		res.reserve(len);

		// Copy the bytes
		begin = EthInternal::AbiParserCopyBytes(
			len,
			begin,
			end,
			std::back_inserter(res)
		);

		// Now, Skip the trailing zero bytes
		begin = EthInternal::AbiParserSkipPadding<true>(
			AbiParserConst::sk_chunkSize() - len,
			begin,
			end
		);

		return std::make_tuple(
			res,
			begin
		);
	}
}; // struct AbiParser<Internal::Obj::ObjCategory::Bool>


} // namespace Eth
} // namespace EclipseMonitor
