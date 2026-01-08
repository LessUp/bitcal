#include <numeric>
#include <string>
#ifdef __x86_64__
#include <emmintrin.h>
#elif __aarch64__
#include "sse2neon.h"
#endif
#include "BitCalXmm.h"
#include "spdlog/spdlog.h"

//============================================================================================================================
// bit shift left
//============================================================================================================================
bool BitCalXmm::bitShiftLeft128(uint64_t *data, int count, uint64_t *carryout) {
    if (!data || !carryout) {
        spdlog::error("BitCalXmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    if (count < 0 || count > 63) {
        spdlog::error("BitCalXmm::{} shift count is illegal!", __FUNCTION__);
        return false;
    }

    if (count == 0) {
        *(__m128i*)carryout = _mm_setzero_si128();
        return true;
    }

  __m128i innerCarry = _mm_srli_epi64(*(__m128i*)data, 64 - count);  // carry outs in bit 0 of each qword
  __m128i carryOut   = _mm_shuffle_epi32(innerCarry, 0xFE);  // upper carry in Xmm bit 0, others zero
  _mm_storeu_si128((__m128i*)carryout, carryOut);
  innerCarry = _mm_shuffle_epi32(innerCarry, 0xCF);  // lower carry in Xmm bit 64, others zero

    _mm_storeu_si128((__m128i *) data, _mm_or_si128(_mm_slli_epi64(*(__m128i *) data, count),
                                                    innerCarry));                // propagate carry out from low qword
    return true;
}

bool BitCalXmm::bitShiftLeft256(uint64_t *data, int count, uint64_t *carry_out) {
    if (!data || !carry_out) {
        spdlog::error("BitCalXmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    if (count < 0 || count > 63) {
        spdlog::error("BitCalXmm::{} shift count is illegal!", __FUNCTION__);
        return false;
    }

    __m128i carryOut0{}, carryOut1{};
    bitShiftLeft128(data, count, (uint64_t * ) & carryOut0);
    bitShiftLeft128(data + 2, count, (uint64_t * ) & carryOut1);
    _mm_storeu_si128((__m128i *) (data + 2), _mm_or_si128(_mm_load_si128((__m128i *) (data + 2)), carryOut0));

    _mm_storeu_si128((__m128i *) carry_out, carryOut1);

    return true;
}

bool BitCalXmm::bitShiftLeft512(uint64_t *data, int count, uint64_t *carry_out) {
    if (!data || !carry_out) {
        spdlog::error("BitCalXmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    if (count < 0 || count > 63) {
        spdlog::error("BitCalXmm::{} shift count is illegal!", __FUNCTION__);
        return false;
    }

    __m128i carryOut0{}, carryOut1{};
    bitShiftLeft256(data, count, (uint64_t * ) & carryOut0);
    bitShiftLeft256(data + 4, count, (uint64_t * ) & carryOut1);
    _mm_storeu_si128((__m128i *) (data + 4), _mm_or_si128(_mm_load_si128((__m128i *) (data + 4)), carryOut0));

    _mm_storeu_si128((__m128i *) carry_out, carryOut1);

    return true;
}

//============================================================================================================================
// bit shift right
//============================================================================================================================
bool BitCalXmm::bitShiftRight128(uint64_t *data, int count, uint64_t *carry_out) {
    if (!data) {
        spdlog::error("BitCalXmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    if (count < 0 || count > 63) {
        spdlog::error("BitCalXmm::{} shift count is illegal!", __FUNCTION__);
        return false;
    }

    if (count == 0) {
        *(__m128i *)carry_out = _mm_setzero_si128();
        return true;
    }

    __m128i innerCarry = _mm_slli_epi64(*(__m128i *) data, 64 - count);      // carry outs in bit 0 of each qword
    __m128i carryOut = _mm_shuffle_epi32(innerCarry, 0x40);               // lower carry in Xmm bit 64, others zero
    innerCarry = _mm_shuffle_epi32(innerCarry, 0x0E);                     // upper carry in Xmm bit 0, others zero
    _mm_storeu_si128((__m128i *) data, _mm_or_si128(_mm_srli_epi64(*(__m128i *) data, count), innerCarry));
    _mm_storeu_si128((__m128i *) carry_out, carryOut);

    return true;
}

bool BitCalXmm::bitShiftRight256(uint64_t *data, int count, uint64_t *carry_out) {
    if (!data || !carry_out) {
        spdlog::error("BitCalXmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    if (count < 0 || count > 63) {
        spdlog::error("BitCalXmm::{} shift count is illegal!", __FUNCTION__);
        return false;
    }

    __m128i carryOut0{}, carryOut1{};
    bitShiftRight128(data, count, (uint64_t * ) & carryOut0);
    bitShiftRight128(data + 2, count, (uint64_t * ) & carryOut1);
    _mm_storeu_si128((__m128i *) data, _mm_or_si128(*(__m128i *) data, carryOut1));
    _mm_storeu_si128((__m128i *) carry_out, carryOut0);

    return true;
}

bool BitCalXmm::bitShiftRight512(uint64_t *data, int count, uint64_t *carry_out) {
    if (!data || !carry_out) {
        spdlog::error("BitCalXmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    if (count < 0 || count > 63) {
        spdlog::error("BitCalXmm::{} shift count is illegal!", __FUNCTION__);
        return false;
    }

    __m128i carryOut0{}, carryOut1{};
    bitShiftRight256(data, count, (uint64_t * ) & carryOut0);
    bitShiftRight256(data + 4, count, (uint64_t * ) & carryOut1);
    _mm_storeu_si128((__m128i *) data + 1, _mm_or_si128(*(__m128i *) (data + 2), carryOut1));
    _mm_storeu_si128((__m128i *) carry_out, carryOut0);

    return true;
}

//============================================================================================================================
// bit And
//============================================================================================================================
bool BitCalXmm::bitAnd128(const uint64_t *data0, const uint64_t *data1, uint64_t *data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalXmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    _mm_storeu_si128((__m128i *) data_out, _mm_and_si128(*(__m128i *) data0, *(__m128i *) data1));
    return true;
}

bool BitCalXmm::bitAnd256(const uint64_t *data0, const uint64_t *data1, uint64_t *data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalXmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    _mm_storeu_si128((__m128i *) data_out, _mm_and_si128(*(__m128i *) data0, *(__m128i *) data1));
    _mm_storeu_si128((__m128i *) (data_out + 2), _mm_and_si128(*(__m128i *) (data0 + 2), *(__m128i *) (data1 + 2)));
    return true;
}

bool BitCalXmm::bitAnd512(const uint64_t *data0, const uint64_t *data1, uint64_t *data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalXmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    bitAnd256(data0, data1, data_out);
    bitAnd256(data0 + 4, data1 + 4, data_out + 4);
    return true;
}

//============================================================================================================================
// bit Or
//============================================================================================================================
bool BitCalXmm::bitOr128(const uint64_t *data0, const uint64_t *data1, uint64_t *data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalXmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    _mm_storeu_si128((__m128i *) data_out, _mm_or_si128(*(__m128i *) data0, *(__m128i *) data1));
    return true;
}

bool BitCalXmm::bitOr256(const uint64_t *data0, const uint64_t *data1, uint64_t *data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalXmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    bitOr128(data0, data1, data_out);
    bitOr128(data0 + 2, data1 + 2, data_out + 2);
    return true;
}

bool BitCalXmm::bitOr512(const uint64_t *data0, const uint64_t *data1, uint64_t *data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalXmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    bitOr256(data0, data1, data_out);
    bitOr256(data0 + 4, data1 + 4, data_out + 4);
    return true;
}

//============================================================================================================================
// bit Xor
//============================================================================================================================
bool BitCalXmm::bitXor128(const uint64_t *data0, const uint64_t *data1, uint64_t *data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalXmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    _mm_storeu_si128((__m128i *) data_out, _mm_xor_si128(*(__m128i *) data0, *(__m128i *) data1));
    return true;
}

bool BitCalXmm::bitXor256(const uint64_t *data0, const uint64_t *data1, uint64_t *data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalXmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    bitXor128(data0, data1, data_out);
    bitXor128(data0 + 2, data1 + 2, data_out + 2);
    return true;
}

bool BitCalXmm::bitXor512(const uint64_t *data0, const uint64_t *data1, uint64_t *data_out) {
    if (!data0 || !data1 || !data_out) {
        spdlog::error("BitCalXmm::{} input data is illegal!", __FUNCTION__);
        return false;
    }

    bitXor256(data0, data1, data_out);
    bitXor256(data0 + 4, data1 + 4, data_out + 4);
    return true;
}

//============================================================================================================================
// bit pop_cnt
//============================================================================================================================
uint64_t BitCalXmm::bitPopcnt128(const uint64_t* data, uint8_t* cnt_arr) {
  if(!data) {
    spdlog::error("BitCalXmm::{} input data is illegal!", __FUNCTION__);
    return {};
  }

  cnt_arr[0] = _mm_popcnt_u64(data[0]);
  cnt_arr[1] = _mm_popcnt_u64(data[1]);

  return std::accumulate(cnt_arr, cnt_arr + 2, 0);
}

uint64_t BitCalXmm::bitPopcnt256(const uint64_t* data, uint8_t* cnt_arr) {
  if(!data) {
    spdlog::error("BitCalXmm::{} input data is illegal!", __FUNCTION__);
    return {};
  }

  cnt_arr[0] = _mm_popcnt_u64(data[0]);
  cnt_arr[1] = _mm_popcnt_u64(data[1]);
  cnt_arr[2] = _mm_popcnt_u64(data[2]);
  cnt_arr[3] = _mm_popcnt_u64(data[3]);

  return std::accumulate(cnt_arr, cnt_arr + 4, 0);
}

uint64_t BitCalXmm::bitPopcnt512(const uint64_t* data, uint8_t* cnt_arr) {
  if(!data) {
    spdlog::error("BitCalXmm::{} input data is illegal!", __FUNCTION__);
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