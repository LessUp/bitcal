#pragma once

#include <cstddef>
#include <cstdint>

struct EncodeDispatchTable {
  bool (*encodeForward)(uint64_t* data, size_t size, const char* base, size_t length);
  bool (*encodeReverse)(uint64_t* data, size_t size, const char* base, size_t length);

  bool (*decodeForward)(const uint64_t* data, size_t size, char* base, size_t length);
  bool (*decodeReverse)(const uint64_t* data, size_t size, char* base, size_t length);

  bool (*decodeForward256)(const uint64_t* data, char* base, size_t length);
  bool (*decodeReverse256)(const uint64_t* data, char* base, size_t length);

  bool (*decodeForward512)(const uint64_t* data, char* base, size_t length);
  bool (*decodeReverse512)(const uint64_t* data, char* base, size_t length);
};

const EncodeDispatchTable& encodeGetDispatch();

namespace bitencode {

bool encodeForward(uint64_t* data, size_t size, const char* base, size_t length);
bool encodeReverse(uint64_t* data, size_t size, const char* base, size_t length);

bool decodeForward(const uint64_t* data, size_t size, char* base, size_t length);
bool decodeReverse(const uint64_t* data, size_t size, char* base, size_t length);

bool decodeForward256(const uint64_t* data, char* base, size_t length);
bool decodeReverse256(const uint64_t* data, char* base, size_t length);

bool decodeForward512(const uint64_t* data, char* base, size_t length);
bool decodeReverse512(const uint64_t* data, char* base, size_t length);

}
