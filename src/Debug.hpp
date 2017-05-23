#ifndef DUSK_DEBUG_HPP
#define DUSK_DEBUG_HPP

#include <dusk/Config.hpp>
#include <dusk/Util.hpp>
#include <dusk/UI.hpp>

#include <cstdarg>
#include <cassert>
#include <cstdio>
#include <cstring>

namespace dusk {

// clang-format off

#define MAX_LOG_LINE_LEN 1024

    enum LogLevel {
        LOG_INFO,
        LOG_WARN,
        LOG_ERROR,
        LOG_PERF,
    };

    static inline void Log(LogLevel level, const char * format, ...)
    {
        const short FG_DEFAULT = 39;
        const short BG_DEFAULT = 49;

        va_list valist;
        static char buffer[MAX_LOG_LINE_LEN];

        ImVec4 imColor;
        short fgColor, bgColor;

        va_start(valist, format);
        vsnprintf(buffer, MAX_LOG_LINE_LEN, format, valist);
        va_end(valist);

        bgColor = 49; // Default

        switch (level)
        {
        case LOG_INFO:
            imColor = ImColor(255, 255, 255);
            fgColor = 97; // White
            break;
        case LOG_WARN:
            imColor = ImColor(255, 102, 0);
            fgColor = 33; // Yellow
            break;
        case LOG_ERROR:
            imColor = ImColor(204, 0, 0);
            fgColor = 31; // Red
            break;
        case LOG_PERF:
            imColor = ImColor(139, 0, 139);
            fgColor = 35; // Magenta
            break;
        }

        UI::Log(imColor, buffer);

        #ifndef DUSK_OS_WINDOWS
            printf("\033[%dm\033[%dm", fgColor, bgColor);
        #endif

        printf("%s\n", buffer);

        #ifndef DUSK_OS_WINDOWS
            printf("\033[%dm\033[%dm", FG_DEFAULT, BG_DEFAULT);
        #endif
    }

#if defined(NDEBUG)
#   define DebugInfo(M, ...)  do { } while(0)
#   define DebugWarn(M, ...)  do { } while(0)
#   define DebugError(M, ...) do { } while(0)
#   define DebugPerf(M, ...)  do { } while(0)
#else

#   define DebugInfo(M, ...) \
        do { dusk::Log(dusk::LogLevel::LOG_INFO, "[INFO](%s:%d) " M, GetBasename(__FILE__).c_str(), __LINE__, ##__VA_ARGS__); } while (0)

#   define DebugWarn(M, ...) \
        do { dusk::Log(dusk::LogLevel::LOG_WARN, "[WARN](%s:%d) " M, GetBasename(__FILE__).c_str(), __LINE__, ##__VA_ARGS__); } while (0)

#   define DebugError(M, ...) \
        do { dusk::Log(dusk::LogLevel::LOG_ERROR, "[ERROR](%s:%d) " M, GetBasename(__FILE__).c_str(), __LINE__, ##__VA_ARGS__); } while (0)

#   define DebugPerf(M, ...) \
        do { dusk::Log(dusk::LogLevel::LOG_PERF, "[PERF](%s:%d) " M, GetBasename(__FILE__).c_str(), __LINE__, ##__VA_ARGS__); } while (0)

#endif

// clang-format on

}


#endif // DUSK_DEBUG_HPP
