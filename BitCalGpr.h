#ifndef HYBRIDENGINE_BITCALGPR_H
#define HYBRIDENGINE_BITCALGPR_H

#ifdef __x86_64__
#include <immintrin.h>
#elif __aarch64__
#include <arm_neon.h>
#endif
#include "Base.h"

#include <string>

class BitCalGpr : public BitCalBase {
public:
  BitCalGpr()  = default;

  ~BitCalGpr() = default;

  //============================================================================================================================
  // bit shift left
  //============================================================================================================================
  // bit shift left a 64-bit value using general purpose registers
  //          uint64_t *data - data to shift
  //          int count      - number of bits to shift
  // return:  uint64_t       - carry out bit(s)
  uint64_t bitShiftLeft64(uint64_t* data, int count) override;

  // bit shift left a 128-bit value using general purpose registers
  //          uint64_t *data - data to shift, ls part stored first
  //          int count      - number of bits to shift
  // return:  uint64_t       - carry out bit(s)
  bool bitShiftLeft128(uint64_t* data, int count, uint64_t* carryout) override;

  // bit shift left a 256-bit value using general purpose registers
  //          uint64_t *data - data to shift, ls part stored first
  //          int count      - number of bits to shift
  // return:  uint64_t       - carry out bit(s)
  bool bitShiftLeft256(uint64_t* data, int count, uint64_t* carryout) override;

  // bit shift left a 512-bit value using general purpose registers
  //          uint64_t *data - data to shift, ls part stored first
  //          int count      - number of bits to shift
  // return:  uint64_t       - carry out bit(s)
  bool bitShiftLeft512(uint64_t* data, int count, uint64_t* carryout) override;

  //============================================================================================================================
  // bit shift right
  //============================================================================================================================
  // bit shift right a 64-bit value using general purpose registers
  //          uint64_t *data - data to shift
  //          int count      - number of bits to shift
  // return:  uint64_t       - carry out bit(s)
  uint64_t bitShiftRight64(uint64_t* data, int count) override;

  // bit shift right a 128-bit value using general purpose registers
  //          uint64_t *data - data to shift
  //          int count      - number of bits to shift
  // return:  uint64_t       - carry out bit(s)
  bool bitShiftRight128(uint64_t* data, int count, uint64_t* carryout) override;

  // bit shift right a 256-bit value using general purpose registers
  //          uint64_t *data - data to shift
  //          int count      - number of bits to shift
  // return:  uint64_t       - carry out bit(s)
  bool bitShiftRight256(uint64_t* data, int count, uint64_t* carryout) override;

  // bit shift right a 512-bit value using general purpose registers
  //          uint64_t *data - data to shift
  //          int count      - number of bits to shift
  // return:  uint64_t       - carry out bit(s)
  bool bitShiftRight512(uint64_t* data, int count, uint64_t* carryout) override;

  //============================================================================================================================
  // bit And
  //============================================================================================================================
  // bit or a 64-bit value using general purpose registers
  //          uint64_t data -  data to and
  //          uint64_t data  - data to or
  // return:  uint64_t       - bit or result
  uint64_t bitAnd64(uint64_t data0, uint64_t data1) override;

  // bit or a 128-bit value using general purpose registers
  //          uint64_t *data0 -     data to or
  //          uint64_t *data1 -     data to or
  //          uint64_t *data_out -  bit or result
  bool bitAnd128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  // bit or a 256-bit value using general purpose registers
  //          uint64_t *data0 -     data to or
  //          uint64_t *data1 -     data to or
  //          uint64_t *data_out -  bit or result
  bool bitAnd256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  // bit or a 512-bit value using general purpose registers
  //          uint64_t *data0 -     data to or
  //          uint64_t *data1 -     data to or
  //          uint64_t *data_out -  bit or result
  bool bitAnd512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  //============================================================================================================================
  // bit Or
  //============================================================================================================================
  // bit or a 64-bit value using general purpose registers
  //          uint64_t data0 - op1
  //          uint64_t data1 - op2
  // return:  uint64_t        -or operation result
  uint64_t bitOr64(uint64_t data0, uint64_t data1) override;

  // bit or a 128-bit value using general purpose registers
  //          uint64_t * data0 -    op1
  //          uint64_t * data1      op2
  //          uint64_t * data_out - bit Or result
  bool bitOr128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  // bit or a 256-bit value using general purpose registers
  //          uint64_t * data0 -    op1
  //          uint64_t * data1      op2
  //          uint64_t * data_out - bit Or result
  bool bitOr256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  // bit or a 512-bit value using general purpose registers
  //          uint64_t * data0 -    op1
  //          uint64_t * data1      op2
  //          uint64_t * data_out - bit Or result
  bool bitOr512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  //============================================================================================================================
  // bit Xor
  //============================================================================================================================
  // bit Xor a 64-bit value using general purpose registers
  //          uint64_t*  *data0 -    op1
  //          uint64_t*  *data1 -    op2
  // return： uint64_t   *data_out - xor result
  uint64_t bitXor64(uint64_t data0, uint64_t data1) override;

  // bit Xor a 128-bit value using general purpose registers
  //          uint64_t*  *data0 -    op1
  //          uint64_t*  *data1 -    op2
  //          uint64_t*  *data_out - xor result
  bool bitXor128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  // bit Xor a 256-bit value using general purpose registers
  //          uint64_t*  *data0 -    op1
  //          uint64_t*  *data1 -    op2
  //          uint64_t*  *data_out - xor result
  bool bitXor256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  // bit Xor a 512-bit value using general purpose registers
  //          uint64_t*  *data0 -    op1
  //          uint64_t*  *data1 -    op2
  //          uint64_t*  *data_out - xor result
  bool bitXor512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  //============================================================================================================================
  // bit pop_cnt
  //============================================================================================================================
  // bit pop_cnt a 64-bit value using general purpose registers
  //          uint64_t *data -    data to popcnt
  // return:  uint64_t       -    allBitCount amount number
  uint64_t bitPopcnt64(uint64_t data) override;

  // bit pop_cnt a 128-bit value using general purpose registers
  //          uint64_t *data -   data to popcnt
  //          uint64_t *cnt_arr -len = 2, cnt_arr number from font to end is the bitcnt of uint64 data form font to end
  // return:  uint64_t       -   set 1 bit count
  uint64_t bitPopcnt128(const uint64_t* data, uint8_t* cnt_arr) override;

  // bit pop_cnt a 256-bit value using general purpose registers
  //          uint64_t *data -   data to popcnt
  //          uint64_t *cnt_arr -len = 4, cnt_arr number from font to end is the bitcnt of uint64 data form font to end
  // return:  uint64_t       -   set 1 bit count
  uint64_t bitPopcnt256(const uint64_t* data, uint8_t* cnt_arr) override;

  // bit pop_cnt a 512-bit value using general purpose registers
  //          uint64_t *data -   data to popcnt
  //          uint64_t *cnt_arr -len = 8, cnt_arr number from font to end is the bitcnt of uint64 data form font to end
  // return:  uint64_t       -   set 1 bit count
  uint64_t bitPopcnt512(const uint64_t* data, uint8_t* cnt_arr) override;

  //============================================================================================================================
  // bit reverse
  //============================================================================================================================
  // bit reverse a 64-bit value using general purpose registers
  //          uint64_t 	dataIn	    data to reverse
  // 			uint64_t    dataOut    -data reversed
  uint64_t bitReverse64(uint64_t data) override;

  // bit reverse a 128-bit value using general purpose registers
  //          uint64_t* 	dataIn	    data to reverse
  //          uint64_t*   dataOut    -data reversed
  bool bitReverse128(uint64_t* dataIn, uint64_t* dataOut) override;

  // bit reverse a 256-bit value using general purpose registers
  //          uint64_t *data - data to reverse
  //          uint64_t *data - data to reverse
  bool bitReverse256(uint64_t* dataIn, uint64_t* dataOut) override;

  // bit reverse a 512-bit value using general purpose registers
  //          uint64_t* 	dataIn	    data to reverse
  //          uint64_t*   dataOut    -data reversed
  bool bitReverse512(uint64_t* dataIn, uint64_t* dataOut) override;

  // bit reverse inplace a 128-bit value using general purpose registers
  //          uint64_t    *data -     data to reverse and reversed
  bool bitReverseInPlace128(uint64_t* data) override;

  // bit reverse  inplace a 256-bit value using general purpose registers
  //          uint64_t    *data -     data to reverse and reversed
  bool bitReverseInPlace256(uint64_t* data) override;

  // bit reverse  inplace a 512-bit value using general purpose registers
  //          uint64_t* 	data	    data to reverse and reversed
  bool bitReverseInPlace512(uint64_t* data) override;
};
#endif// HYBRIDENGINE_BITCALGPR_H
