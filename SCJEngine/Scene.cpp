#include "pch.h"
#include "Scene.h"

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
