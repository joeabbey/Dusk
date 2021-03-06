#include "dusk/Camera.hpp"

#include <dusk/Log.hpp>
#include <dusk/App.hpp>

namespace dusk {

Camera::Camera(float fov /*= 45.0f*/, glm::vec3 up /*= glm::vec3(0, 1, 0)*/, glm::vec2 clip /*= glm::vec2(0.1f, 1000.0f)*/)
    : _baseTransform(1)
    , _viewInvalid(true)
    , _projectionInvalid(true)
    , _view(1)
    , _projection(1)
    , _fov(fov)
    , _aspect()
    , _clip(clip)
    , _position(0)
    , _forward(1)
    , _up(up)
    , _friction(0.9f)
    , _velocity(0)
{
    // TODO: Something?
    int width, height;
    glfwGetFramebufferSize(App::GetInst()->GetGLFWWindow(), &width, &height);
    SetAspect((float)width, (float)height);
}

std::unique_ptr<Camera> Camera::Parse(nlohmann::json & data)
{
	std::unique_ptr<Camera> camera(new Camera());

	if (data.find("Position") != data.end())
	{
		camera->SetPosition({
			data["Position"][0], data["Position"][1], data["Position"][2]
		});
	}

	if (data.find("Forward") != data.end())
	{
		camera->SetForward({
			data["Forward"][0], data["Forward"][1], data["Forward"][2]
		});
	}

	if (data.find("Up") != data.end())
	{
		camera->SetUp({
			data["Up"][0], data["Up"][1], data["Up"][2]
		});
	}

	if (data.find("FOV") != data.end())
	{
		camera->SetFOV(data["FOV"]);
	}

	if (data.find("Aspect") != data.end())
	{
		camera->SetAspect(data["Aspect"]);
	}

	if (data.find("Clip") != data.end())
	{
		camera->SetClip(data["Clip"][0], data["Clip"][1]);
	}

	return camera;
}

std::unique_ptr<Camera> Camera::Clone()
{
    std::unique_ptr<Camera> camera(new Camera(GetFOV(), GetUp(), GetClip()));

    camera->SetBaseTransform(_baseTransform);
    camera->SetPosition(GetPosition());
    camera->SetForward(GetForward());

    return camera;
}

void Camera::SetBaseTransform(const glm::mat4& baseTransform)
{
    _baseTransform = baseTransform;
    _viewInvalid = true;
}

glm::mat4 Camera::GetView()
{
    if (_viewInvalid)
    {
        _view = glm::lookAt(_position, _position + _forward, _up) * _baseTransform;
        _viewInvalid = false;
    }
    return _view;
}

glm::mat4 Camera::GetProjection()
{
    if (_projectionInvalid)
    {
        _projection = glm::perspective(_fov, _aspect, _clip.x, _clip.y);
        _projectionInvalid = false;
    }
    return _projection;
}

void Camera::SetFOV(float fov)
{
    _fov = fov;
    _projectionInvalid = true;
}

void Camera::SetAspect(float aspect)
{
    _aspect = aspect;
    _projectionInvalid = true;
}

void Camera::SetClip(const glm::vec2& clip)
{
    _clip = clip;
    _projectionInvalid = true;
}

void Camera::SetPosition(const glm::vec3& pos)
{
    _position = pos;
    _projectionInvalid = true;
}

void Camera::SetForward(const glm::vec3& forward)
{
    _forward = forward;
    _projectionInvalid = true;
}

void Camera::SetUp(const glm::vec3& up)
{
    _up = up;
    _projectionInvalid = true;
}

void Camera::AddVelocity(const glm::vec3& vel)
{
    _velocity += vel;
}

void Camera::Update()
{
    if (_velocity == glm::vec3(0))
    {
        return;
    }

    _position += _velocity;
    _viewInvalid = true;

    _velocity *= _friction;

    if (_velocity.x < _zeroClamp && _velocity.x > -_zeroClamp)
        _velocity.x = 0.0f;

    if (_velocity.y < _zeroClamp && _velocity.y > -_zeroClamp)
        _velocity.y = 0.0f;

    if (_velocity.z < _zeroClamp && _velocity.z > -_zeroClamp)
        _velocity.z = 0.0f;
}

} // namespace dusk
