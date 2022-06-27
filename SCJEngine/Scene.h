#pragma once
#include "pch.h"
#include "Object.h"
class Widget;
class Scene
{
protected:
	Widget* wnd=NULL;
	Object* root = NULL;
public:
	void start();
	void update();

	Widget* getWidget() { return wnd; };

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
			parent->addChild(res);
			res->_scene = this;
			return res;
		}
		return NULL;
	};

	Scene(Widget* wnd):
		wnd(wnd) 
	{
		root = new Object;
		root->_scene = this;
		root->name = "Root";
	};
};

