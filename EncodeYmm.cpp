#ifdef __x86_64__
#include "EncodeYmm.h"

#include "src/Base/Helper.h"
#include "src/Utils/CheckCpu.h"
#include "BitCal.h"
#include "spdlog/spdlog.h"

#include <cstdint>
#include <cstring>
#include <immintrin.h>
//============================================================================================================================
// encode
//============================================================================================================================
bool EncodeYmm::encodeForward(uint64_t* data, size_t size, const char* base, size_t length) {
    if(!data || !base) {
        spdlog::error("EncodeYmm::{} data or base is nullptr!", __FUNCTION__);
        return false;
    }

    if(size * 32 < length) {
        spdlog::error("EncodeYmm::{} data size is mismatch with base length!", __FUNCTION__);
        return false;
    }

    uint64_t _32c = length / 32;
    uint64_t _32d = length % 32;
    int      i    = 0;
    memset(data, 0, size * sizeof(uint64_t));

    __m256i tmp_base[4];
    __m256i bit_mask = _mm256_set1_epi8(0x03);
    __m256i idx1     = _mm256_set_epi32(
        0x80808080, 0x80808080, 0x0C080400, 0x0C080400, 0x80808080, 0x80808080, 0x0C080400, 0x0C080400);
    __m256i idx2 = _mm256_set_epi32(0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00);

    for(i = 0; i <= _32c; ++i) {
        tmp_base[0] = _mm256_loadu_si256(((__m256i*)base) + i);

        tmp_base[0] = _mm256_srli_epi64(tmp_base[0], 1);
        tmp_base[0] = _mm256_and_si256(tmp_base[0], bit_mask);

        tmp_base[1] = _mm256_srli_epi64(tmp_base[0], 6);
        tmp_base[2] = _mm256_srli_epi64(tmp_base[0], 12);
        tmp_base[3] = _mm256_srli_epi64(tmp_base[0], 18);

        tmp_base[0] = _mm256_or_si256(tmp_base[0], tmp_base[1]);
        tmp_base[0] = _mm256_or_si256(tmp_base[0], tmp_base[2]);
        tmp_base[0] = _mm256_or_si256(tmp_base[0], tmp_base[3]);

        tmp_base[0] = _mm256_shuffle_epi8(tmp_base[0], idx1);
        tmp_base[0] = _mm256_permutevar8x32_epi32(tmp_base[0], idx2);

        memcpy(&data[i], tmp_base, sizeof(uint64_t));
    }
    if(_32d) {
        uint64_t _32d_mask = ((uint64_t)1 << (_32d * 2)) - 1;
        data[i - 1] &= _32d_mask;
    }

    return true;
}

bool EncodeYmm::encodeReverse(uint64_t* data, size_t size, const char* base, size_t length) {
    if(!data || !base) {
        spdlog::error("EncodeYmm::{} data or base is nullptr!", __FUNCTION__);
        return false;
    }

    if(size * 32 < length) {
        spdlog::error("EncodeYmm::{} data size is mismatch with base length!", __FUNCTION__);
        return false;
    }

    uint64_t _32c = length / 32;
    uint64_t _32d = length % 32;
    int      i    = 0;
    memset(data, 0, size * sizeof(uint64_t));

    __m256i tmp_base[4];
    __m256i bit_addi = _mm256_set1_epi8(0x04);
    __m256i bit_mask = _mm256_set1_epi8(0xC0);
    __m256i idx1     = _mm256_set_epi32(
        0x80808080, 0x80808080, 0x0004080C, 0x0004080C, 0x80808080, 0x80808080, 0x0004080C, 0x0004080C);
    __m256i idx2 = _mm256_set_epi32(0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04);

    for(i = _32c; i >= 0; i--) {
        if(unlikely(i == 0)) {
            memcpy((char*)tmp_base + 32 - _32d, base, sizeof(char) * _32d);
        } else {
            tmp_base[0] = _mm256_loadu_si256(((__m256i*)(base + _32d)) + i - 1);
        }

        tmp_base[0] = _mm256_add_epi8(tmp_base[0], bit_addi);
        tmp_base[0] = _mm256_slli_epi64(tmp_base[0], 5);
        tmp_base[0] = _mm256_and_si256(tmp_base[0], bit_mask);

        tmp_base[1] = _mm256_srli_epi64(tmp_base[0], 10);
        tmp_base[2] = _mm256_srli_epi64(tmp_base[0], 20);
        tmp_base[3] = _mm256_srli_epi64(tmp_base[0], 30);

        tmp_base[0] = _mm256_or_si256(tmp_base[0], tmp_base[1]);
        tmp_base[0] = _mm256_or_si256(tmp_base[0], tmp_base[2]);
        tmp_base[0] = _mm256_or_si256(tmp_base[0], tmp_base[3]);

        tmp_base[0] = _mm256_shuffle_epi8(tmp_base[0], idx1);
        tmp_base[0] = _mm256_permutevar8x32_epi32(tmp_base[0], idx2);

        memcpy(&data[_32c - i], tmp_base, sizeof(uint64_t));
    }
    if(_32d) {
        uint64_t _32d_mask = ((uint64_t)1 << (_32d * 2)) - 1;
        data[_32c] &= _32d_mask;
    }

    return true;
}

//============================================================================================================================
// decode
//============================================================================================================================
bool EncodeYmm::decodeForward256(const uint64_t* data, char* base, size_t length) {
    if(!data || !base) {
        spdlog::error("EncodeYmm::{} data or base is nullptr!", __FUNCTION__);
        return false;
    }

    if(length > 128) {
        spdlog::error("EncodeYmm::{} data size is mismatch with base length!", __FUNCTION__);
        return false;
    }

    uint64_t index[4] = { 0x4754434147544341, 0x4754434147544341, 0x4754434147544341, 0x4754434147544341 };
    uint64_t mask[4]  = { 0x7f7f7f7f7f7f7f7f, 0x7f7f7f7f7f7f7f7f, 0x7f7f7f7f7f7f7f7f, 0x7f7f7f7f7f7f7f7f };
    __m256i  m_index  = _mm256_lddqu_si256((__m256i*)index);
    __m256i  m_mask   = _mm256_lddqu_si256((__m256i*)mask);
    __m256i  m_data   = _mm256_lddqu_si256((__m256i*)data);

    __m256i src_data = _mm256_and_si256(m_data, m_mask);
    __m256i base1    = _mm256_shuffle_epi8(m_index, src_data);
    __m256i dst_data = _mm256_srli_epi16(m_data, 2);
    src_data         = _mm256_and_si256(dst_data, m_mask);
    __m256i base2    = _mm256_shuffle_epi8(m_index, src_data);
    dst_data         = _mm256_srli_epi16(dst_data, 2);
    src_data         = _mm256_and_si256(dst_data, m_mask);
    __m256i base3    = _mm256_shuffle_epi8(m_index, src_data);
    dst_data         = _mm256_srli_epi16(dst_data, 2);
    src_data         = _mm256_and_si256(dst_data, m_mask);
    __m256i base4    = _mm256_shuffle_epi8(m_index, src_data);

    for(size_t i = 0, j = 0; j < length; i++) {
        base[j++] = ((char*)&base1)[i];
        if(j == length) break;  // should not change over range data
        base[j++] = ((char*)&base2)[i];
        if(j == length) break;
        base[j++] = ((char*)&base3)[i];
        if(j == length) break;
        base[j++] = ((char*)&base4)[i];
    }
    return true;
}

bool EncodeYmm::decodeReverse256(const uint64_t* data, char* base, size_t length) {
    if(!data || !base) {
        spdlog::error("EncodeYmm::{} data or base is nullptr!", __FUNCTION__);
        return false;
    }

    // only decode within data's length=4 (bit length == 256)
    if(length > 128) {
        spdlog::error("EncodeYmm::{} data size is mismatch with base length!", __FUNCTION__);
        return false;
    }

    uint64_t index[4] = { 0x4341475443414754, 0x4341475443414754, 0x4341475443414754, 0x4341475443414754 };
    uint64_t mask[4]  = { 0x7f7f7f7f7f7f7f7f, 0x7f7f7f7f7f7f7f7f, 0x7f7f7f7f7f7f7f7f, 0x7f7f7f7f7f7f7f7f };

    __m256i m_index = _mm256_lddqu_si256((__m256i*)index);
    __m256i m_mask  = _mm256_lddqu_si256((__m256i*)mask);
    __m256i m_data  = _mm256_lddqu_si256((__m256i*)data);

    __m256i src_data = _mm256_and_si256(m_data, m_mask);
    __m256i base1    = _mm256_shuffle_epi8(m_index, src_data);
    __m256i dst_data = _mm256_srli_epi16(m_data, 2);
    src_data         = _mm256_and_si256(dst_data, m_mask);
    __m256i base2    = _mm256_shuffle_epi8(m_index, src_data);
    dst_data         = _mm256_srli_epi16(dst_data, 2);
    src_data         = _mm256_and_si256(dst_data, m_mask);
    __m256i base3    = _mm256_shuffle_epi8(m_index, src_data);
    dst_data         = _mm256_srli_epi16(dst_data, 2);
    src_data         = _mm256_and_si256(dst_data, m_mask);
    __m256i base4    = _mm256_shuffle_epi8(m_index, src_data);

    for(size_t i = 0, j = length - 1; static_cast<signed long>(j) >= 0; i++) {
        base[j--] = ((char*)&base1)[i];
        if(static_cast<signed long>(j) < 0) break;
        base[j--] = ((char*)&base2)[i];
        if(static_cast<signed long>(j) < 0) break;
        base[j--] = ((char*)&base3)[i];
        if(static_cast<signed long>(j) < 0) break;
        base[j--] = ((char*)&base4)[i];
    }
    return true;
}

bool EncodeYmm::decodeForward512(const uint64_t* data, char* base, size_t length) {
    if(length <= 128) {
        return decodeForward256(data, base, length);
    } else {
        return decodeForward256(data, base, 128) && decodeForward256(data + 4, base + 128, length - 128);
    }
}

bool EncodeYmm::decodeReverse512(const uint64_t* data, char* base, size_t length) {
    if(length <= 128) {
        return decodeReverse256(data, base, length);
    } else {
        auto char_offset = length - 128;
        return decodeReverse256(data + 4, base, char_offset) && decodeReverse256(data, base + char_offset, 128);
    }
}
#endif
