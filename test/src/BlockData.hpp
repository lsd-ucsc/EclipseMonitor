// Copyright (c) 2023 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once


#include <fstream>
#include <string>
#include <vector>

#include <SimpleRlp/SimpleRlp.hpp>


namespace EclipseMonitor_Test
{
namespace BlockData
{


inline std::vector<uint8_t> ReadBinary(const std::string& filepath)
{
	std::ifstream file(filepath, std::ios::binary);
	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file: " + filepath);
	}

	std::vector<uint8_t> data = std::vector<uint8_t>(
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	);

	return data;
}

/**
 * @brief Get the RLP file path
 *
 * @param filename The filename WITHOUT its extension
 * @return std::string
 */
inline std::string GetRlpFilePath(const std::string& filename)
{
	return ECLIPSEMONITOR_TEST_DIR + ("/BlockData/" + filename) + ".rlp";
}


/**
 * @brief Read RLP data from a file under the testing block data directory
 *
 * @param filename The filename WITHOUT its extension
 * @return SimpleObjects::Object
 */
inline SimpleObjects::Object ReadRlp(const std::string& filename)
{
	std::vector<uint8_t> data = ReadBinary(GetRlpFilePath(filename));
	return SimpleRlp::GeneralParser().Parse(data);
}


inline SimpleRlp::EthHeader ReadHeader(const std::string& filename)
{
	std::vector<uint8_t> data = ReadBinary(GetRlpFilePath(filename));
	return SimpleRlp::EthHeaderParser().Parse(data);
}


} // namespace BlockData
} // namespace EclipseMonitor_Test
