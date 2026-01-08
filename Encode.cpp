#include "Encode.h"

#include "src/Utils/CheckCpu.h"
#include "EncodeGpr.h"
#include "EncodeYmm.h"

#include <cstdint>
#include <cstring>

Encode::Encode() {
#if defined(__x86_64__) || defined(_M_X64)
  if(CheckCpu::getInstance().avx2Available()) {
    m_encode = new EncodeYmm();
  } else {
    m_encode = new EncodeGpr();
  }
#else
  m_encode = new EncodeGpr();
#endif
}

Encode::~Encode() {
  if(m_encode != nullptr) {
    delete m_encode;
    m_encode = nullptr;
  }
};

//============================================================================================================================
// encode
//============================================================================================================================
bool Encode::encodeForward(uint64_t* data, size_t size, const char* base, size_t length) {
  return m_encode->encodeForward(data, size, base, length);
}

bool Encode::encodeReverse(uint64_t* data, size_t size, const char* base, size_t length) {
  return m_encode->encodeReverse(data, size, base, length);
}

//============================================================================================================================
// decode
//============================================================================================================================
bool Encode::decodeForward(const uint64_t* data, size_t size, char* base, size_t length) {
  return m_encode->decodeForward(data, size, base, length);
}

bool Encode::decodeReverse(const uint64_t* data, size_t size, char* base, size_t length) {
  return m_encode->decodeReverse(data, size, base, length);
}

bool Encode::decodeForward256(const uint64_t* data, char* base, size_t length) {
  return m_encode->decodeForward256(data, base, length);
}

bool Encode::decodeReverse256(const uint64_t* data, char* base, size_t length) {
  return m_encode->decodeReverse256(data, base, length);
}

bool Encode::decodeForward512(const uint64_t* data, char* base, size_t length) {
  return m_encode->decodeForward512(data, base, length);
}

bool Encode::decodeReverse512(const uint64_t* data, char* base, size_t length) {
  return m_encode->decodeReverse512(data, base, length);
}
