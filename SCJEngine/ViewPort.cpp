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

void ViewPort::setActive()
{
	glViewport(x, y, w, h);
}
