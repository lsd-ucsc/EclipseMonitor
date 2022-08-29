// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <algorithm>
#include <memory>

#include "EthDAA.hpp"
#include "EthHeaderMgr.hpp"

namespace EclipseMonitor
{

class EthValidatorBase
{
public:
	EthValidatorBase() = default;

	virtual ~EthValidatorBase() = default;

	virtual bool CommonValidate(
		const EthHeaderMgr& parent,
		const EthHeaderMgr& current
	) const = 0;
};

class EthValidator : public EthValidatorBase
{
public: // Static member:
	using Base = EthValidatorBase;
	using Self = EthValidator;

public:
	EthValidator(std::unique_ptr<EthDAABase> diffCalculator) :
		m_diffCalculator(std::move(diffCalculator))
	{}

	virtual ~EthValidator() = default;

	virtual bool CommonValidate(
		const EthHeaderMgr& parent,
		const EthHeaderMgr& current
	) const override
	{
		// 1. check block number
		if (current.GetNumber() != parent.GetNumber() + 1)
		{
			return false;
		}

		// 2. check local time (0 means history block)
		// TODO

		// 2. check parent_hash == parent.hash
		const auto& parentHash = current.GetRawHeader().get_ParentHash();
		if ((parentHash.size() != parent.GetHash().size()) ||
			(!std::equal(parent.GetHash().begin(), parent.GetHash().end(),
				parentHash.data())))
		{
			return false;
		}

		// 3. check difficulty value
		auto expDiff = (*m_diffCalculator)(
			parent.GetNumber(), parent.GetTime(),
			parent.GetDiff(), parent.HasUncle(),
			current.GetNumber(), current.GetTime());
		if (current.GetDiff() != expDiff)
		{
			return false;
		}

		// 4. Check hash puzzle
		// TODO

		return true;
	}

private:

	std::unique_ptr<EthDAABase> m_diffCalculator;
}; // class EthValidator

} // namespace EclipseMonitor
