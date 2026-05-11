add_executable(test_bitcal test_bitcal.cpp)
bitcal_setup_dev_target(test_bitcal)

add_test(NAME test_bitcal COMMAND test_bitcal)
