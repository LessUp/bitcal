cmake_minimum_required(VERSION 3.16)

# Benchmark executable
add_executable(bitcal_benchmark benchmark_bitcal.cpp)

target_include_directories(bitcal_benchmark PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(bitcal_benchmark PRIVATE cxx_std_17)

# Add SIMD flags for benchmarks
if(BITCAL_NATIVE_ARCH AND BITCAL_SIMD_FLAGS)
    target_compile_options(bitcal_benchmark PRIVATE ${BITCAL_SIMD_FLAGS})
endif()

# Try to link against Google Benchmark if available
find_package(benchmark QUIET)
if(benchmark_FOUND)
    message(STATUS "Google Benchmark found - using for benchmarks")
    target_link_libraries(bitcal_benchmark PRIVATE benchmark::benchmark)
    target_compile_definitions(bitcal_benchmark PRIVATE BITCAL_HAS_GBENCH)
else()
    message(STATUS "Google Benchmark not found - using simple timing")
endif()
