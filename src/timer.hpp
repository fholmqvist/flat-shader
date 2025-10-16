#pragma once

#include "pch.hpp" // IWYU pragma: keep

using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::steady_clock;

struct Timer {
  private:
    steady_clock::time_point clock;

  public:
    Timer() {
        reset();
    }

    void reset();
    u64 stop();
    std::string stop_string();
};

extern Timer GLOBAL_TIMER;
