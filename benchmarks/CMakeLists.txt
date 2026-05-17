cmake_minimum_required(VERSION 3.16)

# Benchmark executable
add_executable(bitcal_benchmark benchmark_bitcal.cpp)

bitcal_setup_dev_target(bitcal_benchmark)

# Comparison benchmark: BitCal vs std::bitset
add_executable(benchmark_compare benchmark_compare.cpp)

bitcal_setup_dev_target(benchmark_compare)

# Try to link against Google Benchmark if available
find_package(benchmark QUIET)
if(benchmark_FOUND)
    message(STATUS "Google Benchmark found - using for benchmarks")
    target_link_libraries(bitcal_benchmark PRIVATE benchmark::benchmark)
    target_compile_definitions(bitcal_benchmark PRIVATE BITCAL_HAS_GBENCH)
else()
    message(STATUS "Google Benchmark not found - using simple timing")
endif()
