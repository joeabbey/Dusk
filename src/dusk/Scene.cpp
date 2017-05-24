#include "dusk/Scene.hpp"

#include <dusk/Benchmark.hpp>

namespace dusk {

Scene::Scene(const std::string& name)
    : _loaded(false)
    , _name(name)
    , _actors()
{

}

Scene::~Scene()
{
    for (Actor * actor : _actors)
    {
        delete actor;
    }
    for (Camera * camera : _cameras)
    {
        delete camera;
    }
}

void Scene::AddActor(Actor * actor)
{
    _actors.push_back(actor);
}

void Scene::AddCamera(Camera * camera)
{
    _cameras.push_back(camera);
    
    if (NULL == _camera)
    {
        _camera = camera;
    }
}

bool Scene::Load()
{
    DuskBenchStart();

    for (Actor * actor : _actors)
    {
        if (!actor->Load())
        {
            DuskLogError("Failed to load scene '%s'", GetName().c_str());
            return false;
        }
    }

    _loaded = true;

    DuskBenchEnd("Scene::Load()");
    return true;
}

void Scene::Free()
{
    for (Actor * actor : _actors)
    {
        actor->Free();
    }
}

void Scene::Update()
{
    if (!_loaded) return;

    for (Camera * camera : _cameras)
    {
        camera->Update();
    }

    for (Actor * actor : _actors)
    {
        actor->Update();
    }
}

void Scene::Render()
{
    if (!_loaded) return;

    for (Actor * actor : _actors)
    {
        actor->Render();
    }
}


} // namespace dusk
