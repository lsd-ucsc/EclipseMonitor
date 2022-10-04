// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/Trie/Nibbles.hpp>

namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor_Test;

using namespace EclipseMonitor;
using namespace EclipseMonitor::Trie;

GTEST_TEST(TestEthTrieNibble, CountTestFile)
{
	static auto tmp = ++g_numOfTestFile;
	(void)tmp;
}

GTEST_TEST(TestEthTrieNibble, IsNibble)
{
	for(uint i = 0; i < 20; i++)
	{
		bool isNibbleExpected = (i <= 15);
		EXPECT_EQ(isNibbleExpected, Trie::NibbleHelper::IsNibble(i));
	}
}

GTEST_TEST(TestEthTrieNibble, FromNibbleByte)
{
	for(uint i = 0; i < 20; i++)
	{
		bool isNibbleExpected = (i <= 15);
		if(isNibbleExpected)
		{
			EXPECT_EQ(i, NibbleHelper::FromNibbleByte(i));
		}
		else
		{
			EXPECT_THROW(
				NibbleHelper::FromNibbleByte(i),
				NibblesConversionException
			);
		}
	}
}

GTEST_TEST(TestEthTrieNibble, FromNibbleBytes)
{
	{
		std::vector<uint8_t> nibbleBytes = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		std::vector<Trie::Nibble> nibbles =
			Trie::NibbleHelper::FromNibbleBytes(nibbleBytes);
		EXPECT_EQ(nibbleBytes.size(), nibbles.size());
		EXPECT_EQ(nibbleBytes, nibbles);
	}
	{
		std::vector<uint8_t> nibbleBytes = {0, 1, 2, 3, 4, 5, 16, 17, 18, 19};
		EXPECT_THROW(
			NibbleHelper::FromNibbleBytes(nibbleBytes),
			NibblesConversionException
		);
	}
}

GTEST_TEST(TestEthTrieNibble, FromByte)
{
	EXPECT_EQ(std::vector<Nibble>({0, 0}), NibbleHelper::FromByte(0));
	EXPECT_EQ(std::vector<Nibble>({0, 10}), NibbleHelper::FromByte(10));
	EXPECT_EQ(std::vector<Nibble>({1, 0}), NibbleHelper::FromByte(16));
	EXPECT_EQ(std::vector<Nibble>({15, 15}), NibbleHelper::FromByte(255));
}

GTEST_TEST(TestEthTrieNibble, FromBytes)
{
	{
		std::vector<Nibble> nibbles = {0, 0, 0, 1};
		std::vector<uint8_t> bytes = {0, 1};
		EXPECT_EQ(nibbles, NibbleHelper::FromBytes(bytes));
	}
	{
		std::vector<Nibble> nibbles = {0, 0, 0, 1, 0, 2, 0, 3, 0, 4};
		std::vector<uint8_t> bytes = {0, 1, 2, 3, 4};
		EXPECT_EQ(nibbles, NibbleHelper::FromBytes(bytes));
	}
	{
		std::vector<Nibble> nibbles = {0, 1, 0, 10, 1, 0, 15, 15};
		std::vector<uint8_t> bytes = {1, 10, 16, 255};
		EXPECT_EQ(nibbles, NibbleHelper::FromBytes(bytes));
	}
	{
		std::vector<Nibble> nibbles = {15, 15, 15, 14, 15, 13, 15, 12, 15, 11};
		std::vector<uint8_t> bytes = {255, 254, 253, 252, 251};
		EXPECT_EQ(nibbles, NibbleHelper::FromBytes(bytes));
	}
}

GTEST_TEST(TestEthTrieNibble, ToBytes)
{
	{
		std::vector<uint8_t> bytes = {0};
		std::vector<Nibble> nibbles = NibbleHelper::FromBytes(bytes);
		EXPECT_EQ(bytes, NibbleHelper::ToBytes(nibbles));
	}
	{
		std::vector<uint8_t> bytes = {0, 1};
		std::vector<Nibble> nibbles = NibbleHelper::FromBytes(bytes);
		EXPECT_EQ(bytes, NibbleHelper::ToBytes(nibbles));
	}
	{
		std::vector<uint8_t> bytes = {255, 254, 253, 252, 251};
		std::vector<Nibble> nibbles = NibbleHelper::FromBytes(bytes);
		EXPECT_EQ(bytes, NibbleHelper::ToBytes(nibbles));
	}
}

GTEST_TEST(TestEthTrieNibble, ToPrefixed)
{
	{
		std::vector<Nibble> nibbles = {1};
		bool isLeafNode = false;
		std::vector<Nibble> expected = {1, 1};
		EXPECT_EQ(expected, NibbleHelper::ToPrefixed(nibbles, isLeafNode));
	}
	{
		std::vector<Nibble> nibbles = {1};
		bool isLeafNode = true;
		std::vector<Nibble> expected = {3, 1};
		EXPECT_EQ(expected, NibbleHelper::ToPrefixed(nibbles, isLeafNode));
	}
	{
		std::vector<Nibble> nibbles = {7};
		bool isLeafNode = true;
		std::vector<Nibble> expected = {3, 7};
		EXPECT_EQ(expected, NibbleHelper::ToPrefixed(nibbles, isLeafNode));
	}
	{
		std::vector<Nibble> nibbles = {1, 2};
		bool isLeafNode = false;
		std::vector<Nibble> expected = {0, 0, 1, 2};
		EXPECT_EQ(expected, NibbleHelper::ToPrefixed(nibbles, isLeafNode));
	}
	{
		std::vector<Nibble> nibbles = {1, 2};
		bool isLeafNode = true;
		std::vector<Nibble> expected = {2, 0, 1, 2};
		EXPECT_EQ(expected, NibbleHelper::ToPrefixed(nibbles, isLeafNode));
	}
	{
		std::vector<Nibble> nibbles = {5, 0, 6};
		bool isLeafNode = true;
		std::vector<Nibble> expected = {3, 5, 0, 6};
		EXPECT_EQ(expected, NibbleHelper::ToPrefixed(nibbles, isLeafNode));
	}
	{
		std::vector<Nibble> nibbles = {14, 3};
		bool isLeafNode = false;
		std::vector<Nibble> expected = {0, 0, 14, 3};
		EXPECT_EQ(expected, NibbleHelper::ToPrefixed(nibbles, isLeafNode));
	}
	{
		std::vector<Nibble> nibbles = {9, 3, 6, 5};
		bool isLeafNode = true;
		std::vector<Nibble> expected = {2, 0, 9, 3, 6, 5};
		EXPECT_EQ(expected, NibbleHelper::ToPrefixed(nibbles, isLeafNode));
	}
	{
		std::vector<Nibble> nibbles = {1, 3, 3, 5};
		bool isLeafNode = true;
		std::vector<Nibble> expected = {2, 0, 1, 3, 3, 5};
		EXPECT_EQ(expected, NibbleHelper::ToPrefixed(nibbles, isLeafNode));
	}
}

GTEST_TEST(TestEthTrieNibble, PrefixMatchedLen)
{
	{
		std::vector<Nibble> nibbles1 = {1, 2, 3, 4};
		std::vector<Nibble> nibbles2 = {1, 2, 3};
		EXPECT_EQ(3, NibbleHelper::PrefixMatchedLen(nibbles1, nibbles2));
	}
	{
		std::vector<Nibble> nibbles1 = {1, 2, 3, 4};
		std::vector<Nibble> nibbles2 = {1, 2, 3, 4};
		EXPECT_EQ(4, NibbleHelper::PrefixMatchedLen(nibbles1, nibbles2));
	}
	{
		std::vector<Nibble> nibbles1 = {1, 2, 3, 4};
		std::vector<Nibble> nibbles2 = {1, 2, 3, 4, 5, 6, 7, 8};
		EXPECT_EQ(4, NibbleHelper::PrefixMatchedLen(nibbles1, nibbles2));
	}
	{
		std::vector<Nibble> nibbles1 = {1, 2, 3, 4};
		std::vector<Nibble> nibbles2 = {5, 6, 7, 8};
		EXPECT_EQ(0, NibbleHelper::PrefixMatchedLen(nibbles1, nibbles2));
	}
}
