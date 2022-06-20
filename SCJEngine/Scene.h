#pragma once
#include "pch.h"
#include "Object.h"
class Window;
class Scene
{
protected:
	Window* wnd=NULL;
	Object* root = NULL;
public:
	void start();
	void update();

	Window* getWindow() { return wnd; };

	inline Object* getRoot() { return root; };
	inline Object* createObject(string name) {
		if (root) {
			Object* res = new Object;
			res->name = name;
			root->addChild(res);
			return res;
		}
		return NULL;
	};

	inline Object* createObject(string name,Object* parent) {
		if (parent && parent->_scene == this) {
			Object* res = new Object;
			res->name = name;
			res->parent = parent;
			res->_scene = this;
			return res;
		}
		return NULL;
	};

	Scene(Window* wnd):
		wnd(wnd) 
	{
		root = new Object;
		root->_scene = this;
		root->name = "Root";
	};
};

