//
// Created by shijiashuai on 2022/9/23.
//

#ifndef ALIGNERHELPER_CHECKCPU_H
#define ALIGNERHELPER_CHECKCPU_H

class CheckCpu {
private:
    CheckCpu();

    ~CheckCpu() = default;

public:
    CheckCpu(const CheckCpu&) = delete;

    CheckCpu& operator=(const CheckCpu&) = delete;

public:
    // check for processor support for avx instructions
    int avxAvailable() const {
        return isSupportAvx;
    }

    // check for processor support for avx2 instructions
    int avx2Available() const {
        return isSupportAvx2;
    }

    // singleton design
    static CheckCpu& getInstance() {
        static CheckCpu check_cpu;
        return check_cpu;
    }

private:
    bool isSupportAvx{};
    bool isSupportAvx2{};
};

#endif  // ALIGNERHELPER_CHECKCPU_H
