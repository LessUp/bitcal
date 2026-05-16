# Installation

BitCal vNext is a **header-only C++23 library**. Installation is mostly about putting `include/bitcal/` on your include path and choosing how you want to validate the retained development baseline.

## Requirements

| Component | Baseline | Notes |
| --- | --- | --- |
| Language mode | **C++23** | The interface target requests `cxx_std_23` |
| Compiler | Recent GCC, Clang, Apple Clang, or MSVC | Use an x86-64 toolchain when you want the primary validation path |
| Build system | Optional for consumers | CMake is mainly needed for tests, examples, benchmarks, and install/export flows |
| Delivery | Header-only | No runtime library or generated source step |

## Recommended installation methods

### 1. Vendor the headers directly

Copy `include/bitcal/` into your project and include:

```cpp
#include <bitcal/bitcal.hpp>
```

Example compile line:

```bash
g++ -std=c++23 -O3 -march=native main.cpp -I/path/to/vendor -o app
```

### 2. Use CMake as a subdirectory

```cmake
add_subdirectory(third_party/bitcal EXCLUDE_FROM_ALL)
target_link_libraries(my_app PRIVATE bitcal)
```

The `bitcal` target is an `INTERFACE` library. It supplies include directories and the C++23 requirement, but it leaves CPU-specific optimization flags to the consumer.

### 3. Install and consume the exported package

```bash
cmake -S . -B build-install -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/path/to/install-prefix
cmake --build build-install --config Release --target install --parallel
```

Choose any writable install prefix that matches your environment.

Then in your own project:

```cmake
find_package(bitcal CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE bitcal::bitcal)
```

If you installed to a custom prefix, point CMake at that prefix when configuring your consumer project:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/install-prefix
```

## Compiler guidance

| Toolchain | Typical command |
| --- | --- |
| GCC / Clang | `-std=c++23 -O3 -march=native` for local native validation, or `-std=c++23 -O3 -mavx2` for an explicit AVX2 build |
| MSVC | `/std:c++23 /O2 /arch:AVX2` when validating the AVX2-oriented path |
| Generic portability builds | keep C++23, drop native CPU flags, and accept a more conservative backend choice |

## vNext validation path

When you need to verify the retained repository baseline instead of only consuming headers, use:

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release --parallel
ctest --test-dir build-test --output-on-failure -C Release
```

Then run the generated `bitcal_benchmark` executable from the `build-test` tree. For multi-config generators, use the `Release` output location.

This path validates the public include, the retained smoke tests, and the current benchmark baseline from one build directory.

## Quick sanity check

```cpp
#include <bitcal/bitcal.hpp>

int main() {
    bitcal::bit_block<256> block;
    return bitcal::is_zero(block.view()) ? 0 : 1;
}
```

Compile it with C++23 and your chosen target flags. If that builds, your include path is working and the vNext public model is reachable.

## Read next

- [Quick Start](./quickstart.md)
- [Build Options](./build-options.md)
- [Types Reference](../api/types.md)
