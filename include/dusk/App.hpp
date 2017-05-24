#ifndef DUSK_APP_HPP
#define DUSK_APP_HPP

#include <dusk/Config.hpp>
#include <dusk/Scene.hpp>

#include <string>
#include <unordered_map>

namespace dusk {

class App
{
public:

    DISALLOW_COPY_AND_ASSIGN(App);

    static App * Inst() { return _Inst; }

    explicit App(int argc, char** argv);
    virtual ~App();

    bool LoadConfig(const std::string& filename);

    void Run();

    // TODO: Make private
    unsigned int WindowWidth  = 1024;
    unsigned int WindowHeight = 768;
    std::string WindowTitle   = "Dusk";

private:

    static App * _Inst;

    void CreateWindow();
    void DestroyWindow();

    // TODO: Move
    std::unordered_map<std::string, Shader *> _shaders;

    std::unordered_map<std::string, Scene *> _scenes;
    Scene * _currentScene = nullptr;

    SDL_Window *  _sdlWindow  = nullptr;
    SDL_GLContext _sdlContext = nullptr;

}; // class App

} // namespace dusk

#endif // DUSK_APP_HPP
