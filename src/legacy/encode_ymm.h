#ifndef HYBRIDENGINE_ENCODEYMM_H
#define HYBRIDENGINE_ENCODEYMM_H

#include <cstdint>
#include <cstdio>
#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#elif defined(__aarch64__) || defined(_M_ARM64)
#include <arm_neon.h>
#endif
#include "Base.h"

class EncodeYmm : public EncodeBase {
public:
  EncodeYmm()  = default;

  ~EncodeYmm() = default;

  //============================================================================================================================
  // encode
  //============================================================================================================================
  bool encodeForward(uint64_t* data, size_t size, const char* base, size_t length) override;

  bool encodeReverse(uint64_t* data, size_t size, const char* base, size_t length) override;

  //============================================================================================================================
  // decode
  //============================================================================================================================
  bool decodeForward256(const uint64_t* data, char* base, size_t length) override;

  bool decodeReverse256(const uint64_t* data, char* base, size_t length) override;

  bool decodeForward512(const uint64_t* data, char* base, size_t length) override;

  bool decodeReverse512(const uint64_t* data, char* base, size_t length) override;
};

#endif// HYBRIDENGINE_ENCODEYMM_H
