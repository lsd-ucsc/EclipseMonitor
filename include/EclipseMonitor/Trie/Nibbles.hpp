// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <cstdint>

#include "EclipseMonitor/Internal/SimpleRlp.hpp"

namespace EclipseMonitor
{

// from: https://github.com/zhenghaven/SimpleUtf/blob/main/include/SimpleUtf/Exceptions.hpp
class Exception : public std::runtime_error
{
public:
    Exception(const std::string& what_arg) :
        std::runtime_error(what_arg)
    {}
};

class NibblesConversionException : public Exception
{
public:
    NibblesConversionException(const std::string& what_arg) :
        Exception(what_arg)
    {}
};

using Nibble = uint8_t;

struct NibbleHelper
{
    static bool IsNibble(const uint8_t &n)
    {
        return n <= 15;
    }

    static Nibble FromNibbleByte(const uint8_t &n)
    {
        return IsNibble(n) ? Nibble(n) :
               throw NibblesConversionException(
                   "non-nibble byte " +
                       std::to_string(n));
    }

    static std::vector<Nibble> FromNibbleBytes(std::vector<uint8_t> nibbleBytes)
    {
        std::vector<Nibble> nibbles;
        nibbles.reserve(nibbleBytes.size());

        for (const auto &nibbleByte: nibbleBytes)
        {
            try
            {
                Nibble nibble = FromNibbleByte(nibbleByte);
                nibbles.push_back(nibble);
            }
            catch (const NibblesConversionException &e)
            {
                throw NibblesConversionException(
                    "contains non-nibble byte " +
                        std::to_string(nibbleByte));
            }
        }

        return nibbles;
    }

    static std::vector<Nibble> FromByte(const uint8_t& byte)
    {
        std::vector<Nibble> nibbles = {FromNibbleByte(byte >> 4),
                                       FromNibbleByte(byte % 16)};
        return nibbles;
    }

    static std::vector<Nibble> FromBytes(const std::vector<uint8_t>& bytes)
    {
        std::vector<Nibble> nibbles;
        nibbles.reserve(bytes.size() * 2);

        for (const auto &byte: bytes)
        {
            std::vector<Nibble> nibblesFromByte = FromByte(byte);
            nibbles.insert(nibbles.end(),
                           nibblesFromByte.begin(),
                           nibblesFromByte.end());
        }

        return nibbles;
    }

    static std::vector<uint8_t> ToBytes(const std::vector<Nibble>& nibbles)
    {
        std::vector<uint8_t> nibbleBytes;
        nibbleBytes.reserve(nibbles.size() / 2);

        for (size_t i = 0; i < nibbles.size(); i += 2)
        {
            uint8_t nibbleByte = static_cast<uint8_t>(nibbles[i] << 4) +
                static_cast<uint8_t>(nibbles[i + 1]);
            nibbleBytes.push_back(nibbleByte);
        }

        return nibbleBytes;
    }

    static std::vector<Nibble> ToPrefixed(const std::vector<Nibble>& nibbles,
                                          const bool& isLeafNode)
    {
        std::vector<Nibble> prefixBytes;

        if (nibbles.size() % 2 == 1)
        {
            prefixBytes = {1};
        }
        else
        {
            prefixBytes = {0, 0};
        }

        std::vector<Nibble> prefixed;

        // first append the prefix then the Nibbles
        prefixed.reserve(prefixBytes.size() + nibbles.size());
        prefixed.insert(prefixed.end(),
                        prefixBytes.begin(),
                        prefixBytes.end());

        prefixed.insert(prefixed.end(),
                        nibbles.begin(),
                        nibbles.end());

        if (isLeafNode)
        {
            prefixed[0] += 2;
        }

        return prefixed;
    }

    static int PrefixMatchedLen(const std::vector<Nibble>& nibbles1,
                                const std::vector<Nibble>& nibbles2)
    {
        int matchedLen = 0;

        for (size_t i = 0; i < nibbles1.size() && i < nibbles2.size(); i++)
        {
            if (nibbles1[i] == nibbles2[i])
            {
                matchedLen++;
            }
            else
            {
                break;
            }
        }
        return matchedLen;
    }

}; // struct NibbleHelper

} // namespace EclipseMonitor