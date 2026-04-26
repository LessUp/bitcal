cmake_minimum_required(VERSION 3.16)

# Canonical version source: include/bitcal/config.hpp
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/include/bitcal/config.hpp" BITCAL_CONFIG_HPP)
foreach(component MAJOR MINOR PATCH)
  string(REGEX MATCH "#define BITCAL_VERSION_${component} ([0-9][0-9]*)" _match "${BITCAL_CONFIG_HPP}")
  if(_match STREQUAL "")
    message(FATAL_ERROR "Failed to read BITCAL_VERSION_${component} from include/bitcal/config.hpp")
  endif()
  set(BITCAL_VERSION_${component} "${CMAKE_MATCH_1}")
endforeach()

project(bitcal VERSION "${BITCAL_VERSION_MAJOR}.${BITCAL_VERSION_MINOR}.${BITCAL_VERSION_PATCH}" LANGUAGES CXX)

option(BITCAL_BUILD_TESTS "Build tests" ON)
option(BITCAL_BUILD_EXAMPLES "Build examples" ON)
option(BITCAL_BUILD_BENCHMARKS "Build benchmarks" OFF)
option(BITCAL_NATIVE_ARCH "Use -march=native or equivalent for tests/examples" ON)
option(BITCAL_ENABLE_LTO "Enable Link Time Optimization" ON)
option(BITCAL_ENABLE_HARDENING "Enable security hardening flags" OFF)

# Compiler checks
include(CheckCXXCompilerFlag)
include(CheckIPOSupported)

# Check for compiler features
check_cxx_compiler_flag("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
check_cxx_compiler_flag("-mtune=native" COMPILER_SUPPORTS_MTUNE_NATIVE)

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

# Link Time Optimization support
if(BITCAL_ENABLE_LTO)
  check_ipo_supported(RESULT ipo_supported OUTPUT ipo_output)
  if(ipo_supported)
    message(STATUS "Link Time Optimization enabled")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
  else()
    message(STATUS "Link Time Optimization not supported: ${ipo_output}")
  endif()
endif()

# Security hardening flags
if(BITCAL_ENABLE_HARDENING)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(bitcal INTERFACE
      -D_FORTIFY_SOURCE=2
      -fstack-protector-strong
      -Wformat
      -Wformat-security
    )
    target_link_options(bitcal INTERFACE
      -Wl,-z,relro
      -Wl,-z,now
      -Wl,-z,noexecstack
    )
  endif()
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
