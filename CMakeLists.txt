cmake_minimum_required(VERSION 3.16)
project(bitcal LANGUAGES CXX)

add_library(bitcal
  BitCal.cpp
  BitCalGpr.cpp
  BitCalXmm.cpp
  BitCalYmm.cpp
  CheckCpu.cpp
  Encode.cpp
  EncodeGpr.cpp
  EncodeYmm.cpp
  bitcal_dispatch.cpp
  encode_dispatch.cpp
)

target_include_directories(bitcal
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(bitcal PUBLIC cxx_std_17)

if(MSVC)
  target_compile_options(bitcal PRIVATE /W4)
else()
  target_compile_options(bitcal PRIVATE -Wall -Wextra)
endif()
