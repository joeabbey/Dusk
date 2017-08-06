#include "dusk/Component.hpp"

#include <dusk/Log.hpp>
#include <dusk/App.hpp>
#include <dusk/Actor.hpp>

namespace dusk {

Component::Component(bool isTempalte /*= false*/)
    : _actor(nullptr)
    , _isTemplate(isTempalte)
{
}

Component::~Component()
{
}

void Component::SetActor(Actor * actor)
{
    _actor = actor;
}

ModelComponent::ModelComponent(std::unique_ptr<Model> model)
    : Component()
    , _model(std::move(model))
{
}

ModelComponent::~ModelComponent()
{
    if (GetActor())
    {
        GetActor()->EvtUpdate.RemoveListener(updateBindId);
        GetActor()->EvtRender.RemoveListener(renderBindId);
    }
}

void ModelComponent::SetActor(Actor * actor)
{
    if (GetActor())
    {
        GetActor()->EvtUpdate.RemoveListener(updateBindId);
        GetActor()->EvtRender.RemoveListener(renderBindId);
    }

    Component::SetActor(actor);

    updateBindId = GetActor()->EvtUpdate.AddMember(this, &ModelComponent::OnUpdate);
    renderBindId = GetActor()->EvtRender.AddMember(this, &ModelComponent::OnRender);
}

void ModelComponent::OnUpdate(const UpdateContext& ctx)
{
    _model->SetBaseTransform(GetActor()->GetTransform());
    _model->Update(ctx);
}

void ModelComponent::OnRender(RenderContext& ctx)
{
    _model->Render(ctx);
}

CameraComponent::CameraComponent(std::unique_ptr<Camera> camera, bool isTempalte /*= false*/)
    : Component(isTempalte)
    , _camera(std::move(camera))
{
}

CameraComponent::~CameraComponent()
{
    if (GetActor() && !IsTemplate())
    {
        GetActor()->RemoveEventListener((EventID)Actor::Events::UPDATE, this, &CameraComponent::OnUpdate);
    }
}

std::unique_ptr<Component> CameraComponent::Clone()
{
    CameraComponent * component = new CameraComponent(GetCamera()->Clone());

    component->SetActor(GetActor());

    return std::unique_ptr<Component>(component);
}

void CameraComponent::SetActor(Actor * actor)
{
    if (GetActor() && !IsTemplate())
    {
        GetActor()->RemoveEventListener((EventID)Actor::Events::UPDATE, this, &CameraComponent::OnUpdate);
    }

    Component::SetActor(actor);

    if (!IsTemplate())
    {
        GetActor()->AddEventListener((EventID)Actor::Events::UPDATE, this, &CameraComponent::OnUpdate);
    }
}

void CameraComponent::OnUpdate(const Event& event)
{
    _camera->SetBaseTransform(glm::inverse(GetActor()->GetTransform()));
}

ScriptComponent::ScriptComponent(const std::string& filename, bool isTempalte /*= false*/)
    : Component(isTempalte)
    , _scriptHost()
    , _filename(filename)
{
    lua_State * L = _scriptHost.GetLuaState();

    lua_pushinteger(L, (ptrdiff_t)this);
    lua_setglobal(L, "dusk_current_ScriptComponent");

    // Wait until we have an actor to run the script
}

std::unique_ptr<Component> ScriptComponent::Clone()
{
    ScriptComponent * component = new ScriptComponent(_filename);

    component->SetActor(GetActor());

    return std::unique_ptr<Component>(component);
}

void ScriptComponent::SetActor(Actor * actor)
{
    if (GetActor() && !IsTemplate())
    {
        // TODO: Cleanup script host?
    }

    Component::SetActor(actor);

    if (!IsTemplate())
    {
        // TODO: Fix
        _scriptHost.RunFile(_filename);
    }
}

} // namespace dusk
