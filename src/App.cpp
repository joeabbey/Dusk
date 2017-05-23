#include "dusk/App.hpp"

#include "Debug.hpp"
#include "Benchmark.hpp"

namespace dusk {

App::App(int argc, char** argv)
{
    DuskBenchStart();
    DebugInfo("Starting Application");

    DebugWarn("Test Warning");
    DebugError("Test Error");

    if (0 > SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER))
    {
        DebugError("SDL_Init failed: %s", SDL_GetError());
        return;
    }

    _sdlWindow = SDL_CreateWindow("Dusk example - Cube",
                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                    1024, 768, SDL_WINDOW_OPENGL);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    _sdlContext = SDL_GL_CreateContext(_sdlWindow);

    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        DebugError("Failed to initialize OpenGL context");
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

    ImGui_ImplSdlGL3_Init(_sdlWindow);

    glEnable(GL_MULTISAMPLE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    DuskBenchEnd("App::App()");
}

App::~App()
{
    ImGui_ImplSdlGL3_Shutdown();

    SDL_DestroyWindow(_sdlWindow);
    _sdlWindow = NULL;

    SDL_GL_DeleteContext(_sdlContext);
    _sdlContext = NULL;

    SDL_Quit();
}

void App::Run()
{
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
            if (SDL_KEYDOWN == event.type)
            {
                if (SDLK_F2 == event.key.keysym.sym)
                {
                    UI::ConsoleShown ^= 1;
                }
            }
        }

        ImGui_ImplSdlGL3_NewFrame(_sdlWindow);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        UI::Render();

        SDL_GL_SwapWindow(_sdlWindow);
    }
}

} // namespace dusk
