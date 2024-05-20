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

#include "AbiCommon.hpp"


namespace EclipseMonitor
{
namespace Eth
{




// ==========
// Utilities
// ==========


namespace EthInternal
{


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


template<bool _CheckVal, typename _SrcIt, typename _DestIt>
inline _SrcIt AbiParserCopyBytesThenSkip(
	size_t copyLen,
	size_t skipLen,
	_SrcIt begin,
	_SrcIt end,
	_DestIt dest
)
{
	begin = AbiParserCopyBytes(copyLen, begin, end, dest);
	return AbiParserSkipPadding<_CheckVal>(skipLen, begin, end);
}


} // namespace EthInternal


// ==========
// Essential parser implementations
// ==========


namespace EthInternal
{


template<
	Internal::Obj::ObjCategory _DataType,
	typename... _Args
>
struct AbiParserImpl;


// ==========
// AbiParserImpl for integer types
// ==========


template<Internal::Obj::RealNumType _RealNumType>
struct AbiParserImpl<
	Internal::Obj::ObjCategory::Integer,
	std::integral_constant<Internal::Obj::RealNumType, _RealNumType>
> :
	public AbiCodecImpl<
		Internal::Obj::ObjCategory::Integer,
		std::integral_constant<Internal::Obj::RealNumType, _RealNumType>
	>
{
	using Base = AbiCodecImpl<
		Internal::Obj::ObjCategory::Integer,
		std::integral_constant<Internal::Obj::RealNumType, _RealNumType>
	>;
	using Self = AbiParserImpl<
		Internal::Obj::ObjCategory::Integer,
		std::integral_constant<Internal::Obj::RealNumType, _RealNumType>
	>;
	using Codec = Base;
	using Primitive = typename Base::Primitive;


	template<typename _ItType>
	std::tuple<
		Primitive, /* Parsed value */
		_ItType,   /* Iterator of where the parsing stopped */
		size_t     /* Number of chunks consumed */
	>
	ToPrimitive(_ItType begin, _ItType end) const
	{
		// Skip the leading zero bytes that are larger than the target type
		begin = AbiParserSkipPadding<true>(
			Base::sk_leadPadSize,
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
		>::Parse(Base::sk_consumedSize, inFunc);

		return std::make_tuple(res, begin, 1);
	}
}; // struct AbiParserImpl<Internal::Obj::ObjCategory::Integer, ...>


// ==========
// AbiParserImpl for bool types
// ==========


template<>
struct AbiParserImpl<
	Internal::Obj::ObjCategory::Bool
> : public AbiCodecImpl<Internal::Obj::ObjCategory::Bool>
{
	using Base = AbiCodecImpl<Internal::Obj::ObjCategory::Bool>;
	using Self = AbiParserImpl<Internal::Obj::ObjCategory::Bool>;
	using Codec = Base;
	using Primitive = typename Base::Primitive;

	using IntParserImpl = AbiParserImpl<
		Internal::Obj::ObjCategory::Integer,
		AbiUInt8
	>;
	static_assert(
		std::is_same<typename IntParserImpl::Codec, typename Codec::Base>::value,
		"ABI parser - bool parser must have the same base as uint8 parser"
	);

	template<typename _ItType>
	std::tuple<
		Primitive,  /* Parsed value */
		_ItType,    /* Iterator of where the parsing stopped */
		size_t      /* Number of chunks consumed */
	>
	ToPrimitive(_ItType begin, _ItType end) const
	{
		uint8_t valInt = 0;
		size_t chunkConsumed = 0;
		std::tie(valInt, begin, chunkConsumed) =
			IntParserImpl().ToPrimitive(begin, end);

		bool valBool =
			(valInt == 1 ? true  :
			(valInt == 0 ? false :
				throw Exception("ABI parser - invalid bool value")));

		return std::make_tuple(valBool, begin, chunkConsumed);
	}
}; // struct AbiParserImpl<Internal::Obj::ObjCategory::Bool>


// ==========
// AbiParserImpl for bytes<M> types
// ==========


template<>
struct AbiParserImpl<
	Internal::Obj::ObjCategory::Bytes,
	std::false_type /* IsDynamic? - false */
> : public AbiCodecImpl<Internal::Obj::ObjCategory::Bytes, std::false_type>
{
	using Base =
		AbiCodecImpl<Internal::Obj::ObjCategory::Bytes, std::false_type>;
	using Self =
		AbiParserImpl<Internal::Obj::ObjCategory::Bytes, std::false_type>;
	using Codec = Base;
	using Primitive = typename Base::Primitive;

	/**
	 * @brief Construct a new Abi Parser Impl object
	 *        NOTE: This constructor does not check the size value,
	 *        since this is an internal type and we assume the size is checked
	 *        before calling this
	 *
	 * @param size The size of the bytes, which should be within the range of
	 *             (0, 32]
	 */
	AbiParserImpl(size_t size) :
		m_size(size),
		m_padSize(AbiCodecConst::sk_chunkSize() - m_size)
	{}
	~AbiParserImpl() = default;


	template<typename _ItType>
	std::tuple<
		Primitive,  /* Parsed value */
		_ItType,    /* Iterator of where the parsing stopped */
		size_t      /* Number of chunks consumed */
	>
	ToPrimitive(_ItType begin, _ItType end) const
	{
		Primitive res;
		res.reserve(m_size);

		begin = EthInternal::AbiParserCopyBytesThenSkip<true>(
			m_size,
			m_padSize,
			begin,
			end,
			std::back_inserter(res)
		);

		return std::make_tuple(res, begin, 1);
	}

private:

	size_t m_size;
	size_t m_padSize;

}; // struct AbiParserImpl<Internal::Obj::ObjCategory::Bytes, false>


// ==========
// AbiParserImpl for bytes types
// ==========


template<>
struct AbiParserImpl<
	Internal::Obj::ObjCategory::Bytes,
	std::true_type /* IsDynamic? - true */
> : public AbiCodecImpl<Internal::Obj::ObjCategory::Bytes, std::true_type>
{
	using Base =
		AbiCodecImpl<Internal::Obj::ObjCategory::Bytes, std::true_type>;
	using Self =
		AbiParserImpl<Internal::Obj::ObjCategory::Bytes, std::true_type>;
	using Codec = Base;
	using Primitive = typename Base::Primitive;

	using DynLenParser =
		AbiParserImpl<Internal::Obj::ObjCategory::Integer, AbiUInt64>;

	template<typename _ItType>
	std::tuple<
		Primitive, /* Parsed value */
		_ItType,       /* Iterator of where the parsing stopped */
		size_t         /* Number of chunks consumed */
	>
	ToPrimitive(_ItType begin, _ItType end) const
	{
		// first, parse the length of the bytes
		uint64_t len = 0;
		size_t chunkConsumed = 0;
		std::tie(len, begin, chunkConsumed) =
			DynLenParser().ToPrimitive(begin, end);

		size_t numChunk = EthInternal::AbiCeilingDiv(
			static_cast<size_t>(len),
			AbiCodecConst::sk_chunkSize()
		);
		size_t paddingSize =
			(numChunk * AbiCodecConst::sk_chunkSize()) - len;

		Primitive res;
		res.reserve(len);
		begin = EthInternal::AbiParserCopyBytesThenSkip<true>(
			len,
			paddingSize,
			begin,
			end,
			std::back_inserter(res)
		);

		// Calc the number of chunks consumed
		chunkConsumed += numChunk;

		return std::make_tuple(res, begin, chunkConsumed);
	}

}; // struct AbiParserImpl<Internal::Obj::ObjCategory::Bytes, true>


// ==========
// AbiParserImpl for T[k] types, where T is static type
// ==========


template<typename _ItemParser>
struct AbiParserImpl<
	Internal::Obj::ObjCategory::List,
	_ItemParser,
	std::false_type, /* IsLenDynamic? - false */
	std::false_type  /* IsItemDynamic? - false */
> :
	public AbiCodecImpl<
		Internal::Obj::ObjCategory::List,
		typename _ItemParser::Codec,
		std::false_type,
		std::false_type
	>
{
	using Base = AbiCodecImpl<
		Internal::Obj::ObjCategory::List,
		typename _ItemParser::Codec,
		std::false_type,
		std::false_type
	>;
	using Self = AbiParserImpl<
		Internal::Obj::ObjCategory::List,
		_ItemParser,
		std::false_type,
		std::false_type
	>;

	using Codec = Base;
	using ItemParser = _ItemParser;
	using ItemPrimitive = typename Base::ItemPrimitive;
	using Primitive = typename Base::Primitive;

	AbiParserImpl(ItemParser itemParser, size_t size) :
		m_itemParser(std::move(itemParser)),
		m_size(size)
	{}
	~AbiParserImpl() = default;


	template<typename _ItType>
	std::tuple<
		Primitive, /* Parsed value */
		_ItType,       /* Iterator of where the parsing stopped */
		size_t         /* Number of chunks consumed */
	>
	ToPrimitive(size_t size, _ItType begin, _ItType end) const
	{
		// parse through heads
		Primitive res;
		res.reserve(size);
		size_t totalChunkConsumed = 0;
		for (size_t i = 0; i < size; ++i)
		{
			ItemPrimitive item;
			size_t chunkConsumed = 0;
			std::tie(item, begin, chunkConsumed) =
				m_itemParser.HeadToPrimitive(begin, end);
			res.push_back(item);
			totalChunkConsumed += chunkConsumed;
		}

		return std::make_tuple(res, begin, totalChunkConsumed);
	}


	template<typename _ItType>
	std::tuple<
		Primitive, /* Parsed value */
		_ItType,       /* Iterator of where the parsing stopped */
		size_t         /* Number of chunks consumed */
	>
	ToPrimitive(_ItType begin, _ItType end) const
	{
		return ToPrimitive(m_size, begin, end);
	}

private:

	ItemParser m_itemParser;
	size_t m_size;

}; // struct AbiParserImpl<Internal::Obj::ObjCategory::List, _Item, false, false>


// ==========
// AbiParserImpl for T[k] types, where T is dynamic type
// ==========


template<typename _ItemParser>
struct AbiParserImpl<
	Internal::Obj::ObjCategory::List,
	_ItemParser,
	std::false_type, /* IsLenDynamic? - false */
	std::true_type  /* IsItemDynamic? - true */
> :
	public AbiCodecImpl<
		Internal::Obj::ObjCategory::List,
		typename _ItemParser::Codec,
		std::false_type,
		std::true_type
	>
{
	using Base = AbiCodecImpl<
		Internal::Obj::ObjCategory::List,
		typename _ItemParser::Codec,
		std::false_type,
		std::true_type
	>;
	using Self = AbiParserImpl<
		Internal::Obj::ObjCategory::List,
		_ItemParser,
		std::false_type,
		std::true_type
	>;

	using Codec = Base;
	using ItemParser = _ItemParser;
	using ItemPrimitive = typename Base::ItemPrimitive;
	using Primitive = typename Base::Primitive;


	AbiParserImpl(ItemParser itemParser, size_t size) :
		m_itemParser(std::move(itemParser)),
		m_size(size)
	{}
	~AbiParserImpl() = default;


	template<typename _ItType>
	std::tuple<
		Primitive,  /* Parsed value */
		_ItType,    /* Iterator of where the parsing stopped */
		size_t      /* Number of chunks consumed */
	>
	ToPrimitive(size_t size, _ItType begin, _ItType end) const
	{
		using HeadPrimitive = typename ItemParser::HeadPrimitive;

		// parse through heads
		std::vector<HeadPrimitive> heads;
		heads.reserve(size);
		size_t totalChunkConsumed = 0;
		for (size_t i = 0; i < size; ++i)
		{
			HeadPrimitive head = 0;
			size_t chunkConsumed = 0;
			std::tie(head, begin, chunkConsumed) =
				m_itemParser.HeadToPrimitive(begin, end);
			heads.push_back(head);
			totalChunkConsumed += chunkConsumed;
		}

		// parse through tails
		Primitive res;
		res.reserve(size);
		for (const auto& head : heads)
		{
			// check the offset is correct
			size_t bytesConsumed =
				totalChunkConsumed * AbiCodecConst::sk_chunkSize();
			if (head != bytesConsumed)
			{
				throw Exception("ABI parser - invalid offset");
			}
			ItemPrimitive item;
			size_t chunkConsumed = 0;
			std::tie(item, begin, chunkConsumed) =
				m_itemParser.TailToPrimitive(begin, end);
			res.push_back(item);
			totalChunkConsumed += chunkConsumed;
		}

		return std::make_tuple(res, begin, totalChunkConsumed);
	}


	template<typename _ItType>
	std::tuple<
		Primitive,  /* Parsed value */
		_ItType,    /* Iterator of where the parsing stopped */
		size_t      /* Number of chunks consumed */
	>
	ToPrimitive(_ItType begin, _ItType end) const
	{
		return ToPrimitive(m_size, begin, end);
	}

private:

	ItemParser m_itemParser;
	size_t m_size;

}; // struct AbiParserImpl<Internal::Obj::ObjCategory::List, _Item, false, true>


// ==========
// AbiParserImpl for T[] types, where T is static type
// ==========


template<typename _ItemParser, typename _IsItemDyn>
struct AbiParserDynLenListImpl :
	public AbiCodecImpl<
		Internal::Obj::ObjCategory::List,
		typename _ItemParser::Codec,
		std::true_type,
		_IsItemDyn
	>
{
	using Base = AbiCodecImpl<
		Internal::Obj::ObjCategory::List,
		typename _ItemParser::Codec,
		std::true_type,
		_IsItemDyn
	>;
	using Self = AbiParserDynLenListImpl<_ItemParser, _IsItemDyn>;

	using Codec = Base;
	using ItemParser = _ItemParser;
	using ItemPrimitive = typename Base::ItemPrimitive;
	using Primitive = typename Base::Primitive;

	using DynLenParser = AbiParserImpl<
		Internal::Obj::ObjCategory::Integer,
		AbiUInt64
	>;
	using DataParser = AbiParserImpl<
		Internal::Obj::ObjCategory::List,
		ItemParser,
		std::false_type, /* IsLenDynamic? - false */
		_IsItemDyn  /* IsItemDynamic? - false */
	>;


	AbiParserDynLenListImpl(ItemParser itemParser) :
		m_dataParser(std::move(itemParser), 0)
	{}
	~AbiParserDynLenListImpl() = default;


	template<typename _ItType>
	std::tuple<
		Primitive,  /* Parsed value */
		_ItType,    /* Iterator of where the parsing stopped */
		size_t      /* Number of chunks consumed */
	>
	ToPrimitive(_ItType begin, _ItType end) const
	{
		// first, parse the length of the bytes
		size_t totalChunkConsumed = 0;

		uint64_t len = 0;
		size_t chunkConsumed = 0;
		std::tie(len, begin, chunkConsumed) =
			DynLenParser().ToPrimitive(begin, end);
		totalChunkConsumed += chunkConsumed;

		size_t lenSize = static_cast<size_t>(len);

		// then, parse the data / list items
		Primitive res;
		std::tie(res, begin, chunkConsumed) =
			m_dataParser.ToPrimitive(lenSize, begin, end);
		totalChunkConsumed += chunkConsumed;

		return std::make_tuple(res, begin, totalChunkConsumed);
	}

private:

	DataParser m_dataParser;

}; // struct AbiParserDynLenListImpl


template<typename _ItemParser>
struct AbiParserImpl<
	Internal::Obj::ObjCategory::List,
	_ItemParser,
	std::true_type, /* IsLenDynamic? - true */
	std::false_type  /* IsItemDynamic? - true */
> :
	public AbiParserDynLenListImpl<_ItemParser, std::false_type>
{
	using Base = AbiParserDynLenListImpl<_ItemParser, std::false_type>;
	using Self = AbiParserImpl<
		Internal::Obj::ObjCategory::List,
		_ItemParser,
		std::true_type,
		std::false_type
	>;

	using Base::Base;
}; // struct AbiParserImpl<Internal::Obj::ObjCategory::List, _Item, true, false>


// ==========
// AbiParserImpl for T[] types, where T is dynamic type
// ==========


template<typename _ItemParser>
struct AbiParserImpl<
	Internal::Obj::ObjCategory::List,
	_ItemParser,
	std::true_type, /* IsLenDynamic? - true */
	std::true_type  /* IsItemDynamic? - true */
> :
	public AbiParserDynLenListImpl<_ItemParser, std::true_type>
{
	using Base = AbiParserDynLenListImpl<_ItemParser, std::true_type>;
	using Self = AbiParserImpl<
		Internal::Obj::ObjCategory::List,
		_ItemParser,
		std::true_type,
		std::true_type
	>;

	using Base::Base;
}; // struct AbiParserImpl<Internal::Obj::ObjCategory::List, _Item, true, true>


// ==========
// Wrappers for ABI parser
// ==========


template<
	typename _ParserImpl
>
struct AbiParserHeadOnlyTypes
{
	using ParserImpl = _ParserImpl;
	using Codec = typename ParserImpl::Codec;

	using HeadParserImpl = ParserImpl;

	static constexpr bool sk_hasTail = false;
	using HeadPrimitive = typename HeadParserImpl::Primitive;
	using Primitive = HeadPrimitive;

	// constructors
	AbiParserHeadOnlyTypes(HeadParserImpl headCodec) :
		m_headCodec(std::move(headCodec))
	{}
	// destructor
	// LCOV_EXCL_START
	virtual ~AbiParserHeadOnlyTypes() = default;
	// LCOV_EXCL_STOP


	template<typename _ItType>
	std::tuple<
		HeadPrimitive, /* Parsed value */
		_ItType,       /* Iterator of where the parsing stopped */
		size_t         /* Number of chunks consumed */
	>
	HeadToPrimitive(_ItType begin, _ItType end) const
	{
		return m_headCodec.ToPrimitive(begin, end);
	}


	template<typename _ItType>
	std::tuple<
		Primitive, /* Parsed value */
		_ItType    /* Iterator of where the *head* parsing stopped */
	>
	ToPrimitive(_ItType begin, _ItType end, _ItType) const
	{
		auto res = HeadToPrimitive(begin, end);
		return std::make_tuple(
			std::get<0>(res),
			std::get<1>(res)
		);
	}

protected:

	HeadParserImpl m_headCodec;

}; // struct AbiParserHeadOnlyTypes


template<
	typename _ParserImpl
>
struct AbiParserHeadTailTypes
{
	using ParserImpl = _ParserImpl;
	using Codec = typename ParserImpl::Codec;

	/**
	 * @brief The type of the parser used to parse the head part of the ABI data
	 *        NOTE: the ABI spec assume the head is always a uint256 offset,
	 *        but here we assume a uint64 offset for simplicity, and it's
	 *        very unlikely that the offset will be larger than uint64 in
	 *        real-world cases.
	 *
	 */
	using HeadParserImpl = AbiParserImpl<
		Internal::Obj::ObjCategory::Integer,
		AbiUInt64
	>; // head is a uint256 offset

	using TailParserImpl = ParserImpl;

	static constexpr bool sk_hasTail = true;
	using HeadPrimitive = typename HeadParserImpl::Primitive;
	using TailPrimitive = typename TailParserImpl::Primitive;
	using Primitive = TailPrimitive;

	// constructors
	AbiParserHeadTailTypes(TailParserImpl tailCodec) :
		m_headCodec(),
		m_tailCodec(std::move(tailCodec))
	{}
	// destructor
	// LCOV_EXCL_START
	virtual ~AbiParserHeadTailTypes() = default;
	// LCOV_EXCL_STOP


	template<typename _ItType>
	std::tuple<
		HeadPrimitive, /* Parsed value */
		_ItType,       /* Iterator of where the parsing stopped */
		size_t         /* Number of chunks consumed */
	>
	HeadToPrimitive(_ItType begin, _ItType end) const
	{
		return m_headCodec.ToPrimitive(begin, end);
	}


	template<typename _ItType>
	std::tuple<
		TailPrimitive, /* Parsed value */
		_ItType,       /* Iterator of where the parsing stopped */
		size_t         /* Number of chunks consumed */
	>
	TailToPrimitive(_ItType begin, _ItType end) const
	{
		return m_tailCodec.ToPrimitive(begin, end);
	}


	/**
	 * @brief Parse the ABI-encoded data
	 *        NOTE: this function will parse the offset and then jump to the
	 *        tail part of the data. So if the given iterator is type of
	 *        `input_iterator` or `forward_iterator`, the data in the range
	 *        that is being jumped over will be lost.
	 *
	 * @tparam _ItType
	 * @param begin
	 * @param end
	 * @param blockBegin
	 * @return
	 */
	template<typename _ItType>
	std::tuple<
		Primitive, /* Parsed value */
		_ItType    /* Iterator of where the *head* parsing stopped */
	>
	ToPrimitive(_ItType begin, _ItType end, _ItType blockBegin) const
	{
		using _ItDiffType =
			typename std::iterator_traits<_ItType>::difference_type;

		HeadPrimitive offset = 0;
		_ItType headEnd = begin;
		std::tie(offset, headEnd, std::ignore) = HeadToPrimitive(begin, end);

		// make sure we can jump to the offset
		auto totalSizeDiff = std::distance(blockBegin, end);
		if (totalSizeDiff < 0)
		{
			throw Exception("ABI parser - invalid block begin iterator");
		}
		size_t totalSize = static_cast<size_t>(totalSizeDiff);
		if (offset > totalSize)
		{
			throw Exception("ABI parser - the input is too short");
		}
		begin = std::next(blockBegin, static_cast<_ItDiffType>(offset));

		// parse the tail
		Primitive res;
		std::tie(res, std::ignore, std::ignore) =
			TailToPrimitive(begin, end);

		return std::make_tuple(res, headEnd);
	}

protected:

	HeadParserImpl m_headCodec;
	TailParserImpl m_tailCodec;

}; // struct AbiParserHeadTailTypes


template<bool _HasTail, typename _ParserImpl>
using AbiHeadTailTypesSelector = typename std::conditional<
	_HasTail,
	AbiParserHeadTailTypes<_ParserImpl>,
	AbiParserHeadOnlyTypes<_ParserImpl>
>::type;


} // namespace EthInternal


// ==========
// AbiParser general template
// ==========

template<
	Internal::Obj::ObjCategory _DataType,
	typename... _Args
>
struct AbiParser;


// ==========
// AbiParser for integer types
// ==========


template<Internal::Obj::RealNumType _RealNumType>
struct AbiParser<
	Internal::Obj::ObjCategory::Integer,
	std::integral_constant<Internal::Obj::RealNumType, _RealNumType>
> :
	EthInternal::AbiParserHeadOnlyTypes<
		EthInternal::AbiParserImpl<
			Internal::Obj::ObjCategory::Integer,
			std::integral_constant<Internal::Obj::RealNumType, _RealNumType>
		>
	>
{
	using ParserImpl = EthInternal::AbiParserImpl<
		Internal::Obj::ObjCategory::Integer,
		std::integral_constant<Internal::Obj::RealNumType, _RealNumType>
	>;
	using Base = EthInternal::AbiParserHeadOnlyTypes<ParserImpl>;
	using Codec = typename ParserImpl::Codec;

	AbiParser() :
		Base(ParserImpl())
	{}
	// LCOV_EXCL_START
	virtual ~AbiParser() = default;
	// LCOV_EXCL_STOP
}; // struct AbiParser<Internal::Obj::ObjCategory::Integer, ...>


// ==========
// AbiParser for bool type
// ==========


template<>
struct AbiParser<
	Internal::Obj::ObjCategory::Bool
> :
	EthInternal::AbiParserHeadOnlyTypes<
		EthInternal::AbiParserImpl<
			Internal::Obj::ObjCategory::Bool
		>
	>
{
	using ParserImpl = EthInternal::AbiParserImpl<
		Internal::Obj::ObjCategory::Bool
	>;
	using Base = EthInternal::AbiParserHeadOnlyTypes<ParserImpl>;
	using Codec = typename ParserImpl::Codec;

	AbiParser() :
		Base(ParserImpl())
	{}
	// LCOV_EXCL_START
	virtual ~AbiParser() = default;
	// LCOV_EXCL_STOP
}; // struct AbiParser<Internal::Obj::ObjCategory::Bool>


// ==========
// AbiParser for bytes<M> types
// ==========


template<>
struct AbiParser<
	Internal::Obj::ObjCategory::Bytes,
	std::false_type /* IsDynamic? - false */
> :
	EthInternal::AbiParserHeadOnlyTypes<
		EthInternal::AbiParserImpl<
			Internal::Obj::ObjCategory::Bytes,
			std::false_type
		>
	>
{
	using ParserImpl = EthInternal::AbiParserImpl<
		Internal::Obj::ObjCategory::Bytes,
		std::false_type
	>;
	using Base = EthInternal::AbiParserHeadOnlyTypes<ParserImpl>;
	using Codec = typename ParserImpl::Codec;


	AbiParser(size_t size) :
		Base(ParserImpl(EthInternal::AbiWithinChunkSize(size)))
	{}
	// LCOV_EXCL_START
	virtual ~AbiParser() = default;
	// LCOV_EXCL_STOP
}; // struct AbiParser<Internal::Obj::ObjCategory::Bytes, false>


template<size_t _Size>
struct AbiParser<
	Internal::Obj::ObjCategory::Bytes,
	std::integral_constant<size_t, _Size>
> :
	EthInternal::AbiParserHeadOnlyTypes<
		EthInternal::AbiParserImpl<
			Internal::Obj::ObjCategory::Bytes,
			std::false_type
		>
	>
{
	using ParserImpl = EthInternal::AbiParserImpl<
		Internal::Obj::ObjCategory::Bytes,
		std::false_type
	>;
	using Base = EthInternal::AbiParserHeadOnlyTypes<ParserImpl>;
	using Codec = typename ParserImpl::Codec;

	static constexpr size_t sk_size = _Size;
	static_assert(
		sk_size <= AbiCodecConst::sk_chunkSize(),
		"ABI parser - bytes type is too large"
	);

	AbiParser() :
		Base(ParserImpl(sk_size))
	{}
	// LCOV_EXCL_START
	virtual ~AbiParser() = default;
	// LCOV_EXCL_STOP
}; // struct AbiParser<Internal::Obj::ObjCategory::Bytes, size_t>


// ==========
// AbiParser for bytes types
// ==========


template<>
struct AbiParser<
	Internal::Obj::ObjCategory::Bytes,
	std::true_type /* IsDynamic? - true */
> :
	EthInternal::AbiParserHeadTailTypes<
		EthInternal::AbiParserImpl<
			Internal::Obj::ObjCategory::Bytes,
			std::true_type
		>
	>
{
	using ParserImpl = EthInternal::AbiParserImpl<
		Internal::Obj::ObjCategory::Bytes,
		std::true_type
	>;
	using Base = EthInternal::AbiParserHeadTailTypes<ParserImpl>;
	using Codec = typename ParserImpl::Codec;


	AbiParser() :
		Base(ParserImpl())
	{}
	// LCOV_EXCL_START
	virtual ~AbiParser() = default;
	// LCOV_EXCL_STOP
}; // struct AbiParser<Internal::Obj::ObjCategory::Bytes, true>


// ==========
// AbiParser for list types (T[k])
// ==========


template<typename _ItemParser>
struct AbiParser<
	Internal::Obj::ObjCategory::List,
	_ItemParser,
	std::false_type /* IsLenDynamic? - false */
> :
	EthInternal::AbiHeadTailTypesSelector<
		_ItemParser::Codec::sk_isDynamic,
		EthInternal::AbiParserImpl<
			Internal::Obj::ObjCategory::List,
			_ItemParser,
			std::false_type,
			typename _ItemParser::Codec::IsDynamic
		>
	>
{
	using ParserImpl = EthInternal::AbiParserImpl<
		Internal::Obj::ObjCategory::List,
		_ItemParser,
		std::false_type,
		typename _ItemParser::Codec::IsDynamic
	>;
	using Base = EthInternal::AbiHeadTailTypesSelector<
		_ItemParser::Codec::sk_isDynamic,
		ParserImpl
	>;
	using Codec = typename ParserImpl::Codec;


	AbiParser(_ItemParser itemParser, size_t len) :
		Base(ParserImpl(std::move(itemParser), len))
	{}
	// LCOV_EXCL_START
	virtual ~AbiParser() = default;
	// LCOV_EXCL_STOP
}; // struct AbiParser<Internal::Obj::ObjCategory::List, _Item, false>


template<typename _ItemParser, size_t _Size>
struct AbiParser<
	Internal::Obj::ObjCategory::List,
	_ItemParser,
	std::integral_constant<size_t, _Size>
> :
	EthInternal::AbiHeadTailTypesSelector<
		_ItemParser::Codec::sk_isDynamic,
		EthInternal::AbiParserImpl<
			Internal::Obj::ObjCategory::List,
			_ItemParser,
			std::false_type,
			typename _ItemParser::Codec::IsDynamic
		>
	>
{
	using ParserImpl = EthInternal::AbiParserImpl<
		Internal::Obj::ObjCategory::List,
		_ItemParser,
		std::false_type,
		typename _ItemParser::Codec::IsDynamic
	>;
	using Base = EthInternal::AbiHeadTailTypesSelector<
		_ItemParser::Codec::sk_isDynamic,
		ParserImpl
	>;
	using Codec = typename ParserImpl::Codec;

	static constexpr size_t sk_size = _Size;

	AbiParser(_ItemParser itemParser) :
		Base(ParserImpl(std::move(itemParser), sk_size))
	{}
	// LCOV_EXCL_START
	virtual ~AbiParser() = default;
	// LCOV_EXCL_STOP
}; // struct AbiParser<Internal::Obj::ObjCategory::List, _Item, Len>


// ==========
// AbiParser for list types (T[])
// ==========


template<typename _ItemParser>
struct AbiParser<
	Internal::Obj::ObjCategory::List,
	_ItemParser,
	std::true_type /* IsLenDynamic? - true */
> :
	EthInternal::AbiParserHeadTailTypes<
		// dynamic length make this type always has tail
		EthInternal::AbiParserImpl<
			Internal::Obj::ObjCategory::List,
			_ItemParser,
			std::true_type,
			typename _ItemParser::Codec::IsDynamic
		>
	>
{
	using ParserImpl = EthInternal::AbiParserImpl<
		Internal::Obj::ObjCategory::List,
		_ItemParser,
		std::true_type,
		typename _ItemParser::Codec::IsDynamic
	>;
	using Base = EthInternal::AbiParserHeadTailTypes<ParserImpl>;
	using Codec = typename ParserImpl::Codec;


	explicit AbiParser(_ItemParser itemParser) :
		Base(ParserImpl(std::move(itemParser)))
	{}
	// LCOV_EXCL_START
	virtual ~AbiParser() = default;
	// LCOV_EXCL_STOP
}; // struct AbiParser<Internal::Obj::ObjCategory::List, _Item, true>


} // namespace Eth
} // namespace EclipseMonitor

