add_executable(test_bitcal test_bitcal.cpp)
target_link_libraries(test_bitcal PRIVATE bitcal)
target_compile_features(test_bitcal PRIVATE cxx_std_20)

add_test(NAME test_bitcal COMMAND test_bitcal)
