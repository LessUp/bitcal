#ifndef HYBRIDENGINE_BITCAL_H
#define HYBRIDENGINE_BITCAL_H

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#elif defined(__aarch64__) || defined(_M_ARM64)
#include <arm_neon.h>
#endif
#include "Base.h"
#include "bitcal_dispatch.h"

#include <string>

class BitCal {
private:
  BitCal();

  BitCal(const BitCal& other)                  = delete;

  const BitCal& operator=(const BitCal& other) = delete;

public:
  static BitCal& getInstance() {
    static BitCal instance;
    return instance;
  };

  ~BitCal() = default;

  //============================================================================================================================
  // bit shift left
  //============================================================================================================================
  uint64_t bitShiftLeft64(uint64_t* data, int count);

  bool bitShiftLeft128(uint64_t* data, int count, uint64_t* carryout);

  bool bitShiftLeft256(uint64_t* data, int count, uint64_t* carryout);

  bool bitShiftLeft512(uint64_t* data, int count, uint64_t* carryout);

  //============================================================================================================================
  // bit shift right
  //============================================================================================================================
  uint64_t bitShiftRight64(uint64_t* data, int count);

  bool bitShiftRight128(uint64_t* data, int count, uint64_t* carryout);

  bool bitShiftRight256(uint64_t* data, int count, uint64_t* carryout);

  bool bitShiftRight512(uint64_t* data, int count, uint64_t* carryout);

  //============================================================================================================================
  // bit And
  //============================================================================================================================
  uint64_t bitAnd64(uint64_t data0, uint64_t data1);

  bool bitAnd128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out);

  bool bitAnd256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out);

  bool bitAnd512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out);

  //============================================================================================================================
  // bit Or
  //============================================================================================================================
  uint64_t bitOr64(uint64_t data0, uint64_t data1);

  bool bitOr128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out);

  bool bitOr256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out);

  bool bitOr512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out);

  //============================================================================================================================
  // bit Xor
  //============================================================================================================================
  uint64_t bitXor64(uint64_t data0, uint64_t data1);

  bool bitXor128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out);

  bool bitXor256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out);

  bool bitXor512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out);

  //============================================================================================================================
  // bit pop_cnt
  //============================================================================================================================
  uint64_t bitPopcnt64(uint64_t data);

  uint64_t bitPopcnt128(const uint64_t* data, uint8_t* cnt_arr);

  uint64_t bitPopcnt256(const uint64_t* data, uint8_t* cnt_arr);

  uint64_t bitPopcnt512(const uint64_t* data, uint8_t* cnt_arr);

  //============================================================================================================================
  // bit reverse
  //============================================================================================================================
  uint64_t bitReverse64(uint64_t data);

  bool bitReverse128(uint64_t* dataIn, uint64_t* dataOut);

  bool bitReverse256(uint64_t* dataIn, uint64_t* dataOut);

  bool bitReverse512(uint64_t* dataIn, uint64_t* dataOut);

  bool bitReverseInPlace128(uint64_t* data);

  bool bitReverseInPlace256(uint64_t* data);

  bool bitReverseInPlace512(uint64_t* data);

  //============================================================================================================================
  // bit cal helper
  //============================================================================================================================
  static std::string convUint64ToBinStr(const uint64_t* data, uint32_t size);
};

#endif  // HYBRIDENGINE_BITCAL_H
