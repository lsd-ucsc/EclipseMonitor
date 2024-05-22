// Copyright (c) 2024 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once


#include <cstdint>

#include <tuple>
#include <type_traits>

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


/**
 * @brief Write padding to the output stream
 *
 * @tparam _It The type of the iterator that accepts a byte in a time
 * @tparam _ValType The type of the value to be filled as padding
 *
 * @param it The output iterator that accepts a byte in a time
 * @param size The length of the padding in bytes
 * @param val The value to be filled as padding
 * @return The iterator pointing the latest end of the output stream
 */
template<typename _It, typename _ValType>
inline _It AbiWritePadding(_It it, size_t size, const _ValType& val)
{
	for (size_t i = 0; i < size; ++i)
	{
		*(it++) = val;
	}

	return it;
}


/**
 * @brief Write given bytes to the destination stream
 *
 * @tparam _DestIt The type of the iterators of the output stream
 * @tparam _SrcIt The type of the iterators of the input byte stream
 *
 * @param destIt The iterator of the output stream; the iterator should accept
 *               a byte in a time
 * @param begin The iterator pointing the beginning of the input steam
 * @param end   The iterator pointing the end of the input stream
 * @return The iterator pointing the latest end of the output stream
 */
template<typename _DestIt, typename _SrcIt>
inline _DestIt AbiWriteBytes(
	_DestIt destIt,
	_SrcIt  begin,
	_SrcIt  end
)
{
	for (auto it = begin; it != end; ++it)
	{
		*(destIt++) = *(it);
	}
	return destIt;
}


/**
 * @brief Write given bytes to the destination stream and then fill in padding
 *        bytes
 *
 * @tparam _DestIt The type of the iterators of the output stream
 * @tparam _SrcIt  The type of the iterators of the input byte stream
 * @tparam _ValType The type of the value to be filled as padding
 *
 * @param destIt The iterator of the output stream
 * @param begin The iterator pointing the beginning of the input steam
 * @param end   The iterator pointing the end of the input stream
 * @param padSize The length of the padding
 * @param val The value to be filled as padding
 * @return The iterator pointing the latest end of the output stream
 */
template<typename _DestIt, typename _SrcIt, typename _ValType>
inline _DestIt AbiWriteBytesThenPad(
	_DestIt destIt,
	_SrcIt  begin,
	_SrcIt  end,
	size_t  padSize,
	const _ValType& val
)
{
	destIt = AbiWriteBytes(destIt, begin, end);
	destIt = AbiWritePadding(destIt, padSize, val);
	return destIt;
}


/**
 * @brief The type that the writer is going to write the bytes to.
 *
 */
using WrittenBytes = std::vector<uint8_t>;

/**
 * @brief The type of output iterator that accepts a byte in a time
 *
 */
using WriteIterator = Internal::Obj::OutIterator<uint8_t>;


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
struct AbiWriterImpl;


// ==========
// AbiWriterImpl for integer types
// ==========


template<Internal::Obj::RealNumType _RealNumType>
struct AbiWriterUIntImpl :
	public AbiCodecImpl<
		Internal::Obj::ObjCategory::Integer,
		std::integral_constant<Internal::Obj::RealNumType, _RealNumType>
	>
{
	using Base = AbiCodecImpl<
		Internal::Obj::ObjCategory::Integer,
		std::integral_constant<Internal::Obj::RealNumType, _RealNumType>
	>;
	using Self = AbiWriterUIntImpl<_RealNumType>;
	using Codec = Base;

	/**
	 * @brief Get the number of head chunks required to store the integer
	 *        NOTE: The integer type is always a single head chunk
	 *
	 * @return The number of chunks required to store the integer
	 */
	size_t GetNumHeadChunks() const
	{
		return 1;
	}

	/**
	 * @brief Get the number of tail chunks required to store the integer
	 *        NOTE: The integer type does not have tail chunks
	 *
	 * @return The number of chunks required to store the integer
	 */
	size_t GetNumTailChunks() const
	{
		return 0;
	}

	size_t GetNumTailChunks(const Internal::Obj::BaseObj&) const
	{
		return GetNumTailChunks();
	}

	/**
	 * @brief Write the given integer value to the destination stream
	 *
	 * @tparam _DestIt  The type of output iterator that accepts a byte in
	 *                  a time
	 * @tparam _IntType The type of the integer value to be written
	 *
	 * @param destIt   The output iterator that accepts a byte in a time
	 * @param val      The integer value to be written
	 * @param bytesWidth The width of the integer in bytes;
	 *                   NOTE: it's the caller's responsibility to ensure
	 *                   that an appropriate width value is given
	 * @return The iterator pointing the latest end of the output stream
	 */
	template<typename _DestIt, typename _IntType>
	_DestIt Write(_DestIt destIt, const _IntType& val, size_t bytesWidth) const
	{
		if (bytesWidth > AbiCodecConst::sk_chunkSize())
		{
			throw Exception(
				"ABI writer - integer width exceeds the maximum width"
			);
		}

		size_t padNeeded = AbiCodecConst::sk_chunkSize() - bytesWidth;
		destIt = AbiWritePadding(destIt, padNeeded, uint8_t(0));

		destIt = Internal::Rlp::EncodePrimitiveIntValue<
			_IntType,
			Internal::Rlp::Endian::native,
			false
		>::WriteBytes(destIt, val, bytesWidth);

		return destIt;
	}

}; // struct AbiWriterUIntImpl


template<Internal::Obj::RealNumType _RealNumType>
struct AbiWriterImpl<
	Internal::Obj::ObjCategory::Integer,
	std::integral_constant<Internal::Obj::RealNumType, _RealNumType>
> :
	public AbiWriterUIntImpl<_RealNumType>
{
	using Base = AbiWriterUIntImpl<_RealNumType>;
	using Self = AbiWriterImpl<
		Internal::Obj::ObjCategory::Integer,
		std::integral_constant<Internal::Obj::RealNumType, _RealNumType>
	>;

	using Codec = typename Base::Codec;
	using RealNumTraits = typename Codec::RealNumTraits;
	using IntType = typename RealNumTraits::Primitive;

	template<typename _DestIt>
	_DestIt Write(_DestIt destIt, const IntType& val) const
	{
		return Base::Write(destIt, val, RealNumTraits::sk_consumedSize());
	}

	template<typename _DestIt>
	_DestIt Write(_DestIt destIt, const Internal::Obj::RealNumBaseObj& val) const
	{
		return Self::Write(destIt, RealNumTraits::FromRealNumBase(val));
	}

	template<typename _DestIt>
	_DestIt WriteObj(_DestIt destIt, const Internal::Obj::BaseObj& val) const
	{
		return Self::Write(destIt, val.AsRealNum());
	}

}; // struct AbiWriterImpl<Internal::Obj::ObjCategory::Integer, RealNumType>


// ==========
// AbiWriterImpl for bool types
// ==========


template<>
struct AbiWriterImpl<
	Internal::Obj::ObjCategory::Bool
> :
	public AbiCodecImpl<Internal::Obj::ObjCategory::Bool>
{
	using Base = AbiCodecImpl<Internal::Obj::ObjCategory::Bool>;
	using Self = AbiWriterImpl<Internal::Obj::ObjCategory::Bool>;

	using Codec = Base;
	using IntWriterImpl = AbiWriterImpl<
		Internal::Obj::ObjCategory::Integer,
		AbiUInt8
	>;
	static_assert(
		std::is_same<typename IntWriterImpl::Codec, typename Codec::Base>::value,
		"ABI codec - bool writer must have the same codec as uint8 parser"
	);

	// AbiWriterImpl(IntWriterImpl intWriter) :
	// 	m_intWriter(std::move(intWriter))
	// {}

	// ~AbiWriterImpl() = default;

	/**
	 * @brief Get the number of head chunks required to store the bool
	 *        NOTE: The bool type is always a single head chunk
	 *
	 * @return The number of chunks required to store the bool
	 */
	size_t GetNumHeadChunks() const
	{
		return 1;
	}

	/**
	 * @brief Get the number of tail chunks required to store the bool
	 *        NOTE: The bool type does not have tail chunks
	 *
	 * @return The number of chunks required to store the bool
	 */
	size_t GetNumTailChunks() const
	{
		return 0;
	}

	size_t GetNumTailChunks(const Internal::Obj::BaseObj&) const
	{
		return GetNumTailChunks();
	}

	/**
	 * @brief Write the given bool value to the destination stream
	 *
	 * @tparam _DestIt  The type of output iterator that accepts a byte in
	 *                  a time
	 *
	 * @param destIt   The output iterator that accepts a byte in a time
	 * @param val      The bool value to be written
	 * @return The iterator pointing the latest end of the output stream
	 */
	template<typename _DestIt>
	_DestIt Write(_DestIt destIt, bool val) const
	{
		return IntWriterImpl().Write(destIt, val ? uint8_t(1) : uint8_t(0));
	}

	template<typename _DestIt>
	_DestIt Write(_DestIt destIt, const Internal::Obj::RealNumBaseObj& val) const
	{
		return Self::Write(destIt, val.IsTrue());
	}

	template<typename _DestIt>
	_DestIt WriteObj(_DestIt destIt, const Internal::Obj::BaseObj& val) const
	{
		return Self::Write(destIt, val.AsRealNum());
	}

private:

	// IntWriterImpl m_intWriter;

}; // struct AbiWriterImpl<Internal::Obj::ObjCategory::Bool>


// ==========
// AbiWriterImpl for bytes<M> types
// ==========


template<>
struct AbiWriterImpl<
	Internal::Obj::ObjCategory::Bytes,
	std::false_type // IsDynamic? - false
> : public AbiCodecImpl<Internal::Obj::ObjCategory::Bytes, std::false_type>
{
	using Base =
		AbiCodecImpl<Internal::Obj::ObjCategory::Bytes, std::false_type>;
	using Self =
		AbiWriterImpl<Internal::Obj::ObjCategory::Bytes, std::false_type>;

	using Codec = Base;

	/**
	 * @brief Construct a new Abi Parser Impl object
	 *        NOTE: This constructor does not check the size value,
	 *        since this is an internal type and we assume the size is checked
	 *        before calling this
	 *
	 * @param size The size of the bytes, which should be within the range of
	 *             (0, 32]
	 */
	AbiWriterImpl(size_t size) :
		m_size(size),
		m_padSize(AbiCodecConst::sk_chunkSize() - m_size)
	{}

	~AbiWriterImpl() = default;

	/**
	 * @brief Get the number of head chunks required to store the static bytes
	 *        NOTE: The static bytes is always a single head chunk
	 *
	 * @return The number of chunks required to store the static bytes
	 */
	size_t GetNumHeadChunks() const
	{
		return 1;
	}

	/**
	 * @brief Get the number of tail chunks required to store the static bytes
	 *        NOTE: The static bytes type does not have tail chunks
	 *
	 * @return The number of chunks required to store the static bytes
	 */
	size_t GetNumTailChunks() const
	{
		return 0;
	}

	size_t GetNumTailChunks(const Internal::Obj::BaseObj&) const
	{
		return GetNumTailChunks();
	}

	/**
	 * @brief Write the given bytes to the destination stream
	 *
	 * @tparam _DestIt The type of output iterator that accepts a byte in a time
	 * @tparam _SrcIt  The type of input iterator that provides a byte in a time
	 *
	 * @param destIt   The output iterator that accepts a byte in a time
	 * @param begin    The iterator pointing the beginning of the input steam
	 * @param end      The iterator pointing the end of the input stream
	 * @return The iterator pointing the latest end of the output stream
	 */
	template<typename _DestIt, typename _SrcIt>
	_DestIt Write(_DestIt destIt, _SrcIt begin, _SrcIt end) const
	{
		return AbiWriteBytesThenPad(
			destIt,
			begin,
			end,
			m_padSize,
			uint8_t(0)
		);
	}

	/**
	 * @brief Write the given bytes to the destination stream
	 *
	 * @tparam _DestIt The type of output iterator that accepts a byte in a time
	 * @tparam _CntT   The type of the container that stores the bytes
	 *
	 * @param destIt   The output iterator that accepts a byte in a time
	 * @param cnt      The container that stores the bytes
	 * @return The iterator pointing the latest end of the output stream
	 */
	template<typename _DestIt, typename _CntT>
	_DestIt Write(_DestIt destIt, const _CntT& cnt) const
	{
		if (cnt.size() != m_size)
		{
			throw Exception(
				"ABI writer - the given bytes should have exactly the same "
				"length as the static bytes type"
			);
		}
		return Write(destIt, cnt.begin(), cnt.end());
	}

	template<typename _DestIt>
	_DestIt WriteObj(_DestIt destIt, const Internal::Obj::BaseObj& val) const
	{
		return Write(destIt, val.AsBytes());
	}

	size_t GetPadSize() const
	{
		return m_padSize;
	}

private:

	size_t m_size;
	size_t m_padSize;

}; // struct AbiWriterImpl<Internal::Obj::ObjCategory::Bytes, false>


// ==========
// AbiWriterImpl for bytes types
// ==========


template<>
struct AbiWriterImpl<
	Internal::Obj::ObjCategory::Bytes,
	std::true_type // IsDynamic? - true
> : public AbiCodecImpl<Internal::Obj::ObjCategory::Bytes, std::true_type>
{
	using Base =
		AbiCodecImpl<Internal::Obj::ObjCategory::Bytes, std::true_type>;
	using Self =
		AbiWriterImpl<Internal::Obj::ObjCategory::Bytes, std::true_type>;

	using Codec = Base;
	using DynLenWriterImpl =
		AbiWriterImpl<Internal::Obj::ObjCategory::Integer, AbiUInt64>;

	// AbiWriterImpl(DynLenWriterImpl dynLenWriter) :
	// 	m_dynLenWriter(std::move(dynLenWriter))
	// {}

	// ~AbiWriterImpl() = default;

	/**
	 * @brief Get the number of head chunks required to store the dynamic bytes
	 *        NOTE: The dynamic bytes always requires a single head chunk
	 *
	 * @return The number of chunks required
	 */
	size_t GetNumHeadChunks() const
	{
		return 1;
	}

	/**
	 * @brief Get the number of data chunks required to store the dynamic bytes
	 *
	 * @return The number of chunks required
	 */
	size_t GetNumDataChunks(size_t len) const
	{
		return EthInternal::AbiCeilingDiv(len, AbiCodecConst::sk_chunkSize());
	}

	/**
	 * @brief Get the number of tail chunks required to store the dynamic bytes
	 *        NOTE: The number of tail chunks required by dynamic bytes:
	 *              - 1 chunk stores the length of the bytes, plus
	 *              - num of data chunks
	 *
	 * @return The number of chunks required
	 */
	size_t GetNumTailChunks(size_t len) const
	{
		return 1 + GetNumDataChunks(len);
	}

	size_t GetNumTailChunks(const Internal::Obj::BytesBaseObj& val) const
	{
		return GetNumTailChunks(val.size());
	}

	size_t GetNumTailChunks(const Internal::Obj::BaseObj& val) const
	{
		return GetNumTailChunks(val.AsBytes());
	}

	/**
	 * @brief Get the size of the padding needed to store the dynamic bytes
	 *
	 * @param len The length of the dynamic bytes
	 * @return The size of the padding needed
	 */
	size_t GetPadSize(size_t len) const
	{
		return (GetNumDataChunks(len) * AbiCodecConst::sk_chunkSize()) - len;
	}

	/**
	 * @brief Write the given bytes to the destination stream
	 *
	 * @tparam _DestIt The type of output iterator that accepts a byte in a time
	 * @tparam _SrcIt  The type of input iterator that provides a byte in a time
	 *
	 * @param destIt   The output iterator that accepts a byte in a time
	 * @param begin    The iterator pointing the beginning of the input steam
	 * @param end      The iterator pointing the end of the input stream
	 * @param len      The length of the input stream; NOTE: it's caller's
	 *                 responsibility to ensure that the length is correct
	 * @return The iterator pointing the latest end of the output stream
	 */
	template<typename _DestIt, typename _SrcIt>
	_DestIt Write(_DestIt destIt, _SrcIt begin, _SrcIt end, size_t len) const
	{
		size_t padSize = GetPadSize(len);

		destIt = DynLenWriterImpl().Write(destIt, len);
		return AbiWriteBytesThenPad(
			destIt,
			begin,
			end,
			padSize,
			uint8_t(0)
		);
	}

	/**
	 * @brief Write the given bytes to the destination stream
	 *
	 * @tparam _DestIt The type of output iterator that accepts a byte in a time
	 * @tparam _SrcIt  The type of input iterator that provides a byte in a time
	 *
	 * @param destIt   The output iterator that accepts a byte in a time
	 * @param begin    The iterator pointing the beginning of the input steam
	 * @param end      The iterator pointing the end of the input stream
	 *
	 * @return The iterator pointing the latest end of the output stream
	 */
	template<typename _DestIt, typename _SrcIt>
	_DestIt Write(_DestIt destIt, _SrcIt begin, _SrcIt end) const
	{
		auto len = std::distance(begin, end);
		if (len < 0)
		{
			throw Exception(
				"ABI writer - invalid iterator range for input bytes"
			);
		}
		return Write(destIt, begin, end, static_cast<size_t>(len));
	}

	/**
	 * @brief Write the given bytes to the destination stream
	 *
	 * @tparam _DestIt The type of output iterator that accepts a byte in a time
	 * @tparam _CntT   The type of the container that stores the bytes
	 *
	 * @param destIt   The output iterator that accepts a byte in a time
	 * @param cnt      The container that stores the bytes
	 *
	 * @return The iterator pointing the latest end of the output stream
	 */
	template<typename _DestIt, typename _CntT>
	_DestIt Write(_DestIt destIt, const _CntT& cnt) const
	{
		return Write(destIt, cnt.begin(), cnt.end(), cnt.size());
	}

	template<typename _DestIt>
	_DestIt WriteObj(_DestIt destIt, const Internal::Obj::BaseObj& val) const
	{
		return Write(destIt, val.AsBytes());
	}

private:

	// DynLenWriterImpl m_dynLenWriter;

}; // struct AbiWriterImpl<Internal::Obj::ObjCategory::Bytes, true>


/**
 * @brief This is one of the classes used to manage the nested writers for a
 *        type that has nested types;
 *        for example, T[] types.
 *        And this class is used to manage the nested writers for T[] types,
 *        which only have a single type, T, as the nested type, and the length
 *        is dynamic.
 *
 * @tparam _ItemWriter
 */
template<typename _ItemWriter>
struct AbiNestedWriterMgrListDynLen
{
	using ItemWriter = _ItemWriter;

	AbiNestedWriterMgrListDynLen(ItemWriter itemWriter) :
		m_itemWriter(std::move(itemWriter))
	{}

	~AbiNestedWriterMgrListDynLen() = default;

	/**
	 * @brief Get the total number of head chunks of nested types,
	 *        NO MATTER IF THIS TYPE IS DYNAMIC OR STATIC
	 *
	 * @param len The length of the list
	 *
	 * @return The total number of head chunks
	 */
	size_t GetTotalNumHeadChunks(size_t len) const
	{
		return len * m_itemWriter.GetNumHeadChunks();
	}

	template<typename _Func, typename _ValIt>
	void IterateVals(_Func func, _ValIt begin, _ValIt end) const
	{
		for(auto it = begin; it != end; ++it)
		{
			func(m_itemWriter, *it);
		}
	}

	ItemWriter m_itemWriter;

}; // struct AbiNestedWriterMgrListDynLen


/**
 * @brief This is one of the classes used to manage the nested writers for a
 *        type that has nested types;
 *        for example, T[k] types, or tuple (T1, T2, ..., Tn) types.
 *        And this class is used to manage the nested writers for T[k] types,
 *        which only have a single type, T, as the nested type; and the length
 *        is constant.
 *
 * @tparam _ItemWriter
 */
template<typename _ItemWriter>
struct AbiNestedWriterMgrListConstLen :
	public AbiNestedWriterMgrListDynLen<_ItemWriter>
{
	using ItemWriter = _ItemWriter;

	using Base = AbiNestedWriterMgrListDynLen<ItemWriter>;
	using Self = AbiNestedWriterMgrListConstLen<ItemWriter>;

	AbiNestedWriterMgrListConstLen(ItemWriter itemWriter, size_t size) :
		Base(std::move(itemWriter)),
		m_size(size)
	{}

	AbiNestedWriterMgrListConstLen(std::pair<ItemWriter, size_t> pair) :
		AbiNestedWriterMgrListConstLen(std::move(pair.first), pair.second)
	{}

	~AbiNestedWriterMgrListConstLen() = default;

	bool IsDynamicType() const
	{
		return (Base::m_itemWriter).IsDynamicType();
	}

	/**
	 * @brief Get the total number of head chunks of nested types,
	 *        NO MATTER IF THIS TYPE IS DYNAMIC OR STATIC
	 *
	 * @return The total number of head chunks
	 */
	size_t GetTotalNumHeadChunks() const
	{
		return Base::GetTotalNumHeadChunks(m_size);
	}

	template<typename _Func, typename _ValIt>
	void IterateVals(_Func func, _ValIt begin, _ValIt end) const
	{
		size_t size = m_size;
		for(auto it = begin; it != end; ++it)
		{
			if (size == 0)
			{
				throw Exception(
					"ABI writer - too many given data than declared"
				);
			}
			func((Base::m_itemWriter), *it);
			--size;
		}
		if (size != 0)
		{
			throw Exception(
				"ABI writer - the number of given data is less than declared"
			);
		}
	}

	size_t m_size;

}; // struct AbiNestedWriterMgrListConstLen


template<typename _NestedWriterMgr>
struct AbiWriterNestedTypeDynLenImpl
{

	using NestedWriterMgr = _NestedWriterMgr;

	using DynLenWriterImpl = AbiWriterImpl<
		Internal::Obj::ObjCategory::Integer,
		AbiUInt64
	>;

	AbiWriterNestedTypeDynLenImpl(NestedWriterMgr nestedWriterMgr) :
		m_dynLenWriter(),
		m_nestedWriterMgr(std::move(nestedWriterMgr))
	{}

	~AbiWriterNestedTypeDynLenImpl() = default;

	bool IsDynamicType() const
	{
		// List with dynamic length is always dynamic type
		return true;
	}

	size_t GetNumHeadChunks() const
	{
		// List with dynamic length is always dynamic type
		// dynamic type - the head chunk stores the offset of the data
		return 1;
	}

	struct NumTailChunksFunctor
	{
		NumTailChunksFunctor(size_t& size) :
			m_size(size)
		{}

		~NumTailChunksFunctor() = default;

		template<typename _Writer, typename _ValType>
		void operator()(const _Writer& writer, const _ValType& val)
		{
			m_size += writer.GetNumTailChunks(val);
		}

		size_t& m_size;
	}; // struct NumTailChunksFunctor

	template<typename _It>
	size_t SumNumTailChunks(
		size_t initialOffset,
		_It begin,
		_It end
	) const
	{
		// dynamic type - the tail is
		// sub_head_1, sub_head_2, ..., sub_head_n,
		// sub_tail_1, sub_tail_2, ..., sub_tail_n
		size_t numTailChunks = initialOffset;

		m_nestedWriterMgr.IterateVals(
			NumTailChunksFunctor(numTailChunks),
			begin,
			end
		);

		return numTailChunks;
	}

	template<typename _It>
	size_t GetNumTailChunks(
		size_t len,
		_It begin,
		_It end
	) const
	{
		// List with dynamic length is always dynamic type
		// dynamic type - the tail is
		// len_of_list,
		// sub_head_1, sub_head_2, ..., sub_head_n,
		// sub_tail_1, sub_tail_2, ..., sub_tail_n
		size_t initialOffset =
			1 + // the length of the list
			m_nestedWriterMgr.GetTotalNumHeadChunks(len);

		return SumNumTailChunks(
			initialOffset,
			begin,
			end
		);
	}

	template<typename _DestIt>
	struct WriteHeadsFunctor
	{
		WriteHeadsFunctor(_DestIt& destIt, size_t& dataOffset) :
			m_destIt(destIt),
			m_dataOffset(dataOffset)
		{}

		template<typename _Writer, typename _ValType>
		void operator()(const _Writer& writer, const _ValType& val)
		{
			std::tie(m_destIt, m_dataOffset) =
				writer.WriteHead(m_destIt, val, m_dataOffset);
		}

		_DestIt& m_destIt;
		size_t& m_dataOffset;
	}; // struct WriteHeadsFunctor

	template<typename _DestIt>
	struct WriteTailsFunctor
	{
		WriteTailsFunctor(_DestIt& destIt) :
			m_destIt(destIt)
		{}

		template<typename _Writer, typename _ValType>
		void operator()(const _Writer& writer, const _ValType& val)
		{
			m_destIt = writer.WriteTail(m_destIt, val);
		}

		_DestIt& m_destIt;
	}; // struct WriteTailsFunctor

	template<typename _DestIt, typename _SrcIt>
	_DestIt WriteHeadsAndTails(
		_DestIt destIt,
		size_t initialOffset,
		_SrcIt begin,
		_SrcIt end
	) const
	{
		// calculate the offset of the data area
		size_t dataOffset = initialOffset;

		// write all head chunks
		m_nestedWriterMgr.IterateVals(
			WriteHeadsFunctor<_DestIt>(destIt, dataOffset),
			begin,
			end
		);

		// write all tail chunks
		m_nestedWriterMgr.IterateVals(
			WriteTailsFunctor<_DestIt>(destIt),
			begin,
			end
		);

		return destIt;
	}

	template<typename _DestIt, typename _SrcIt>
	_DestIt Write(_DestIt destIt, size_t len, _SrcIt begin, _SrcIt end) const
	{
		uint64_t len64 = static_cast<uint64_t>(len);
		m_dynLenWriter.Write(destIt, len64);

		// calculate the offset of the data area
		size_t dataOffset =
			m_nestedWriterMgr.GetTotalNumHeadChunks(len) *
				AbiCodecConst::sk_chunkSize();

		return WriteHeadsAndTails(destIt, dataOffset, begin, end);
	}

	DynLenWriterImpl m_dynLenWriter;
	NestedWriterMgr m_nestedWriterMgr;

}; // struct AbiWriterNestedTypeDynLenImpl


template<typename _NestedWriterMgr>
struct AbiWriterNestedTypeConstLenImpl :
	public AbiWriterNestedTypeDynLenImpl<_NestedWriterMgr>
{

	using Base = AbiWriterNestedTypeDynLenImpl<_NestedWriterMgr>;
	using Self = AbiWriterNestedTypeConstLenImpl<_NestedWriterMgr>;

	using NestedWriterMgr = _NestedWriterMgr;

	AbiWriterNestedTypeConstLenImpl(NestedWriterMgr nestedWriterMgr) :
		Base(std::move(nestedWriterMgr))
	{}

	~AbiWriterNestedTypeConstLenImpl() = default;

	bool IsDynamicType() const
	{
		return (Base::m_nestedWriterMgr).IsDynamicType();
	}

	size_t GetNumHeadChunks() const
	{
		if (!IsDynamicType())
		{
			// static type - everything is stored at head chunks
			return (Base::m_nestedWriterMgr).GetTotalNumHeadChunks();
		}
		else
		{
			// dynamic type - the head chunk stores the offset of the data
			return 1;
		}
	}

	template<typename _It>
	size_t GetNumTailChunks(_It begin, _It end) const
	{
		if (!IsDynamicType())
		{
			// static type - no tail chunks
			return 0;
		}
		else
		{
			// dynamic type - let base class's function to handle it
			return Base::SumNumTailChunks(
				(Base::m_nestedWriterMgr).GetTotalNumHeadChunks(),
				begin,
				end
			);
		}
	}

	template<typename _DestIt, typename _SrcIt>
	_DestIt Write(_DestIt destIt, _SrcIt begin, _SrcIt end) const
	{
		// calculate the offset of the data area
		size_t dataOffset =
			(Base::m_nestedWriterMgr).GetTotalNumHeadChunks() *
				AbiCodecConst::sk_chunkSize();

		return Base::WriteHeadsAndTails(destIt, dataOffset, begin, end);
	}

}; // struct AbiWriterNestedTypeConstLenImpl


template<class _NestedWriterMgr>
struct AbiWriterNestedListConstLenImpl :
	public AbiWriterNestedTypeConstLenImpl<_NestedWriterMgr>
{
	using NestedWriterMgr = _NestedWriterMgr;
	using Base = AbiWriterNestedTypeConstLenImpl<_NestedWriterMgr>;

	using Base::Base;

	~AbiWriterNestedListConstLenImpl() = default;

	size_t GetNumTailChunks(const Internal::Obj::BaseObj& val) const
	{
		const auto& list = val.AsList();
		return Base::GetNumTailChunks(list.begin(), list.end());
	}

	template<typename _DestIt>
	_DestIt WriteObj(_DestIt destIt, const Internal::Obj::BaseObj& val) const
	{
		const auto& list = val.AsList();
		return Base::Write(destIt, list.begin(), list.end());
	}
}; // struct AbiWriterNestedListConstLenImpl


template<class _NestedWriterMgr>
struct AbiWriterNestedListDynLenImpl :
	public AbiWriterNestedTypeDynLenImpl<_NestedWriterMgr>
{
	using NestedWriterMgr = _NestedWriterMgr;
	using Base = AbiWriterNestedTypeDynLenImpl<_NestedWriterMgr>;

	using Base::Base;

	~AbiWriterNestedListDynLenImpl() = default;

	size_t GetNumTailChunks(const Internal::Obj::BaseObj& val) const
	{
		const auto& list = val.AsList();
		return Base::GetNumTailChunks(list.size(), list.begin(), list.end());
	}

	template<typename _DestIt>
	_DestIt WriteObj(_DestIt destIt, const Internal::Obj::BaseObj& val) const
	{
		const auto& list = val.AsList();
		return Base::Write(destIt, list.size(), list.begin(), list.end());
	}
}; // struct AbiWriterNestedListDynLenImpl


// ==========
// AbiWriterImpl for (T1, T2, Tn) types, where T is static type
// ==========


// ==========
// AbiWriterImpl for (T1, T2, Tn) types, where T is dynamic type
// ==========


} // namespace EthInternal


class AbiWriterBase
{
public: // static members:

	using WriteIterator = EthInternal::WriteIterator;

public:

	AbiWriterBase() = default;

	virtual ~AbiWriterBase() = default;

	virtual bool IsDynamicType() const = 0;

	virtual size_t GetNumHeadChunks() const = 0;

	virtual size_t GetNumTailChunks(
		const Internal::Obj::BaseObj& data
	) const = 0;

	virtual
	std::tuple<
		WriteIterator,
		size_t
	>
	WriteHead(
		WriteIterator destIt,
		const Internal::Obj::BaseObj& data,
		size_t currDataOffset
	) const = 0;

	virtual WriteIterator WriteTail(
		WriteIterator destIt,
		const Internal::Obj::BaseObj& data
	) const = 0;

	// virtual WriteIterator Write(
	// 	WriteIterator destIt,
	// 	const Internal::Obj::BaseObj& data
	// ) const = 0;

}; // class AbiWriterBase


template<typename _Impl>
class AbiWriterHeadOnlyBase : public AbiWriterBase
{
public: // static members:

	using Base = AbiWriterBase;
	using Self = AbiWriterHeadOnlyBase<_Impl>;

	using WriterImpl = _Impl;

	using WriteIterator = typename Base::WriteIterator;

public:

	AbiWriterHeadOnlyBase(WriterImpl impl) :
		Base(),
		m_impl(std::move(impl))
	{}

	virtual ~AbiWriterHeadOnlyBase() = default;

	bool IsDynamicType() const override
	{
		return m_impl.IsDynamicType();
	}

	size_t GetNumHeadChunks() const override
	{
		return m_impl.GetNumHeadChunks();
	}

	size_t GetNumTailChunks(const Internal::Obj::BaseObj&) const override
	{
		return 0;
	}

	virtual
	std::tuple<
		WriteIterator,
		size_t
	>
	WriteHead(
		WriteIterator destIt,
		const Internal::Obj::BaseObj& data,
		size_t currDataOffset
	) const override
	{
		return std::make_tuple(
			m_impl.WriteObj(destIt, data),
			currDataOffset
		);
	}

	virtual WriteIterator WriteTail(
		WriteIterator destIt,
		const Internal::Obj::BaseObj&
	) const override
	{
		// Head only writer does not have tail chunks
		// so we do nothing here
		return destIt;
	}

private:

	WriterImpl m_impl;

}; // class AbiWriterHeadOnlyBase


template<typename _Impl>
class AbiWriterHeadTailBase : public AbiWriterBase
{
public: // static members:

	using Base = AbiWriterBase;
	using Self = AbiWriterHeadTailBase<_Impl>;

	using WriterImpl = _Impl;

	/**
	 * @brief The type of the parser used to parse the head part of the ABI data
	 *        NOTE: the ABI spec assume the head is always a uint256 offset,
	 *        but here we assume a uint64 offset for simplicity, and it's
	 *        very unlikely that the offset will be larger than uint64 in
	 *        real-world cases.
	 *
	 */
	using HeadWriterImpl = EthInternal::AbiWriterImpl<
		Internal::Obj::ObjCategory::Integer,
		AbiUInt64
	>; // head is a uint256 offset

	using WriteIterator = typename Base::WriteIterator;

public:

	AbiWriterHeadTailBase(WriterImpl impl) :
		Base(),
		m_headWriter(HeadWriterImpl()),
		m_impl(std::move(impl))
	{}

	virtual ~AbiWriterHeadTailBase() = default;

	bool IsDynamicType() const override
	{
		return m_impl.IsDynamicType();
	}

	size_t GetNumHeadChunks() const override
	{
		return m_impl.GetNumHeadChunks();
	}

	size_t GetNumTailChunks(const Internal::Obj::BaseObj& val) const override
	{
		return m_impl.GetNumTailChunks(val);
	}

	virtual
	std::tuple<
		WriteIterator,
		size_t
	>
	WriteHead(
		WriteIterator destIt,
		const Internal::Obj::BaseObj& val,
		size_t currDataOffset
	) const override
	{
		if (!IsDynamicType())
		{
			// static type - head only
			return std::make_tuple(
				m_impl.WriteObj(destIt, val),
				currDataOffset
			);
		}
		else
		{
			// dynamic type - head is a uint representing the data offset
			auto headIt = m_headWriter.Write(
				destIt,
				static_cast<uint64_t>(currDataOffset)
			);

			// calculate the new data offset
			currDataOffset += (
				m_impl.GetNumTailChunks(val) *
				AbiCodecConst::sk_chunkSize()
			);

			return std::make_tuple(headIt, currDataOffset);
		}
	}

	virtual WriteIterator WriteTail(
		WriteIterator destIt,
		const Internal::Obj::BaseObj& val
	) const override
	{
		if (!IsDynamicType())
		{
			// static type - no tail chunks
			return destIt;
		}
		else
		{
			// dynamic type - write the tail chunks
			return m_impl.WriteObj(destIt, val);
		}
	}

protected:

	HeadWriterImpl m_headWriter;
	WriterImpl m_impl;

}; // class AbiWriterHeadTailBase


// ==========
// AbiWriter general template
// ==========


template<
	Internal::Obj::ObjCategory _DataType,
	typename... _Args
>
struct AbiWriter;


// ==========
// AbiWriter for integer types
// ==========


template<Internal::Obj::RealNumType _RealNumType>
struct AbiWriter<
	Internal::Obj::ObjCategory::Integer,
	std::integral_constant<Internal::Obj::RealNumType, _RealNumType>
> :
	public AbiWriterHeadOnlyBase<
		EthInternal::AbiWriterImpl<
			Internal::Obj::ObjCategory::Integer,
			std::integral_constant<Internal::Obj::RealNumType, _RealNumType>
		>
	>
{
	using WriterImpl = EthInternal::AbiWriterImpl<
		Internal::Obj::ObjCategory::Integer,
		std::integral_constant<Internal::Obj::RealNumType, _RealNumType>
	>;
	using Base = AbiWriterHeadOnlyBase<WriterImpl>;
	using Self = AbiWriter<
		Internal::Obj::ObjCategory::Integer,
		std::integral_constant<Internal::Obj::RealNumType, _RealNumType>
	>;

	AbiWriter() :
		Base(WriterImpl())
	{}

	// LCOV_EXCL_START
	virtual ~AbiWriter() = default;
	// LCOV_EXCL_STOP

}; // struct AbiWriter<Internal::Obj::ObjCategory::Integer, ...>


// ==========
// AbiWriter for bool type
// ==========


template<>
struct AbiWriter<
	Internal::Obj::ObjCategory::Bool
> :
	public AbiWriterHeadOnlyBase<
		EthInternal::AbiWriterImpl<
			Internal::Obj::ObjCategory::Bool
		>
	>
{
	using WriterImpl =
		EthInternal::AbiWriterImpl<Internal::Obj::ObjCategory::Bool>;
	using Base = AbiWriterHeadOnlyBase<WriterImpl>;
	using Self = AbiWriter<Internal::Obj::ObjCategory::Bool>;

	AbiWriter() :
		Base(WriterImpl())
	{}

	// LCOV_EXCL_START
	virtual ~AbiWriter() = default;
	// LCOV_EXCL_STOP

}; // struct AbiWriter<Internal::Obj::ObjCategory::Bool>


// ==========
// AbiWriter for bytes<M> types
// ==========


template<>
struct AbiWriter<
	Internal::Obj::ObjCategory::Bytes,
	std::false_type // IsDynamic? - false
> :
	public AbiWriterHeadOnlyBase<
		EthInternal::AbiWriterImpl<
			Internal::Obj::ObjCategory::Bytes,
			std::false_type
		>
	>
{
	using WriterImpl = EthInternal::AbiWriterImpl<
		Internal::Obj::ObjCategory::Bytes,
		std::false_type
	>;
	using Base = AbiWriterHeadOnlyBase<WriterImpl>;
	using Self = AbiWriter<Internal::Obj::ObjCategory::Bytes, std::false_type>;


	AbiWriter(size_t size) :
		AbiWriter(EthInternal::AbiWithinChunkSize(size), size_t())
	{}

	// LCOV_EXCL_START
	virtual ~AbiWriter() = default;
	// LCOV_EXCL_STOP

protected:

	/**
	 * @brief A constructor that does not check the size value
	 *        NOTE: The caller is responsible to ensure the size is within
	 *        the chunk size; thus, this constructor is protected, and only
	 *        accessible by this class and the derived classes
	 *
	 * @param size The size of the bytes, which should be within the chunk size
	 */
	AbiWriter(size_t size, size_t) :
		Base(WriterImpl(size))
	{}

}; // struct AbiWriter<Internal::Obj::ObjCategory::Bytes, false>


template<size_t _Size>
struct AbiWriter<
	Internal::Obj::ObjCategory::Bytes,
	std::integral_constant<size_t, _Size>
> :
	public AbiWriter<
		Internal::Obj::ObjCategory::Bytes,
		std::false_type // IsDynamic? - false
	>
{
	using Base = AbiWriter<Internal::Obj::ObjCategory::Bytes, std::false_type>;
	using Self = AbiWriter<
		Internal::Obj::ObjCategory::Bytes,
		std::integral_constant<size_t, _Size>
	>;

	static constexpr size_t sk_size = _Size;
	static_assert(
		sk_size <= AbiCodecConst::sk_chunkSize(),
		"ABI parser - bytes type is too large"
	);

	AbiWriter() :
		Base(sk_size, size_t())
	{}

	// LCOV_EXCL_START
	virtual ~AbiWriter() = default;
	// LCOV_EXCL_STOP

}; // struct AbiWriter<Internal::Obj::ObjCategory::Bytes, size_t>


// ==========
// AbiWriter for bytes types
// ==========


template<>
struct AbiWriter<
	Internal::Obj::ObjCategory::Bytes,
	std::true_type // IsDynamic? - true
> :
	public AbiWriterHeadTailBase<
		EthInternal::AbiWriterImpl<
			Internal::Obj::ObjCategory::Bytes,
			std::true_type
		>
	>
{
	using WriterImpl = EthInternal::AbiWriterImpl<
		Internal::Obj::ObjCategory::Bytes,
		std::true_type
	>;
	using Base = AbiWriterHeadTailBase<WriterImpl>;
	using Self = AbiWriter<Internal::Obj::ObjCategory::Bytes, std::true_type>;

	AbiWriter() :
		Base(WriterImpl())
	{}

	// LCOV_EXCL_START
	virtual ~AbiWriter() = default;
	// LCOV_EXCL_STOP

}; // struct AbiWriter<Internal::Obj::ObjCategory::Bytes, true>


// ==========
// AbiWriter for list types (T[k])
// ==========


template<typename _ItemWriter>
struct AbiWriter<
	Internal::Obj::ObjCategory::List,
	_ItemWriter,
	std::false_type // IsLenDynamic? - false
> :
	public AbiWriterHeadTailBase<
		EthInternal::AbiWriterNestedListConstLenImpl<
			EthInternal::AbiNestedWriterMgrListConstLen<_ItemWriter>
		>
	>
{
	using NestedWritersMgr =
		EthInternal::AbiNestedWriterMgrListConstLen<_ItemWriter>;
	using WriterImpl = EthInternal::AbiWriterNestedListConstLenImpl<
		NestedWritersMgr
	>;
	using Base = AbiWriterHeadTailBase<WriterImpl>;
	using Self = AbiWriter<
		Internal::Obj::ObjCategory::List,
		_ItemWriter,
		std::false_type
	>;

	AbiWriter(_ItemWriter itemWriter, size_t size) :
		Base(
			WriterImpl(
				NestedWritersMgr(std::make_pair(itemWriter, size))
			)
		)
	{}

	// LCOV_EXCL_START
	virtual ~AbiWriter() = default;
	// LCOV_EXCL_STOP

}; // struct AbiParser<Internal::Obj::ObjCategory::List, _Item, false>


template<typename _ItemWriter, size_t _Size>
struct AbiWriter<
	Internal::Obj::ObjCategory::List,
	_ItemWriter,
	std::integral_constant<size_t, _Size>
> :
	public AbiWriter<
		Internal::Obj::ObjCategory::List,
		_ItemWriter,
		std::false_type
	>
{
	using Base = AbiWriter<
		Internal::Obj::ObjCategory::List,
		_ItemWriter,
		std::false_type
	>;
	using Self = AbiWriter<
		Internal::Obj::ObjCategory::List,
		_ItemWriter,
		std::integral_constant<size_t, _Size>
	>;

	static constexpr size_t sk_size = _Size;

	AbiWriter(_ItemWriter itemWriter) :
		Base(itemWriter, sk_size)
	{}

	// LCOV_EXCL_START
	virtual ~AbiWriter() = default;
	// LCOV_EXCL_STOP

}; // struct AbiWriter<Internal::Obj::ObjCategory::List, _Item, size_t>


// ==========
// AbiWriter for list types (T[])
// ==========


template<typename _ItemWriter>
struct AbiWriter<
	Internal::Obj::ObjCategory::List,
	_ItemWriter,
	std::true_type // IsLenDynamic? - true
> :
	public AbiWriterHeadTailBase<
		EthInternal::AbiWriterNestedListDynLenImpl<
			EthInternal::AbiNestedWriterMgrListDynLen<_ItemWriter>
		>
	>
{
	using NestedWritersMgr =
		EthInternal::AbiNestedWriterMgrListDynLen<_ItemWriter>;
	using WriterImpl = EthInternal::AbiWriterNestedListDynLenImpl<
		NestedWritersMgr
	>;
	using Base = AbiWriterHeadTailBase<WriterImpl>;
	using Self = AbiWriter<
		Internal::Obj::ObjCategory::List,
		_ItemWriter,
		std::true_type
	>;

	AbiWriter(_ItemWriter itemWriter) :
		Base(
			WriterImpl(
				NestedWritersMgr(itemWriter)
			)
		)
	{}

	// LCOV_EXCL_START
	virtual ~AbiWriter() = default;
	// LCOV_EXCL_STOP

}; // struct AbiParser<Internal::Obj::ObjCategory::List, _Item, false>



} // namespace Eth
} // namespace EclipseMonitor

