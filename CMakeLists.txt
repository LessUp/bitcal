cmake_minimum_required(VERSION 3.16)

# Practice-scale header-only library: no install/export, no LTO, no hardening,
# no version regex. Version lives in include/bitcal/config.hpp for the code
# and is hardcoded here for CMake's project() metadata only.
project(bitcal VERSION 4.1.0 LANGUAGES CXX)

option(BITCAL_BUILD_TESTS "Build tests" ON)
option(BITCAL_BUILD_EXAMPLES "Build examples" ON)
option(BITCAL_BUILD_BENCHMARKS "Build benchmarks" OFF)
option(BITCAL_NATIVE_ARCH "Use -march=native or equivalent for tests/examples" ON)

include(CheckCXXCompilerFlag)

add_library(bitcal INTERFACE)

target_include_directories(bitcal INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(bitcal INTERFACE cxx_std_23)

# SIMD flags helper: applied to tests/examples so they can exercise SIMD paths.
# The INTERFACE library itself does NOT carry these flags - users pick their own.
set(BITCAL_SIMD_FLAGS "")
if(BITCAL_NATIVE_ARCH)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    set(BITCAL_SIMD_FLAGS -march=native)
  elseif(MSVC)
    # /arch:AVX2 enables AVX/AVX2/SSE* on MSVC
    check_cxx_compiler_flag("/arch:AVX2" BITCAL_MSVC_HAS_AVX2)
    if(BITCAL_MSVC_HAS_AVX2)
      set(BITCAL_SIMD_FLAGS /arch:AVX2)
    endif()
  endif()
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
