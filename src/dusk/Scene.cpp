#include "dusk/Scene.hpp"

#include <dusk/App.hpp>
#include <dusk/Benchmark.hpp>

namespace dusk {

Scene::Scene(const std::string& name)
    : _name(name)
	, _camera(nullptr)
    , _actors()
{

}

Scene::~Scene()
{
    for (Actor * actor : _actors)
    delete actor;
    {
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

Actor * Scene::GetActorByName(const std::string& name)
{
    for (const auto& actor : _actors)
    {
        if (actor->GetName() == name)
        {
            return actor;
        }
    }
    return nullptr;
}

void Scene::AddCamera(Camera * camera)
{
    _cameras.push_back(camera);

    if (!_camera)
    {
        _camera = camera;
    }
}

void Scene::AddActorTag(Actor * actor, const std::string& tag)
{
    if (_actorsByTag.find(tag) == _actorsByTag.end())
    {
        _actorsByTag.emplace(tag, std::vector<Actor *>());
    }

    _actorsByTag[tag].push_back(actor);
}

std::vector<Actor *> Scene::GetActorsByTag(const std::string& tag) const
{
    if (_actorsByTag.find(tag) == _actorsByTag.end())
    {
        return std::vector<Actor *>();
    }

    return _actorsByTag.at(tag);
}

void Scene::Start()
{
    App * app = App::GetInst();
    app->AddEventListener((EventID)App::Events::UPDATE, this, &Scene::Update);
    app->AddEventListener((EventID)App::Events::RENDER, this, &Scene::Render);

    if (!IsLoaded())
    {
        Load();
    }

    DispatchEvent(Event((EventID)Events::START));
}

void Scene::Stop()
{
    App * app = App::GetInst();
    app->RemoveEventListener((EventID)App::Events::UPDATE, this, &Scene::Update);
    app->RemoveEventListener((EventID)App::Events::RENDER, this, &Scene::Render);

    if (IsLoaded())
    {
        Free();
    }

    DispatchEvent(Event((EventID)Events::STOP));
}

bool Scene::Load()
{
    DuskBenchStart();

    DispatchEvent(Event((EventID)Events::LOAD_START));

    if (!_scriptHost.Load())
    {
        return false;
    }

    for (const std::string& filename : _scripts)
    {
        _scriptHost.RunFile(filename);
    }

    for (Actor * actor : _actors)
    {
        if (!actor->Load())
        {
            DuskLogError("Failed to load scene '%s'", GetName().c_str());
            return false;
        }
    }

    _loaded = true;

    DispatchEvent(Event((EventID)Events::LOAD_FINISHED));

    DuskBenchEnd("Scene::Load()");
    return true;
}

void Scene::Free()
{
    DispatchEvent(Event((EventID)Events::FREE_START));

    _scriptHost.Free();

    for (Actor * actor : _actors)
    {
        actor->Free();
    }

    _loaded = false;

    DispatchEvent(Event((EventID)Events::FREE_FINISHED));
}

void Scene::Update(const Event& event)
{
    if (!_loaded) return;

    for (Camera * camera : _cameras)
    {
        camera->Update();
    }

    DispatchEvent(Event((EventID)Events::UPDATE, event.GetData()));
}

void Scene::Render(const Event& event)
{
    if (!_loaded) return;

    //DuskLogInfo("Rendering %s", GetName().c_str());

    DispatchEvent(Event((EventID)Events::RENDER));
}

void Scene::InitScripting()
{
    ScriptHost::AddFunction("dusk_Scene_GetActorByName", &Scene::Script_GetActorByName);
}

int Scene::Script_GetActorByName(lua_State * L)
{
    Scene * scene = (Scene *)lua_tointeger(L, 1);

    std::string actorName = lua_tostring(L, 2);
    Actor * actor = scene->GetActorByName(actorName);
    if (!actor)
    {
        lua_pushnil(L);
        return 1;
    }

    lua_pushinteger(L, (ptrdiff_t)actor);

    return 1;
}

} // namespace dusk
