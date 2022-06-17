#include "pch.h"
#include "Object.h"

void Object::updateTransform()
{
	auto rot = mat3_cast(storged_rotation);
	rot[0] *= storged_scale[0];
	rot[1] *= storged_scale[1];
	rot[2] *= storged_scale[2];
	transform[0] = vec4(rot[0], 0);
	transform[1] = vec4(rot[1], 0);
	transform[2] = vec4(rot[2], 0);
}

void Object::onGui()
{
	ImGui::Text(name.c_str());
	ImGui::InputFloat3("position",(float*)& transform[3]);
	for (auto comp : components) {
		if (!ImGui::CollapsingHeader(comp->get_type_name().c_str()))
			continue;
		ImGui::TreePush();
		comp->onGui();
		ImGui::TreePop();
	}
}

vec3 Object::getTranslation()
{
	return transform[3];
}

quat Object::getRotation()
{
	return storged_rotation;
}

vec3 Object::getScale()
{
	return storged_scale;
}

void Object::setTranslation(const vec3& translation)
{
	transform[3] = vec4(translation, 1);
}

void Object::setRotation(const quat& rotation)
{
	storged_rotation = rotation;
	updateTransform();
}

void Object::setScale(const vec3& scale)
{
	storged_scale = scale;
	updateTransform();
}

mat4 Object::getWorldTransform()
{
	Object* it = this;
	mat4 result = mat4(1);
	while (it != NULL) {
		result = it->transform * result;
		it = it->parent;
	}
	return result;
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

void Object::start()
{
	for (auto comp : components) {
		comp->start();
	}
	for (auto child : childs) {
		child->start();
	}
}

void Object::update()
{
	for (auto comp : components) {
		comp->update();
	}
	for (auto child : childs) {
		child->update();
	}
}

Object::~Object()
{
	detach();
	for (auto comp : components) {
		delete comp;
	}
	for (auto child : childs) {
		delete child;
	}
}
