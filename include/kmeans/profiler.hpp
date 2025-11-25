#ifndef PROFILER
#define PROFILER

#include <chrono>

using TimePoint = std::chrono::high_resolution_clock::time_point;

inline TimePoint start() { return std::chrono::high_resolution_clock::now(); }

inline long end(TimePoint start) {
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start)
        .count();
}

#endif
