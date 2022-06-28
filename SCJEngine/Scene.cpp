#include "pch.h"
#include "Scene.h"

Object* Scene::findByName(string name)
{
	function<bool(Object*)> search;
	Object* res=NULL;
	search = [&search,&res,name](Object* node)->bool {
		if (node->name == name) {
			res = node;
			return true;
		}
		for (int i = 0; i < node->numChildren();i++) {
			if(search(node->children(i)))
				return true;
		}
		return false;
	};

	search(root);
	return res;
}

void Scene::start()
{
}

void Scene::update()
{
	if (root) {
		if (root->is_new) {
			root->start();
			root->is_new = false;
		}
		root->update();
	}
}
