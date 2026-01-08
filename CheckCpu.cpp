//
// Created by shijiashuai on 2022/9/23.
//

#include "CheckCpu.h"

#include <cstdint>

#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))
#include <intrin.h>
#endif

#ifndef _XCR_XFEATURE_ENABLED_MASK
#define _XCR_XFEATURE_ENABLED_MASK 0
#endif

// #ifdef __x86_64__
// #include <immintrin.h>
// #elif __aarch64__
// #include <arm_neon.h>
// #endif

#if defined(__GNUC__) && !defined(_xgetbv) && (defined(__x86_64__) || defined(__i386__))
 
static uint64_t _xgetbv(uint32_t ecx) {
    uint32_t eax, edx;
    __asm__("xgetbv" : "=a"(eax), "=d"(edx) : "c"(ecx));
    return ((uint64_t)edx << 32) | eax;
}
#endif

static void cpuid(int CPUInfo[4], int InfoType) {
#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))
    __cpuidex(CPUInfo, InfoType, 0);
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
    __asm__ __volatile__("xorl %%ecx, %%ecx;"  // Clear ecx before executing cpuid
                         "cpuid"
                         : "=a"(CPUInfo[0]), "=b"(CPUInfo[1]), "=c"(CPUInfo[2]), "=d"(CPUInfo[3])
                         : "a"(InfoType));
#else
    (void)CPUInfo;
    (void)InfoType;
    return;
#endif
}

static int checkAvxAvailable() {
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    int      regs[4];
    uint64_t xcrFeatureMask;
    cpuid(regs, 1);
    if((regs[2] & 0x18000000) != 0x18000000) return 0;

#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))
    xcrFeatureMask = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
#else
    xcrFeatureMask = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
#endif
    if((xcrFeatureMask & 6) != 6) return 0;
    return 1;
#else
    return 0;
#endif
}

static int checkAvx2Available() {
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    int regs[4];
    if(!checkAvxAvailable()) return 0;
    cpuid(regs, 7);
    if((regs[1] & 0x20) == 0) return 0;
    return 1;
#else
    return 0;
#endif
}

CheckCpu::CheckCpu() {
    isSupportAvx  = checkAvxAvailable();
    isSupportAvx2 = checkAvx2Available();
}
