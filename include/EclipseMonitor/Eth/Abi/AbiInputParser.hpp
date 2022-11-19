// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <iostream>
#include <string>

#include <EclipseMonitor/Internal/SimpleRlp.hpp>
#include <SimpleObjects/Internal/make_unique.hpp>

#include "AbiParam.hpp"

namespace EclipseMonitor
{
namespace Eth
{
namespace Abi
{

class AbiInputParser
{

static const uint64_t BYTE_CHUNK = 32;

// first 4 bytes of keccak of function signature
static const uint64_t FUNC_SIG_SIZE = 4;

public:

	AbiInputParser() = default;

	static Internal::Obj::List DataToChunks(Internal::Obj::Bytes& data)
	{
		uint64_t numChunks = data.size() / BYTE_CHUNK;
		Internal::Obj::List chunks;

		for (uint64_t i = 0; i < numChunks; i++)
		{
			Internal::Obj::Bytes chunk =
			{
				data.begin() + i * BYTE_CHUNK,
				data.begin() + (i + 1) * BYTE_CHUNK
			};
			chunks.push_back(chunk);
		}
		return chunks;
	}

	static Internal::Obj::UInt64 ChunkToInt(
		Internal::Obj::Object& chunkObj,
		uint64_t (*func)(const uint8_t (&b)[sizeof(uint64_t)])
	)
	{
		Internal::Obj::BytesBaseObj& chunk = chunkObj.AsBytes();

		uint8_t numInBytes[8];
		uint8_t k = chunk.size() - 1;

		for (uint j = 0; j < 8; j++, k--)
		{
			numInBytes[j] = chunk[k];
		}
		Internal::Obj::UInt64 val(func(numInBytes));

		return val;
	}

	static Internal::Obj::Bytes ChunksToBytes(
		Internal::Obj::List& chunks,
		uint& dataPos,
		uint64_t (*IntWriter)(const uint8_t (&b)[sizeof(uint64_t)])
	)
	{
		Internal::Obj::UInt64 numBytes =
			ChunkToInt(chunks[dataPos++], IntWriter);

		uint64_t numChunks =
			(numBytes.GetVal() + BYTE_CHUNK - 1) / BYTE_CHUNK;

		Internal::Obj::Bytes bytes;
		for (uint i = 0; i < numChunks; i++)
		{
			Internal::Obj::Object& currChunk = chunks[dataPos++];
			Internal::Obj::BytesBaseObj& chunk = currChunk.AsBytes();
			bytes += chunk;
		}

		return bytes;
	}

	static std::vector<std::unique_ptr<AbiParam>> ParseInput(
		std::vector<std::unique_ptr<AbiParamType>> paramTypes,
		Internal::Obj::Bytes& input,
		bool functionHeader
	)
	{
		// writing bytes into an int
		auto IntWriter = SimpleRlp::Internal::DecodeIntBytes<uint64_t>;
		Internal::Obj::Bytes inputData;

		if (functionHeader)
		{
			inputData =
				{input.data() + FUNC_SIG_SIZE, input.data() + input.size()};
		}
		else
		{
			inputData = input;
		}

		std::vector<std::unique_ptr<AbiParam>> parsedParams;
		parsedParams.reserve(paramTypes.size());

		Internal::Obj::List chunks = DataToChunks(inputData);
		Internal::Obj::Object paramObj;

		uint dataPos = paramTypes.size();
		uint paramsSize = paramTypes.size();
		for (uint i = 0; i < paramsSize; i++)
		{
			AbiParamType& param = *paramTypes[i];

			// static parameters are stored directly
			if (param.IsStatic())
			{
				if (param.GetType() == ParamType::Uint64)
				{
					// TODO: why num displayed as byte here while
					// TestEThAbiDecode shows as int correctly
					paramObj = std::move(ChunkToInt(chunks[i], IntWriter));
				}
				else if (param.GetType() == ParamType::Bool)
				{
					Internal::Obj::BytesBaseObj& chunk = chunks[i].AsBytes();
					paramObj =
						std::move(Internal::Obj::Bool(chunk[chunk.size() - 1]));
				}
				else if (param.GetType() == ParamType::Bytes32)
				{
					paramObj = std::move(chunks[i].AsBytes());
				}

			}
			else if(param.IsArray())
			{
				// array parameters are stored as a list of chunks
				// the first chunk is the length of the array
				// the rest of the chunks are the elements
				// TODO: refactor redundant code parsing arrays
				Internal::Obj::UInt64 arrLen =
					ChunkToInt(chunks[dataPos++], IntWriter);
				Internal::Obj::List arrayData;
				bool bytesSkip = false;

				for (uint i = 0; i < arrLen.GetVal(); i++)
				{
					if(param.GetType() == ParamType::Uint64)
					{
						Internal::Obj::UInt64 val =
							ChunkToInt(chunks[dataPos++], IntWriter);

						arrayData.push_back(std::move(val));
					}
					else if(param.GetType() == ParamType::Bool)
					{
						Internal::Obj::BytesBaseObj& chunk =
							chunks[dataPos++].AsBytes();
						Internal::Obj::Bool val(chunk[chunk.size() - 1]);

						arrayData.push_back(std::move(val));
					}
					else if(param.GetType() == ParamType::Bytes)
					{

						/* NOTE
						* for bytes arrays, starting from the current datapos
						* the next n chunks (n is the size of the array)
						* contains the offset (in bytes) where the length of
						* the "Bytes" elements can be found.

						* Because we are iteratively parsing these chunks, we
						* can skip these n chunks.
						*
						* Chunk datapos + n will contain the length of the first
						* element, followed by the content of the first element.
						*/
						if (!bytesSkip)
						{
							dataPos += arrLen.GetVal();
							bytesSkip = true;
						}

						Internal::Obj::Bytes bytes =
							ChunksToBytes(chunks, dataPos, IntWriter);
						arrayData.push_back(std::move(bytes));
					}
				}
				paramObj = std::move(arrayData);
			}
			else if(param.GetType() == ParamType::Bytes)
			{
				Internal::Obj::Bytes bytes =
					ChunksToBytes(chunks, dataPos, IntWriter);
				paramObj = std::move(bytes);
			}

			auto abiParam =
				Internal::Obj::Internal::make_unique<AbiParam>
				(
					std::move(paramTypes[i]),
					std::move(paramObj)
				);

			parsedParams.push_back(std::move(abiParam));
		}

		return parsedParams;
	} // ParseInput

}; // class AbiInputParser


} // namespace Abi
} // namespace Eth
} // namespace EclipseMonitor
