#include "encode_dispatch.h"

#include "EncodeGpr.h"

#if defined(__x86_64__) || defined(_M_X64)
#include "CheckCpu.h"
#include "EncodeYmm.h"
#endif

static EncodeGpr g_gpr;

#if defined(__x86_64__) || defined(_M_X64)
static EncodeYmm g_ymm;
#endif

static bool gpr_encodeForward(uint64_t* data, size_t size, const char* base, size_t length) { return g_gpr.encodeForward(data, size, base, length); }
static bool gpr_encodeReverse(uint64_t* data, size_t size, const char* base, size_t length) { return g_gpr.encodeReverse(data, size, base, length); }

static bool gpr_decodeForward(const uint64_t* data, size_t size, char* base, size_t length) { return g_gpr.decodeForward(data, size, base, length); }
static bool gpr_decodeReverse(const uint64_t* data, size_t size, char* base, size_t length) { return g_gpr.decodeReverse(data, size, base, length); }

static bool gpr_decodeForward256(const uint64_t* data, char* base, size_t length) { return g_gpr.decodeForward256(data, base, length); }
static bool gpr_decodeReverse256(const uint64_t* data, char* base, size_t length) { return g_gpr.decodeReverse256(data, base, length); }

static bool gpr_decodeForward512(const uint64_t* data, char* base, size_t length) { return g_gpr.decodeForward512(data, base, length); }
static bool gpr_decodeReverse512(const uint64_t* data, char* base, size_t length) { return g_gpr.decodeReverse512(data, base, length); }

#if defined(__x86_64__) || defined(_M_X64)
static bool ymm_encodeForward(uint64_t* data, size_t size, const char* base, size_t length) { return g_ymm.encodeForward(data, size, base, length); }
static bool ymm_encodeReverse(uint64_t* data, size_t size, const char* base, size_t length) { return g_ymm.encodeReverse(data, size, base, length); }

static bool ymm_decodeForward256(const uint64_t* data, char* base, size_t length) { return g_ymm.decodeForward256(data, base, length); }
static bool ymm_decodeReverse256(const uint64_t* data, char* base, size_t length) { return g_ymm.decodeReverse256(data, base, length); }

static bool ymm_decodeForward512(const uint64_t* data, char* base, size_t length) { return g_ymm.decodeForward512(data, base, length); }
static bool ymm_decodeReverse512(const uint64_t* data, char* base, size_t length) { return g_ymm.decodeReverse512(data, base, length); }
#endif

static EncodeDispatchTable makeGprTable() {
  EncodeDispatchTable t{};

  t.encodeForward = gpr_encodeForward;
  t.encodeReverse = gpr_encodeReverse;

  t.decodeForward = gpr_decodeForward;
  t.decodeReverse = gpr_decodeReverse;

  t.decodeForward256 = gpr_decodeForward256;
  t.decodeReverse256 = gpr_decodeReverse256;

  t.decodeForward512 = gpr_decodeForward512;
  t.decodeReverse512 = gpr_decodeReverse512;

  return t;
}

static EncodeDispatchTable makeRuntimeTable() {
  EncodeDispatchTable t = makeGprTable();

#if defined(__x86_64__) || defined(_M_X64)
  if(CheckCpu::getInstance().avx2Available()) {
    t.encodeForward = ymm_encodeForward;
    t.encodeReverse = ymm_encodeReverse;

    t.decodeForward256 = ymm_decodeForward256;
    t.decodeReverse256 = ymm_decodeReverse256;

    t.decodeForward512 = ymm_decodeForward512;
    t.decodeReverse512 = ymm_decodeReverse512;
  }
#endif

  return t;
}

const EncodeDispatchTable& encodeGetDispatch() {
  static EncodeDispatchTable table = makeRuntimeTable();
  return table;
}

namespace bitencode {

bool encodeForward(uint64_t* data, size_t size, const char* base, size_t length) {
  return encodeGetDispatch().encodeForward(data, size, base, length);
}

bool encodeReverse(uint64_t* data, size_t size, const char* base, size_t length) {
  return encodeGetDispatch().encodeReverse(data, size, base, length);
}

bool decodeForward(const uint64_t* data, size_t size, char* base, size_t length) {
  return encodeGetDispatch().decodeForward(data, size, base, length);
}

bool decodeReverse(const uint64_t* data, size_t size, char* base, size_t length) {
  return encodeGetDispatch().decodeReverse(data, size, base, length);
}

bool decodeForward256(const uint64_t* data, char* base, size_t length) {
  return encodeGetDispatch().decodeForward256(data, base, length);
}

bool decodeReverse256(const uint64_t* data, char* base, size_t length) {
  return encodeGetDispatch().decodeReverse256(data, base, length);
}

bool decodeForward512(const uint64_t* data, char* base, size_t length) {
  return encodeGetDispatch().decodeForward512(data, base, length);
}

bool decodeReverse512(const uint64_t* data, char* base, size_t length) {
  return encodeGetDispatch().decodeReverse512(data, base, length);
}

}
