// Copyright (c) 2022 Haofan Zheng, Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <cstdint>

#include <tuple>

#include "../../Internal/SimpleObj.hpp"
#include "../../Internal/SimpleRlp.hpp"


namespace EclipseMonitor
{
namespace Eth
{


enum class TnxVersion : uint8_t
{
	Legacy  = 0,
	Ver0x01 = 1,
	Ver0x02 = 2,
} // enum class TnxVersion


class TransactionMgr
{
public: // static members:

	static std::tuple<TnxVersion, Internal::Rlp::List> ParseTnx(
		const Internal::Rlp::Bytes& rlpBytes
	); // TODO[Tuan]


	static TransactionMgr FromBytes(
		const Internal::Rlp::Bytes& rlpBytes
	)
	{
		TnxVersion ver;
		Internal::Rlp::List body;
		std::tie(ver, body) = ParseTnx(rlpBytes);

		return TransactionMgr(ver, body);
	}

public:

	TransactionMgr(
		TnxVersion version,
		const Internal::Rlp::List& body
	) :
		m_version(version),
		m_tnxBody(body)
	{};

	// LCOV_EXCL_START
	~TransactionMgr() = default;
	// LCOV_EXCL_STOP

	const Internal::Rlp::Bytes& GetContractAddr() const; // TODO[Tuan]

	const Internal::Rlp::List& GetContactParams() const; // TODO[Tuan]

private:
	TnxVersion m_version;
	Internal::Rlp::List m_tnxBody;
};


} // namespace Eth
} // namespace EclipseMonitor
