// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once


#include <cstdint>

#include "../Internal/SimpleRlp.hpp"

namespace EclipseMonitor
{
namespace Eth
{

template<typename _PrimitiveType>
struct PrimitiveTypeTrait
{
	using value_type = _PrimitiveType;

	static value_type FromBytes(const Internal::Rlp::BytesObjType& b)
	{
		size_t i = 0;
		auto res = Internal::Rlp::ParsePrimitiveIntValue<
			value_type,
			Internal::Rlp::Endian::native>::Parse(
				b.size(),
				[&i, &b](){
					return b[i++];
				}
			);
		return res;
	}

	static Internal::Rlp::BytesObjType ToBytes(const value_type& v)
	{
		auto res = Internal::Rlp::BytesObjType();
		Internal::Rlp::EncodePrimitiveIntValue<
			value_type,
			Internal::Rlp::Endian::native,
			false>::
				Encode(res, v);
		return res;
	}
}; // struct PrimitiveTypeTrait

struct BlkNumTypeTrait : PrimitiveTypeTrait<uint64_t>
{}; // struct BlkNumTypeTrait

struct TimeTypeTrait : PrimitiveTypeTrait<uint64_t>
{}; // struct TimeTypeTrait

struct DiffTypeTrait : PrimitiveTypeTrait<uint64_t>
{}; // struct DiffTypeTrait

} // namespace Eth
} // namespace EclipseMonitor
