// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <array>

#include <gtest/gtest.h>

#include <EclipseMonitor/EthDAA.hpp>

using namespace EclipseMonitor;

namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

GTEST_TEST(TestEthDAA, CountTestFile)
{
	static auto tmp = ++EclipseMonitor_Test::g_numOfTestFile;
	(void)tmp;
}

namespace
{

struct TestBlkHeader
{
	using BlkNumType = typename EthBlkNumTypeTrait::value_type;
	using TimeType   = typename EthTimeTypeTrait::value_type;
	using DiffType   = typename EthDiffTypeTrait::value_type;

	BlkNumType m_blkNum;
	TimeType   m_time;
	DiffType   m_diff;
	bool       m_hasUncle;

	TestBlkHeader(
		const BlkNumType& blkNum,
		const TimeType&   time,
		const DiffType    diff,
		bool              hasUncle) :
		m_blkNum(blkNum),
		m_time(time),
		m_diff(diff),
		m_hasUncle(hasUncle)
	{}
}; // struct TestBlkHeader

static void TestDiffCalcResult(
	const TestBlkHeader& parent,
	const TestBlkHeader& curr)
{
	EXPECT_EQ(
		EthMainnetDAA()(
			parent.m_blkNum, parent.m_time, parent.m_diff, parent.m_hasUncle,
			curr.m_blkNum, curr.m_time),
		curr.m_diff);
}

// python script to get history data from GethDBReader:
// def PrintTestChain(num):
// 	d = -2
// 	for i in range(0, 5):
// 		header = db.GetHeaderByNum(num + d + i)
// 		print('{}UL, {}UL, {}ULL, {}'.format(header.Number, header.Time, header.Difficulty, 'false' if header.UncleHash == b'\x1d\xccM\xe8\xde\xc7]z\xab\x85\xb5g\xb6\xcc\xd4\x1a\xd3\x12E\x1b\x94\x8at\x13\xf0\xa1B\xfd@\xd4\x93G' else 'true'))

} // namespace

GTEST_TEST(TestEthDAA, HardforkTest_Homestead)
{
	// Homestead - 1150000

	std::array<TestBlkHeader, 5> testChain = {
		// 1149998
		TestBlkHeader(1149998UL, 1457981335UL, 20503153557831ULL, false),
		// 1149999
		TestBlkHeader(1149999UL, 1457981342UL, 20513164863791ULL, false),
		// 1150000
		TestBlkHeader(1150000UL, 1457981393UL, 20473100089179ULL, false),
		// 1150001
		TestBlkHeader(1150001UL, 1457981402UL, 20483096720593ULL, false),
		// 1150002
		TestBlkHeader(1150002UL, 1457981409UL, 20493098233175ULL, false),
	};

	TestDiffCalcResult(testChain[0], testChain[1]);
	TestDiffCalcResult(testChain[1], testChain[2]);
	TestDiffCalcResult(testChain[2], testChain[3]);
	TestDiffCalcResult(testChain[3], testChain[4]);
}

GTEST_TEST(TestEthDAA, HardforkTest_Byzantium)
{
	// Byzantium - 4370000

	std::array<TestBlkHeader, 5> testChain = {
		// 4369998
		TestBlkHeader(4369998UL, 1508131301UL, 2993613347578811ULL, false),
		// 4369999
		TestBlkHeader(4369999UL, 1508131303UL, 2997274096101735ULL, false),
		// 4370000
		TestBlkHeader(4370000UL, 1508131331UL, 2994347070619309ULL, false),
		// 4370001
		TestBlkHeader(4370001UL, 1508131362UL, 2991422903560207ULL, false),
		// 4370002
		TestBlkHeader(4370002UL, 1508131367UL, 2992883559276884ULL, false),
	};

	TestDiffCalcResult(testChain[0], testChain[1]);
	TestDiffCalcResult(testChain[1], testChain[2]);
	TestDiffCalcResult(testChain[2], testChain[3]);
	TestDiffCalcResult(testChain[3], testChain[4]);
}

GTEST_TEST(TestEthDAA, HardforkTest_Constantinople)
{
	// Byzantium - 7280000

	std::array<TestBlkHeader, 5> testChain = {
		// 7279998
		TestBlkHeader(7279998UL, 1551383460UL, 2961785543514205ULL, false),
		// 7279999
		TestBlkHeader(7279999UL, 1551383501UL, 2958546502099724ULL, false),
		// 7280000
		TestBlkHeader(7280000UL, 1551383524UL, 2957101900364072ULL, false),
		// 7280001
		TestBlkHeader(7280001UL, 1551383544UL, 2955658004000361ULL, false),
		// 7280002
		TestBlkHeader(7280002UL, 1551383549UL, 2957101197433702ULL, false),
	};

	TestDiffCalcResult(testChain[0], testChain[1]);
	TestDiffCalcResult(testChain[1], testChain[2]);
	TestDiffCalcResult(testChain[2], testChain[3]);
	TestDiffCalcResult(testChain[3], testChain[4]);
}

GTEST_TEST(TestEthDAA, HardforkTest_MuirGlacier)
{
	// MuirGlacier - 9200000

	std::array<TestBlkHeader, 5> testChain = {
		// 9199998
		TestBlkHeader(9199998UL, 1577953765UL, 2465257961147622ULL, false),
		// 9199999
		TestBlkHeader(9199999UL, 1577953806UL, 2462196499244987ULL, false),
		// 9200000
		TestBlkHeader(9200000UL, 1577953849UL, 2458589766091800ULL, false),
		// 9200001
		TestBlkHeader(9200001UL, 1577953868UL, 2457389282807577ULL, false),
		// 9200002
		TestBlkHeader(9200002UL, 1577953880UL, 2457389282807578ULL, false),
	};

	TestDiffCalcResult(testChain[0], testChain[1]);
	TestDiffCalcResult(testChain[1], testChain[2]);
	TestDiffCalcResult(testChain[2], testChain[3]);
	TestDiffCalcResult(testChain[3], testChain[4]);
}

GTEST_TEST(TestEthDAA, HardforkTest_London)
{
	// London - 12965000

	std::array<TestBlkHeader, 5> testChain = {
		// 12964998
		TestBlkHeader(12964998UL, 1628166802UL, 7742356048949784ULL, false),
		// 12964999
		TestBlkHeader(12964999UL, 1628166812UL, 7742493487903256ULL, false),
		// 12965000
		TestBlkHeader(12965000UL, 1628166822UL, 7742494561645080ULL, false),
		// 12965001
		TestBlkHeader(12965001UL, 1628166835UL, 7742495635386904ULL, false),
		// 12965002
		TestBlkHeader(12965002UL, 1628166854UL, 7738716193681762ULL, false),
	};

	TestDiffCalcResult(testChain[0], testChain[1]);
	TestDiffCalcResult(testChain[1], testChain[2]);
	TestDiffCalcResult(testChain[2], testChain[3]);
	TestDiffCalcResult(testChain[3], testChain[4]);
}

GTEST_TEST(TestEthDAA, HardforkTest_ArrowGlacier)
{
	// ArrowGlacier - 13773000

	std::array<TestBlkHeader, 5> testChain = {
		// 13772998
		TestBlkHeader(13772998UL, 1639079700UL, 11869544073827424ULL, false),
		// 13772999
		TestBlkHeader(13772999UL, 1639079715UL, 11869818951734368ULL, false),
		// 13773000
		TestBlkHeader(13773000UL, 1639079723UL, 11875615030204850ULL, false),
		// 13773001
		TestBlkHeader(13773001UL, 1639079742UL, 11869816658488839ULL, false),
		// 13773002
		TestBlkHeader(13773002UL, 1639079781UL, 11852429500178464ULL, false),
	};

	TestDiffCalcResult(testChain[0], testChain[1]);
	TestDiffCalcResult(testChain[1], testChain[2]);
	TestDiffCalcResult(testChain[2], testChain[3]);
	TestDiffCalcResult(testChain[3], testChain[4]);
}

GTEST_TEST(TestEthDAA, HardforkTest_GrayGlacier)
{
	// GrayGlacier - 15050000

	std::array<TestBlkHeader, 5> testChain = {
		// 15049998
		TestBlkHeader(15049998UL, 1656586412UL, 14301309434301090ULL, false),
		// 15049999
		TestBlkHeader(15049999UL, 1656586434UL, 14296525396309425ULL, true),
		// 15050000
		TestBlkHeader(15050000UL, 1656586444UL, 14303523301469775ULL, false),
		// 15050001
		TestBlkHeader(15050001UL, 1656586446UL, 14310524623576004ULL, false),
		// 15050002
		TestBlkHeader(15050002UL, 1656586459UL, 14310541803445188ULL, false),
	};

	TestDiffCalcResult(testChain[0], testChain[1]);
	TestDiffCalcResult(testChain[1], testChain[2]);
	TestDiffCalcResult(testChain[2], testChain[3]);
	TestDiffCalcResult(testChain[3], testChain[4]);
}
