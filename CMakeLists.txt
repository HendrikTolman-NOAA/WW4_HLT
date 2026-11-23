#+ -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- +
#| WAVEWATCH IV, open source, code management by NOAA / NWS |
#+ -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- +
#
#@file CMakeLists.txt
#@brief CMake build configuration for WAVEWATCH IV (WW4).
#@details This file defines the project structure, dependencies (yaml-cpp, NetCDF-C, GTest;
#installed copies preferred, Git submodules as fallback),
#and build targets for the WW4 libraries, standalone application, and tests.
#
#@copyright © 2026 National Weather Service, National Oceanic and Atmospheric
#Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks
#of the National Weather Service.
#NWS often uses Generative AI (GenAI) for code development and refactoring. Whenever GenAI is used, NWS requires a full human review of code before it is added to its repositories.
#@author Main Author(s) : Aldgisl (AI Persona), Hendrik L. Tolman
#@author Contributors : Jules (Agentic AI), Kit Stokes, Jessica Meixner, Kiro (Agentic AI)
#@date Initial : 2026-07-09
#@date Last update : 2026-11-23

cmake_minimum_required(VERSION 3.25)

project(WW4_Utils VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Options for strict warnings, sanitizers, and testing
option(WW4_STRICT_WARNINGS "Enable strict compiler warnings and treat as errors" OFF)
option(WW4_USE_SANITIZERS "Enable address and undefined behavior sanitizers" OFF)
option(WW4_ENABLE_TESTING "Enable WAVEWATCH IV test suite" ON)

# Define common compile options for WW4 targets
set(WW4_COMMON_COMPILE_OPTIONS "")
set(WW4_COMMON_LINK_OPTIONS "")

if(MSVC)
    if(WW4_STRICT_WARNINGS)
        list(APPEND WW4_COMMON_COMPILE_OPTIONS /W4 /WX)
    endif()
else()
    list(APPEND WW4_COMMON_COMPILE_OPTIONS -Wall -Wextra -Wpedantic)
    if(WW4_STRICT_WARNINGS)
        list(APPEND WW4_COMMON_COMPILE_OPTIONS -Werror)
        # Suppress unknown warning option and conversion errors for LLVM-based compilers (Clang, IntelLLVM)
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM")
            list(APPEND WW4_COMMON_COMPILE_OPTIONS -Wno-unknown-warning-option -Wno-character-conversion)
            # Suppress Intel LLVM driver warning about unsupported options (e.g., -Winline)
            if(CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM")
                list(APPEND WW4_COMMON_COMPILE_OPTIONS -diag-disable=10430)
            endif()
        endif()
    endif()
    if(WW4_USE_SANITIZERS)
        list(APPEND WW4_COMMON_COMPILE_OPTIONS -fsanitize=address,undefined)
        list(APPEND WW4_COMMON_LINK_OPTIONS -fsanitize=address,undefined)
    endif()
endif()

# ---------------------------------------------------------------------------
# Third-party dependencies: yaml-cpp, NetCDF-C, GoogleTest
#
# WW4_DEPENDENCY_MODE controls where they come from:
#   AUTO      (default) use installed copies (e.g. from environment modules) when
#             CMake can find them, otherwise build the Git submodule in externals/.
#   SYSTEM    only use installed copies; stop with an error if one is missing.
#   SUBMODULE only use the submodules in externals/ (portable, ignores modules).
#
# With environment modules, make sure the module sets CMAKE_PREFIX_PATH or
# <pkg>_ROOT (e.g. netCDF_ROOT, yaml-cpp_ROOT), or has its bin/ directory on PATH.
# ---------------------------------------------------------------------------
set(WW4_DEPENDENCY_MODE "AUTO" CACHE STRING "Source of third-party libraries: AUTO, SYSTEM or SUBMODULE")
set_property(CACHE WW4_DEPENDENCY_MODE PROPERTY STRINGS AUTO SYSTEM SUBMODULE)
option(WW4_AUTO_INIT_SUBMODULES "Run 'git submodule update --init' for a needed but empty submodule" ON)

if(NOT WW4_DEPENDENCY_MODE MATCHES "^(AUTO|SYSTEM|SUBMODULE)$")
    message(FATAL_ERROR "WW4_DEPENDENCY_MODE must be AUTO, SYSTEM or SUBMODULE (got '${WW4_DEPENDENCY_MODE}')")
endif()

# Ensure externals/<name> is populated, initializing the submodule if allowed.
function(ww4_require_submodule name)
    set(_dir "${CMAKE_SOURCE_DIR}/externals/${name}")
    if(NOT EXISTS "${_dir}/CMakeLists.txt" AND WW4_AUTO_INIT_SUBMODULES AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
        find_package(Git QUIET)
        if(GIT_FOUND)
            message(STATUS "Initializing submodule externals/${name}")
            execute_process(
                COMMAND ${GIT_EXECUTABLE} submodule update --init -- externals/${name}
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                RESULT_VARIABLE _res
                OUTPUT_QUIET ERROR_QUIET
            )
        endif()
    endif()
    if(NOT EXISTS "${_dir}/CMakeLists.txt")
        message(FATAL_ERROR "${name} not found as an installed package and submodule externals/${name} is empty. "
                            "Load the matching module, set CMAKE_PREFIX_PATH, or run: git submodule update --init --recursive")
    endif()
endfunction()

# yaml-cpp for configuration parsing
set(WW4_YAML_TARGET "")
if(NOT WW4_DEPENDENCY_MODE STREQUAL "SUBMODULE")
    find_package(yaml-cpp QUIET CONFIG)
    if(TARGET yaml-cpp::yaml-cpp)
        set(WW4_YAML_TARGET yaml-cpp::yaml-cpp)
    elseif(TARGET yaml-cpp)
        set(WW4_YAML_TARGET yaml-cpp)
    endif()
    if(WW4_YAML_TARGET)
        message(STATUS "Using installed yaml-cpp (${yaml-cpp_DIR})")
    elseif(WW4_DEPENDENCY_MODE STREQUAL "SYSTEM")
        message(FATAL_ERROR "WW4_DEPENDENCY_MODE=SYSTEM but yaml-cpp was not found. Load its module or set CMAKE_PREFIX_PATH.")
    endif()
endif()
if(NOT WW4_YAML_TARGET)
    ww4_require_submodule(yaml-cpp)
    message(STATUS "Using yaml-cpp submodule in externals/yaml-cpp")
    set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
    set(YAML_CPP_INSTALL OFF CACHE BOOL "" FORCE)
    add_subdirectory(externals/yaml-cpp EXCLUDE_FROM_ALL SYSTEM)
    set(WW4_YAML_TARGET yaml-cpp)
endif()

# NetCDF-C for NetCDF / UGRID file reading
set(WW4_NETCDF_TARGET "")
if(NOT WW4_DEPENDENCY_MODE STREQUAL "SUBMODULE")
    find_package(netCDF QUIET CONFIG)
    if(TARGET netCDF::netcdf)
        set(WW4_NETCDF_TARGET netCDF::netcdf)
        message(STATUS "Using installed netCDF (${netCDF_DIR})")
    elseif(WW4_DEPENDENCY_MODE STREQUAL "SYSTEM")
        message(FATAL_ERROR "WW4_DEPENDENCY_MODE=SYSTEM but netCDF was not found. Load its module or set CMAKE_PREFIX_PATH / netCDF_ROOT.")
    endif()
endif()
if(NOT WW4_NETCDF_TARGET)
    ww4_require_submodule(netcdf-c)
    message(STATUS "Using NetCDF-C submodule in externals/netcdf-c")
    find_package(HDF5 QUIET COMPONENTS C)
    find_package(ZLIB QUIET)

    if(HDF5_FOUND AND ZLIB_FOUND)
        message(STATUS "NetCDF-4 dependencies found (HDF5 ${HDF5_VERSION}, ZLIB). Enabling NetCDF-4 support.")
        set(NETCDF_ENABLE_HDF5 ON CACHE BOOL "" FORCE)
        set(NETCDF_ENABLE_NETCDF_4 ON CACHE BOOL "" FORCE)
    else()
        message(STATUS "HDF5/ZLIB not found in local environment. Enabling NetCDF-3 classic support.")
        set(NETCDF_ENABLE_HDF5 OFF CACHE BOOL "" FORCE)
        set(NETCDF_ENABLE_NETCDF_4 OFF CACHE BOOL "" FORCE)
    endif()

    set(NETCDF_ENABLE_DAP OFF CACHE BOOL "" FORCE)
    set(NETCDF_ENABLE_NCZARR OFF CACHE BOOL "" FORCE)
    set(NETCDF_ENABLE_UTILITIES OFF CACHE BOOL "" FORCE)
    set(NETCDF_BUILD_UTILITIES OFF CACHE BOOL "" FORCE)
    set(NETCDF_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
    set(NETCDF_ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    unset(ENABLE_DAP CACHE)
    unset(ENABLE_EXAMPLES CACHE)
    unset(ENABLE_HDF5 CACHE)
    unset(ENABLE_NCZARR CACHE)
    unset(ENABLE_NETCDF_4 CACHE)
    unset(ENABLE_TESTS CACHE)
    add_subdirectory(externals/netcdf-c EXCLUDE_FROM_ALL SYSTEM)

    # Suppress warnings on NetCDF-C targets in production mode
    foreach(nc_target netcdf netcdf3 dispatch)
        if(TARGET ${nc_target})
            if(MSVC)
                target_compile_options(${nc_target} PRIVATE /w)
            else()
                target_compile_options(${nc_target} PRIVATE -w)
            endif()
        endif()
    endforeach()
    set(WW4_NETCDF_TARGET netcdf)
endif()

# Library: ww4_utils
add_library(
    ww4_utils
    src/ww4_utils/time_management.cpp
    src/ww4_utils/memory_utils.cpp
    src/ww4_utils/spatial_grid.cpp
    src/ww4_utils/ww4_std_out.cpp
    src/ww4_utils/ww4_logfile.cpp
    src/ww4_utils/ww4_run_config.cpp
    src/ww4_utils/ww4_input_utils.cpp
    src/ww4_utils/ww4_output_utils.cpp
    src/ww4_utils/ww4_service.cpp
)
target_include_directories(ww4_utils PUBLIC src)
target_link_libraries(ww4_utils PRIVATE ${WW4_YAML_TARGET})
target_compile_options(ww4_utils PRIVATE ${WW4_COMMON_COMPILE_OPTIONS})
target_link_options(ww4_utils PRIVATE ${WW4_COMMON_LINK_OPTIONS})

# Library: ww4_core
add_library(
    ww4_core
    src/ww4_core/w4core_init.cpp
    src/ww4_core/w4core_wave.cpp
    src/ww4_core/w4core_finalize.cpp
    src/ww4_core/scheme_factory.cpp
    src/ww4_core/solver_base.cpp
    src/ww4_core/grid_rectilinear/grid_rectilinear.cpp
    src/ww4_core/grid_rectilinear/grid_rectilinear_init.cpp
    src/ww4_core/grid_rectilinear/solver_1UP/solver_rectilinear_1UP.cpp
    src/ww4_core/grid_rectilinear/solver_UQ/solver_rectilinear_UQ.cpp
    src/ww4_core/grid_rectilinear/rectilinear_domains.cpp
    src/ww4_core/grid_rectilinear/rectilinear_domain_topology.cpp
    src/ww4_core/grid_rectilinear/rectilinear_one_domain.cpp
    src/ww4_core/grid_rectilinear/utils_rectilinear_IO/rectilinear_read_grid.cpp
    src/ww4_core/grid_rectilinear/utils_rectilinear_IO/rectilinear_write_grid.cpp
    src/ww4_core/grid_rectilinear/utils_rectilinear_IO/rectilinear_write_partition.cpp
    src/ww4_core/grid_rectilinear/utils_rectilinear_IO/rectilinear_read_partition.cpp
    src/ww4_core/grid_triangular/grid_triangular.cpp
    src/ww4_core/grid_triangular/grid_triangular_init.cpp
    src/ww4_core/grid_triangular/solver_implicit/solver_triangular_implicit.cpp
    src/ww4_core/grid_triangular/solver_explicit/solver_triangular_explicit.cpp
    src/ww4_core/grid_SMC/grid_SMC.cpp
    src/ww4_core/grid_SMC/grid_SMC_init.cpp
    src/ww4_core/grid_SMC/solver_UKMET/solver_SMC_UKMET.cpp
    src/ww4_core/source_terms/ComputeAllSources/ComputeAllSources.cpp
    src/ww4_core/source_terms/ww4_ST1/ww4_ST1.cpp
    src/ww4_core/source_terms/ww4_ST2/ww4_ST2.cpp
    src/ww4_core/source_terms/ww4_ST4/ww4_ST4.cpp
    src/ww4_core/source_terms/ww4_ST6/ww4_ST6.cpp
    src/ww4_core/source_terms/ww4_LN1/ww4_LN1.cpp
    src/ww4_core/source_terms/ww4_NL1/ww4_NL1.cpp
    src/ww4_core/source_terms/ww4_NL2/ww4_NL2.cpp
    src/ww4_core/source_terms/ww4_NL3/ww4_NL3.cpp
    src/ww4_core/source_terms/ww4_BT1/ww4_BT1.cpp
    src/ww4_core/source_terms/ww4_BT4/ww4_BT4.cpp
    src/ww4_core/wave_model_solver.cpp
)
target_include_directories(ww4_core PUBLIC src)
target_link_libraries(ww4_core PUBLIC ww4_utils PUBLIC ${WW4_NETCDF_TARGET})
target_compile_options(ww4_core PRIVATE ${WW4_COMMON_COMPILE_OPTIONS})
target_link_options(ww4_core PRIVATE ${WW4_COMMON_LINK_OPTIONS})

# Executable: ww4_standalone
add_executable(
    ww4_standalone
    src/ww4_progs/ww4_standalone.cpp
    src/ww4_progs/ww4_standalone_args.cpp
)
target_link_libraries(ww4_standalone PRIVATE ww4_core ww4_utils)
target_compile_options(ww4_standalone PRIVATE ${WW4_COMMON_COMPILE_OPTIONS})
target_link_options(ww4_standalone PRIVATE ${WW4_COMMON_LINK_OPTIONS})

# Executable: ww4_grid_inspect
add_executable(
    ww4_grid_inspect
    src/ww4_progs/ww4_grid_inspect.cpp
)
target_link_libraries(ww4_grid_inspect PRIVATE ww4_core ww4_utils)
target_compile_options(ww4_grid_inspect PRIVATE ${WW4_COMMON_COMPILE_OPTIONS})
target_link_options(ww4_grid_inspect PRIVATE ${WW4_COMMON_LINK_OPTIONS})

# Place the main programs in the exe directory
set_target_properties(
    ww4_standalone
    ww4_grid_inspect
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/exe
)

# Tests (skipped when WW4_ENABLE_TESTING is OFF for operational builds)
if(WW4_ENABLE_TESTING)
    set(WW4_GTEST_FROM_SUBMODULE OFF)
    if(NOT WW4_DEPENDENCY_MODE STREQUAL "SUBMODULE")
        find_package(GTest QUIET)
        if(TARGET GTest::gtest_main)
            message(STATUS "Using installed GoogleTest")
        elseif(WW4_DEPENDENCY_MODE STREQUAL "SYSTEM")
            message(FATAL_ERROR "WW4_DEPENDENCY_MODE=SYSTEM but GoogleTest was not found. Load its module or set CMAKE_PREFIX_PATH.")
        endif()
    endif()
    if(NOT TARGET GTest::gtest_main)
        ww4_require_submodule(googletest)
        message(STATUS "Using googletest submodule in externals/googletest")
        set(WW4_GTEST_FROM_SUBMODULE ON)
        # For Windows: Prevent overriding the parent project's compiler/linker settings
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
        set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
        add_subdirectory(externals/googletest EXCLUDE_FROM_ALL SYSTEM)
    endif()

    # Suppress warnings from GTest itself for Clang/IntelLLVM during compilation
    if(WW4_GTEST_FROM_SUBMODULE AND (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM"))
        set(GTEST_SUPPRESS_FLAGS -Wno-unknown-warning-option -Wno-character-conversion)
        if(CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM")
            list(APPEND GTEST_SUPPRESS_FLAGS -diag-disable=10430)
        endif()
        foreach(target gtest gtest_main gmock gmock_main)
            if(TARGET ${target})
                target_compile_options(${target} INTERFACE ${GTEST_SUPPRESS_FLAGS})
                target_compile_options(${target} PRIVATE ${GTEST_SUPPRESS_FLAGS})
            endif()
        endforeach()
    endif()

    enable_testing()
    include(GoogleTest)

    # Helper function to define tests
    function(ww4_add_test test_name source_file dependency_lib)
        add_executable(${test_name} ${source_file})
        target_link_libraries(${test_name} PRIVATE ${dependency_lib} GTest::gtest_main)
        target_compile_options(${test_name} PRIVATE ${WW4_COMMON_COMPILE_OPTIONS})
        target_link_options(${test_name} PRIVATE ${WW4_COMMON_LINK_OPTIONS})
        gtest_discover_tests(${test_name})
    endfunction()

    # Utils tests
    ww4_add_test(test_time_management tests/ww4_utils/L1_test_time_management.cpp ww4_utils)
    ww4_add_test(test_memory_utils tests/ww4_utils/L1_test_memory_utils.cpp ww4_utils)
    ww4_add_test(test_spatial_grid tests/ww4_utils/L1_test_spatial_grid.cpp ww4_utils)
    ww4_add_test(test_ww4_std_out tests/ww4_utils/L1_test_ww4_std_out.cpp ww4_utils)
    ww4_add_test(test_ww4_logfile tests/ww4_utils/L1_test_ww4_logfile.cpp ww4_utils)
    ww4_add_test(test_ww4_run_config tests/ww4_utils/L1_test_ww4_run_config.cpp ww4_utils)
    ww4_add_test(test_ww4_input_utils tests/ww4_utils/L1_test_ww4_input_utils.cpp ww4_utils)
    ww4_add_test(test_ww4_output_utils tests/ww4_utils/L1_test_ww4_output_utils.cpp ww4_utils)
    ww4_add_test(test_ww4_service tests/ww4_utils/L1_test_ww4_service.cpp ww4_utils)
    ww4_add_test(test_ww4_constants tests/ww4_utils/L1_test_ww4_constants.cpp ww4_utils)

    # Core tests
    ww4_add_test(test_scheme_factory tests/ww4_core/L1_test_scheme_factory.cpp ww4_core)
    ww4_add_test(test_rectilinear_domains tests/ww4_core/L1_test_rectilinear_domains.cpp ww4_core)
    ww4_add_test(test_rectilinear_domain_topology tests/ww4_core/L1_test_rectilinear_domain_topology.cpp ww4_core)
    ww4_add_test(test_rectilinear_one_domain tests/ww4_core/L1_test_rectilinear_one_domain.cpp ww4_core)
    ww4_add_test(test_rectilinear_read_grid tests/ww4_core/L1_test_rectilinear_read_grid.cpp ww4_core)
    ww4_add_test(test_rectilinear_write_grid tests/ww4_core/L1_test_rectilinear_write_grid.cpp ww4_core)
    ww4_add_test(test_rectilinear_write_partition tests/ww4_core/L1_test_rectilinear_write_partition.cpp ww4_core)
    ww4_add_test(test_rectilinear_read_partition tests/ww4_core/L1_test_rectilinear_read_partition.cpp ww4_core)
    ww4_add_test(test_grid_rectilinear tests/ww4_core/L1_test_grid_rectilinear.cpp ww4_core)
    ww4_add_test(test_grid_rectilinear_init tests/ww4_core/L1_test_grid_rectilinear_init.cpp ww4_core)
    ww4_add_test(test_solver_rectilinear_1UP tests/ww4_core/L1_test_solver_rectilinear_1UP.cpp ww4_core)
    ww4_add_test(test_solver_rectilinear_UQ tests/ww4_core/L1_test_solver_rectilinear_UQ.cpp ww4_core)
    ww4_add_test(test_grid_triangular tests/ww4_core/L1_test_grid_triangular.cpp ww4_core)
    ww4_add_test(test_grid_triangular_init tests/ww4_core/L1_test_grid_triangular_init.cpp ww4_core)
    ww4_add_test(test_solver_triangular_implicit tests/ww4_core/L1_test_solver_triangular_implicit.cpp ww4_core)
    ww4_add_test(test_solver_triangular_explicit tests/ww4_core/L1_test_solver_triangular_explicit.cpp ww4_core)
    ww4_add_test(test_grid_SMC tests/ww4_core/L1_test_grid_SMC.cpp ww4_core)
    ww4_add_test(test_grid_SMC_init tests/ww4_core/L1_test_grid_SMC_init.cpp ww4_core)
    ww4_add_test(test_solver_SMC_UKMET tests/ww4_core/L1_test_solver_SMC_UKMET.cpp ww4_core)
    ww4_add_test(test_ww4_standalone_args tests/ww4_core/L1_test_ww4_standalone_args.cpp ww4_utils)
    target_sources(test_ww4_standalone_args PRIVATE src/ww4_progs/ww4_standalone_args.cpp)
    ww4_add_test(test_ww4_standalone tests/ww4_core/L1_test_ww4_standalone.cpp ww4_core)
    target_compile_definitions(test_ww4_standalone PRIVATE STANDALONE_EXE_PATH="${CMAKE_SOURCE_DIR}/exe/ww4_standalone")
    ww4_add_test(test_ww4_grid_inspect tests/ww4_core/L1_test_ww4_grid_inspect.cpp ww4_core)
    target_compile_definitions(test_ww4_grid_inspect PRIVATE GRID_INSPECT_EXE_PATH="${CMAKE_SOURCE_DIR}/exe/ww4_grid_inspect")
    ww4_add_test(test_w4core_init tests/ww4_core/L1_test_w4core_init.cpp ww4_core)
    ww4_add_test(test_w4core_wave tests/ww4_core/L1_test_w4core_wave.cpp ww4_core)
    ww4_add_test(test_w4core_finalize tests/ww4_core/L1_test_w4core_finalize.cpp ww4_core)
    ww4_add_test(test_ComputeAllSources tests/ww4_core/L1_test_ComputeAllSources.cpp ww4_core)
    ww4_add_test(test_ST1 tests/ww4_core/L1_test_ww4_ST1.cpp ww4_core)
    ww4_add_test(test_ST2 tests/ww4_core/L1_test_ww4_ST2.cpp ww4_core)
    ww4_add_test(test_ST4 tests/ww4_core/L1_test_ww4_ST4.cpp ww4_core)
    ww4_add_test(test_ST6 tests/ww4_core/L1_test_ww4_ST6.cpp ww4_core)
    ww4_add_test(test_LN1 tests/ww4_core/L1_test_ww4_LN1.cpp ww4_core)
    ww4_add_test(test_NL1 tests/ww4_core/L1_test_ww4_NL1.cpp ww4_core)
    ww4_add_test(test_NL2 tests/ww4_core/L1_test_ww4_NL2.cpp ww4_core)
    ww4_add_test(test_NL3 tests/ww4_core/L1_test_ww4_NL3.cpp ww4_core)
    ww4_add_test(test_BT1 tests/ww4_core/L1_test_ww4_BT1.cpp ww4_core)
    ww4_add_test(test_BT4 tests/ww4_core/L1_test_ww4_BT4.cpp ww4_core)
    ww4_add_test(test_wave_model_solver tests/ww4_core/L2_test_wave_model_solver.cpp ww4_core)
    ww4_add_test(test_w4core_time tests/ww4_core/L2_test_w4core_time.cpp ww4_core)
    ww4_add_test(test_w4core_wave_output tests/ww4_core/L2_test_w4core_wave_output.cpp ww4_core)
    ww4_add_test(test_w4core_zero_step tests/ww4_core/L2_test_w4core_zero_step.cpp ww4_core)
    ww4_add_test(test_w4core_hom_input tests/ww4_core/L2_test_w4core_hom_input.cpp ww4_core)
    ww4_add_test(test_w4core_input_cycling tests/ww4_core/L2_test_w4core_input_cycling.cpp ww4_core)
    ww4_add_test(test_interpolation_output tests/ww4_core/L2_test_interpolation_output.cpp ww4_core)
    ww4_add_test(test_w4core_screen_output tests/ww4_core/L2_test_w4core_screen_output.cpp ww4_core)
endif()
