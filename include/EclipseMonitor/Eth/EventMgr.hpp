// Copyright (c) 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <cstdint>
#include <tuple>

#include "../Internal/SimpleObj.hpp"
#include "../Internal/SimpleRlp.hpp"

#include "Keccak256.hpp"
#include "ReceiptMgr.hpp"
#include "TransactionMgr.hpp"


namespace EclipseMonitor
{
namespace Eth
{

class EventMgr
{

public:

	void Subscribe(
		const Internal::Obj::Bytes& address,
		const std::string& event
	)
	{
		if (!m_subscriptions.HasKey(address))
		{
			m_subscriptions[address] = Internal::Obj::List();
		}

		Internal::Obj::ListBaseObj& eventList =
			m_subscriptions[address].AsList();

		// hash the function and get the signature (first 4 bytes)
		std::vector<uint8_t> eventBytes(event.begin(), event.end());
		std::array<uint8_t, 32> hashedEvent = Keccak256(eventBytes);
		SimpleObjects::Bytes eventSig(hashedEvent.begin(), hashedEvent.end());

		if (!eventList.Contains(eventSig))
		{
			eventList.push_back(Internal::Obj::Object(eventSig));
		}
	}

	bool WatchTxn(const Internal::Obj::Bytes& txnBytes)
	{
		TransactionMgr mgr = TransactionMgr::FromBytes(txnBytes);

		const Internal::Obj::Bytes& contractAddr = mgr.GetContractAddr();

		return m_subscriptions.HasKey(contractAddr);
	}

	std::tuple<bool, Internal::Obj::Bytes> IsEventEmitted(
		const Internal::Obj::Bytes& address,
		const Internal::Obj::Bytes& receiptBytes
	)
	{
		ReceiptMgr receiptMgr = ReceiptMgr::FromBytes(receiptBytes);

		Internal::Obj::ListBaseObj& eventList =
			m_subscriptions[address].AsList();

		bool eventEmitted = false;
		Internal::Obj::Bytes eventData;
		auto it = eventList.begin();
		for (; it != eventList.end(); it++)
		{
			Internal::Obj::BytesBaseObj& eventSig = it->AsBytes();
			Internal::Obj::Bytes eventBytes(
				eventSig.data(), eventSig.data() + eventSig.size());

			std::tie(eventEmitted, eventData) =
				receiptMgr.IsEventEmitted(address, eventBytes);

			if (eventEmitted)
			{
				break;
			}
		}

		return std::make_tuple(eventEmitted, eventData);
	}

public:
	EventMgr()
	{
		m_subscriptions = Internal::Obj::Dict();
	}

private:
	Internal::Obj::Dict m_subscriptions;

}; // class EventMgr

} // namespace Eth
} // namespace EclipseMonitor
