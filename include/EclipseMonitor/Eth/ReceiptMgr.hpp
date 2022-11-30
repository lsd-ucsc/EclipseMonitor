// Copyright (c) 2022 Haofan Zheng, Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <cstdint>

#include <tuple>

#include "../Internal/SimpleObj.hpp"
#include "../Internal/SimpleRlp.hpp"

namespace EclipseMonitor
{
namespace Eth
{

/*
 * ReceiptMgr parses an rlp-encoded Ethereum receipt, and can determine whether
 * an event has been emitted by a smart contract
 *
 * The structure of an Ethereum receipt can be found in the following link:
 * https://github.com/ethereum/go-ethereum/blob/master/core/types/receipt.go
 *
 * The rlp receipt data is expected to come from the Geth API call
 * "debug_getRawReceipts". This API call, however, does not encode the entire
 * receipt but only four fields, namely:
 * 	- 1. Status
 * 	- 2. Cumulative gas used
 * 	- 3. Bloom
 * 	- 4. Logs
 *
 * The logs of each receipt contains the data, if any, relating to events that
 * were emitted by a contract.
 *
 * The structure of a receipt log can be found here:
 * https://github.com/ethereum/go-ethereum/blob/master/core/types/log.go
 *
 * This log structure, too, is also not fully encoded to rlp. The rlp encoding
 * from the API call only contains the first three fields:
 *
 * 	- 1. Address
 * 	- 2. Topics
 * 	- 3. Data
 *
*/

class ReceiptMgr
{
public: // static members:

	static Internal::Obj::Object ParseReceipt(
		const Internal::Obj::BytesBaseObj& rlpBytes
	)
	{
		uint8_t firstByte = rlpBytes[0];

		auto itBegin = rlpBytes.begin();
		auto itEnd = rlpBytes.end();
		size_t size = rlpBytes.size();
		if (firstByte == 0x01 || firstByte == 0x02)
		{
			++itBegin;
			--size;
		}

		using _FrItType = typename Internal::Rlp::GeneralParser::IteratorType;
		return Internal::Rlp::GeneralParser().Parse(
			_FrItType(itBegin.CopyPtr()),
			_FrItType(itEnd.CopyPtr()),
			size
		);
	}

	static ReceiptMgr FromBytes(
		const Internal::Obj::BytesBaseObj& rlpBytes
	)
	{
		return ReceiptMgr(ParseReceipt(rlpBytes));
	}

public:

	explicit ReceiptMgr(Internal::Obj::Object receiptObj) :
		m_receiptObj(std::move(receiptObj)),
		m_receiptBody(m_receiptObj.AsList()),
		m_receiptLogs(m_receiptBody[3].AsList())
	{};

	// LCOV_EXCL_START
	~ReceiptMgr() = default;
	// LCOV_EXCL_STOP

	const Internal::Obj::Object& GetReceiptObj() const
	{
		return m_receiptObj;
	}

	const Internal::Obj::ListBaseObj& GetReceiptBody() const
	{
		return m_receiptBody;
	}

	const Internal::Obj::ListBaseObj& GetReceiptLogs() const
	{
		return m_receiptLogs;
	}

	std::tuple<bool, Internal::Obj::Bytes> IsEventEmitted(
		const Internal::Obj::BytesBaseObj& contractAddress,
		const Internal::Obj::BytesBaseObj& eventHash
	)
	{
		// iterate over the logs to see if any event is emitted by the contract
		for (const auto& logEntryObj : m_receiptLogs)
		{
			const Internal::Obj::ListBaseObj& logEntry = logEntryObj.AsList();

			const Internal::Obj::BytesBaseObj& logAddress =
				logEntry[0].AsBytes();

			if(contractAddress == logAddress)
			{
				const Internal::Obj::ListBaseObj& logTopics =
					logEntry[1].AsList();
				const Internal::Obj::BytesBaseObj& logEventHash =
					logTopics[0].AsBytes();

				if(eventHash == logEventHash)
				{
					const Internal::Obj::BytesBaseObj& logDataRef =
						logEntry[2].AsBytes();

					// TODO: indexed data?
					// TODO: consider only returning true/false here

					Internal::Obj::Bytes logData = {
						logDataRef.data(),
						logDataRef.data() + logDataRef.size()
					};
					return std::make_tuple(true, logData);
				}
			}
		}

		return std::make_tuple(false, Internal::Obj::Bytes());
	}

private:
	Internal::Obj::Object m_receiptObj;
	Internal::Obj::ListBaseObj& m_receiptBody;
	Internal::Obj::ListBaseObj& m_receiptLogs;

}; // class ReceiptMgr

} // namespace Eth
} // namespace EclipseMonitor
