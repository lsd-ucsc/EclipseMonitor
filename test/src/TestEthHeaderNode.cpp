// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

#include <EclipseMonitor/EthHeaderNode.hpp>

#include "EthHistHdr_0_100.hpp"

namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor;
using namespace EclipseMonitor_Test;

GTEST_TEST(TestEthHeaderNode, CountTestFile)
{
	static auto tmp = ++g_numOfTestFile;
	(void)tmp;
}

GTEST_TEST(TestEthHeaderNode, AddChildAndFindDesc)
{
	static constexpr size_t testingEnd = 50;

	std::unique_ptr<EthHeaderNode> root =
		SimpleObjects::Internal::make_unique<EthHeaderNode>(
			SimpleObjects::Internal::make_unique<EthHeaderMgr>(
				GetEthHistHdr_0_100()[0], 0)
		);

	// Add children
	EthHeaderNode* currPtr = root.get();
	for (size_t i = 1; i < testingEnd; ++i)
	{
		EXPECT_EQ(currPtr->GetNumOfChildren(), 0);
		auto header = SimpleObjects::Internal::make_unique<EthHeaderMgr>(
			GetEthHistHdr_0_100()[i], 0);
		auto nextPtr = currPtr->AddChild(std::move(header));
		EXPECT_EQ(currPtr->GetNumOfChildren(), 1);
		EXPECT_EQ(nextPtr->GetNumOfChildren(), 0);

		currPtr = nextPtr;
	}

	// Find descendants
	for (size_t i = 0; i < testingEnd - 2; ++i)
	{
		auto child = root->ReleaseChildHasNDesc(testingEnd - i - 1);
		ASSERT_EQ(child, nullptr);

		child = root->ReleaseChildHasNDesc(testingEnd - i - 2);
		ASSERT_NE(child, nullptr);

		auto rootBlkNum = i;
		auto rootExpHeader =
			EthHeaderMgr(GetEthHistHdr_0_100()[rootBlkNum], 0);
		EXPECT_EQ(
			root->GetHeader().GetRawHeader(),
			rootExpHeader.GetRawHeader()
		);
		EXPECT_EQ(
			root->ReleaseHeader()->GetRawHeader(),
			rootExpHeader.GetRawHeader()
		);
		EXPECT_EQ(
			root->ReleaseHeader(),
			nullptr
		);

		auto childBlkNum = i + 1;
		auto childExpHeader =
			EthHeaderMgr(GetEthHistHdr_0_100()[childBlkNum], 0);
		EXPECT_EQ(
			child->GetHeader().GetRawHeader(),
			childExpHeader.GetRawHeader()
		);

		root = std::move(child);
	}
}
