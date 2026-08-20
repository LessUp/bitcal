cmake_minimum_required(VERSION 3.16)

# Practice-scale header-only library: no install/export, no LTO, no hardening,
# no version regex. Version lives in include/bitcal/config.hpp for the code
# and is hardcoded here for CMake's project() metadata only.
project(bitcal VERSION 4.2.0 LANGUAGES CXX)

# Export compile_commands.json by default so clangd's `CompilationDatabase: build`
# works out of the box (harmless for an INTERFACE-only library).
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dev targets default ON only when BitCal is the top-level project, so
# source-integration consumers (add_subdirectory / FetchContent) don't
# inherit test/example targets they didn't ask for.
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(BITCAL_IS_TOP_LEVEL ON)
else()
  set(BITCAL_IS_TOP_LEVEL OFF)
endif()

option(BITCAL_BUILD_TESTS "Build tests" ${BITCAL_IS_TOP_LEVEL})
option(BITCAL_BUILD_EXAMPLES "Build examples" ${BITCAL_IS_TOP_LEVEL})
option(BITCAL_BUILD_BENCHMARKS "Build benchmarks" OFF)
option(BITCAL_NATIVE_ARCH "Use -march=native or equivalent for tests/examples" ON)

add_library(bitcal INTERFACE)

target_include_directories(bitcal INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(bitcal INTERFACE cxx_std_23)

# SIMD flags helper: applied to tests/examples so they can exercise SIMD paths.
# The INTERFACE library itself does NOT carry these flags - users pick their own.
set(BITCAL_SIMD_FLAGS "")
if(BITCAL_NATIVE_ARCH)
  set(BITCAL_SIMD_FLAGS -march=native)
endif()

# Helper function for dev targets (tests, examples, benchmarks).
# Applies consistent C++23 requirement, bitcal linking, and optional SIMD flags.
function(bitcal_setup_dev_target target_name)
  target_link_libraries(${target_name} PRIVATE bitcal)
  target_compile_features(${target_name} PRIVATE cxx_std_23)
  if(BITCAL_NATIVE_ARCH AND BITCAL_SIMD_FLAGS)
    target_compile_options(${target_name} PRIVATE ${BITCAL_SIMD_FLAGS})
  endif()
endfunction()

if(BITCAL_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(BITCAL_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(BITCAL_BUILD_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()
