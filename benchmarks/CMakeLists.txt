add_executable(bitcal_benchmark benchmark_bitcal.cpp)
bitcal_setup_dev_target(bitcal_benchmark)

# Comparison benchmark: BitCal vs std::bitset
add_executable(benchmark_compare benchmark_compare.cpp)
bitcal_setup_dev_target(benchmark_compare)
