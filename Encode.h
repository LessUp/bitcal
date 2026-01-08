#ifndef HYBRIDENGINE_ENCODE_H
#define HYBRIDENGINE_ENCODE_H

#include "Base.h"

#include <cstddef>
#include <cstdint>

class Encode {
private:
  EncodeBase* m_encode = nullptr;

private:
  Encode();

  Encode(const Encode& other)                  = delete;

  const Encode& operator=(const Encode& other) = delete;

public:
  static Encode& getInstance() {
    static Encode instance;
    return instance;
  };

  ~Encode();

  //============================================================================================================================
  // encode
  //============================================================================================================================
  bool encodeForward(uint64_t* data, size_t size, const char* base, size_t length);

  bool encodeReverse(uint64_t* data, size_t size, const char* base, size_t length);
  //============================================================================================================================
  // decode
  //============================================================================================================================
  bool decodeForward(const uint64_t* data, size_t size, char* base, size_t length);

  bool decodeReverse(const uint64_t* data, size_t size, char* base, size_t length);

  bool decodeForward256(const uint64_t* data, char* base, size_t length);

  bool decodeReverse256(const uint64_t* data, char* base, size_t length);

  bool decodeForward512(const uint64_t* data, char* base, size_t length);

  bool decodeReverse512(const uint64_t* data, char* base, size_t length);
};

#endif// HYBRIDENGINE_ENCODE_H
