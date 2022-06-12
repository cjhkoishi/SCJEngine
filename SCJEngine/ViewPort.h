#pragma once
#include "Object.h"
#include "Camera.h"
class ViewPort
{
protected:
	int x, y;
	int w, h;
	Camera* active_camera = NULL;
public:
	void setViewPortSize(int x, int y, int w, int h) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	};
	int getW() { return w; };
	int getH() { return h; };
	int getX() { return x; };
	int getY() { return y; };

	bool setActiveCamera(Object* camera_obj);
	mat4 getViewMat();
	mat4 getProjMat();

	void setActive();
};

