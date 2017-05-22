#include "dusk/Dusk.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl_gl3.h>

#include "Debug.hpp"
#include "Benchmark.hpp"

namespace dusk {

void Init()
{
    DuskBenchStart();

    SDL_Window *  sdlWindow  = NULL;
    SDL_GLContext sdlContext = NULL;

    if (0 > SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER))
    {
        DEBUG_ERROR("SDL_Init failed: %s", SDL_GetError());
        return;
    }

    sdlWindow = SDL_CreateWindow("Dusk example - Cube",
                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                    1024, 768, SDL_WINDOW_OPENGL);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    sdlContext = SDL_GL_CreateContext(sdlWindow);

    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        DEBUG_ERROR("Failed to initialize OpenGL context");
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    ImGui_ImplSdlGL3_Init(sdlWindow);

    glEnable(GL_MULTISAMPLE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ImVec4 clear_color = ImColor(114, 144, 154);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    DuskBenchEnd("dusk::Init");

    bool consoleShown = false;

    bool running = true;
    while(running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSdlGL3_ProcessEvent(&event);
            if (SDL_QUIT == event.type)
            {
                running = false;
                break;
            }
        }

        ImGui_ImplSdlGL3_NewFrame(sdlWindow);

        if (ImGui::BeginMainMenuBar())
		{
			ImGui::SameLine(1024 - 150, 0.0f);
			ImGui::Text("%.2f FPS (%.2f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
			ImGui::EndMainMenuBar();
		}


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui::Render();

        SDL_GL_SwapWindow(sdlWindow);
    }

    ImGui_ImplSdlGL3_Shutdown();

    SDL_DestroyWindow(sdlWindow);
    sdlWindow = NULL;

    SDL_GL_DeleteContext(sdlContext);
    sdlContext = NULL;

    SDL_Quit();
}

} // namespace dusk
