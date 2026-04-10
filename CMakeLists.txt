#      +--------------------------------------------------------+
#      | WAVEWATCH IV, open source, code management by NOAA/NWS |
#      +--------------------------------------------------------+
#
# @file CMakeLists.txt
# @brief CMake build configuration for WAVEWATCH IV (WW4) Utilities.
# @details This file defines the project structure, dependencies (GTest),
#          and build targets for the WW4_Utils library and its tests.
#
# @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
#               Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks
#               of the National Weather Service.
# @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
# @author Contributors: Jules (Agentic AI)
# @date Initial, 2026-03-20
# @date Last Update, 2026-04-10

cmake_minimum_required(VERSION 3.20)
project(WW4_Utils VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/exe)

# Include directories
include_directories(include)

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Library
add_library(ww4_utils
    src/ww4_utils/time_management.cpp
    src/ww4_utils/memory_utils.cpp
    src/ww4_utils/ww4_service.cpp
)
target_include_directories(ww4_utils PUBLIC include)

# Strict warnings and sanitizers for local targets
set(WW4_STRICT_FLAGS "")
set(WW4_LINK_FLAGS "")
if(MSVC)
    set(WW4_STRICT_FLAGS /W4 /WX)
else()
    set(WW4_STRICT_FLAGS -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined)
    set(WW4_LINK_FLAGS -fsanitize=address,undefined)
endif()

target_compile_options(ww4_utils PRIVATE ${WW4_STRICT_FLAGS})
target_link_options(ww4_utils PRIVATE ${WW4_LINK_FLAGS})

# Tests
enable_testing()

add_executable(test_time_management tests_L1_L2/ww4_utils/test_time_management.cpp)
target_link_libraries(test_time_management PRIVATE ww4_utils GTest::gtest_main)
target_compile_options(test_time_management PRIVATE ${WW4_STRICT_FLAGS})
target_link_options(test_time_management PRIVATE ${WW4_LINK_FLAGS})

add_executable(test_memory_utils tests_L1_L2/ww4_utils/test_memory_utils.cpp)
target_link_libraries(test_memory_utils PRIVATE ww4_utils GTest::gtest_main)
target_compile_options(test_memory_utils PRIVATE ${WW4_STRICT_FLAGS})
target_link_options(test_memory_utils PRIVATE ${WW4_LINK_FLAGS})

add_executable(test_ww4_service tests_L1_L2/ww4_utils/test_ww4_service.cpp)
target_link_libraries(test_ww4_service PRIVATE ww4_utils GTest::gtest_main)
target_compile_options(test_ww4_service PRIVATE ${WW4_STRICT_FLAGS})
target_link_options(test_ww4_service PRIVATE ${WW4_LINK_FLAGS})

include(GoogleTest)
gtest_discover_tests(test_time_management)
gtest_discover_tests(test_memory_utils)
gtest_discover_tests(test_ww4_service)
