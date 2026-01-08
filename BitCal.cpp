
#include "BitCal.h"

#include "src/Utils/CheckCpu.h"
#include "spdlog/spdlog.h"

#include <algorithm>
#include <cstdint>
#include <string>

BitCal::BitCal() {
#ifdef __x86_64__
  // NOTE: AVX2 is a superset of AVX, so check AVX2 first!
  // CPUs supporting AVX2 also support AVX, checking AVX first would never use AVX2.
  if(CheckCpu::getInstance().avx2Available()) {
    m_bitcal = new BitCalYmm();
  } else if(CheckCpu::getInstance().avxAvailable()) {
    m_bitcal = new BitCalXmm();
  } else {
    m_bitcal = new BitCalGpr();
  }
#else
  m_bitcal = new BitCalGpr();
#endif
}

BitCal::~BitCal() {
  if(m_bitcal != nullptr) {
    delete m_bitcal;
    m_bitcal = nullptr;
  }
};

//============================================================================================================================
// bit shift left
//============================================================================================================================
uint64_t BitCal::bitShiftLeft64(uint64_t* data, int count) {
  return m_bitcal->bitShiftLeft64(data, count);
}

bool BitCal::bitShiftLeft128(uint64_t* data, int count, uint64_t* carryout) {
  return m_bitcal->bitShiftLeft128(data, count, carryout);
}

bool BitCal::bitShiftLeft256(uint64_t* data, int count, uint64_t* carryout) {
  return m_bitcal->bitShiftLeft256(data, count, carryout);
}

bool BitCal::bitShiftLeft512(uint64_t* data, int count, uint64_t* carryout) {
  return m_bitcal->bitShiftLeft512(data, count, carryout);
}

//============================================================================================================================
// bit shift right
//============================================================================================================================
uint64_t BitCal::bitShiftRight64(uint64_t* data, int count) {
  return m_bitcal->bitShiftRight64(data, count);
}

bool BitCal::bitShiftRight128(uint64_t* data, int count, uint64_t* carryout) {
  return m_bitcal->bitShiftRight128(data, count, carryout);
}

bool BitCal::bitShiftRight256(uint64_t* data, int count, uint64_t* carryout) {
  return m_bitcal->bitShiftRight256(data, count, carryout);
}

bool BitCal::bitShiftRight512(uint64_t* data, int count, uint64_t* carryout) {
  return m_bitcal->bitShiftRight512(data, count, carryout);
}

//============================================================================================================================
// bit And
//============================================================================================================================
uint64_t BitCal::bitAnd64(const uint64_t data0, const uint64_t data1) {
  return m_bitcal->bitAnd64(data0, data1);
}

bool BitCal::bitAnd128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
  return m_bitcal->bitAnd128(data0, data1, data_out);
}

bool BitCal::bitAnd256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {

  return m_bitcal->bitAnd256(data0, data1, data_out);
}

bool BitCal::bitAnd512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
  return m_bitcal->bitAnd512(data0, data1, data_out);
}

//============================================================================================================================
// bit Or
//============================================================================================================================
uint64_t BitCal::bitOr64(uint64_t data0, uint64_t data1) {
  return m_bitcal->bitOr64(data0, data1);
}

bool BitCal::bitOr128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
  return m_bitcal->bitOr128(data0, data1, data_out);
}

bool BitCal::bitOr256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
  return m_bitcal->bitOr256(data0, data1, data_out);
}

bool BitCal::bitOr512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
  return m_bitcal->bitOr512(data0, data1, data_out);
}

//============================================================================================================================
// bit Xor
//============================================================================================================================
uint64_t BitCal::bitXor64(uint64_t data0, uint64_t data1) {
  return m_bitcal->bitXor64(data0, data1);
}

bool BitCal::bitXor128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
  return m_bitcal->bitXor128(data0, data1, data_out);
}

bool BitCal::bitXor256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {

  return m_bitcal->bitXor256(data0, data1, data_out);
}

bool BitCal::bitXor512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
  return m_bitcal->bitXor512(data0, data1, data_out);
}

//============================================================================================================================
// bit pop_cnt
//============================================================================================================================
uint64_t BitCal::bitPopcnt64(const uint64_t data) {
  return m_bitcal->bitPopcnt64(data);
}

uint64_t BitCal::bitPopcnt128(const uint64_t* data, uint8_t* cnt_arr) {
  return m_bitcal->bitPopcnt128(data, cnt_arr);
}

uint64_t BitCal::bitPopcnt256(const uint64_t* data, uint8_t* cnt_arr) {

  return m_bitcal->bitPopcnt256(data, cnt_arr);
}

uint64_t BitCal::bitPopcnt512(const uint64_t* data, uint8_t* cnt_arr) {

  return m_bitcal->bitPopcnt512(data, cnt_arr);
}

//============================================================================================================================
// bit reverse
//============================================================================================================================
uint64_t BitCal::bitReverse64(uint64_t data) {
  return m_bitcal->bitReverse64(data);
}

bool BitCal::bitReverse128(uint64_t* dataIn, uint64_t* dataOut) {
  return m_bitcal->bitReverse128(dataIn, dataOut);
}

bool BitCal::bitReverse256(uint64_t* dataIn, uint64_t* dataOut) {
  return m_bitcal->bitReverse256(dataIn, dataOut);
}

bool BitCal::bitReverse512(uint64_t* dataIn, uint64_t* dataOut) {
  return m_bitcal->bitReverse512(dataIn, dataOut);
}

bool BitCal::bitReverseInPlace128(uint64_t* data) {
  return m_bitcal->bitReverseInPlace128(data);
}

bool BitCal::bitReverseInPlace256(uint64_t* data) {
  return m_bitcal->bitReverseInPlace256(data);
}

bool BitCal::bitReverseInPlace512(uint64_t* data) {
  return m_bitcal->bitReverseInPlace512(data);
}

//============================================================================================================================
// bit cal helper
//============================================================================================================================
std::string BitCal::convUint64ToBinStr(const uint64_t* data, uint32_t size) {
  if(!data) {
    spdlog::error("BitCal::{} input data is illegal!", __FUNCTION__);
    return {};
  }

  std::string s;
  for(uint32_t i = 0; i < size; ++i) {
    uint64_t tmp = data[i];
    for(int j = 0; j < 64; ++j) {
      if(j % 8 == 0 && j != 0) s.append(" ");
      s.append(std::to_string((tmp >> (63 - j)) & 1));
    }
    s.append(" ");
  }
  return s;
}