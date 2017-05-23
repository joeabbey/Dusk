#ifndef DUSK_APP_HPP
#define DUSK_APP_HPP

#include <dusk/Config.hpp>

namespace dusk {

class App
{
public:

    explicit App(int argc, char** argv);
    virtual ~App();

    App(const App&) = delete;
    App& operator=(const App&) = delete;

    void Run();

private:

    SDL_Window *  _sdlWindow  = NULL;
    SDL_GLContext _sdlContext = NULL;

}; // class App

} // namespace dusk

#endif // DUSK_APP_HPP
