// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include "EthDataTypes.hpp"
#include "EthParams.hpp"

namespace EclipseMonitor
{

// Reference: https://github.com/ethereum/go-ethereum/blob/master/consensus/ethash/consensus.go


class EthDAABase
{
public: // static members:

	using BlkNumType = typename EthBlkNumTypeTrait::value_type;
	using TimeType   = typename EthTimeTypeTrait::value_type;
	using DiffType   = typename EthDiffTypeTrait::value_type;

public:

	EthDAABase() = default;

	virtual ~EthDAABase() = default;

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
}; // class EthDAABase


class EthDAACalculator : public EthDAABase
{
public: // static members:
	using Base = EthDAABase;

	using BlkNumType = typename Base::BlkNumType;
	using TimeType   = typename Base::TimeType;
	using DiffType   = typename Base::DiffType;

	static const EthDAACalculator& GetEip5133()
	{
		static const EthDAACalculator inst(
			/* considerUncle */     true,
			/* deltaDivisor */      DiffType(9),
			/* hasMaxCheck */       true,
			/* hasBombDelay */      true,
			/* bombDelay */         BlkNumType(11400000U) );
		return inst;
	}

	static const EthDAACalculator& GetEip4345()
	{
		static const EthDAACalculator inst(
			/* considerUncle */     true,
			/* deltaDivisor */      DiffType(9),
			/* hasMaxCheck */       true,
			/* hasBombDelay */      true,
			/* bombDelay */         BlkNumType(10700000U));
		return inst;
	}

	static const EthDAACalculator& GetEip3554()
	{
		static const EthDAACalculator inst(
			/* considerUncle */     true,
			/* deltaDivisor */      DiffType(9),
			/* hasMaxCheck */       true,
			/* hasBombDelay */      true,
			/* bombDelay */         BlkNumType(9700000U));
		return inst;
	}

	static const EthDAACalculator& GetEip2384()
	{
		static const EthDAACalculator inst(
			/* considerUncle */     true,
			/* deltaDivisor */      DiffType(9),
			/* hasMaxCheck */       true,
			/* hasBombDelay */      true,
			/* bombDelay */         BlkNumType(9000000U));
		return inst;
	}

	static const EthDAACalculator& GetConstantinople()
	{
		static const EthDAACalculator inst(
			/* considerUncle */     true,
			/* deltaDivisor */      DiffType(9),
			/* hasMaxCheck */       true,
			/* hasBombDelay */      true,
			/* bombDelay */         BlkNumType(5000000U));
		return inst;
	}

	static const EthDAACalculator& GetByzantium()
	{
		static const EthDAACalculator inst(
			/* considerUncle */     true,
			/* deltaDivisor */      DiffType(9),
			/* hasMaxCheck */       true,
			/* hasBombDelay */      true,
			/* bombDelay */         BlkNumType(3000000U));
		return inst;
	}

	static const EthDAACalculator& GetHomestead()
	{
		static const EthDAACalculator inst(
			/* considerUncle */     false,
			/* deltaDivisor */      DiffType(10),
			/* hasMaxCheck */       true,
			/* hasBombDelay */      false,
			/* bombDelay */         BlkNumType(0));
		return inst;
	}

public:
	EthDAACalculator(
		bool considerUncle,
		const DiffType& deltaDivisor,
		bool hasMaxCheck,
		bool hasBombDelay,
		const BlkNumType& bombDelay) :
		EthDAABase(),
		m_considerUncle(considerUncle),
		m_deltaDivisor(deltaDivisor),
		m_hasMaxCheck(hasMaxCheck),
		m_hasBombDelay(hasBombDelay),
		m_bombDelay(bombDelay),
		m_bombDelayFromParent(m_bombDelay - BlkNumType(1))
	{}

	virtual ~EthDAACalculator() = default;

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
			DiffType(EthParams::GetMinimumDifficulty());

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
		DiffType y = parentDiff >> EthParams::GetDifficultyBoundDivisorBitShift();

		// (parent_diff / 2048) *
		//     max((2 if len(parent.uncles) else 1) - ((timestamp - parent.timestamp) // 9), -99)
		x = y * x;

		// parent_diff +
		//     (parent_diff / 2048 * max((2 if len(parent.uncles) else 1) - ((timestamp - parent.timestamp) // 9), -99))
		if (isReducing)
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
}; // class EthDAACalculator


class EthDAACalculatorFrontier : public EthDAABase
{
public: // static members:
	using Base = EthDAABase;

	using BlkNumType = typename Base::BlkNumType;
	using TimeType   = typename Base::TimeType;
	using DiffType   = typename Base::DiffType;

	static const EthDAACalculatorFrontier& GetInstance()
	{
		static const EthDAACalculatorFrontier inst;
		return inst;
	}

public:
	EthDAACalculatorFrontier() = default;

	virtual ~EthDAACalculatorFrontier() = default;

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
			DiffType(EthParams::GetMinimumDifficulty());

		static const BlkNumType sk_blkNBig1      = BlkNumType(1);
		static const BlkNumType sk_blkNBig2      = BlkNumType(2);
		static const BlkNumType sk_expDiffPeriod = BlkNumType(100000);

		// Frontier algorithm:
		// diff =
		//    (parent_diff + (parent_diff / 2048))
		//        if (timestamp - parent_timestamp) < DurationLimit else
		//            (parent_diff - (parent_diff / 2048)) +
		//                2^(periodCount - 2)

		DiffType adjust = parentDiff >> EthParams::GetDifficultyBoundDivisorBitShift();

		TimeType delta = currTime - parentTime;

		DiffType diff = parentDiff;
		if (delta < EthParams::GetDurationLimit())
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

}; // class EthDAACalculatorFrontier


struct EthMainnetConfig
{
public: // static members:

	using BlkNumType = typename EthBlkNumTypeTrait::value_type;

	static const BlkNumType& GetGrayGlacierBlkNum()
	{
		static const BlkNumType blkNum(15050000UL);
		return blkNum;
	}

	static const BlkNumType& GetArrowGlacierBlkNum()
	{
		static const BlkNumType blkNum(13773000UL);
		return blkNum;
	}

	static const BlkNumType& GetLondonBlkNum()
	{
		static const BlkNumType blkNum(12965000UL);
		return blkNum;
	}

	static const BlkNumType& GetMuirGlacierBlkNum()
	{
		static const BlkNumType blkNum(9200000UL);
		return blkNum;
	}

	static const BlkNumType& GetConstantinopleBlkNum()
	{
		static const BlkNumType blkNum(7280000UL);
		return blkNum;
	}

	static const BlkNumType& GetByzantiumBlkNum()
	{
		static const BlkNumType blkNum(4370000UL);
		return blkNum;
	}

	static const BlkNumType& GetHomesteadBlkNum()
	{
		static const BlkNumType blkNum(1150000UL);
		return blkNum;
	}
}; // struct EthMainnetConfig


template<typename _ChainConfig>
class EthGenericDAAImpl : public EthDAABase
{
public: // static members:
	using Self = EthGenericDAAImpl<_ChainConfig>;
	using Base = EthDAABase;

	using ChainConfig = _ChainConfig;

	using BlkNumType = typename Base::BlkNumType;
	using TimeType   = typename Base::TimeType;
	using DiffType   = typename Base::DiffType;

	static const Base& GetCalculator(const BlkNumType& blkNum)
	{
		if (blkNum >= ChainConfig::GetGrayGlacierBlkNum())
		{
			return EthDAACalculator::GetEip5133();
		}
		else if (blkNum >= ChainConfig::GetArrowGlacierBlkNum())
		{
			return EthDAACalculator::GetEip4345();
		}
		else if (blkNum >= ChainConfig::GetLondonBlkNum())
		{
			return EthDAACalculator::GetEip3554();
		}
		else if (blkNum >= ChainConfig::GetMuirGlacierBlkNum())
		{
			return EthDAACalculator::GetEip2384();
		}
		else if (blkNum >= ChainConfig::GetConstantinopleBlkNum())
		{
			return EthDAACalculator::GetConstantinople();
		}
		else if (blkNum >= ChainConfig::GetByzantiumBlkNum())
		{
			return EthDAACalculator::GetByzantium();
		}
		else if (blkNum >= ChainConfig::GetHomesteadBlkNum())
		{
			return EthDAACalculator::GetHomestead();
		}
		else
		{
			return EthDAACalculatorFrontier::GetInstance();
		}
	}

public:

	EthGenericDAAImpl() = default;

	virtual ~EthGenericDAAImpl() = default;

	virtual DiffType operator()(
		const BlkNumType& parentBlkNum,
		const TimeType& parentTime,
		const DiffType& parentDiff,
		bool parentHasUncle,
		const BlkNumType& currBlkNum,
		const TimeType& currTime) const override
	{
		const EthDAABase& calculator = GetCalculator(currBlkNum);
		return calculator(
			parentBlkNum, parentTime, parentDiff, parentHasUncle,
			currBlkNum, currTime);
	}

}; // class EthGenericDAAImpl

using EthMainnetDAA = EthGenericDAAImpl<EthMainnetConfig>;

} // namespace EclipseMonitor
