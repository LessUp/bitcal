add_executable(test_bitcal test_bitcal.cpp)
target_link_libraries(test_bitcal PRIVATE bitcal)

add_test(NAME test_bitcal COMMAND test_bitcal)
