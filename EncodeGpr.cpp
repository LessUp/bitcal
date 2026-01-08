#include "EncodeGpr.h"

#include "spdlog/spdlog.h"

#include <cstdint>
#include <cstring>

const unsigned char forward2int[128] = { 00,      00,      00,      00,      00,      00, 00, 00,      00, 00, 00, 00, 00, 00,       00, 00,
                                         00,      00,      00,      00,      00,      00, 00, 00,      00, 00, 00, 00, 00, 00,       00, 00,
                                         00,      00,      00,      00,      00,      00, 00, 00,      00, 00, 00, 00, 00, 00,       00, 00,
                                         0 /*0*/, 1 /*1*/, 2 /*2*/, 3 /*3*/, 00,      00, 00, 00,      00, 00, 00, 00, 00, 00 /*=*/, 00, 00,
                                         00,      0 /*A*/, 00,      1 /*C*/, 00,      00, 00, 3 /*G*/, 00, 00, 00, 00, 00, 00 /*N*/, 00, 00,
                                         00,      00,      00,      00,      2 /*T*/, 00, 00, 00,      00, 00, 00, 00, 00, 00,       00, 00,
                                         00,      0 /*a*/, 00,      1 /*c*/, 00,      00, 00, 3 /*g*/, 00, 00, 00, 00, 00, 00 /*n*/, 00, 00,
                                         00,      00,      00,      00,      2 /*t*/, 00, 00, 00,      00, 00, 00, 00, 00, 00,       00, 00 };

const unsigned char reverse2int[128] = { 00,      00,      00,      00,      00,      00, 00, 00,      00, 00, 00, 00, 00, 00,       00, 00,
                                         00,      00,      00,      00,      00,      00, 00, 00,      00, 00, 00, 00, 00, 00,       00, 00,
                                         00,      00,      00,      00,      00,      00, 00, 00,      00, 00, 00, 00, 00, 00,       00, 00,
                                         0 /*0*/, 1 /*1*/, 2 /*2*/, 3 /*3*/, 00,      00, 00, 00,      00, 00, 00, 00, 00, 00 /*=*/, 00, 00,
                                         00,      2 /*A*/, 00,      3 /*C*/, 00,      00, 00, 1 /*G*/, 00, 00, 00, 00, 00, 00 /*N*/, 00, 00,
                                         00,      00,      00,      00,      0 /*T*/, 00, 00, 00,      00, 00, 00, 00, 00, 00,       00, 00,
                                         00,      2 /*a*/, 00,      3 /*c*/, 00,      00, 00, 1 /*g*/, 00, 00, 00, 00, 00, 00 /*n*/, 00, 00,
                                         00,      00,      00,      00,      0 /*t*/, 00, 00, 00,      00, 00, 00, 00, 00, 00,       00, 00 };

//============================================================================================================================
// encode
//============================================================================================================================
bool EncodeGpr::encodeForward(uint64_t* data, size_t size, const char* pbase, size_t length) {
  if(size * 32 < length) {
    spdlog::error("EncodeGpr::{} data size is mismatch with base length!", __FUNCTION__);
    return false;
  }
  uint64_t _32c = length / 32;
  uint64_t _32d = length % 32;
  uint32_t i    = 0;
  memset(data, 0, size * sizeof(uint64_t));
  auto base = reinterpret_cast<const uint8_t*>(pbase);

  for(i = 0; i < _32c; ++i) {
    data[i] |= (((uint64_t)forward2int[base[32 * i + 0]]) << 0);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 1]]) << 2);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 2]]) << 4);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 3]]) << 6);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 4]]) << 8);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 5]]) << 10);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 6]]) << 12);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 7]]) << 14);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 8]]) << 16);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 9]]) << 18);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 10]]) << 20);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 11]]) << 22);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 12]]) << 24);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 13]]) << 26);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 14]]) << 28);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 15]]) << 30);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 16]]) << 32);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 17]]) << 34);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 18]]) << 36);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 19]]) << 38);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 20]]) << 40);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 21]]) << 42);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 22]]) << 44);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 23]]) << 46);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 24]]) << 48);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 25]]) << 50);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 26]]) << 52);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 27]]) << 54);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 28]]) << 56);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 29]]) << 58);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 30]]) << 60);
    data[i] |= (((uint64_t)forward2int[base[32 * i + 31]]) << 62);
  }
  switch(_32d) {
  case 31:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 31]]) << 62);
  case 30:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 30]]) << 60);
  case 29:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 29]]) << 58);
  case 28:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 28]]) << 56);
  case 27:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 27]]) << 54);
  case 26:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 26]]) << 52);
  case 25:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 25]]) << 50);
  case 24:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 24]]) << 48);
  case 23:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 23]]) << 46);
  case 22:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 22]]) << 44);
  case 21:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 21]]) << 42);
  case 20:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 20]]) << 40);
  case 19:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 19]]) << 38);
  case 18:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 18]]) << 36);
  case 17:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 17]]) << 34);
  case 16:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 16]]) << 32);
  case 15:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 15]]) << 30);
  case 14:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 14]]) << 28);
  case 13:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 13]]) << 26);
  case 12:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 12]]) << 24);
  case 11:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 11]]) << 22);
  case 10:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 10]]) << 20);
  case 9:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 9]]) << 18);
  case 8:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 8]]) << 16);
  case 7:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 7]]) << 14);
  case 6:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 6]]) << 12);
  case 5:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 5]]) << 10);
  case 4:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 4]]) << 8);
  case 3:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 3]]) << 6);
  case 2:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 2]]) << 4);
  case 1:
    data[i] |= (((uint64_t)forward2int[base[32 * i + 1]]) << 2);

    data[i] |= (((uint64_t)forward2int[base[32 * i + 0]]) << 0);
  default:;
  }
  return true;
}

bool EncodeGpr::encodeReverse(uint64_t* data, size_t size, const char* pbase, size_t length) {
  if(size * 32 < length) {
    spdlog::error("EncodeGpr::{} data size is mismatch with base length!", __FUNCTION__);
    return false;
  }
  uint64_t _32c = length / 32;
  uint64_t _32d = length % 32;
  uint32_t i    = 0;
  memset(data, 0, size * sizeof(uint64_t));
  auto base = reinterpret_cast<const uint8_t*>(pbase);

  for(i = 0; i < _32c; ++i) {
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 0)]]) << 0);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 1)]]) << 2);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 2)]]) << 4);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 3)]]) << 6);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 4)]]) << 8);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 5)]]) << 10);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 6)]]) << 12);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 7)]]) << 14);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 8)]]) << 16);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 9)]]) << 18);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 10)]]) << 20);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 11)]]) << 22);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 12)]]) << 24);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 13)]]) << 26);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 14)]]) << 28);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 15)]]) << 30);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 16)]]) << 32);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 17)]]) << 34);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 18)]]) << 36);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 19)]]) << 38);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 20)]]) << 40);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 21)]]) << 42);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 22)]]) << 44);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 23)]]) << 46);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 24)]]) << 48);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 25)]]) << 50);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 26)]]) << 52);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 27)]]) << 54);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 28)]]) << 56);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 29)]]) << 58);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 30)]]) << 60);
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 31)]]) << 62);
  }
  switch(_32d) {
  case 31:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 31)]]) << 62);
  case 30:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 30)]]) << 60);
  case 29:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 29)]]) << 58);
  case 28:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 28)]]) << 56);
  case 27:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 27)]]) << 54);
  case 26:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 26)]]) << 52);
  case 25:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 25)]]) << 50);
  case 24:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 24)]]) << 48);
  case 23:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 23)]]) << 46);
  case 22:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 22)]]) << 44);
  case 21:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 21)]]) << 42);
  case 20:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 20)]]) << 40);
  case 19:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 19)]]) << 38);
  case 18:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 18)]]) << 36);
  case 17:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 17)]]) << 34);
  case 16:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 16)]]) << 32);
  case 15:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 15)]]) << 30);
  case 14:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 14)]]) << 28);
  case 13:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 13)]]) << 26);
  case 12:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 12)]]) << 24);
  case 11:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 11)]]) << 22);
  case 10:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 10)]]) << 20);
  case 9:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 9)]]) << 18);
  case 8:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 8)]]) << 16);
  case 7:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 7)]]) << 14);
  case 6:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 6)]]) << 12);
  case 5:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 5)]]) << 10);
  case 4:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 4)]]) << 8);
  case 3:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 3)]]) << 6);
  case 2:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 2)]]) << 4);
  case 1:
    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 1)]]) << 2);

    data[i] |= (((uint64_t)reverse2int[base[length - 1 - (32 * i + 0)]]) << 0);
  default:;
  }
  return true;
}

//============================================================================================================================
// decode
//============================================================================================================================
bool EncodeGpr::decodeForward(const uint64_t* data, size_t size, char* base, size_t length) {
  if(size * 32 < length) {
    spdlog::error("EncodeGpr::{} data size is mismatch with base length!", __FUNCTION__);
    return false;
  }
  for(size_t i = 0; i < length; ++i) {
    base[i] = "ACTG"[(data[i / 32] >> ((i % 32) * 2)) & 0x3];
  }
  base[length] = 0;
  return true;
}

bool EncodeGpr::decodeReverse(const uint64_t* data, size_t size, char* base, size_t length) {
  if(size * 32 < length) {
    spdlog::error("EncodeGpr::{} data size is mismatch with base length!", __FUNCTION__);
    return false;
  }
  for(size_t i = 0, j = length - 1; i < length; ++i, --j) {
    base[j] = "TGAC"[(data[i / 32] >> ((i % 32) * 2)) & 0x3];
  }
  base[length] = 0;
  return true;
}

bool EncodeGpr::decodeForward256(const uint64_t* data, char* base, size_t length) {
  return decodeForward(data, 4, base, length);
}

bool EncodeGpr::decodeReverse256(const uint64_t* data, char* base, size_t length) {
  return decodeReverse(data, 4, base, length);
}

bool EncodeGpr::decodeForward512(const uint64_t* data, char* base, size_t length) {
  return decodeForward(data, 8, base, length);
}

bool EncodeGpr::decodeReverse512(const uint64_t* data, char* base, size_t length) {
  return decodeReverse(data, 8, base, length);
}
