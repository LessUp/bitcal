#pragma once
//
// Created by liwei on 22-11-8.
//

#include <string>

class BitCalBase {

public:
  //============================================================================================================================
  // bit shift left
  //============================================================================================================================
  // bit shift left a 64-bit value using general purpose registers
  //          uint64_t *data - data to shift
  //          int count      - number of bits to shift
  // return:  uint64_t       - carry out bit(s)
  virtual uint64_t bitShiftLeft64(uint64_t* data, int count) {
    return 0;
  };

  // bit shift left a 128-bit value using general purpose registers
  //          uint64_t *data - data to shift, ls part stored first
  //          int count      - number of bits to shift
  // return:  uint64_t       - carry out bit(s)
  virtual bool bitShiftLeft128(uint64_t* data, int count, uint64_t* carryout) {
    return false;
  };

  // bit shift left a 256-bit value using general purpose registers
  //          uint64_t *data - data to shift, ls part stored first
  //          int count      - number of bits to shift
  // return:  uint64_t       - carry out bit(s)
  virtual bool bitShiftLeft256(uint64_t* data, int count, uint64_t* carryout) {
    return false;
  };

  // bit shift left a 512-bit value using general purpose registers
  //          uint64_t *data - data to shift, ls part stored first
  //          int count      - number of bits to shift
  // return:  uint64_t       - carry out bit(s)
  virtual bool bitShiftLeft512(uint64_t* data, int count, uint64_t* carryout) {
    return false;
  };

  //============================================================================================================================
  // bit shift right
  //============================================================================================================================
  // bit shift right a 64-bit value using general purpose registers
  //          uint64_t *data - data to shift
  //          int count      - number of bits to shift
  // return:  uint64_t       - carry out bit(s)
  virtual uint64_t bitShiftRight64(uint64_t* data, int count) {
    return 0;
  };

  // bit shift right a 128-bit value using general purpose registers
  //          uint64_t *data - data to shift
  //          int count      - number of bits to shift
  // return:  uint64_t       - carry out bit(s)
  virtual bool bitShiftRight128(uint64_t* data, int count, uint64_t* carryout) {
    return false;
  };

  // bit shift right a 256-bit value using general purpose registers
  //          uint64_t *data - data to shift
  //          int count      - number of bits to shift
  // return:  uint64_t       - carry out bit(s)
  virtual bool bitShiftRight256(uint64_t* data, int count, uint64_t* carryout) {
    return false;
  };

  // bit shift right a 512-bit value using general purpose registers
  //          uint64_t *data - data to shift
  //          int count      - number of bits to shift
  // return:  uint64_t       - carry out bit(s)
  virtual bool bitShiftRight512(uint64_t* data, int count, uint64_t* carryout) {
    return false;
  };

  //============================================================================================================================
  // bit And
  //============================================================================================================================
  // bit or a 64-bit value using general purpose registers
  //          uint64_t data -  data to and
  //          uint64_t data  - data to or
  // return:  uint64_t       - bit or result
  virtual uint64_t bitAnd64(uint64_t data0, uint64_t data1) {
    return 0;
  };

  // bit or a 128-bit value using general purpose registers
  //          uint64_t *data0 -     data to or
  //          uint64_t *data1 -     data to or
  //          uint64_t *data_out -  bit or result
  virtual bool bitAnd128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
    return false;
  };

  // bit or a 256-bit value using general purpose registers
  //          uint64_t *data0 -     data to or
  //          uint64_t *data1 -     data to or
  //          uint64_t *data_out -  bit or result
  virtual bool bitAnd256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
    return false;
  };

  // bit or a 512-bit value using general purpose registers
  //          uint64_t *data0 -     data to or
  //          uint64_t *data1 -     data to or
  //          uint64_t *data_out -  bit or result
  virtual bool bitAnd512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
    return false;
  };

  //============================================================================================================================
  // bit Or
  //============================================================================================================================
  // bit or a 64-bit value using general purpose registers
  //          uint64_t data0 - op1
  //          uint64_t data1 - op2
  // return:  uint64_t        -or operation result
  virtual uint64_t bitOr64(uint64_t data0, uint64_t data1) {
    return 0;
  };

  // bit or a 128-bit value using general purpose registers
  //          uint64_t * data0 -    op1
  //          uint64_t * data1      op2
  //          uint64_t * data_out - bit Or result
  virtual bool bitOr128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
    return false;
  };

  // bit or a 256-bit value using general purpose registers
  //          uint64_t * data0 -    op1
  //          uint64_t * data1      op2
  //          uint64_t * data_out - bit Or result
  virtual bool bitOr256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
    return false;
  };

  // bit or a 512-bit value using general purpose registers
  //          uint64_t * data0 -    op1
  //          uint64_t * data1      op2
  //          uint64_t * data_out - bit Or result
  virtual bool bitOr512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
    return false;
  };

  //============================================================================================================================
  // bit Xor
  //============================================================================================================================
  // bit Xor a 64-bit value using general purpose registers
  //          uint64_t*  *data0 -    op1
  //          uint64_t*  *data1 -    op2
  // return： uint64_t   *data_out - xor result
  virtual uint64_t bitXor64(uint64_t data0, uint64_t data1) {
    return 0;
  };

  // bit Xor a 128-bit value using general purpose registers
  //          uint64_t*  *data0 -    op1
  //          uint64_t*  *data1 -    op2
  //          uint64_t*  *data_out - xor result
  virtual bool bitXor128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
    return false;
  };

  // bit Xor a 256-bit value using general purpose registers
  //          uint64_t*  *data0 -    op1
  //          uint64_t*  *data1 -    op2
  //          uint64_t*  *data_out - xor result
  virtual bool bitXor256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
    return false;
  };

  // bit Xor a 512-bit value using general purpose registers
  //          uint64_t*  *data0 -    op1
  //          uint64_t*  *data1 -    op2
  //          uint64_t*  *data_out - xor result
  virtual bool bitXor512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
    return false;
  };

  //============================================================================================================================
  // bit pop_cnt
  //============================================================================================================================
  // bit pop_cnt a 64-bit value using general purpose registers
  //          uint64_t *data -    data to popcnt
  // return:  uint64_t       -    allBitCount amount number
  virtual uint64_t bitPopcnt64(uint64_t data) {
    return 0;
  };

  // bit pop_cnt a 128-bit value using general purpose registers
  //          uint64_t *data -   data to popcnt
  //          uint64_t *cnt_arr -len = 2, cnt_arr number from font to end is the bitcnt of uint64 data form font to end
  // return:  uint64_t       -   set 1 bit count
  virtual uint64_t bitPopcnt128(const uint64_t* data, uint8_t* cnt_arr) {
    return 0;
  };

  // bit pop_cnt a 256-bit value using general purpose registers
  //          uint64_t *data -   data to popcnt
  //          uint64_t *cnt_arr -len = 4, cnt_arr number from font to end is the bitcnt of uint64 data form font to end
  // return:  uint64_t       -   set 1 bit count
  virtual uint64_t bitPopcnt256(const uint64_t* data, uint8_t* cnt_arr) {
    return 0;
  };

  // bit pop_cnt a 512-bit value using general purpose registers
  //          uint64_t *data -   data to popcnt
  //          uint64_t *cnt_arr -len = 8, cnt_arr number from font to end is the bitcnt of uint64 data form font to end
  // return:  uint64_t       -   set 1 bit count
  virtual uint64_t bitPopcnt512(const uint64_t* data, uint8_t* cnt_arr) {
    return 0;
  };

  //============================================================================================================================
  // bit reverse
  //============================================================================================================================
  // bit reverse a 64-bit value using general purpose registers
  //          uint64_t 	dataIn	    data to reverse
  // 			uint64_t    dataOut    -data reversed
  virtual uint64_t bitReverse64(uint64_t data) {
    return 0;
  };

  // bit reverse a 128-bit value using general purpose registers
  //          uint64_t* 	dataIn	    data to reverse
  //          uint64_t*   dataOut    -data reversed
  virtual bool bitReverse128(uint64_t* dataIn, uint64_t* dataOut) {
    return false;
  };

  // bit reverse a 256-bit value using general purpose registers
  //          uint64_t *data - data to reverse
  //          uint64_t *data - data to reverse
  virtual bool bitReverse256(uint64_t* dataIn, uint64_t* dataOut) {
    return false;
  };

  // bit reverse a 512-bit value using general purpose registers
  //          uint64_t* 	dataIn	    data to reverse
  //          uint64_t*   dataOut    -data reversed
  virtual bool bitReverse512(uint64_t* dataIn, uint64_t* dataOut) {
    return false;
  };

  // bit reverse inplace a 128-bit value using general purpose registers
  //          uint64_t    *data -     data to reverse and reversed
  virtual bool bitReverseInPlace128(uint64_t* data) {
    return false;
  };

  // bit reverse  inplace a 256-bit value using general purpose registers
  //          uint64_t    *data -     data to reverse and reversed
  virtual bool bitReverseInPlace256(uint64_t* data) {
    return false;
  };

  // bit reverse  inplace a 512-bit value using general purpose registers
  //          uint64_t* 	data	    data to reverse and reversed
  virtual bool bitReverseInPlace512(uint64_t* data) {
    return false;
  };
};

class EncodeBase {
public:
  //============================================================================================================================
  // encode
  //============================================================================================================================
  virtual bool encodeForward(uint64_t* data, size_t size, const char* base, size_t length) {
    return false;
  };

  virtual bool encodeReverse(uint64_t* data, size_t size, const char* base, size_t length) {
    return false;
  };
  //============================================================================================================================
  // decode
  //============================================================================================================================
  virtual bool decodeForward(const uint64_t* data, size_t size, char* base, size_t length) {
    return false;
  };

  virtual bool decodeReverse(const uint64_t* data, size_t size, char* base, size_t length) {
    return false;
  };

  virtual bool decodeForward256(const uint64_t* data, char* base, size_t length) {
    return false;
  };

  virtual bool decodeReverse256(const uint64_t* data, char* base, size_t length) {
    return false;
  };

  virtual bool decodeForward512(const uint64_t* data, char* base, size_t length) {
    return false;
  };

  virtual bool decodeReverse512(const uint64_t* data, char* base, size_t length) {
    return false;
  };
};