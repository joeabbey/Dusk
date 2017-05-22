#ifndef DUSK_DEBUG_HPP
#define DUSK_DEBUG_HPP

#include <cassert>
#include <cstdio>
#include <cstring>

// clang-format off

#if defined(NDEBUG)
#  define DEBUG_INFO(M, ...) do { } while(0)
#  define DEBUG_WARN(M, ...) do { } while(0)
#  define DEBUG_ERROR(M, ...) do { } while(0)
#  define DEBUG_PERF(M, ...) do { } while(0)
#else

#  define DEBUG_INFO(M, ...) \
          do { fprintf(stdout, "[INFO](%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__); } while (0)

#  define DEBUG_WARN(M, ...) \
          do { fprintf(stderr, "[WARN](%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__); } while (0)

#  define DEBUG_ERROR(M, ...) \
          do { fprintf(stderr, "[ERROR](%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__); } while (0)

#  define DEBUG_PERF(M, ...) \
          do { fprintf(stderr, "[PERF](%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__); } while (0)

#endif

// clang-format on

#endif // DUSK_DEBUG_HPP
