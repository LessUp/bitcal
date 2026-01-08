#ifndef HYBRIDENGINE_BITCALYMM_H
#define HYBRIDENGINE_BITCALYMM_H

#ifdef __x86_64__
#include <immintrin.h>
#elif __aarch64__
#include <arm_neon.h>
#endif
#include "Base.h"

#include <string>

class BitCalYmm : public BitCalBase {
public:
  BitCalYmm()  = default;

  ~BitCalYmm() = default;

  //============================================================================================================================
  // bit shift left
  //============================================================================================================================
  // bit shift left a 256-bit value using Ymm registers
  //          uint64_t *data - data to shift
  //          int count     - number of bits to shift
  //          uint64_t       - carry out bit(s)
  bool bitShiftLeft256(uint64_t* data, int count, uint64_t* data_out) override;

  // bit shift left a 512-bit value using Ymm registers
  //          uint64_t *data - data to shift, ls part stored first
  //          int count     - number of bits to shift
  //          uint64_t      -  carry out bit(s)
  bool bitShiftLeft512(uint64_t* data, int count, uint64_t* data_out) override;

  //============================================================================================================================
  // bit shift right
  //============================================================================================================================
  // bit shift right a 512-bit value using Ymm registers
  //          uint64_t    *data -               data to shift
  //          int count     - number of bits to shift
  //          uint64_t    *carry_out  -         carry out bit(s)
  bool bitShiftRight256(uint64_t* data, int count, uint64_t* carry_out) override;

  // bit shift right a 512-bit value using Ymm registers
  //          uint64_t    *data -               data to shift
  //          int count     - number of bits to shift
  //          uint64_t    *carry_out  -         carry out bit(s)
  bool bitShiftRight512(uint64_t* data, int count, uint64_t* carry_out) override;

  //============================================================================================================================
  // bit And
  //============================================================================================================================
  // bit shift left a 256-bit value using Ymm registers
  //          uint64_t * data0 -    op1
  //          uint64_t * data1      op2
  //          uint64_t * data_out - bit and result
  bool bitAnd256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  // bit shift left a 512-bit value using Ymm registers
  //          uint64_t * data0 -    op1
  //          uint64_t * data1      op2
  //          uint64_t * data_out - bit and result
  bool bitAnd512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  //============================================================================================================================
  // bit Or
  //============================================================================================================================
  // bit or a 256-bit value using Ymm registers
  //          uint64_t * data0 -    op1
  //          uint64_t * data1      op2
  //          uint64_t * data_out - bit Or result
  bool bitOr256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  // bit or a 256-bit value using Ymm registers
  //          uint64_t * data0 -    op1
  //          uint64_t * data1      op2
  //          uint64_t * data_out - bit Or result
  bool bitOr512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  //============================================================================================================================
  // bit Xor
  //============================================================================================================================
  // bit Xor a 256-bit value using Ymm registers
  //          uint64_t *data0 -    op1
  //          uint64_t *data1 -    op2
  //          uint64_t *data_out - xor result
  bool bitXor256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  // bit Xor a 256-bit value using Ymm registers
  //          uint64_t *data0 -    op1
  //          uint64_t *data1 -    op2
  //          uint64_t *data_out - xor result
  bool bitXor512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) override;

  //============================================================================================================================
  // bit Popcnt
  //============================================================================================================================
  // bit pop_cnt a 256-bit value using Ymm registers
  //          uint64_t *data -   data to shift
  //          uint64_t *cnt_arr -len = 8, cnt_arr number from font to end is the bitcnt of uint64 data form font to end
  // return:  uint64_t          -set 1 bit count
  uint64_t bitPopcnt256(const uint64_t* data, uint8_t* cnt_arr) override;

  // bit pop_cnt a 512-bit value using Ymm registers
  //          uint64_t *data -   data to shift
  //          uint64_t *cnt_arr -len = 8, cnt_arr number from font to end is the bitcnt of uint64 data form font to end
  // return:  uint64_t          -set 1 bit count
  uint64_t bitPopcnt512(const uint64_t* data, uint8_t* cnt_arr) override;
};

#endif //HYBRIDENGINE_BITCALYMM_H
