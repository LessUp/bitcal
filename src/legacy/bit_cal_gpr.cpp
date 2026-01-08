#include "BitCalGpr.h"

#if defined(__aarch64__) || defined(_M_ARM64)
#include "bit_cal_gpr.h"

#include <iostream>
#include <stdexcept>

#include <cstring>
#include <numeric>
  0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0, 0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8,
  0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8, 0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
  0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC, 0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2,
  0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2, 0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
  0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6, 0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE,
  0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE, 0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
  0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9, 0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5,
  0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5, 0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
  0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3, 0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB,
  0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB, 0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
  0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};

//============================================================================================================================
// bit shift left
//============================================================================================================================
uint64_t BitCalGpr::bitShiftLeft64(uint64_t* data, int count) {
    if (!data) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return {};
    }

    if (count<0 || count>63) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return {};
    }

  uint64_t carryOut = count == 0 ? 0 : *data >> (64 - count);
  *data <<= count;
  return carryOut;
}

bool BitCalGpr::bitShiftLeft128(uint64_t* data, int count, uint64_t* carryout) {
    if (!data) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    if (count<0 || count>63) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

  uint64_t carryOut0 = bitShiftLeft64(&data[0], count);
  uint64_t carryOut1 = bitShiftLeft64(&data[1], count);
  data[1] |= carryOut0;

    memcpy(carryout, &carryOut1, sizeof(uint64_t));
    return true;
}

bool BitCalGpr::bitShiftLeft256(uint64_t* data, int count, uint64_t* carryout) {
    if (!data) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    if (count<0 || count>63) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    uint64_t carryOut0{}, carryOut1{};
    bitShiftLeft128(&data[0], count, &carryOut0);
    bitShiftLeft128(&data[2], count, &carryOut1);
    data[2] |= carryOut0;

    memcpy(carryout, &carryOut1, sizeof(uint64_t));
    return true;
}

bool BitCalGpr::bitShiftLeft512(uint64_t* data, int count, uint64_t* carryout) {
    if (!data) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    if (count<0 || count>63) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    uint64_t carryOut0{}, carryOut1{};
    bitShiftLeft256(&data[0], count, &carryOut0);
    bitShiftLeft256(&data[4], count, &carryOut1);
    data[4] |= carryOut0;

    memcpy(carryout, &carryOut1, sizeof(uint64_t));
    return true;
}

//============================================================================================================================
// bit shift right
//============================================================================================================================
uint64_t BitCalGpr::bitShiftRight64(uint64_t* data, int count) {
    if (!data) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return {};
    }

    if (count<0 || count>63) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return {};
    }

  uint64_t carryOut = count == 0 ? 0 : *data << (64 - count);  //& ~((1<<count)-1);
  *data >>= count;
  return carryOut;
}

bool BitCalGpr::bitShiftRight128(uint64_t* data, int count, uint64_t* carryout) {
    if (!data) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    if (count<0 || count>63) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

  uint64_t carryOut0 = bitShiftRight64(&data[0], count);
  uint64_t carryOut1 = bitShiftRight64(&data[1], count);
  data[0] |= carryOut1;

    memcpy(carryout, &carryOut0, sizeof(uint64_t));

    return true;
}

bool BitCalGpr::bitShiftRight256(uint64_t* data, int count, uint64_t* carryout) {
    if (!data) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    if (count<0 || count>63) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    uint64_t carryOut0{}, carryOut1{};
    bitShiftRight128(&data[0], count, &carryOut0);
    bitShiftRight128(&data[2], count, &carryOut1);
    data[1] |= carryOut1;

    memcpy(carryout, &carryOut0, sizeof(uint64_t));
    return true;
}

bool BitCalGpr::bitShiftRight512(uint64_t* data, int count, uint64_t* carryout) {
    if (!data) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    if (count<0 || count>63) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    uint64_t carryOut0{}, carryOut1{};
    bitShiftRight256(&data[0], count, &carryOut0);
    bitShiftRight256(&data[4], count, &carryOut1);
    data[3] |= carryOut1;

    memcpy(carryout, &carryOut0, sizeof(uint64_t));
    return true;
}

//============================================================================================================================
// bit And
//============================================================================================================================
uint64_t BitCalGpr::bitAnd64(uint64_t data0, uint64_t data1) {
  return (data0 & data1);
}

bool BitCalGpr::bitAnd128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    data_out[0] = bitAnd64(data0[0], data1[0]);
    data_out[1] = bitAnd64(data0[1], data1[1]);
    return true;
}

bool BitCalGpr::bitAnd256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    bitAnd128(&data0[0], &data1[0], &data_out[0]);
    bitAnd128(&data0[2], &data1[2], &data_out[2]);
    return true;
}

bool BitCalGpr::bitAnd512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    bitAnd256(&data0[0], &data1[0], &data_out[0]);
    bitAnd256(&data0[4], &data1[4], &data_out[4]);
    return true;
}

//============================================================================================================================
// bit Or
//============================================================================================================================
uint64_t BitCalGpr::bitOr64(uint64_t data0, uint64_t data1) {
  return (data0 | data1);
}

bool BitCalGpr::bitOr128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    data_out[0] = bitOr64(data0[0], data1[0]);
    data_out[1] = bitOr64(data0[1], data1[1]);
    return true;
}

bool BitCalGpr::bitOr256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    bitOr128(&data0[0], &data1[0], &data_out[0]);
    bitOr128(&data0[2], &data1[2], &data_out[2]);

    return true;
}

bool BitCalGpr::bitOr512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    bitOr256(&data0[0], &data1[0], &data_out[0]);
    bitOr256(&data0[4], &data1[4], &data_out[4]);
    return true;
}

//============================================================================================================================
// bit Xor
//============================================================================================================================
uint64_t BitCalGpr::bitXor64(uint64_t data0, uint64_t data1) {
  return (data0 ^ data1);
}

bool BitCalGpr::bitXor128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    data_out[0] = bitXor64(data0[0], data1[0]);
    data_out[1] = bitXor64(data0[1], data1[1]);

    return true;
}

bool BitCalGpr::bitXor256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    data_out[0] = (data0[0] ^ data1[0]);
    data_out[1] = (data0[1] ^ data1[1]);
    data_out[2] = (data0[2] ^ data1[2]);
    data_out[3] = (data0[3] ^ data1[3]);

    return true;
}

bool BitCalGpr::bitXor512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

  data_out[0] = (data0[0] ^ data1[0]);
  data_out[1] = (data0[1] ^ data1[1]);
  data_out[2] = (data0[2] ^ data1[2]);
  data_out[3] = (data0[3] ^ data1[3]);

    data_out[4] = (data0[4] ^ data1[4]);
    data_out[5] = (data0[5] ^ data1[5]);
    data_out[6] = (data0[6] ^ data1[6]);
    data_out[7] = (data0[7] ^ data1[7]);

    return true;
}

//============================================================================================================================
// bit pop_cnt
//============================================================================================================================
uint64_t BitCalGpr::bitPopcnt64(uint64_t data) {
  return _mm_popcnt_u64(data);
}

uint64_t BitCalGpr::bitPopcnt128(const uint64_t* data, uint8_t* cnt_arr) {
    if (!data) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return {};
    }

  cnt_arr[0] = bitPopcnt64(data[0]);
  cnt_arr[1] = bitPopcnt64(data[1]);

  return std::accumulate(cnt_arr, cnt_arr + 2, 0);
}

uint64_t BitCalGpr::bitPopcnt256(const uint64_t* data, uint8_t* cnt_arr) {
    if (!data) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return {};
    }

  cnt_arr[0] = bitPopcnt64(data[0]);
  cnt_arr[1] = bitPopcnt64(data[1]);
  cnt_arr[2] = bitPopcnt64(data[2]);
  cnt_arr[3] = bitPopcnt64(data[3]);

  return std::accumulate(cnt_arr, cnt_arr + 4, 0);
}

uint64_t BitCalGpr::bitPopcnt512(const uint64_t* data, uint8_t* cnt_arr) {
    if (!data) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return {};
    }

  cnt_arr[0] = bitPopcnt64(data[0]);
  cnt_arr[1] = bitPopcnt64(data[1]);
  cnt_arr[2] = bitPopcnt64(data[2]);
  cnt_arr[3] = bitPopcnt64(data[3]);
  cnt_arr[4] = bitPopcnt64(data[4]);
  cnt_arr[5] = bitPopcnt64(data[5]);
  cnt_arr[6] = bitPopcnt64(data[6]);
  cnt_arr[7] = bitPopcnt64(data[7]);

  return std::accumulate(cnt_arr, cnt_arr + 8, 0);
}

//============================================================================================================================
// bit reverse
//============================================================================================================================
uint64_t BitCalGpr::bitReverse64(uint64_t data) {
  uint64_t res          = 0;
  ((uint8_t*)(&res))[7] = BitReverseTable256[((uint8_t*)(&data))[0]];
  ((uint8_t*)(&res))[6] = BitReverseTable256[((uint8_t*)(&data))[1]];
  ((uint8_t*)(&res))[5] = BitReverseTable256[((uint8_t*)(&data))[2]];
  ((uint8_t*)(&res))[4] = BitReverseTable256[((uint8_t*)(&data))[3]];
  ((uint8_t*)(&res))[3] = BitReverseTable256[((uint8_t*)(&data))[4]];
  ((uint8_t*)(&res))[2] = BitReverseTable256[((uint8_t*)(&data))[5]];
  ((uint8_t*)(&res))[1] = BitReverseTable256[((uint8_t*)(&data))[6]];
  ((uint8_t*)(&res))[0] = BitReverseTable256[((uint8_t*)(&data))[7]];
  return (res);
}

bool BitCalGpr::bitReverse128(uint64_t* dataIn, uint64_t* dataOut) {
    if (!dataIn || !dataOut) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    dataOut[0] = bitReverse64(dataIn[1]);
    dataOut[1] = bitReverse64(dataIn[0]);

    return true;
}

bool BitCalGpr::bitReverse256(uint64_t* dataIn, uint64_t* dataOut) {
    if (!dataIn || !dataOut) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    bitReverse128(&dataIn[2], &dataOut[0]);
    bitReverse128(&dataIn[0], &dataOut[2]);
    return true;
}

bool BitCalGpr::bitReverse512(uint64_t* dataIn, uint64_t* dataOut) {
    if (!dataIn || !dataOut) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    bitReverse256(&dataIn[4], &dataOut[0]);
    bitReverse256(&dataIn[0], &dataOut[4]);
    return true;
}

bool BitCalGpr::bitReverseInPlace128(uint64_t* data) {
    if (!data) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

  data[0] = bitReverse64(data[0]);
  data[1] = bitReverse64(data[1]);

    data[0] ^= data[1];
    data[1] ^= data[0];
    data[0] ^= data[1];

    return true;
}

bool BitCalGpr::bitReverseInPlace256(uint64_t* data) {
    if (!data) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

  data[0] = bitReverse64(data[0]);
  data[1] = bitReverse64(data[1]);
  data[2] = bitReverse64(data[2]);
  data[3] = bitReverse64(data[3]);

  data[0] ^= data[3];
  data[3] ^= data[0];
  data[0] ^= data[3];

    data[1] ^= data[2];
    data[2] ^= data[1];
    data[1] ^= data[2];

    return true;
}

bool BitCalGpr::bitReverseInPlace512(uint64_t* data) {
    if (!data) {
        spdlog::error("BitCalGpr::{} input data is illegal!", __FUNCTION__);
        return false;
    }

  data[0] = bitReverse64(data[0]);
  data[1] = bitReverse64(data[1]);
  data[2] = bitReverse64(data[2]);
  data[3] = bitReverse64(data[3]);
  data[4] = bitReverse64(data[4]);
  data[5] = bitReverse64(data[5]);
  data[6] = bitReverse64(data[6]);
  data[7] = bitReverse64(data[7]);

  data[0] ^= data[7];
  data[7] ^= data[0];
  data[0] ^= data[7];

  data[1] ^= data[6];
  data[6] ^= data[1];
  data[1] ^= data[6];

  data[2] ^= data[5];
  data[5] ^= data[2];
  data[2] ^= data[5];

    data[3] ^= data[4];
    data[4] ^= data[3];
    data[3] ^= data[4];

    return true;
}