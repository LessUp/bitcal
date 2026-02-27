add_executable(test_bitcal test_bitcal.cpp)
target_link_libraries(test_bitcal PRIVATE bitcal)
target_compile_features(test_bitcal PRIVATE cxx_std_17)
if(BITCAL_SIMD_FLAGS)
  target_compile_options(test_bitcal PRIVATE ${BITCAL_SIMD_FLAGS})
endif()

add_test(NAME test_bitcal COMMAND test_bitcal)
