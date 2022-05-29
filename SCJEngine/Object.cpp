#include "pch.h"
#include "Object.h"

vec3 Object::getTranslation()
{
    return transform[3];
}

quat Object::getRotation()
{
	const mat3 rotMtx(
		vec3(transform[0]) / length(transform[0]),
		vec3(transform[1]) / length(transform[1]),
		vec3(transform[2]) / length(transform[2])
	);
	return quat_cast(rotMtx);
}

vec3 Object::getScale()
{
	return vec3(length(transform[0]), length(transform[1]), length(transform[2]));
}

void Object::setTranslation(const vec3& translation)
{
	transform[3] = vec4(translation, 1);
}

void Object::setRotation(const quat& rotation)
{
	auto rot = mat3_cast(rotation);
	rot[0] *= length(transform[0]);
	rot[1] *= length(transform[1]);
	rot[2] *= length(transform[2]);
	transform[0] = vec4(rot[0], 0);
	transform[1] = vec4(rot[1], 0);
	transform[2] = vec4(rot[2], 0);
}

void Object::setScale(const vec3& scale)
{
	transform[0] = normalize(transform[0]) * scale[0];
	transform[1] = normalize(transform[1]) * scale[1];
	transform[2] = normalize(transform[2]) * scale[2];
}

void Object::detach()
{
	_scene = NULL;
	if (parent) {
		auto& container = parent->childs;
		std::remove(container.begin(), container.end(), this);
		parent = NULL;
	}
}

void Object::setParent(Object* parent)
{
	detach();
	if (parent != NULL) {
		this->parent = parent;
		parent->childs.push_back(this);
		this->_scene = parent->_scene;
	}
	else {
		cout << "Error: parent is NULL" << endl;
	}
}

void Object::addChild(Object* child)
{
	if (child) {
		child->detach();
		child->parent = this;
		if (_scene)
			child->_scene = _scene;
		childs.push_back(child);
	}
}

void Object::update()
{
	for (auto comp : components) {
		comp->update();
	}
}
