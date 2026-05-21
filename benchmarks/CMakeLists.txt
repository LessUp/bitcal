cmake_minimum_required(VERSION 3.16)

execute_process(
  COMMAND git rev-parse --short=12 HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE BITCAL_GIT_COMMIT
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)
if(BITCAL_GIT_COMMIT STREQUAL "")
  set(BITCAL_GIT_COMMIT "unknown")
endif()

# Benchmark executable
add_executable(bitcal_benchmark benchmark_bitcal.cpp)

bitcal_setup_dev_target(bitcal_benchmark)

# Comparison benchmark: BitCal vs std::bitset
add_executable(benchmark_compare benchmark_compare.cpp)

bitcal_setup_dev_target(benchmark_compare)

foreach(target bitcal_benchmark benchmark_compare)
  target_compile_definitions(${target} PRIVATE
    BITCAL_GIT_COMMIT="${BITCAL_GIT_COMMIT}"
    BITCAL_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
  )
endforeach()

# Try to link against Google Benchmark if available
find_package(benchmark QUIET)
if(benchmark_FOUND)
    message(STATUS "Google Benchmark found - using for benchmarks")
    target_link_libraries(bitcal_benchmark PRIVATE benchmark::benchmark)
    target_compile_definitions(bitcal_benchmark PRIVATE BITCAL_HAS_GBENCH)
else()
    message(STATUS "Google Benchmark not found - using simple timing")
endif()
