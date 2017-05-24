#ifndef DUSK_BENCHMARK_HPP
#define DUSK_BENCHMARK_HPP

#include <dusk/Config.hpp>
#include <dusk/Log.hpp>

#include <chrono>

namespace dusk {

#define DuskBenchStart() \
    auto duskBenchClockStart = std::chrono::high_resolution_clock::now();

#define DuskBenchEnd(funcName)                                                 \
    DuskLogPerf("Function: %s took %.3f millis", funcName,                     \
        std::chrono::duration_cast<std::chrono::duration<double, std::milli>>( \
            std::chrono::high_resolution_clock::now() - duskBenchClockStart    \
        ).count());

} // namespace

#endif // DUSK_BENCHMARK_HPP
