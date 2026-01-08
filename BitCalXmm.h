#ifndef HYBRIDENGINE_BITCALXmm_H
#define HYBRIDENGINE_BITCALXmm_H

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#elif defined(__aarch64__) || defined(_M_ARM64)
#include <arm_neon.h>
#endif
#include "Base.h"

#include <string>

class BitCalXmm : public BitCalBase {
public:
  BitCalXmm()  = default;

  ~BitCalXmm() = default;

  //============================================================================================================================
  // bit shift left
  //============================================================================================================================
  // bit shift left a 128-bit value using Xmm registers
  //          uint64_t *data - data to shift
  //          int count     - number of bits to shift
  //          uint64_t *data - carry out bit(s)
  bool bitShiftLeft128(uint64_t* data, int count, uint64_t* carryout) override;

  // bit shift left a 256-bit value using Xmm registers
  //          uint64_t *data -data to shift, ls part stored first
  //          int count     - number of bits to shift
  //          uint64_t *data  carry out bit(s)
  bool bitShiftLeft256(uint64_t* data, int count, uint64_t* carry_out) override;

  // bit shift left a 512-bit value using Xmm registers
  //          uint64_t *data -data to shift, ls part stored first
  //          int count     - number of bits to shift
  //          uint64_t *data  carry out bit(s)
  bool bitShiftLeft512(uint64_t* data, int count, uint64_t* carry_out) override;

  //============================================================================================================================
  // bit shift right
  //============================================================================================================================
  // bit shift right a 128-bit value using Xmm registers
  //          uint64_t    *data -               data to shift
  //          int count     - number of bits to shift
  //          uint64_t    *carry_out  -         carry out bit(s)
  bool bitShiftRight128(uint64_t* data, int count, uint64_t* carry_out) override;

  // bit shift right a 256-bit value using Xmm registers
  //          uint64_t    *data -               data to shift
  //          int count     - number of bits to shift
  //          uint64_t    *carry_out  -         carry out bit(s)
  bool bitShiftRight256(uint64_t* data, int count, uint64_t* carry_out) override;

  // bit shift right a 512-bit value using Xmm registers
  //          uint64_t    *data -               data to shift
  //          int count     - number of bits to shift
  //          uint64_t    *carry_out  -         carry out bit(s)
  bool bitShiftRight512(uint64_t* data, int count, uint64_t* carry_out) override;

  //============================================================================================================================
  // bit And
  //============================================================================================================================
  // bit and left a 128-bit value using Xmm registers
  //          uint64_t * data0 -    op1
  //          uint64_t * data1      op2
  //          uint64_t * data_out - bit and result
  bool bitAnd128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  // bit and a 256-bit value using Xmm registers
  //          uint64_t * data0 -    op1
  //          uint64_t * data1      op2
  //          uint64_t * data_out - bit and result
  bool bitAnd256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  // bit and a 512-bit value using Xmm registers
  //          uint64_t * data0 -    op1
  //          uint64_t * data1      op2
  //          uint64_t * data_out - bit and result
  bool bitAnd512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  //============================================================================================================================
  // bit Or
  //============================================================================================================================
  // bit or a 128-bit value using Xmm registers
  //          uint64_t * data0 -    op1
  //          uint64_t * data1      op2
  //          uint64_t * data_out - bit Or result
  bool bitOr128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  // bit or a 256-bit value using Xmm registers
  //          uint64_t * data0 -    op1
  //          uint64_t * data1      op2
  //          uint64_t * data_out - bit Or result
  bool bitOr256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  // bit or a 512-bit value using Xmm registers
  //          uint64_t * data0 -    op1
  //          uint64_t * data1      op2
  //          uint64_t * data_out - bit Or result
  bool bitOr512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  //============================================================================================================================
  // bit Xor
  //============================================================================================================================
  // bit Xor a 128-bit value using Xmm registers
  //          uint64_t *data0 -    op1
  //          uint64_t *data1 -    op2
  //          uint64_t *data_out - xor result
  bool bitXor128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  // bit Xor a 256-bit value using Xmm registers
  //          uint64_t *data0 -    op1
  //          uint64_t *data1 -    op2
  //          uint64_t *data_out - xor result
  bool bitXor256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  // bit Xor a 512-bit value using Xmm registers
  //          uint64_t *data0 -    op1
  //          uint64_t *data1 -    op2
  //          uint64_t *data_out - xor result
  bool bitXor512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  //============================================================================================================================
  // bit pop_cnt
  //============================================================================================================================
  // bit pop_cnt a 128-bit value using Xmm registers
  //          uint64_t *data -    data to popcnt
  //          uint64_t *cnt_arr -len = 2, cnt_arr number from font to end is the bitcnt of uint64 data form font to end
  // return:  uint64_t       -   set 1 bit count
  uint64_t bitPopcnt128(const uint64_t* data, uint8_t* cnt_arr) override;

  // bit pop_cnt a 256-bit value using Xmm registers
  //          uint64_t *data -    data to popcnt
  //          uint64_t *cnt_arr -len = 4, cnt_arr number from font to end is the bitcnt of uint64 data form font to end
  // return:  uint64_t       -   set 1 bit count
  uint64_t bitPopcnt256(const uint64_t* data, uint8_t* cnt_arr) override;

  // bit pop_cnt a 512-bit value using Xmm registers
  //          uint64_t *data -    data to popcnt
  //          uint64_t *cnt_arr -len = 8, cnt_arr number from font to end is the bitcnt of uint64 data form font to end
  // return:  uint64_t       -   set 1 bit count
  uint64_t bitPopcnt512(const uint64_t* data, uint8_t* cnt_arr) override;
};
#endif// HYBRIDENGINE_BITCALXmm_H
