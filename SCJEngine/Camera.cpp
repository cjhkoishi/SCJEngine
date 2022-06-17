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

void Camera::onGui()
{
	ImGui::SliderFloat("fov", &fov, 0.2, 3);
}
