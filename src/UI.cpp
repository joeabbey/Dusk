#include "dusk/UI.hpp"

namespace dusk {

bool UI::ConsoleShown = false;
std::vector<UI::LogItem> UI::_logItems;

void UI::Render()
{
    if (ImGui::BeginMainMenuBar())
    {
        ImGui::SameLine(1024 - 150, 0.0f);
        ImGui::Text("%.2f FPS (%.2f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
        ImGui::EndMainMenuBar();
    }

    if (ConsoleShown)
    {
        static ImGuiTextFilter filter("-PERF");

        ImGui::SetNextWindowSize(ImVec2(500, 300));
        if (ImGui::Begin("Console", &UI::ConsoleShown))
        {
            filter.Draw("Filter", 180);
            ImGui::SameLine();

            if (ImGui::SmallButton("Clear"))
            {
                filter.InputBuf[0] = '\0';
                filter.Build();
            }
            ImGui::SameLine();

            if (ImGui::SmallButton("INFO"))
            {
                strcpy(filter.InputBuf, "INFO");
                filter.Build();
            }
            ImGui::SameLine();

            if (ImGui::SmallButton("WARN"))
            {
                strcpy(filter.InputBuf, "WARN");
                filter.Build();
            }
            ImGui::SameLine();
            if (ImGui::SmallButton("ERROR"))
            {
                strcpy(filter.InputBuf, "ERROR");
                filter.Build();
            }
            ImGui::SameLine();
            if (ImGui::SmallButton("PERF"))
            {
                strcpy(filter.InputBuf, "PERF");
                filter.Build();
            }

            ImGui::Separator();

            ImGui::BeginChild("Scroll", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
            for (const LogItem& item : _logItems)
            {
                if (!filter.PassFilter(item.message.c_str()))
                    continue;

                ImGui::TextColored(item.color, item.message.c_str());
            }
            ImGui::SetScrollHere();
            ImGui::EndChild();

            ImGui::Separator();
        }
        ImGui::End();
    }

    ImGui::Render();
}

void UI::Log(ImVec4 color, const char * message)
{
    _logItems.push_back({ color, std::string(message) });
}

} // namespace dusk
