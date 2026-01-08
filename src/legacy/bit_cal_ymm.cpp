#if defined(__x86_64__) || defined(_M_X64)
#include "BitCalYmm.h"

#include "spdlog/spdlog.h"

#include <numeric>
#include <string>

//============================================================================================================================
// bit shift left
//============================================================================================================================
bool BitCalYmm::bitShiftLeft256(uint64_t *data, int count, uint64_t *carry_out) {
    if (!data || !carry_out) {
        spdlog::error("BitCalYmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    if (count < 0 || count > 63) {
        spdlog::error("BitCalYmm::{} shift count is illegal!", __FUNCTION__);
        return false;
    }

    if (count == 0) {
        *(__m256i*)carry_out = _mm256_setzero_si256();
        return true;
    }

  __m256i innerCarry = _mm256_srli_epi64(_mm256_lddqu_si256((__m256i*)data),
                                         64 - count);  // carry outs in bit 0 of each qword
  __m256i rotate     = _mm256_permute4x64_epi64(innerCarry, 0x93);  // rotate Ymm left 64 bits
  innerCarry         = _mm256_blend_epi32(_mm256_setzero_si256(), rotate, 0xFC);  // clear lower qword
  _mm256_storeu_si256((__m256i*)data, _mm256_or_si256(_mm256_slli_epi64(_mm256_lddqu_si256((__m256i*)data), count), innerCarry));

    auto carry = _mm256_xor_si256(innerCarry, rotate);
    _mm256_storeu_si256((__m256i *) carry_out, carry);
    return true;
}

bool BitCalYmm::bitShiftLeft512(uint64_t *data, int count, uint64_t *carry_out) {
    if (!data || !carry_out) {
        spdlog::error("BitCalYmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    if (count < 0 || count > 63) {
        spdlog::error("BitCalYmm::{} shift count is illegal!", __FUNCTION__);
        return false;
    }

    __m256i carryOut0{}, carryOut1{};
    bitShiftLeft256(data, count, (uint64_t * ) & carryOut0);
    bitShiftLeft256(data + 4, count, (uint64_t * ) & carryOut1);
    _mm256_storeu_si256((__m256i *) data + 1, _mm256_or_si256(_mm256_lddqu_si256((__m256i *) data + 1), carryOut0));

    _mm256_storeu_si256((__m256i *) carry_out, carryOut1);

    return true;
}

//============================================================================================================================
// bit shift right
//============================================================================================================================
bool BitCalYmm::bitShiftRight256(uint64_t *data, int count, uint64_t *carry_out) {
    if (!data || !carry_out) {
        spdlog::error("BitCalYmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    if (count < 0 || count > 63) {
        spdlog::error("BitCalYmm::{} shift count is illegal!", __FUNCTION__);
        return false;
    }

    if (count == 0) {
        *(__m256i*)carry_out = _mm256_setzero_si256();
        return true;
    }

    __m256i innerCarry = _mm256_slli_epi64(_mm256_lddqu_si256((__m256i *) data),
                                           64 - count);                             // carry outs in bit 0 of each qword
    __m256i rotate = _mm256_permute4x64_epi64(innerCarry, 0x39);                 // rotate Ymm left 64 bits
    innerCarry = _mm256_blend_epi32(_mm256_setzero_si256(), rotate, 0x3F);    // clear lower qword
    _mm256_storeu_si256((__m256i *) data,
                        _mm256_or_si256(_mm256_srli_epi64(_mm256_lddqu_si256((__m256i *) data), count), innerCarry));
    auto m256CarryOut = _mm256_xor_si256(innerCarry, rotate);

    _mm256_storeu_si256((__m256i *) carry_out, m256CarryOut);

    return true;
}

bool BitCalYmm::bitShiftRight512(uint64_t *data, int count, uint64_t *carry_out) {
    if (!data || !carry_out) {
        spdlog::error("BitCalYmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    if (count < 0 || count > 63) {
        spdlog::error("BitCalYmm::{} shift count is illegal!", __FUNCTION__);
        return false;
    }

    __m256i carryOut0{}, carryOut1{};
    bitShiftRight256(data, count, (uint64_t * ) & carryOut0);
    bitShiftRight256(data + 4, count, (uint64_t * ) & carryOut1);

    _mm256_storeu_si256((__m256i *) data, _mm256_or_si256(_mm256_lddqu_si256((__m256i *) data), carryOut1));
    _mm256_storeu_si256((__m256i *) carry_out, carryOut0);

    return true;
}

//============================================================================================================================
// bit And
//============================================================================================================================
bool BitCalYmm::bitAnd256(const uint64_t *data0, const uint64_t *data1, uint64_t *data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalYmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    _mm256_storeu_si256((__m256i *) data_out,
                        _mm256_and_si256(_mm256_lddqu_si256((__m256i *) data0), _mm256_lddqu_si256((__m256i *) data1)));

    return true;
}

bool BitCalYmm::bitAnd512(const uint64_t *data0, const uint64_t *data1, uint64_t *data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalYmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    _mm256_storeu_si256((__m256i *) data_out,
                        _mm256_and_si256(_mm256_lddqu_si256((__m256i *) data0), _mm256_lddqu_si256((__m256i *) data1)));
    _mm256_storeu_si256((__m256i *) data_out + 1, _mm256_and_si256(_mm256_lddqu_si256((__m256i *) data0 + 1),
                                                                   _mm256_lddqu_si256((__m256i *) data1 + 1)));

    return true;
}

//============================================================================================================================
// bit Or
//============================================================================================================================
bool BitCalYmm::bitOr256(const uint64_t *data0, const uint64_t *data1, uint64_t *data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalYmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    _mm256_storeu_si256((__m256i *) data_out,
                        _mm256_or_si256(_mm256_lddqu_si256((__m256i *) data0), _mm256_lddqu_si256((__m256i *) data1)));

    return true;
}

bool BitCalYmm::bitOr512(const uint64_t *data0, const uint64_t *data1, uint64_t *data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalYmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    _mm256_storeu_si256((__m256i *) data_out,
                        _mm256_or_si256(_mm256_lddqu_si256((__m256i *) data0), _mm256_lddqu_si256((__m256i *) data1)));
    _mm256_storeu_si256((__m256i *) data_out + 1, _mm256_or_si256(_mm256_lddqu_si256((__m256i *) data0 + 1),
                                                                  _mm256_lddqu_si256((__m256i *) data1 + 1)));

    return true;
}

//============================================================================================================================
// bit Xor
//============================================================================================================================
bool BitCalYmm::bitXor256(const uint64_t *data0, const uint64_t *data1, uint64_t *data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalYmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    _mm256_storeu_si256((__m256i *) data_out,
                        _mm256_xor_si256(_mm256_lddqu_si256((__m256i *) data0), _mm256_lddqu_si256((__m256i *) data1)));
    return true;
}

bool BitCalYmm::bitXor512(const uint64_t *data0, const uint64_t *data1, uint64_t *data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalYmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    bitXor256(data0, data1, data_out);
    bitXor256(data0 + 4, data1 + 4, data_out + 4);
    return true;
}

//============================================================================================================================
// bit pop_cnt
//============================================================================================================================
uint64_t BitCalYmm::bitPopcnt256(const uint64_t* data, uint8_t* cnt_arr) {
  if(!data) {
    spdlog::error("BitCalYmm::{} input data is illegal!", __FUNCTION__);
    return {};
  }

  cnt_arr[0] = _mm_popcnt_u64(data[0]);
  cnt_arr[1] = _mm_popcnt_u64(data[1]);
  cnt_arr[2] = _mm_popcnt_u64(data[2]);
  cnt_arr[3] = _mm_popcnt_u64(data[3]);

  return std::accumulate(cnt_arr, cnt_arr + 4, 0);
}

uint64_t BitCalYmm::bitPopcnt512(const uint64_t* data, uint8_t* cnt_arr) {
  if(!data) {
    spdlog::error("BitCalYmm::{} input data is illegal!", __FUNCTION__);
    return {};
  }

  cnt_arr[0] = _mm_popcnt_u64(data[0]);
  cnt_arr[1] = _mm_popcnt_u64(data[1]);
  cnt_arr[2] = _mm_popcnt_u64(data[2]);
  cnt_arr[3] = _mm_popcnt_u64(data[3]);
  cnt_arr[4] = _mm_popcnt_u64(data[4]);
  cnt_arr[5] = _mm_popcnt_u64(data[5]);
  cnt_arr[6] = _mm_popcnt_u64(data[6]);
  cnt_arr[7] = _mm_popcnt_u64(data[7]);

  return std::accumulate(cnt_arr, cnt_arr + 8, 0);
}
#endif
