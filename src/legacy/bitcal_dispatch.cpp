#include "bitcal_dispatch.h"

#include "bit_cal_gpr.h"

#if defined(__x86_64__) || defined(_M_X64)
#include "bit_cal_xmm.h"
#include "bit_cal_ymm.h"
#include "check_cpu.h"
#endif

static BitCalGpr g_gpr;

#if defined(__x86_64__) || defined(_M_X64)
static BitCalXmm g_xmm;
static BitCalYmm g_ymm;
#endif

static uint64_t gpr_bitShiftLeft64(uint64_t* data, int count) { return g_gpr.bitShiftLeft64(data, count); }
static bool gpr_bitShiftLeft128(uint64_t* data, int count, uint64_t* carryout) { return g_gpr.bitShiftLeft128(data, count, carryout); }
static bool gpr_bitShiftLeft256(uint64_t* data, int count, uint64_t* carryout) { return g_gpr.bitShiftLeft256(data, count, carryout); }
static bool gpr_bitShiftLeft512(uint64_t* data, int count, uint64_t* carryout) { return g_gpr.bitShiftLeft512(data, count, carryout); }

static uint64_t gpr_bitShiftRight64(uint64_t* data, int count) { return g_gpr.bitShiftRight64(data, count); }
static bool gpr_bitShiftRight128(uint64_t* data, int count, uint64_t* carryout) { return g_gpr.bitShiftRight128(data, count, carryout); }
static bool gpr_bitShiftRight256(uint64_t* data, int count, uint64_t* carryout) { return g_gpr.bitShiftRight256(data, count, carryout); }
static bool gpr_bitShiftRight512(uint64_t* data, int count, uint64_t* carryout) { return g_gpr.bitShiftRight512(data, count, carryout); }

static uint64_t gpr_bitAnd64(uint64_t data0, uint64_t data1) { return g_gpr.bitAnd64(data0, data1); }
static bool gpr_bitAnd128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_gpr.bitAnd128(data0, data1, data_out); }
static bool gpr_bitAnd256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_gpr.bitAnd256(data0, data1, data_out); }
static bool gpr_bitAnd512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_gpr.bitAnd512(data0, data1, data_out); }

static uint64_t gpr_bitOr64(uint64_t data0, uint64_t data1) { return g_gpr.bitOr64(data0, data1); }
static bool gpr_bitOr128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_gpr.bitOr128(data0, data1, data_out); }
static bool gpr_bitOr256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_gpr.bitOr256(data0, data1, data_out); }
static bool gpr_bitOr512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_gpr.bitOr512(data0, data1, data_out); }

static uint64_t gpr_bitXor64(uint64_t data0, uint64_t data1) { return g_gpr.bitXor64(data0, data1); }
static bool gpr_bitXor128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_gpr.bitXor128(data0, data1, data_out); }
static bool gpr_bitXor256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_gpr.bitXor256(data0, data1, data_out); }
static bool gpr_bitXor512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_gpr.bitXor512(data0, data1, data_out); }

static uint64_t gpr_bitPopcnt64(uint64_t data) { return g_gpr.bitPopcnt64(data); }
static uint64_t gpr_bitPopcnt128(const uint64_t* data, uint8_t* cnt_arr) { return g_gpr.bitPopcnt128(data, cnt_arr); }
static uint64_t gpr_bitPopcnt256(const uint64_t* data, uint8_t* cnt_arr) { return g_gpr.bitPopcnt256(data, cnt_arr); }
static uint64_t gpr_bitPopcnt512(const uint64_t* data, uint8_t* cnt_arr) { return g_gpr.bitPopcnt512(data, cnt_arr); }

static uint64_t gpr_bitReverse64(uint64_t data) { return g_gpr.bitReverse64(data); }
static bool gpr_bitReverse128(uint64_t* dataIn, uint64_t* dataOut) { return g_gpr.bitReverse128(dataIn, dataOut); }
static bool gpr_bitReverse256(uint64_t* dataIn, uint64_t* dataOut) { return g_gpr.bitReverse256(dataIn, dataOut); }
static bool gpr_bitReverse512(uint64_t* dataIn, uint64_t* dataOut) { return g_gpr.bitReverse512(dataIn, dataOut); }

static bool gpr_bitReverseInPlace128(uint64_t* data) { return g_gpr.bitReverseInPlace128(data); }
static bool gpr_bitReverseInPlace256(uint64_t* data) { return g_gpr.bitReverseInPlace256(data); }
static bool gpr_bitReverseInPlace512(uint64_t* data) { return g_gpr.bitReverseInPlace512(data); }

#if defined(__x86_64__) || defined(_M_X64)
static bool xmm_bitShiftLeft128(uint64_t* data, int count, uint64_t* carryout) { return g_xmm.bitShiftLeft128(data, count, carryout); }
static bool xmm_bitShiftLeft256(uint64_t* data, int count, uint64_t* carryout) { return g_xmm.bitShiftLeft256(data, count, carryout); }
static bool xmm_bitShiftLeft512(uint64_t* data, int count, uint64_t* carryout) { return g_xmm.bitShiftLeft512(data, count, carryout); }

static bool xmm_bitShiftRight128(uint64_t* data, int count, uint64_t* carryout) { return g_xmm.bitShiftRight128(data, count, carryout); }
static bool xmm_bitShiftRight256(uint64_t* data, int count, uint64_t* carryout) { return g_xmm.bitShiftRight256(data, count, carryout); }
static bool xmm_bitShiftRight512(uint64_t* data, int count, uint64_t* carryout) { return g_xmm.bitShiftRight512(data, count, carryout); }

static bool xmm_bitAnd128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_xmm.bitAnd128(data0, data1, data_out); }
static bool xmm_bitAnd256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_xmm.bitAnd256(data0, data1, data_out); }
static bool xmm_bitAnd512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_xmm.bitAnd512(data0, data1, data_out); }

static bool xmm_bitOr128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_xmm.bitOr128(data0, data1, data_out); }
static bool xmm_bitOr256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_xmm.bitOr256(data0, data1, data_out); }
static bool xmm_bitOr512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_xmm.bitOr512(data0, data1, data_out); }

static bool xmm_bitXor128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_xmm.bitXor128(data0, data1, data_out); }
static bool xmm_bitXor256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_xmm.bitXor256(data0, data1, data_out); }
static bool xmm_bitXor512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_xmm.bitXor512(data0, data1, data_out); }

static uint64_t xmm_bitPopcnt128(const uint64_t* data, uint8_t* cnt_arr) { return g_xmm.bitPopcnt128(data, cnt_arr); }
static uint64_t xmm_bitPopcnt256(const uint64_t* data, uint8_t* cnt_arr) { return g_xmm.bitPopcnt256(data, cnt_arr); }
static uint64_t xmm_bitPopcnt512(const uint64_t* data, uint8_t* cnt_arr) { return g_xmm.bitPopcnt512(data, cnt_arr); }

static bool ymm_bitShiftLeft256(uint64_t* data, int count, uint64_t* carryout) { return g_ymm.bitShiftLeft256(data, count, carryout); }
static bool ymm_bitShiftLeft512(uint64_t* data, int count, uint64_t* carryout) { return g_ymm.bitShiftLeft512(data, count, carryout); }

static bool ymm_bitShiftRight256(uint64_t* data, int count, uint64_t* carryout) { return g_ymm.bitShiftRight256(data, count, carryout); }
static bool ymm_bitShiftRight512(uint64_t* data, int count, uint64_t* carryout) { return g_ymm.bitShiftRight512(data, count, carryout); }

static bool ymm_bitAnd256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_ymm.bitAnd256(data0, data1, data_out); }
static bool ymm_bitAnd512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_ymm.bitAnd512(data0, data1, data_out); }

static bool ymm_bitOr256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_ymm.bitOr256(data0, data1, data_out); }
static bool ymm_bitOr512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_ymm.bitOr512(data0, data1, data_out); }

static bool ymm_bitXor256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_ymm.bitXor256(data0, data1, data_out); }
static bool ymm_bitXor512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return g_ymm.bitXor512(data0, data1, data_out); }

static uint64_t ymm_bitPopcnt256(const uint64_t* data, uint8_t* cnt_arr) { return g_ymm.bitPopcnt256(data, cnt_arr); }
static uint64_t ymm_bitPopcnt512(const uint64_t* data, uint8_t* cnt_arr) { return g_ymm.bitPopcnt512(data, cnt_arr); }
#endif

static BitCalDispatchTable makeGprTable() {
  BitCalDispatchTable t{};

  t.bitShiftLeft64 = gpr_bitShiftLeft64;
  t.bitShiftLeft128 = gpr_bitShiftLeft128;
  t.bitShiftLeft256 = gpr_bitShiftLeft256;
  t.bitShiftLeft512 = gpr_bitShiftLeft512;

  t.bitShiftRight64 = gpr_bitShiftRight64;
  t.bitShiftRight128 = gpr_bitShiftRight128;
  t.bitShiftRight256 = gpr_bitShiftRight256;
  t.bitShiftRight512 = gpr_bitShiftRight512;

  t.bitAnd64 = gpr_bitAnd64;
  t.bitAnd128 = gpr_bitAnd128;
  t.bitAnd256 = gpr_bitAnd256;
  t.bitAnd512 = gpr_bitAnd512;

  t.bitOr64 = gpr_bitOr64;
  t.bitOr128 = gpr_bitOr128;
  t.bitOr256 = gpr_bitOr256;
  t.bitOr512 = gpr_bitOr512;

  t.bitXor64 = gpr_bitXor64;
  t.bitXor128 = gpr_bitXor128;
  t.bitXor256 = gpr_bitXor256;
  t.bitXor512 = gpr_bitXor512;

  t.bitPopcnt64 = gpr_bitPopcnt64;
  t.bitPopcnt128 = gpr_bitPopcnt128;
  t.bitPopcnt256 = gpr_bitPopcnt256;
  t.bitPopcnt512 = gpr_bitPopcnt512;

  t.bitReverse64 = gpr_bitReverse64;
  t.bitReverse128 = gpr_bitReverse128;
  t.bitReverse256 = gpr_bitReverse256;
  t.bitReverse512 = gpr_bitReverse512;

  t.bitReverseInPlace128 = gpr_bitReverseInPlace128;
  t.bitReverseInPlace256 = gpr_bitReverseInPlace256;
  t.bitReverseInPlace512 = gpr_bitReverseInPlace512;

  return t;
}

static BitCalDispatchTable makeRuntimeTable() {
  BitCalDispatchTable t = makeGprTable();

#if defined(__x86_64__) || defined(_M_X64)
  if(CheckCpu::getInstance().avxAvailable()) {
    t.bitShiftLeft128 = xmm_bitShiftLeft128;
    t.bitShiftLeft256 = xmm_bitShiftLeft256;
    t.bitShiftLeft512 = xmm_bitShiftLeft512;

    t.bitShiftRight128 = xmm_bitShiftRight128;
    t.bitShiftRight256 = xmm_bitShiftRight256;
    t.bitShiftRight512 = xmm_bitShiftRight512;

    t.bitAnd128 = xmm_bitAnd128;
    t.bitAnd256 = xmm_bitAnd256;
    t.bitAnd512 = xmm_bitAnd512;

    t.bitOr128 = xmm_bitOr128;
    t.bitOr256 = xmm_bitOr256;
    t.bitOr512 = xmm_bitOr512;

    t.bitXor128 = xmm_bitXor128;
    t.bitXor256 = xmm_bitXor256;
    t.bitXor512 = xmm_bitXor512;

    t.bitPopcnt128 = xmm_bitPopcnt128;
    t.bitPopcnt256 = xmm_bitPopcnt256;
    t.bitPopcnt512 = xmm_bitPopcnt512;
  }

  if(CheckCpu::getInstance().avx2Available()) {
    t.bitShiftLeft256 = ymm_bitShiftLeft256;
    t.bitShiftLeft512 = ymm_bitShiftLeft512;

    t.bitShiftRight256 = ymm_bitShiftRight256;
    t.bitShiftRight512 = ymm_bitShiftRight512;

    t.bitAnd256 = ymm_bitAnd256;
    t.bitAnd512 = ymm_bitAnd512;

    t.bitOr256 = ymm_bitOr256;
    t.bitOr512 = ymm_bitOr512;

    t.bitXor256 = ymm_bitXor256;
    t.bitXor512 = ymm_bitXor512;

    t.bitPopcnt256 = ymm_bitPopcnt256;
    t.bitPopcnt512 = ymm_bitPopcnt512;
  }
#endif

  return t;
}

const BitCalDispatchTable& bitcalGetDispatch() {
  static BitCalDispatchTable table = makeRuntimeTable();
  return table;
}

namespace bitcal {

uint64_t bitShiftLeft64(uint64_t* data, int count) { return bitcalGetDispatch().bitShiftLeft64(data, count); }
bool bitShiftLeft128(uint64_t* data, int count, uint64_t* carryout) { return bitcalGetDispatch().bitShiftLeft128(data, count, carryout); }
bool bitShiftLeft256(uint64_t* data, int count, uint64_t* carryout) { return bitcalGetDispatch().bitShiftLeft256(data, count, carryout); }
bool bitShiftLeft512(uint64_t* data, int count, uint64_t* carryout) { return bitcalGetDispatch().bitShiftLeft512(data, count, carryout); }

uint64_t bitShiftRight64(uint64_t* data, int count) { return bitcalGetDispatch().bitShiftRight64(data, count); }
bool bitShiftRight128(uint64_t* data, int count, uint64_t* carryout) { return bitcalGetDispatch().bitShiftRight128(data, count, carryout); }
bool bitShiftRight256(uint64_t* data, int count, uint64_t* carryout) { return bitcalGetDispatch().bitShiftRight256(data, count, carryout); }
bool bitShiftRight512(uint64_t* data, int count, uint64_t* carryout) { return bitcalGetDispatch().bitShiftRight512(data, count, carryout); }

uint64_t bitAnd64(uint64_t data0, uint64_t data1) { return bitcalGetDispatch().bitAnd64(data0, data1); }
bool bitAnd128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return bitcalGetDispatch().bitAnd128(data0, data1, data_out); }
bool bitAnd256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return bitcalGetDispatch().bitAnd256(data0, data1, data_out); }
bool bitAnd512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return bitcalGetDispatch().bitAnd512(data0, data1, data_out); }

uint64_t bitOr64(uint64_t data0, uint64_t data1) { return bitcalGetDispatch().bitOr64(data0, data1); }
bool bitOr128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return bitcalGetDispatch().bitOr128(data0, data1, data_out); }
bool bitOr256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return bitcalGetDispatch().bitOr256(data0, data1, data_out); }
bool bitOr512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return bitcalGetDispatch().bitOr512(data0, data1, data_out); }

uint64_t bitXor64(uint64_t data0, uint64_t data1) { return bitcalGetDispatch().bitXor64(data0, data1); }
bool bitXor128(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return bitcalGetDispatch().bitXor128(data0, data1, data_out); }
bool bitXor256(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return bitcalGetDispatch().bitXor256(data0, data1, data_out); }
bool bitXor512(const uint64_t* data0, const uint64_t* data1, uint64_t* data_out) { return bitcalGetDispatch().bitXor512(data0, data1, data_out); }

uint64_t bitPopcnt64(uint64_t data) { return bitcalGetDispatch().bitPopcnt64(data); }
uint64_t bitPopcnt128(const uint64_t* data, uint8_t* cnt_arr) { return bitcalGetDispatch().bitPopcnt128(data, cnt_arr); }
uint64_t bitPopcnt256(const uint64_t* data, uint8_t* cnt_arr) { return bitcalGetDispatch().bitPopcnt256(data, cnt_arr); }
uint64_t bitPopcnt512(const uint64_t* data, uint8_t* cnt_arr) { return bitcalGetDispatch().bitPopcnt512(data, cnt_arr); }

uint64_t bitReverse64(uint64_t data) { return bitcalGetDispatch().bitReverse64(data); }
bool bitReverse128(uint64_t* dataIn, uint64_t* dataOut) { return bitcalGetDispatch().bitReverse128(dataIn, dataOut); }
bool bitReverse256(uint64_t* dataIn, uint64_t* dataOut) { return bitcalGetDispatch().bitReverse256(dataIn, dataOut); }
bool bitReverse512(uint64_t* dataIn, uint64_t* dataOut) { return bitcalGetDispatch().bitReverse512(dataIn, dataOut); }

bool bitReverseInPlace128(uint64_t* data) { return bitcalGetDispatch().bitReverseInPlace128(data); }
bool bitReverseInPlace256(uint64_t* data) { return bitcalGetDispatch().bitReverseInPlace256(data); }
bool bitReverseInPlace512(uint64_t* data) { return bitcalGetDispatch().bitReverseInPlace512(data); }

}
