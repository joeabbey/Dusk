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
{
}

void Camera::Serialize(nlohmann::json& data)
{
    data["Position"] = { _position.x, _position.y, _position.z };
    data["Forward"] = { _forward.x, _forward.y, _forward.z };
    data["Up"] = { _up.x, _up.y, _up.z };
    data["FOV"] = _fov;
    data["Aspect"] = _aspect;
    data["Clip"] = { _clip.x, _clip.y };
}

void Camera::Deserialize(nlohmann::json& data)
{
	if (data.find("Position") != data.end())
	{
		SetPosition({
			data["Position"][0], data["Position"][1], data["Position"][2]
		});
	}

	if (data.find("Forward") != data.end())
	{
		SetForward({
			data["Forward"][0], data["Forward"][1], data["Forward"][2]
		});
	}

	if (data.find("Up") != data.end())
	{
		SetUp({
			data["Up"][0], data["Up"][1], data["Up"][2]
		});
	}

	if (data.find("FOV") != data.end())
	{
		SetFOV(data["FOV"]);
	}

	if (data.find("Aspect") != data.end())
	{
		SetAspect(data["Aspect"].get<float>());
	}

	if (data.find("Clip") != data.end())
	{
		SetClip(data["Clip"][0], data["Clip"][1]);
	}
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

} // namespace dusk
