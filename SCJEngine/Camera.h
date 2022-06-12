#pragma once
#include "Component.h"
#include "Object.h"
class Camera :
	public Component
{
	COMPONENT
public:
	float fov = radians<float>(45);
	float near_z = 0.01f, far_z = 1000.f;
	float aspect;

	void start()override;
	void update() override;
	mat4 getProjMat();

	virtual ~Camera() {};
};

