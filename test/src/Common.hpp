// Copyright (c) 2024 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once


#define EXPECT_THROW_MSG(statement, exceptionType, expectedMsg) \
{ \
	try \
	{ \
		statement; \
		FAIL() << "Expecting exception of type " #exceptionType; \
	} \
	catch (const exceptionType& e) \
	{ \
		EXPECT_STREQ(e.what(), expectedMsg); \
	} \
	catch (...) \
	{ \
		FAIL() << "Expecting exception of type " #exceptionType; \
	} \
}

