cmake_minimum_required(VERSION 3.16)
project(bitcal VERSION 2.0.0 LANGUAGES CXX)

option(BITCAL_BUILD_TESTS "Build tests" ON)
option(BITCAL_BUILD_EXAMPLES "Build examples" ON)

add_library(bitcal INTERFACE)

target_include_directories(bitcal INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_compile_features(bitcal INTERFACE cxx_std_17)

if(BITCAL_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(BITCAL_BUILD_EXAMPLES)
  add_subdirectory(examples)
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
