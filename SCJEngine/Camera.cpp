#include "pch.h"
#include "Camera.h"

void Camera::start()
{
	_object->setTranslation(vec3(0, 0, 5));
}

void Camera::update()
{
}

mat4 Camera::getProjMat()
{
	return perspective(fov, aspect, near_z, far_z);
}

vec3 Camera::getCursorVector(vec2 screen_coord)
{
	vec4 pointer(screen_coord, 0, 0);
	mat4 view_inv = _object->getTransform();
	vec4 res=view_inv* inverse(getProjMat())* pointer;
	return res;
}

void Camera::onGui()
{
	ImGui::SliderFloat("fov", &fov, 0.2, 3);
}
