#include "pch.h"
#include "Camera.h"
#include "Widget.h"

void Camera::start()
{
	_object->setTranslation(vec3(0, 0, 5));
}

void Camera::update()
{
	
	Widget* app = _object->getScene()->getWidget();
	auto wo = app->input_system.getWheelOffset();
	if (wo[1] != 0) {
		_object->setTransform(translate(_object->getTransform(), vec3(0, 0, -5)));
		_object->setTransform(scale(_object->getTransform(), vec3(pow(0.9f, wo[1]))));
		_object->setTransform(translate(_object->getTransform(), vec3(0, 0, 5)));
	}
	auto co = app->input_system.getCursorOffset();
	if (app->input_system.getMouseButtonState(0)) {
		float angle = length(co);
		if (angle != 0) {
			_object->setTransform(translate(_object->getTransform(), vec3(0, 0, -5)));
			_object->setTransform(rotate(_object->getTransform(), angle * 0.01f, vec3(-co[1], -co[0], 0)));
			_object->setTransform(translate(_object->getTransform(), vec3(0, 0, 5)));
		}
	}
	if (app->input_system.getMouseButtonState(1)) {
		float dist = length(co);
		if (dist != 0) {
			_object->setTransform(translate(_object->getTransform(), 0.01f * vec3(-co[0], co[1], 0)));
		}
	}
}

mat4 Camera::getProjMat()
{
	return perspective(fov, aspect, near_z, far_z);
}

vec3 Camera::getCursorVector(vec2 screen_coord)
{
	vec4 pointer(screen_coord, 0, 0);
	mat4 view_inv = _object->getTransform();
	vec4 res = view_inv * inverse(getProjMat()) * pointer;
	return res;
}

void Camera::onGui()
{
	ImGui::SliderFloat("fov", &fov, 0.2, 3);
}
