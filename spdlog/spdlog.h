#pragma once

#include <cstdio>

namespace spdlog {

template <class... Args>
inline void error(const char* msg, Args&&...) {
  if(msg) {
    std::fprintf(stderr, "%s\n", msg);
  }
}

}
