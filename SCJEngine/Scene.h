#pragma once
#include "pch.h"
#include "Object.h"
class Scene
{
protected:
	Object* root = NULL;
public:
	void start();
	void update();
	inline Object* getRoot() { return root; };
	inline Object* createObject() {
		if (root) {
			Object* res = new Object;
			root->addChild(res);
			return res;
		}
		return NULL;
	};

	inline Object* createObject(Object* parent) {
		if (parent && parent->_scene == this) {
			Object* res = new Object;
			res->parent = parent;
			res->_scene = this;
			return res;
		}
		return NULL;
	};

	Scene() {
		root = new Object;
		root->_scene = this;
	};
};

