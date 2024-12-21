// Copyright (c) 2023 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once


#include <cstdint>

#include <array>
#include <vector>

#include "Keccak256.hpp"


namespace EclipseMonitor
{
namespace Eth
{


/**
 * @brief source: https://github.com/ethereum/go-ethereum/blob/master/consensus/ethash/algorithm.go
 *
 */
struct Hashimoto
{
	static constexpr size_t sk_mixBytes       = 128;
	static constexpr size_t sk_loopAccesses   = 64;
	static constexpr size_t sk_hashBytes      = 64;
	static constexpr size_t sk_hashWords      = 16;
	static constexpr size_t sk_datasetParents = 256;

	template<typename _ContainerType>
	struct ContainerRef
	{
		using KIt = typename _ContainerType::const_iterator;

		ContainerRef(_ContainerType c) :
			m_c(c),
			m_itBegin(m_c.cbegin()),
			m_itEnd(m_c.cend())
		{}

		ContainerRef(KIt begin, KIt end) :
			m_c(),
			m_itBegin(begin),
			m_itEnd(end)
		{}

		KIt begin() const
		{
			return m_itBegin;
		}

		KIt end() const
		{
			return m_itEnd;
		}

		_ContainerType m_c;
		KIt m_itBegin;
		KIt m_itEnd;
	}; // struct ContainerRef

	template<typename _IntType, typename _OutIt>
	static void LittleEndianPutInt(const _IntType& v, _OutIt out)
	{
		for (size_t i = 0; i < sizeof(_IntType); ++i)
		{
			*out = static_cast<uint8_t>((v >> (i * 8)) & 0xFFU);
			++out;
		}
	}

	template<typename _IntType, typename _InIt>
	static _IntType LittleEndianGetInt(_InIt in)
	{
		_IntType v = 0;
		for (size_t i = 0; i < sizeof(_IntType); ++i)
		{
			_IntType byte = static_cast<_IntType>((*in) & 0xFFU);
			v |= byte << (i * 8);
			++in;
		}
		return v;
	}

	static uint32_t Fnv(uint32_t a, uint32_t b)
	{
		return (a * 0x01000193) ^ b;
	}

	template<typename _InIt>
	static void FnvHash(
		std::vector<uint32_t>& mix,
		_InIt inIt
	)
	{
		for (size_t i = 0; i < mix.size(); ++i)
		{
			mix[i] = (mix[i] * 0x01000193) ^ *(inIt + i);
		}
	}

	static std::vector<uint8_t> GenerateDatasetItem(
		const std::vector<uint32_t>& cache,
		uint32_t index
	)
	{
		// Calculate the number of theoretical rows (we use one buffer nonetheless)
		uint32_t rows = static_cast<uint32_t>(cache.size() / sk_hashWords);

		// Initialize the mix
		std::vector<uint8_t> mix;
		mix.reserve(sk_hashBytes);

		LittleEndianPutInt(
			cache[(index%rows)*sk_hashWords]^index,
			std::back_inserter(mix)
		);
		for (size_t i = 1; i < sk_hashWords; ++i)
		{
			LittleEndianPutInt(
				cache[(index%rows)*sk_hashWords+i],
				mix.begin() + i * 4
			);
		}
		std::array<uint8_t, 32> mixHash = Keccak256(mix);
		mix.assign(mixHash.begin(), mixHash.end());
		mix.resize(mixHash.size());

		// Convert the mix to uint32s to avoid constant bit shifting
		std::vector<uint32_t> intMix(sk_hashWords);
		for (size_t i = 0; i < intMix.size(); ++i)
		{
			intMix[i] = LittleEndianGetInt<uint32_t>(mix.begin() + i * 4);
		}
		// fnv it with a lot of random cache nodes based on index
		for (uint32_t i = 0; i < sk_datasetParents; ++i)
		{
			uint32_t parent = Fnv(index^i, intMix[i%16]) % rows;
			FnvHash(intMix, cache.begin() + parent * sk_hashWords);
		}
		// Flatten the uint32 mix into a binary one and return
		mix.resize(0);
		for (size_t i = 0; i < intMix.size(); ++i)
		{
			LittleEndianPutInt(intMix[i], std::back_inserter(mix));
		}
		mixHash = Keccak256(mix);
		mix.assign(mixHash.begin(), mixHash.end());
		mix.resize(mixHash.size());

		return mix;
	}

	template<typename _LookupFuncType>
	static std::array<uint8_t, 32> Impl(
		const std::array<uint8_t, 32>& hash,
		uint64_t nonce,
		size_t size,
		_LookupFuncType lookup
	)
	{
		// Calculate the number of theoretical rows (we use one buffer nonetheless)
		uint32_t rows = static_cast<uint32_t>(size / sk_mixBytes);

		// Combine header+nonce into a 40 byte seed
		std::vector<uint8_t> seedData;
		seedData.reserve(40);
		seedData.insert(seedData.end(), hash.begin(), hash.end());
		LittleEndianPutInt(nonce, std::back_inserter(seedData));

		std::array<uint8_t, 32> seed = Keccak256(seedData);
		uint32_t seedHead = LittleEndianGetInt<uint32_t>(seed.begin());

		// Start the mix with replicated seed
		std::vector<uint32_t> mix(sk_mixBytes / 4);
		for (size_t i = 0; i < mix.size(); ++i)
		{
			mix[i] = LittleEndianGetInt<uint32_t>(seed.begin() + (i % 16) * 4);
		}
		// Mix in random dataset nodes
		std::vector<uint32_t> temp(mix.size());

		for (size_t i = 0; i < sk_loopAccesses; ++i)
		{
			uint32_t parent =
				Fnv(
					static_cast<uint32_t>(i)^seedHead,
					mix[i % mix.size()]
				) % rows;

			for (size_t j = 0; j < sk_mixBytes/sk_hashBytes; ++j)
			{
				auto luRange = lookup(2 * parent + j);
				std::copy(
					luRange.begin(), luRange.end(),
					temp.begin() + (j * sk_hashWords)
				);
			}
			FnvHash(mix, temp.begin());
		}
		// Compress mix
		for (size_t i = 0; i < mix.size(); i += 4)
		{
			mix[i/4] = Fnv(Fnv(Fnv(mix[i], mix[i+1]), mix[i+2]), mix[i+3]);
		}
		mix.resize(mix.size() / 4);

		std::vector<uint8_t> digest;
		digest.reserve(sk_hashBytes);
		for (size_t i = 0; i < mix.size(); ++i)
		{
			LittleEndianPutInt(mix[i], std::back_inserter(digest));
		}

		// Keccak256(seed + digest)
		std::vector<uint8_t> seedAndDigest;
		seedAndDigest.reserve(seed.size() + digest.size());
		seedAndDigest.insert(seedAndDigest.end(), seed.begin(), seed.end());
		seedAndDigest.insert(seedAndDigest.end(), digest.begin(), digest.end());

		std::array<uint8_t, 32> result = Keccak256(seedAndDigest);

		return result;
	}

	static std::array<uint8_t, 32> Full(
		const std::vector<uint32_t>& dataset,
		const std::array<uint8_t, 32>& hash,
		uint64_t nonce
	)
	{
		using LUContainer = ContainerRef<std::vector<uint32_t> >;

		auto lookup = [&dataset](uint32_t index) -> LUContainer
		{
			size_t offset = index * sk_hashWords;
			return LUContainer(
				dataset.begin() + offset,
				dataset.begin() + offset + sk_hashWords
			);
		};
		size_t size = dataset.size() * 4;

		return Impl(hash, nonce, size, lookup);
	}

	static std::array<uint8_t, 32> Light(
		size_t size,
		const std::vector<uint32_t>& cache,
		const std::array<uint8_t, 32>& hash,
		uint64_t nonce
	)
	{
		using LUContainer = ContainerRef<std::vector<uint32_t> >;

		auto lookup = [&cache](uint32_t index) -> LUContainer
		{
			std::vector<uint8_t> rawData = GenerateDatasetItem(cache, index);

			std::vector<uint32_t> data(rawData.size() / 4);
			for (size_t i = 0; i < data.size(); ++i)
			{
				data[i] = LittleEndianGetInt<uint32_t>(rawData.begin() + i * 4);
			}
			return LUContainer(std::move(data));
		};

		return Impl(hash, nonce, size, lookup);
	}

}; // struct Hashimoto



} // namespace Eth
} // namespace EclipseMonitor
