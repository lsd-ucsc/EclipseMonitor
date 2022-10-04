// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include "../Exceptions.hpp"

#include "DataTypes.hpp"
#include "Params.hpp"

namespace EclipseMonitor
{
namespace Eth
{


// Reference: https://github.com/ethereum/go-ethereum/blob/master/consensus/ethash/consensus.go


class DAABase
{
public: // static members:

	using BlkNumType = typename BlkNumTypeTrait::value_type;
	using TimeType   = typename TimeTypeTrait::value_type;
	using DiffType   = typename DiffTypeTrait::value_type;

public:

	DAABase() = default;

	// LCOV_EXCL_START
	virtual ~DAABase() = default;
	// LCOV_EXCL_STOP

	virtual DiffType operator()(
		const BlkNumType& parentBlkNum,
		const TimeType& parentTime,
		const DiffType& parentDiff,
		bool parentHasUncle,
		const BlkNumType& currBlkNum,
		const TimeType& currTime) const = 0;

protected:

	static DiffType CalcBomb(
		const BlkNumType& periodCount,
		const BlkNumType& blkNBig2,
		const DiffType&   diffBig1,
		const DiffType&   diffBig2)
	{
		if (periodCount <= blkNBig2)
		{
			// reference: https://pkg.go.dev/math/big#Int.Exp
			return diffBig1;
		}
		else
		{
			BlkNumType exp = periodCount - blkNBig2;
			DiffType y = diffBig1;
			for (BlkNumType i = 0; i < exp; i++)
			{
				y *= diffBig2;
			}
			return y;
		}
	}
}; // class DAABase


class DAACalculator : public DAABase
{
public: // static members:
	using Base = DAABase;

	using BlkNumType = typename Base::BlkNumType;
	using TimeType   = typename Base::TimeType;
	using DiffType   = typename Base::DiffType;

	static const DAACalculator& GetEip5133()
	{
		static const DAACalculator inst(
			/* considerUncle */     true,
			/* deltaDivisor */      DiffType(9),
			/* hasMaxCheck */       true,
			/* hasBombDelay */      true,
			/* bombDelay */         BlkNumType(11400000U) );
		return inst;
	}

	static const DAACalculator& GetEip5133Estimated()
	{
		static const DAACalculator inst(
			/* considerUncle */     true,
			/* deltaDivisor */      DiffType(9),
			/* hasMaxCheck */       false,
			/* hasBombDelay */      true,
			/* bombDelay */         BlkNumType(11400000U) );
		return inst;
	}

	static const DAACalculator& GetEip4345()
	{
		static const DAACalculator inst(
			/* considerUncle */     true,
			/* deltaDivisor */      DiffType(9),
			/* hasMaxCheck */       true,
			/* hasBombDelay */      true,
			/* bombDelay */         BlkNumType(10700000U));
		return inst;
	}

	static const DAACalculator& GetEip3554()
	{
		static const DAACalculator inst(
			/* considerUncle */     true,
			/* deltaDivisor */      DiffType(9),
			/* hasMaxCheck */       true,
			/* hasBombDelay */      true,
			/* bombDelay */         BlkNumType(9700000U));
		return inst;
	}

	static const DAACalculator& GetEip2384()
	{
		static const DAACalculator inst(
			/* considerUncle */     true,
			/* deltaDivisor */      DiffType(9),
			/* hasMaxCheck */       true,
			/* hasBombDelay */      true,
			/* bombDelay */         BlkNumType(9000000U));
		return inst;
	}

	static const DAACalculator& GetConstantinople()
	{
		static const DAACalculator inst(
			/* considerUncle */     true,
			/* deltaDivisor */      DiffType(9),
			/* hasMaxCheck */       true,
			/* hasBombDelay */      true,
			/* bombDelay */         BlkNumType(5000000U));
		return inst;
	}

	static const DAACalculator& GetByzantium()
	{
		static const DAACalculator inst(
			/* considerUncle */     true,
			/* deltaDivisor */      DiffType(9),
			/* hasMaxCheck */       true,
			/* hasBombDelay */      true,
			/* bombDelay */         BlkNumType(3000000U));
		return inst;
	}

	static const DAACalculator& GetHomestead()
	{
		static const DAACalculator inst(
			/* considerUncle */     false,
			/* deltaDivisor */      DiffType(10),
			/* hasMaxCheck */       true,
			/* hasBombDelay */      false,
			/* bombDelay */         BlkNumType(0));
		return inst;
	}

public:
	DAACalculator(
		bool considerUncle,
		const DiffType& deltaDivisor,
		bool hasMaxCheck,
		bool hasBombDelay,
		const BlkNumType& bombDelay) :
		DAABase(),
		m_considerUncle(considerUncle),
		m_deltaDivisor(deltaDivisor),
		m_hasMaxCheck(hasMaxCheck),
		m_hasBombDelay(hasBombDelay),
		m_bombDelay(bombDelay),
		m_bombDelayFromParent(m_bombDelay - BlkNumType(1))
	{}

	// LCOV_EXCL_START
	virtual ~DAACalculator() = default;
	// LCOV_EXCL_STOP

	virtual DiffType operator()(
		const BlkNumType& parentBlkNum,
		const TimeType& parentTime,
		const DiffType& parentDiff,
		bool parentHasUncle,
		const BlkNumType& /* currBlkNum */,
		const TimeType& currTime) const override
	{
		// Reference: https://github.com/ethereum/go-ethereum/blob/master/consensus/ethash/consensus.go

		static const DiffType sk_diffBig1  = DiffType(1);
		static const DiffType sk_diffBig2  = DiffType(2);
		static const DiffType sk_diffBig99 = DiffType(99);
		static const DiffType sk_minDiff   =
			DiffType(Params::GetMinimumDifficulty());

		static const BlkNumType sk_blkNBig0      = BlkNumType(0);
		static const BlkNumType sk_blkNBig1      = BlkNumType(1);
		static const BlkNumType sk_blkNBig2      = BlkNumType(2);
		static const BlkNumType sk_expDiffPeriod = BlkNumType(100000);

		bool isReducing = false;

		// Homestead algorithm:
		// diff = (parent_diff +
		//         (parent_diff / 2048 *
		//            max(1 - (block_timestamp - parent_timestamp) // 10, -99))
		//        ) + 2^(periodCount - 2)
		// Byzantium algorithm:
		// diff = (parent_diff +
		//         (parent_diff / 2048 *
		//            max((2 if len(parent.uncles) else 1) -
		//                   ((timestamp - parent.timestamp) // 9), -99))
		//        ) + 2^(periodCount - 2)


		// (block_timestamp - parent_timestamp) // 9
		// assuming currTime > parentTime is checked during validation
		DiffType x = DiffType(currTime - parentTime);
		x = x / m_deltaDivisor;

		// (2 if len(parent_uncles) else 1) - (block_timestamp - parent_timestamp) // 9
		if (m_considerUncle && parentHasUncle)
		{
			// 2 - x ==> if x > 2; -1 * (x - 2); else; (2 - x)
			if (x > sk_diffBig2)
			{
				isReducing = true;
				x -= sk_diffBig2;
			}
			else
			{
				isReducing = false;
				x = sk_diffBig2 - x;
			}
		}
		else
		{
			// 1 - x ==> if x > 1; -1 * (x - 1); else; (1 - x)
			if (x > sk_diffBig1)
			{
				isReducing = true;
				x -= sk_diffBig1;
			}
			else
			{
				isReducing = false;
				x = sk_diffBig1 - x;
			}
		}

		// max((2 if len(parent.uncles) else 1) - ((timestamp - parent.timestamp) // 9), -99)
		if (m_hasMaxCheck && isReducing && (x > sk_diffBig99))
		{
			x = sk_diffBig99;
		}

		// parent_diff / 2048
		DiffType y = parentDiff >> Params::GetDifficultyBoundDivisorBitShift();

		// (parent_diff / 2048) *
		//     max((2 if len(parent.uncles) else 1) - ((timestamp - parent.timestamp) // 9), -99)
		x = y * x;

		// parent_diff +
		//     (parent_diff / 2048 * max((2 if len(parent.uncles) else 1) - ((timestamp - parent.timestamp) // 9), -99))
		if (!m_hasMaxCheck && isReducing && (x > parentDiff))
		{
			x = sk_minDiff;
		}
		else if (isReducing)
		{
			x = parentDiff - x;
		}
		else
		{
			x = parentDiff + x;
		}

		// minimum difficulty cap
		if (x < sk_minDiff)
		{
			x = sk_minDiff;
		}

		// calculate delay bomb for the ice-age delay
		BlkNumType fakeBlockNumber = sk_blkNBig0;
		if (m_hasBombDelay)
		{
			if (parentBlkNum >= m_bombDelayFromParent)
			{
				fakeBlockNumber = parentBlkNum - m_bombDelayFromParent;
			}
		}
		else
		{
			fakeBlockNumber = parentBlkNum + sk_blkNBig1;
		}

		BlkNumType periodCount = fakeBlockNumber / sk_expDiffPeriod;
		if (periodCount > sk_blkNBig1)
		{
			y = CalcBomb(periodCount, sk_blkNBig2, sk_diffBig1, sk_diffBig2);
			x += y;
		}

		return x;
	}

private:
	bool m_considerUncle;
	DiffType m_deltaDivisor;
	bool m_hasMaxCheck;
	bool m_hasBombDelay;
	BlkNumType m_bombDelay;
	BlkNumType m_bombDelayFromParent;
}; // class DAACalculator


class DAACalculatorFrontier : public DAABase
{
public: // static members:
	using Base = DAABase;

	using BlkNumType = typename Base::BlkNumType;
	using TimeType   = typename Base::TimeType;
	using DiffType   = typename Base::DiffType;

	static const DAACalculatorFrontier& GetInstance()
	{
		static const DAACalculatorFrontier inst;
		return inst;
	}

public:
	DAACalculatorFrontier() = default;

	// LCOV_EXCL_START
	virtual ~DAACalculatorFrontier() = default;
	// LCOV_EXCL_STOP

	virtual DiffType operator()(
		const BlkNumType& parentBlkNum,
		const TimeType& parentTime,
		const DiffType& parentDiff,
		bool /* parentHasUncle */,
		const BlkNumType& /* currBlkNum */,
		const TimeType& currTime) const override
	{
		static const DiffType sk_diffBig1  = DiffType(1);
		static const DiffType sk_diffBig2  = DiffType(2);
		static const DiffType sk_minDiff   =
			DiffType(Params::GetMinimumDifficulty());

		static const BlkNumType sk_blkNBig1      = BlkNumType(1);
		static const BlkNumType sk_blkNBig2      = BlkNumType(2);
		static const BlkNumType sk_expDiffPeriod = BlkNumType(100000);

		// Frontier algorithm:
		// diff =
		//    (parent_diff + (parent_diff / 2048))
		//        if (timestamp - parent_timestamp) < DurationLimit else
		//            (parent_diff - (parent_diff / 2048)) +
		//                2^(periodCount - 2)

		DiffType adjust = parentDiff >> Params::GetDifficultyBoundDivisorBitShift();

		TimeType delta = currTime - parentTime;

		DiffType diff = parentDiff;
		if (delta < Params::GetDurationLimit())
		{
			diff += adjust;
		}
		else
		{
			diff -= adjust;
		}

		if (diff < sk_minDiff)
		{
			diff = sk_minDiff;
		}

		BlkNumType fakeBlockNumber = parentBlkNum + sk_blkNBig1;

		BlkNumType periodCount = fakeBlockNumber / sk_expDiffPeriod;
		if (periodCount > sk_blkNBig1)
		{
			DiffType bombVal =
				CalcBomb(periodCount, sk_blkNBig2, sk_diffBig1, sk_diffBig2);
			diff += bombVal;
		}

		return diff;
	}

}; // class DAACalculatorFrontier


template<typename _ChainConfig>
class GenericDAAImpl : public DAABase
{
public: // static members:
	using Self = GenericDAAImpl<_ChainConfig>;
	using Base = DAABase;

	using ChainConfig = _ChainConfig;

	using BlkNumType = typename Base::BlkNumType;
	using TimeType   = typename Base::TimeType;
	using DiffType   = typename Base::DiffType;

	static const Base& GetCalculator(const BlkNumType& blkNum)
	{
		if (blkNum >= ChainConfig::GetGrayGlacierBlkNum())
		{
			return DAACalculator::GetEip5133();
		}
		else if (blkNum >= ChainConfig::GetArrowGlacierBlkNum())
		{
			return DAACalculator::GetEip4345();
		}
		else if (blkNum >= ChainConfig::GetLondonBlkNum())
		{
			return DAACalculator::GetEip3554();
		}
		else if (blkNum >= ChainConfig::GetMuirGlacierBlkNum())
		{
			return DAACalculator::GetEip2384();
		}
		else if (blkNum >= ChainConfig::GetConstantinopleBlkNum())
		{
			return DAACalculator::GetConstantinople();
		}
		else if (blkNum >= ChainConfig::GetByzantiumBlkNum())
		{
			return DAACalculator::GetByzantium();
		}
		else if (blkNum >= ChainConfig::GetHomesteadBlkNum())
		{
			return DAACalculator::GetHomestead();
		}
		else
		{
			return DAACalculatorFrontier::GetInstance();
		}
	}

public:

	GenericDAAImpl() = default;

	// LCOV_EXCL_START
	virtual ~GenericDAAImpl() = default;
	// LCOV_EXCL_STOP

	virtual DiffType operator()(
		const BlkNumType& parentBlkNum,
		const TimeType& parentTime,
		const DiffType& parentDiff,
		bool parentHasUncle,
		const BlkNumType& currBlkNum,
		const TimeType& currTime) const override
	{
		const DAABase& calculator = GetCalculator(currBlkNum);
		return calculator(
			parentBlkNum, parentTime, parentDiff, parentHasUncle,
			currBlkNum, currTime);
	}

}; // class GenericDAAImpl


template<typename _ChainConfig>
class GenericDAAEstImpl : public DAABase
{
public: // static members:
	using Self = GenericDAAEstImpl<_ChainConfig>;
	using Base = DAABase;

	using ChainConfig = _ChainConfig;

	using BlkNumType = typename Base::BlkNumType;
	using TimeType   = typename Base::TimeType;
	using DiffType   = typename Base::DiffType;

	static const Base& GetCalculator(const BlkNumType& blkNum)
	{
		if (blkNum >= ChainConfig::GetGrayGlacierBlkNum())
		{
			return DAACalculator::GetEip5133Estimated();
		}
		else
		{
			throw Exception("Estimating the difficulty value before EIP-5133 "
				"is not allowed.");
		}
	}

public:

	GenericDAAEstImpl() = default;

	// LCOV_EXCL_START
	virtual ~GenericDAAEstImpl() = default;
	// LCOV_EXCL_STOP

	virtual DiffType operator()(
		const BlkNumType& parentBlkNum,
		const TimeType& parentTime,
		const DiffType& parentDiff,
		bool parentHasUncle,
		const BlkNumType& currBlkNum,
		const TimeType& currTime) const override
	{
		const DAABase& calculator = GetCalculator(currBlkNum);
		return calculator(
			parentBlkNum, parentTime, parentDiff, parentHasUncle,
			currBlkNum, currTime);
	}

}; // class GenericDAAEstImpl


using MainnetDAA = GenericDAAImpl<MainnetConfig>;
using MainnetDAAEstimator = GenericDAAEstImpl<MainnetConfig>;

} // namespace Eth
} // namespace EclipseMonitor
