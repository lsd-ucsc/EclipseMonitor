// Copyright (c) 2024 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once


#include <cstdint>

#include <string>
#include <tuple>
#include <vector>

#include <mbedTLScpp/BigNumber.hpp>
#include <mbedTLScpp/Hmac.hpp>
#include <mbedTLScpp/SecretVector.hpp>

#include "../../Internal/SimpleObj.hpp"
#include "../../Internal/Tls.hpp"
#include "../../Exceptions.hpp"
#include "../DataTypes.hpp"
#include "../Keccak256.hpp"
#include "DynamicFee.hpp"


namespace EclipseMonitor
{
namespace Eth
{
namespace Transaction
{


inline const Internal::Tls::BigNum& GetBigNum1()
{
	static const Internal::Tls::BigNum sk_2(1);
	return sk_2;
}

inline const Internal::Tls::BigNum& GetBigNum2()
{
	static const Internal::Tls::BigNum sk_2(2);
	return sk_2;
}

inline const Internal::Tls::BigNum& GetBigNum3()
{
	static const Internal::Tls::BigNum sk_3(3);
	return sk_3;
}

inline const Internal::Tls::BigNum& GetBigNum4()
{
	static const Internal::Tls::BigNum sk_4(4);
	return sk_4;
}

inline const Internal::Tls::BigNum& GetBigNum8()
{
	static const Internal::Tls::BigNum sk_8(8);
	return sk_8;
}


/**
 * @brief This function is derived from
 * https://github.com/ethereum/eth-keys/blob/main/eth_keys/backends/native/jacobian.py
 *
 */
template<
	typename _ABigNumT,
	typename _NBigNumT
>
inline Internal::Tls::BigNum JacobianInv(
	_ABigNumT&& a,
	_NBigNumT&& n
)
{
	const auto& sk_0 = Internal::Tls::BigNum::Zero();
	const auto& sk_1 = GetBigNum1();

	// if a == 0:
	if (a == 0)
	{
		// return 0
		return sk_0;
	}

	// lm, hm = 1, 0
	auto lm = sk_1;
	auto hm = sk_0;

	// low, high = a % n, n
	auto low = Internal::Tls::Mod(a, n);
	auto high = Internal::Tls::BigNum(n);

	// while low > 1:
	while (low > 1)
	{
		// r = high // low
		auto r = high / low;

		// nm, new = hm - lm * r, high - low * r
		// nm = hm - lm * r
		auto nm = hm - (lm * r);
		// new = high - low * r
		auto new_ = high - (low * r);

		// lm, low, hm, high = nm, new, lm, low
		// hm = lm
		hm = lm; // take the value of lm before it is overwritten
		// lm = nm
		lm = std::move(nm); // nm is no longer used

		// high = low
		high = low; // take the value of low before it is overwritten
		// low = new
		low = std::move(new_); // new_ is no longer used
	}

	// return lm % n
	return Internal::Tls::Mod(lm, n);
}


/**
 * @brief This function is derived from
 * https://github.com/ethereum/eth-keys/blob/main/eth_keys/backends/native/jacobian.py
 *
 */
template<
	typename _XBigNumT,
	typename _YBigNumT,
	typename _ZBigNumT,
	typename _CurveABigNumT,
	typename _CurvePBigNumT
>
inline
std::tuple<
	Internal::Tls::BigNum,
	Internal::Tls::BigNum,
	Internal::Tls::BigNum
>
JacobianDouble(
	_XBigNumT&& x,
	_YBigNumT&& y,
	_ZBigNumT&& z,
	_CurveABigNumT&& curveA,
	_CurvePBigNumT&& curveP
)
{
	const auto& sk_0 = Internal::Tls::BigNum::Zero();

	// NOTE: python's `%` operator has different behavior than C++'s `%` operator
	// python's `%` operator is modular reduction, while C++'s `%` operator is remainder
	// so we need to use `Internal::Tls::Mod` to get the modular reduction

	// if not p[1]:
	if (y == 0)
	{
		// return (0, 0, 0)
		return std::make_tuple(sk_0, sk_0, sk_0);
	}

	// ysq = (p[1] ** 2) % P
	auto ysq = (y * y);
	ysq = Internal::Tls::Mod(ysq, curveP);

	// S = (4 * p[0] * ysq) % P
	auto s = (x * 4);
	s *= ysq;
	s = Internal::Tls::Mod(s, curveP);

	// M = (3 * p[0] ** 2 + A * p[2] ** 4) % P
	// 3 * p[0] ** 2
	auto m1 = (x * x);
	m1 *= 3;
	// A * p[2] ** 4
	auto m2 = (z * z);
	m2 *= m2;
	m2 *= curveA;
	auto m = m1 + m2;
	m = Internal::Tls::Mod(m, curveP);

	// nx = (M**2 - 2 * S) % P
	auto nx = (m * m);
	nx -= (s * 2);
	nx = Internal::Tls::Mod(nx, curveP);

	// ny = (M * (S - nx) - 8 * ysq**2) % P
	auto ny = (s - nx);
	ny *= m;
	ny -= ((ysq * ysq) * 8);
	ny = Internal::Tls::Mod(ny, curveP);

	// nz = (2 * p[1] * p[2]) % P
	auto nz = (y * 2);
	nz *= z;
	nz = Internal::Tls::Mod(nz, curveP);

	// return (nx, ny, nz)
	return std::make_tuple(std::move(nx), std::move(ny), std::move(nz));
}


template<
	typename _CurveABigNumT,
	typename _CurvePBigNumT
>
inline
std::tuple<
	Internal::Tls::BigNum,
	Internal::Tls::BigNum,
	Internal::Tls::BigNum
>
JacobianDouble(
	const std::tuple<
		Internal::Tls::BigNum,
		Internal::Tls::BigNum,
		Internal::Tls::BigNum
	>& p,
	_CurveABigNumT&& curveA,
	_CurvePBigNumT&& curveP
)
{
	return JacobianDouble(
		std::get<0>(p),
		std::get<1>(p),
		std::get<2>(p),
		std::forward<_CurveABigNumT>(curveA),
		std::forward<_CurvePBigNumT>(curveP)
	);
}


/**
 * @brief This function is derived from
 * https://github.com/ethereum/eth-keys/blob/main/eth_keys/backends/native/jacobian.py
 *
 */
template<
	typename _PxBigNumT,
	typename _PyBigNumT,
	typename _PzBigNumT,
	typename _QxBigNumT,
	typename _QyBigNumT,
	typename _QzBigNumT,
	typename _CurveABigNumT,
	typename _CurvePBigNumT
>
inline
std::tuple<
	Internal::Tls::BigNum,
	Internal::Tls::BigNum,
	Internal::Tls::BigNum
>
JacobianAdd(
	_PxBigNumT&& pX,
	_PyBigNumT&& pY,
	_PzBigNumT&& pZ,
	_QxBigNumT&& qX,
	_QyBigNumT&& qY,
	_QzBigNumT&& qZ,
	_CurveABigNumT&& curveA,
	_CurvePBigNumT&& curveP
)
{
	const auto& sk_0 = Internal::Tls::BigNum::Zero();
	const auto& sk_1 = GetBigNum1();

	// if not p[1]:
	if (pY == 0)
	{
		// return q
		return std::make_tuple(
			Internal::Tls::BigNum(qX),
			Internal::Tls::BigNum(qY),
			Internal::Tls::BigNum(qZ)
		);
	}

	// if not q[1]:
	if (qY == 0)
	{
		// return p
		return std::make_tuple(
			Internal::Tls::BigNum(pX),
			Internal::Tls::BigNum(pY),
			Internal::Tls::BigNum(pZ)
		);
	}

	// U1 = (p[0] * q[2] ** 2) % P
	auto u1 = (qZ * qZ);
	u1 *= pX;
	u1 = Internal::Tls::Mod(u1, curveP);

	// U2 = (q[0] * p[2] ** 2) % P
	auto u2 = (pZ * pZ);
	u2 *= qX;
	u2 = Internal::Tls::Mod(u2, curveP);

	// S1 = (p[1] * q[2] ** 3) % P
	auto s1 = (qZ * qZ);
	s1 *= qZ;
	s1 *= pY;
	s1 = Internal::Tls::Mod(s1, curveP);

	// S2 = (q[1] * p[2] ** 3) % P
	auto s2 = (pZ * pZ);
	s2 *= pZ;
	s2 *= qY;
	s2 = Internal::Tls::Mod(s2, curveP);

	// if U1 == U2:
	if (u1 == u2)
	{
		// if S1 != S2:
		if (s1 != s2)
		{
			// return (0, 0, 1)
			return std::make_tuple(sk_0, sk_0, sk_1);
		}
		else
		{
			// return jacobian_double(p)
			return JacobianDouble(
				std::forward<_PxBigNumT>(pX),
				std::forward<_PyBigNumT>(pY),
				std::forward<_PzBigNumT>(pZ),
				std::forward<_CurveABigNumT>(curveA),
				std::forward<_CurvePBigNumT>(curveP)
			);
		}
	}

	// H = U2 - U1
	auto h = u2 - u1;

	// R = S2 - S1
	auto r = s2 - s1;

	// H2 = (H * H) % P
	auto h2 = (h * h);
	h2 = Internal::Tls::Mod(h2, curveP);

	// H3 = (H * H2) % P
	auto h3 = (h * h2);
	h3 = Internal::Tls::Mod(h3, curveP);

	// U1H2 = (U1 * H2) % P
	auto u1h2 = (u1 * h2);
	u1h2 = Internal::Tls::Mod(u1h2, curveP);

	// nx = (R**2 - H3 - 2 * U1H2) % P
	auto nx = (r * r);
	nx -= h3;
	nx -= (u1h2 * 2);
	nx = Internal::Tls::Mod(nx, curveP);

	// ny = (R * (U1H2 - nx) - S1 * H3) % P
	auto ny = (u1h2 - nx);
	ny *= r;
	ny -= (s1 * h3);
	ny = Internal::Tls::Mod(ny, curveP);

	// nz = (H * p[2] * q[2]) % P
	auto nz = (h * pZ);
	nz *= qZ;
	nz = Internal::Tls::Mod(nz, curveP);

	// return (nx, ny, nz)
	return std::make_tuple(std::move(nx), std::move(ny), std::move(nz));
}


template<
	typename _CurveABigNumT,
	typename _CurvePBigNumT
>
inline
std::tuple<
	Internal::Tls::BigNum,
	Internal::Tls::BigNum,
	Internal::Tls::BigNum
>
JacobianAdd(
	const std::tuple<
		Internal::Tls::BigNum,
		Internal::Tls::BigNum,
		Internal::Tls::BigNum
	>& p,
	const std::tuple<
		Internal::Tls::BigNum,
		Internal::Tls::BigNum,
		Internal::Tls::BigNum
	>& q,
	_CurveABigNumT&& curveA,
	_CurvePBigNumT&& curveP
)
{
	return JacobianAdd(
		std::get<0>(p),
		std::get<1>(p),
		std::get<2>(p),
		std::get<0>(q),
		std::get<1>(q),
		std::get<2>(q),
		std::forward<_CurveABigNumT>(curveA),
		std::forward<_CurvePBigNumT>(curveP)
	);
}


/**
 * @brief This function is derived from
 * https://github.com/ethereum/eth-keys/blob/main/eth_keys/backends/native/jacobian.py
 *
 */
template<
	typename _AxBigNumT,
	typename _AyBigNumT,
	typename _AzBigNumT,
	typename _NumBigNumT,
	typename _CurveABigNumT,
	typename _CurveNBigNumT,
	typename _CurvePBigNumT
>
inline
std::tuple<
	Internal::Tls::BigNum,
	Internal::Tls::BigNum,
	Internal::Tls::BigNum
>
JacobianMultiply(
	_AxBigNumT&& aX,
	_AyBigNumT&& aY,
	_AzBigNumT&& aZ,
	_NumBigNumT&& num,
	_CurveABigNumT&& curveA,
	_CurveNBigNumT&& curveN,
	_CurvePBigNumT&& curveP
)
{
	const auto& sk_0 = Internal::Tls::BigNum::Zero();
	const auto& sk_1 = GetBigNum1();

	// if a[1] == 0 or n == 0:
	if (aY == 0 || num == 0)
	{
		// return (0, 0, 1)
		return std::make_tuple(sk_0, sk_0, sk_1);
	}

	// if n == 1:
	if (num == 1)
	{
		// return a
		return std::make_tuple(
			Internal::Tls::BigNum(aX),
			Internal::Tls::BigNum(aY),
			Internal::Tls::BigNum(aZ)
		);
	}

	// if n < 0 or n >= N:
	if (num < 0 || num >= curveN)
	{
		// return jacobian_multiply(a, n % N)
		return JacobianMultiply(
			std::forward<_AxBigNumT>(aX),
			std::forward<_AyBigNumT>(aY),
			std::forward<_AzBigNumT>(aZ),
			Internal::Tls::Mod(num, curveN),
			std::forward<_CurveABigNumT>(curveA),
			std::forward<_CurveNBigNumT>(curveN),
			std::forward<_CurvePBigNumT>(curveP)
		);
	}

	// if (n % 2) == 0:
	uint8_t numMod2 = num.Mod(uint8_t(2));
	if (numMod2 == 0)
	{
		// return jacobian_double(jacobian_multiply(a, n // 2))
		auto tmp = JacobianMultiply(
			std::forward<_AxBigNumT>(aX),
			std::forward<_AyBigNumT>(aY),
			std::forward<_AzBigNumT>(aZ),
			(num / 2),
			std::forward<_CurveABigNumT>(curveA),
			std::forward<_CurveNBigNumT>(curveN),
			std::forward<_CurvePBigNumT>(curveP)
		);
		return JacobianDouble(
			std::get<0>(tmp),
			std::get<1>(tmp),
			std::get<2>(tmp),
			std::forward<_CurveABigNumT>(curveA),
			std::forward<_CurvePBigNumT>(curveP)
		);
	}
	// elif (n % 2) == 1:
	else if (numMod2 == 1)
	{
		// return jacobian_add(jacobian_double(jacobian_multiply(a, n // 2)), a)
		auto tmp = JacobianDouble(
			JacobianMultiply(
				std::forward<_AxBigNumT>(aX),
				std::forward<_AyBigNumT>(aY),
				std::forward<_AzBigNumT>(aZ),
				(num / 2),
				std::forward<_CurveABigNumT>(curveA),
				std::forward<_CurveNBigNumT>(curveN),
				std::forward<_CurvePBigNumT>(curveP)
			),
			std::forward<_CurveABigNumT>(curveA),
			std::forward<_CurvePBigNumT>(curveP)
		);
		return JacobianAdd(
			std::get<0>(tmp),
			std::get<1>(tmp),
			std::get<2>(tmp),
			std::forward<_AxBigNumT>(aX),
			std::forward<_AyBigNumT>(aY),
			std::forward<_AzBigNumT>(aZ),
			std::forward<_CurveABigNumT>(curveA),
			std::forward<_CurvePBigNumT>(curveP)
		);
	}
	// else:
	{
		// raise Exception("Invariant: Unreachable code path")
		throw Exception("Invariant: Unreachable code path");
	}
}


/**
 * @brief This function is derived from
 * https://github.com/ethereum/eth-keys/blob/main/eth_keys/backends/native/jacobian.py
 *
 */
template<
	typename _PxBigNumT,
	typename _PyBigNumT,
	typename _PzBigNumT,
	typename _CurvePBigNumT
>
inline
std::tuple<
	Internal::Tls::BigNum,
	Internal::Tls::BigNum
>
FromJacobian(
	_PxBigNumT&& pX,
	_PyBigNumT&& pY,
	_PzBigNumT&& pZ,
	_CurvePBigNumT&& curveP
)
{
	// z = inv(p[2], P)
	auto z = JacobianInv(
		std::forward<_PzBigNumT>(pZ),
		std::forward<_CurvePBigNumT>(curveP)
	);

	// return ((p[0] * z**2) % P, (p[1] * z**3) % P)
	// (p[0] * z**2) % P
	auto zsqr = (z * z);
	auto nx = (pX * zsqr);
	nx = Internal::Tls::Mod(nx, curveP);
	// (p[1] * z**3) % P
	auto ny = (z * zsqr);
	ny *= pY;
	ny = Internal::Tls::Mod(ny, curveP);

	return std::make_tuple(std::move(nx), std::move(ny));
}


template<typename _CurvePBigNumT>
inline
std::tuple<
	Internal::Tls::BigNum,
	Internal::Tls::BigNum
>
FromJacobian(
	const std::tuple<
		Internal::Tls::BigNum,
		Internal::Tls::BigNum,
		Internal::Tls::BigNum
	>& p,
	_CurvePBigNumT&& curveP
)
{
	return FromJacobian(
		std::get<0>(p),
		std::get<1>(p),
		std::get<2>(p),
		std::forward<_CurvePBigNumT>(curveP)
	);
}


/**
 * @brief This function is derived from
 * https://github.com/ethereum/eth-keys/blob/main/eth_keys/backends/native/jacobian.py
 *
 */
template<
	typename _AxBigNumT,
	typename _AyBigNumT,
	typename _NumBigNumT,
	typename _CurveABigNumT,
	typename _CurveNBigNumT,
	typename _CurvePBigNumT
>
inline
std::tuple<
	Internal::Tls::BigNum,
	Internal::Tls::BigNum
>
JacobianFastMultiply(
	_AxBigNumT&& aX,
	_AyBigNumT&& aY,
	_NumBigNumT&& num,
	_CurveABigNumT&& curveA,
	_CurveNBigNumT&& curveN,
	_CurvePBigNumT&& curveP
)
{
	const auto& sk_1 = GetBigNum1();

	// return from_jacobian(jacobian_multiply(to_jacobian(a), n))
	std::tuple<
		Internal::Tls::BigNum,
		Internal::Tls::BigNum
	> ret;

	return FromJacobian(
		JacobianMultiply(
			std::forward<_AxBigNumT>(aX),
			std::forward<_AyBigNumT>(aY),
			sk_1,
			std::forward<_NumBigNumT>(num),
			std::forward<_CurveABigNumT>(curveA),
			std::forward<_CurveNBigNumT>(curveN),
			std::forward<_CurvePBigNumT>(curveP)
		),
		std::forward<_CurvePBigNumT>(curveP)
	);
}


/**
 * @brief This function is derived from
 * https://github.com/ethereum/eth-keys/blob/main/eth_keys/backends/native/ecdsa.py
 *
 */
template<
	typename _HashCtnT, bool _HashCtnSec,
	typename _PKeyCtnT, bool _PKeyCtnSec
>
inline Internal::Tls::BigNum DeterministicGenerateK(
	const Internal::Tls::ContCtnReadOnlyRef<_HashCtnT, _HashCtnSec>& hashCtnRef,
	const Internal::Tls::ContCtnReadOnlyRef<_PKeyCtnT, _PKeyCtnSec>& pKeyCtnRef
)
{
	// digest_fn: Callable[[], Any] = hashlib.sha256,

	// v_0 = b"\x01" * 32
	static const  Internal::Tls::SecretVector<uint8_t> sk_v0 = {
		0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
		0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	};
	// k_0 = b"\x00" * 32
	static const Internal::Tls::SecretVector<uint8_t> sk_k0 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	};
	static const uint8_t sk_00[] = { 0x00U, };
	static const uint8_t sk_01[] = { 0x01U, };

	// k_1 = hmac.new(k_0, v_0 + b"\x00" + private_key_bytes + msg_hash, digest_fn).digest()
	auto k1 = Internal::Tls::HmacerBase(
		Internal::Tls::GetMdInfo(Internal::Tls::HashType::SHA256),
		Internal::Tls::CtnFullR(sk_k0)
	).Update(
		Internal::Tls::CtnFullR(sk_v0)
	).Update(
		Internal::Tls::CtnFullR(sk_00)
	).Update(
		pKeyCtnRef
	).Update(
		hashCtnRef
	).template Finish<Internal::Tls::SecretVector<uint8_t> >();

	// v_1 = hmac.new(k_1, v_0, digest_fn).digest()
	auto v1 = Internal::Tls::HmacerBase(
		Internal::Tls::GetMdInfo(Internal::Tls::HashType::SHA256),
		Internal::Tls::CtnFullR(k1)
	).Update(
		Internal::Tls::CtnFullR(sk_v0)
	).template Finish<Internal::Tls::SecretVector<uint8_t> >();

	// k_2 = hmac.new(k_1, v_1 + b"\x01" + private_key_bytes + msg_hash, digest_fn).digest()
	auto k2 = Internal::Tls::HmacerBase(
		Internal::Tls::GetMdInfo(Internal::Tls::HashType::SHA256),
		Internal::Tls::CtnFullR(k1)
	).Update(
		Internal::Tls::CtnFullR(v1)
	).Update(
		Internal::Tls::CtnFullR(sk_01)
	).Update(
		pKeyCtnRef
	).Update(
		hashCtnRef
	).template Finish<Internal::Tls::SecretVector<uint8_t> >();

	// v_2 = hmac.new(k_2, v_1, digest_fn).digest()
	auto v2 = Internal::Tls::HmacerBase(
		Internal::Tls::GetMdInfo(Internal::Tls::HashType::SHA256),
		Internal::Tls::CtnFullR(k2)
	).Update(
		Internal::Tls::CtnFullR(v1)
	).template Finish<Internal::Tls::SecretVector<uint8_t> >();

	// kb = hmac.new(k_2, v_2, digest_fn).digest()
	auto kb = Internal::Tls::HmacerBase(
		Internal::Tls::GetMdInfo(Internal::Tls::HashType::SHA256),
		Internal::Tls::CtnFullR(k2)
	).Update(
		Internal::Tls::CtnFullR(v2)
	).template Finish<Internal::Tls::SecretVector<uint8_t> >();

	// k = big_endian_to_int(kb)
	auto k = Internal::Tls::BigNum(
		Internal::Tls::CtnFullR(kb),
		true, // isPositive
		false // isLittleEndian
	);

	// return k
	return k;
}


/**
 * @brief This function is derived from
 * https://github.com/ethereum/eth-keys/blob/main/eth_keys/backends/native/ecdsa.py
 *
 */
template<
	typename _HashCtnT, bool _HashCtnSec,
	typename _PKeyCtnT, bool _PKeyCtnSec,
	typename _PkeyBigNumTraits,
	typename _CurveABigNumTraits,
	typename _CurveGxBigNumTraits,
	typename _CurveGyBigNumTraits,
	typename _CurveNBigNumTraits,
	typename _CurvePBigNumTraits
>
inline
std::tuple<
	uint8_t,
	Internal::Tls::BigNum,
	Internal::Tls::BigNum
>
EcdsaRawSign(
	const Internal::Tls::ContCtnReadOnlyRef<_HashCtnT, _HashCtnSec>& hashCtnRef,
	const Internal::Tls::ContCtnReadOnlyRef<_PKeyCtnT, _PKeyCtnSec>& pKeyCtnRef,
	const Internal::Tls::BigNumber<_PkeyBigNumTraits>& privKeyNum,
	const Internal::Tls::BigNumber<_CurveABigNumTraits>& curveA,
	const Internal::Tls::BigNumber<_CurveGxBigNumTraits>& curveGx,
	const Internal::Tls::BigNumber<_CurveGyBigNumTraits>& curveGy,
	const Internal::Tls::BigNumber<_CurveNBigNumTraits>& curveN,
	const Internal::Tls::BigNumber<_CurvePBigNumTraits>& curveP
)
{
	// z = big_endian_to_int(msg_hash)
	auto z = Internal::Tls::BigNum(
		hashCtnRef,
		true, // isPositive
		false // isLittleEndian
	);

	// k = deterministic_generate_k(msg_hash, private_key_bytes)
	auto k = DeterministicGenerateK(hashCtnRef, pKeyCtnRef);

	// r, y = fast_multiply(G, k)
	Internal::Tls::BigNum r;
	Internal::Tls::BigNum y;
	std::tie(r, y) =
		JacobianFastMultiply(curveGx, curveGy, k, curveA, curveN, curveP);

	// s_raw = inv(k, N) * (z + r * big_endian_to_int(private_key_bytes)) % N
	auto sRaw = JacobianInv(k, curveN);
	sRaw *= (z + (r * privKeyNum));
	sRaw = Internal::Tls::Mod(sRaw, curveN);

	// v = 27 + ((y % 2) ^ (0 if s_raw * 2 < N else 1))
	// (y % 2)
	uint8_t v21 = y.Mod(uint8_t(2));
	// (0 if s_raw * 2 < N else 1)
	uint8_t v22 = ((sRaw * 2) < curveN) ? 0 : 1;
	uint8_t v = 27 + (v21 ^ v22);

	// s = s_raw if s_raw * 2 < N else N - s_raw
	auto s = ((sRaw * 2) < curveN) ? sRaw : (curveN - sRaw);

	// return v - 27, r, s
	return std::make_tuple(v - 27, std::move(r), std::move(s));
}


template<
	typename _HashCtnT, bool _HashCtnSec,
	typename _PkeyBigNumTraits,
	typename _CurveABigNumTraits,
	typename _CurveGxBigNumTraits,
	typename _CurveGyBigNumTraits,
	typename _CurveNBigNumTraits,
	typename _CurvePBigNumTraits
>
inline
std::tuple<
	uint8_t,
	Internal::Tls::BigNum,
	Internal::Tls::BigNum
>
EcdsaRawSign(
	const Internal::Tls::ContCtnReadOnlyRef<_HashCtnT, _HashCtnSec>& hashCtnRef,
	const Internal::Tls::BigNumber<_PkeyBigNumTraits>& privKeyNum,
	const Internal::Tls::BigNumber<_CurveABigNumTraits>& curveA,
	const Internal::Tls::BigNumber<_CurveGxBigNumTraits>& curveGx,
	const Internal::Tls::BigNumber<_CurveGyBigNumTraits>& curveGy,
	const Internal::Tls::BigNumber<_CurveNBigNumTraits>& curveN,
	const Internal::Tls::BigNumber<_CurvePBigNumTraits>& curveP
)
{
	auto privKeyBytes =
		privKeyNum.template SecretBytes</*little endian=*/false>();

	return EcdsaRawSign(
		hashCtnRef,
		Internal::Tls::CtnFullR(privKeyBytes),
		privKeyNum,
		curveA,
		curveGx,
		curveGy,
		curveN,
		curveP
	);
}


} // namespace Transaction
} // namespace Eth
} // namespace EclipseMonitor

