#
#  CMakeLists.txt
#  NAND
#
#  Created by Claude Opus 5 on 2026-08-29.
#  Copyright © 2026 Gabriel Beauchemin. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)

project(NAND
	VERSION 0.1.0
	DESCRIPTION "A computer built up from a single NAND primitive"
	LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()


# ---------------------------------------------------------------------------
# The gates themselves: header-only templates.
# ---------------------------------------------------------------------------

add_library(nand_core INTERFACE)
target_include_directories(nand_core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(nand_core INTERFACE cxx_std_17)


# ---------------------------------------------------------------------------
# googletest, from the submodule. mos6502/ and nand2tetris/ are reference
# material and are deliberately not part of this build.
# ---------------------------------------------------------------------------

if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/gtest/CMakeLists.txt)
	message(FATAL_ERROR
		"The gtest submodule is empty. Run:\n"
		"    git submodule update --init gtest")
endif()

set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(gtest EXCLUDE_FROM_ALL)


# ---------------------------------------------------------------------------
# Test suite. main.cpp supplies its own main(), so link gmock, not gmock_main.
# ---------------------------------------------------------------------------

add_executable(nand_tests
	main.cpp
	circuit.cpp
	nand.cpp
	not.cpp
	and.cpp
	or.cpp
	nor.cpp
	xor.cpp
	xnor.cpp
	dgate.cpp
	enabler.cpp
	register.cpp
	decoder.cpp
	adder.cpp
	alu.cpp
	control.cpp
	comparator.cpp
	cpu.cpp
	ram.cpp
	shift_register.cpp
	stepper.cpp)

target_link_libraries(nand_tests PRIVATE nand_core GTest::gmock)

if (MSVC)
	target_compile_options(nand_tests PRIVATE /W3)
else()
	target_compile_options(nand_tests PRIVATE -Wall)
endif()

enable_testing()

include(GoogleTest)
gtest_discover_tests(nand_tests)
