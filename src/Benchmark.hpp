#ifndef DUSK_BENCHMARK_HPP
#define DUSK_BENCHMARK_HPP

#include "Debug.hpp"

#include <chrono>

namespace dusk {

#define DuskBenchStart() \
    auto duskBenchClockStart = std::chrono::high_resolution_clock::now();

#define DuskBenchEnd(funcName) \
    auto duskBenchClockEnd = std::chrono::high_resolution_clock::now(); \
    double duskBenchMillis = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(duskBenchClockEnd - duskBenchClockStart).count(); \
    DebugPerf("Function: %s took %.3f millis", funcName, duskBenchMillis);

} // namespace

#endif // DUSK_BENCHMARK_HPP
