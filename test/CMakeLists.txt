# Copyright (c) 2022 Haofan Zheng
# Use of this source code is governed by an MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT.

cmake_minimum_required(VERSION 3.14)

OPTION(ECLIPSEMONITOR_TEST_CXX_STANDARD
	"C++ standard version used to build EclipseMonitor test executable." 11)
OPTION(ECLIPSEMONITOR_TEST_LCOV "Option to turn on test code coverage." OFF)

include(TestCoverage.cmake)

################################################################################
# Set compile options
################################################################################

if(MSVC)
	set(COMMON_OPTIONS /W4 /WX /EHsc /MP /GR /Zc:__cplusplus)
	set(DEBUG_OPTIONS /MTd /Od /Zi /DDEBUG)
	set(RELEASE_OPTIONS /MT /Ox /Oi /Ob2 /fp:fast)# /DNDEBUG
else()
	set(COMMON_OPTIONS -pthread -Wall -Wextra -Werror
		-pedantic -Wpedantic -pedantic-errors)
	set(DEBUG_OPTIONS -O0 -g -DDEBUG)
	set(RELEASE_OPTIONS -O2) #-DNDEBUG defined by default
endif()

set(DEBUG_OPTIONS ${COMMON_OPTIONS} ${DEBUG_OPTIONS})
set(RELEASE_OPTIONS ${COMMON_OPTIONS} ${RELEASE_OPTIONS})

if(MSVC)
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")
endif()

################################################################################
# Fetching dependencise
################################################################################

include(FetchContent)

FetchContent_Declare(
	git_googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(git_googletest)

FetchContent_Declare(
	git_simpleutf
	GIT_REPOSITORY https://github.com/zhenghaven/SimpleUtf.git
	GIT_TAG        main
)
FetchContent_MakeAvailable(git_simpleutf)

FetchContent_Declare(
	git_simpleobjects
	GIT_REPOSITORY https://github.com/zhenghaven/SimpleObjects.git
	GIT_TAG        main
)
FetchContent_MakeAvailable(git_simpleobjects)

FetchContent_Declare(
	git_simplerlp
	GIT_REPOSITORY https://github.com/zhenghaven/SimpleRlp.git
	GIT_TAG        dev
)
FetchContent_MakeAvailable(git_simplerlp)

################################################################################
# Adding testing executable
################################################################################

set(SOURCES_DIR_PATH ${CMAKE_CURRENT_LIST_DIR}/src)

file(GLOB_RECURSE SOURCES ${SOURCES_DIR_PATH}/*.[ch]*)

add_executable(EclipseMonitor_test ${SOURCES})

target_compile_options(EclipseMonitor_test
	PRIVATE $<$<CONFIG:>:${DEBUG_OPTIONS}>
			$<$<CONFIG:Debug>:${DEBUG_OPTIONS}>
			$<$<CONFIG:Release>:${RELEASE_OPTIONS}>)
target_link_libraries(EclipseMonitor_test
	EclipseMonitor
	SimpleRlp
	SimpleObjects
	SimpleUtf
	gtest)

add_test(NAME EclipseMonitor_test
	COMMAND EclipseMonitor_test)

set_property(TARGET EclipseMonitor_test
	PROPERTY CXX_STANDARD ${ECLIPSEMONITOR_TEST_CXX_STANDARD})

if (ECLIPSEMONITOR_TEST_LCOV)
	message(STATUS "Setting up test coverage target...")
	setup_cxx_test_coverage(EclipseMonitor_test "${PROJECT_SOURCE_DIR}/include")
endif()
