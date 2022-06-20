#include "pch.h"
#include "ViewPort.h"

bool ViewPort::setActiveCamera(Object* camera_obj)
{
	return camera_obj && (active_camera = camera_obj->getComponent<Camera>());
}

mat4 ViewPort::getViewMat()
{
	return active_camera->getObject()->getTransformInverse();
}

mat4 ViewPort::getProjMat()
{
	active_camera->aspect = (w == 0 || h == 0) ? 0 : ((float)w) / h;
	return active_camera->getProjMat();
}

vec3 ViewPort::getDirection(vec2 screen_coord)
{
	screen_coord = screen_coord * 2.f / vec2(w, h) - 1.f;
	screen_coord[1] *= -1;
	double nz = active_camera->near_z;
	double fz = active_camera->far_z;
	double invary_z = -((-fz + nz - 2 * fz * nz) / (fz - nz)) * (fz + nz) / (fz - nz);
	vec4 pointer(screen_coord, invary_z, 1);
	vec4 dir(vec3(inverse(getProjMat()) * pointer), 0);
	return (active_camera->getObject()->getTransform()) * dir;
}

void ViewPort::setActive()
{
	glViewport(x, y, w, h);
}
