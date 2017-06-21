#include "dusk/Scene.hpp"

#include <dusk/App.hpp>
#include <dusk/Benchmark.hpp>

namespace dusk {

Scene::Scene()
    : _currentCamera(nullptr)
    , _actors()
{ }

std::unique_ptr<Scene> Scene::Parse(nlohmann::json & data)
{
    Scene * scene = new Scene();

    std::string defaultCamera;
    if (data.find("DefaultCamera") != data.end())
    {
        defaultCamera = data["DefaultCamera"];
    }

	for (auto& camera : data["Cameras"])
	{
        std::unique_ptr<Camera> tmp(Camera::Parse(camera));
        if (camera["ID"] == defaultCamera)
        {
            scene->SetCurrentCamera(tmp.get());
        }
        scene->AddCamera(std::move(tmp));
	}

    if (!scene->GetCurrentCamera())
    {
        //if (!_cameras.empty())
        //{
        //    _currentCamera = _cameras.front().get();
        //}
        //else
        //{
        //    DuskLogWarn("No camera set for scene");
        //}
    }

	for (auto& actor : data["Actors"])
	{
        scene->AddActor(Actor::Parse(actor, scene));
	}

	for (auto& script : data["Scripts"])
	{
        scene->RunScript(script.get<std::string>());
	}

    return std::unique_ptr<Scene>(scene);
}

Scene::~Scene()
{
}

void Scene::AddActor(std::unique_ptr<Actor> actor)
{
    _actors.push_back(std::move(actor));
}

void Scene::AddCamera(std::unique_ptr<Camera> camera)
{
    _cameras.push_back(std::move(camera));
}

void Scene::Start()
{
    App * app = App::GetInst();
    app->AddEventListener((EventID)App::Events::UPDATE, this, &Scene::Update);
    app->AddEventListener((EventID)App::Events::RENDER, this, &Scene::Render);

    DispatchEvent(Event((EventID)Events::START));
}

void Scene::Stop()
{
    App * app = App::GetInst();
    app->RemoveEventListener((EventID)App::Events::UPDATE, this, &Scene::Update);
    app->RemoveEventListener((EventID)App::Events::RENDER, this, &Scene::Render);

    DispatchEvent(Event((EventID)Events::STOP));
}

void Scene::Update(const Event& event)
{
    for (auto& camera : _cameras)
    {
        camera->Update();
    }

    DispatchEvent(Event((EventID)Events::UPDATE, event.GetData()));
}

void Scene::Render(const Event& event)
{
    DispatchEvent(Event((EventID)Events::RENDER));
}

void Scene::InitScripting()
{
}

} // namespace dusk
