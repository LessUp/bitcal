cmake_minimum_required(VERSION 3.16)
project(bitcal VERSION 2.1.0 LANGUAGES CXX)

option(BITCAL_BUILD_TESTS "Build tests" ON)
option(BITCAL_BUILD_EXAMPLES "Build examples" ON)
option(BITCAL_BUILD_BENCHMARKS "Build benchmarks" OFF)
option(BITCAL_NATIVE_ARCH "Use -march=native or equivalent for tests/examples" ON)

add_library(bitcal INTERFACE)

target_include_directories(bitcal INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_compile_features(bitcal INTERFACE cxx_std_17)

# SIMD flags helper: applied to tests/examples so they can exercise SIMD paths.
# The INTERFACE library itself does NOT carry these flags — users pick their own.
set(BITCAL_SIMD_FLAGS "")
if(BITCAL_NATIVE_ARCH)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    set(BITCAL_SIMD_FLAGS -march=native)
  elseif(MSVC)
    # /arch:AVX2 enables AVX/AVX2/SSE* on MSVC
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("/arch:AVX2" BITCAL_MSVC_HAS_AVX2)
    if(BITCAL_MSVC_HAS_AVX2)
      set(BITCAL_SIMD_FLAGS /arch:AVX2)
    endif()
  endif()
endif()

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

include(GNUInstallDirs)

install(TARGETS bitcal
  EXPORT bitcal-targets
)

install(DIRECTORY include/bitcal
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT bitcal-targets
  FILE bitcal-targets.cmake
  NAMESPACE bitcal::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bitcal
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/bitcal-config-version.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/bitcal-config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/bitcal-config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bitcal
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/bitcal-config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/bitcal-config-version.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bitcal
)
