#ifndef DUSK_UI_HPP
#define DUSK_UI_HPP

#include <dusk/Config.hpp>

#include <string>
#include <vector>

namespace dusk {

class UI
{
public:

    UI() = delete;
    UI(const UI&) = delete;
    UI& operator=(const UI&) = delete;

    static void Render();

    static void Log(ImVec4 color, const char * message);

    static bool ConsoleShown;

private:

    struct LogItem {
        ImVec4 color;
        std::string message;
    };

    static std::vector<LogItem> _logItems;

};

} // namespace dusk

#endif // DUSK_UI_HPP
