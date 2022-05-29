#pragma once
#include "Component.h"
class Scene;


class Object
{
protected:
	Scene* _scene;
	vector<Component*> components;
	mat4 transform;
	vector<Object*> childs;
	Object* parent;
public:
	vec3 getTranslation();
	quat getRotation();
	vec3 getScale();
	void setTranslation(const vec3& translation);
	void setRotation(const quat& rotation);
	void setScale(const vec3& scale);

	void detach();
	void setParent(Object* parent);
	void addChild(Object* child);

	void update();
};

