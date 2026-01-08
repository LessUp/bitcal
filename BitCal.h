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
  // bit pop_cnt a 64-bit value using general purpose registers
  //          uint64_t *data -    data to popcnt
  // return:  uint64_t       -    allBitCount amount number
  uint64_t bitPopcnt64(uint64_t data);

  // bit pop_cnt a 128-bit value using general purpose registers
  //          uint64_t *data -   data to popcnt
  //          uint64_t *cnt_arr -len = 2, cnt_arr number from font to end is the bitcnt of uint64 data form font to end
  // return:  uint64_t       -   set 1 bit count
  uint64_t bitPopcnt128(const uint64_t* data, uint8_t* cnt_arr);

  // bit pop_cnt a 256-bit value using general purpose registers
  //          uint64_t *data -   data to popcnt
  //          uint64_t *cnt_arr -len = 4, cnt_arr number from font to end is the bitcnt of uint64 data form font to end
  // return:  uint64_t       -   set 1 bit count
  uint64_t bitPopcnt256(const uint64_t* data, uint8_t* cnt_arr);

  // bit pop_cnt a 512-bit value using general purpose registers
  //          uint64_t *data -   data to popcnt
  //          uint64_t *cnt_arr -len = 8, cnt_arr number from font to end is the bitcnt of uint64 data form font to end
  // return:  uint64_t       -   set 1 bit count
  uint64_t bitPopcnt512(const uint64_t* data, uint8_t* cnt_arr);

  //============================================================================================================================
  // bit reverse
  //============================================================================================================================
  // bit reverse a 64-bit value using general purpose registers
  //          uint64_t 	dataIn	    data to reverse
  // 			uint64_t    dataOut    -data reversed
  uint64_t bitReverse64(uint64_t data);

  // bit reverse a 128-bit value using general purpose registers
  //          uint64_t*	dataIn	    data to reverse
  //          uint64_t*   dataOut    -data reversed
  bool bitReverse128(uint64_t* dataIn, uint64_t* dataOut);

  // bit reverse a 256-bit value using general purpose registers
  //          uint64_t *data - data to reverse
  //          uint64_t *data - data to reverse
  bool bitReverse256(uint64_t* dataIn, uint64_t* dataOut);

  // bit reverse a 512-bit value using general purpose registers
  //          uint64_t*	dataIn	    data to reverse
  //          uint64_t*   dataOut    -data reversed
  bool bitReverse512(uint64_t* dataIn, uint64_t* dataOut);

  // bit reverse inplace a 128-bit value using general purpose registers
  //          uint64_t    *data -     data to reverse and reversed
  bool bitReverseInPlace128(uint64_t* data);

  // bit reverse  inplace a 256-bit value using general purpose registers
  //          uint64_t    *data -     data to reverse and reversed
  bool bitReverseInPlace256(uint64_t* data);

  // bit reverse  inplace a 512-bit value using general purpose registers
  //          uint64_t*	data	    data to reverse and reversed
  bool bitReverseInPlace512(uint64_t* data);

  //============================================================================================================================
  // bit cal helper
  //============================================================================================================================
  // convUint64ToBinStr
  //          uint64_t *data - data to reverse
  //          int count      - number of bits to or
  // return:  uint64_t       - carry out bit(s)
  static std::string convUint64ToBinStr(const uint64_t* data, uint32_t size);
};

#endif  // HYBRIDENGINE_BITCAL_H
