#include "Encode.h"

#include <cstdint>
#include <cstring>

Encode::Encode() {
}

//============================================================================================================================
// encode
//============================================================================================================================
bool Encode::encodeForward(uint64_t* data, size_t size, const char* base, size_t length) {
  return bitencode::encodeForward(data, size, base, length);
}

bool Encode::encodeReverse(uint64_t* data, size_t size, const char* base, size_t length) {
  return bitencode::encodeReverse(data, size, base, length);
}

//============================================================================================================================
// decode
//============================================================================================================================
bool Encode::decodeForward(const uint64_t* data, size_t size, char* base, size_t length) {
  return bitencode::decodeForward(data, size, base, length);
}

bool Encode::decodeReverse(const uint64_t* data, size_t size, char* base, size_t length) {
  return bitencode::decodeReverse(data, size, base, length);
}

bool Encode::decodeForward256(const uint64_t* data, char* base, size_t length) {
  return bitencode::decodeForward256(data, base, length);
}

bool Encode::decodeReverse256(const uint64_t* data, char* base, size_t length) {
  return bitencode::decodeReverse256(data, base, length);
}

bool Encode::decodeForward512(const uint64_t* data, char* base, size_t length) {
  return bitencode::decodeForward512(data, base, length);
}

bool Encode::decodeReverse512(const uint64_t* data, char* base, size_t length) {
  return bitencode::decodeReverse512(data, base, length);
}
