#pragma once
#include "Component.h"
class Scene;


class Object
{
protected:
	Scene* _scene = NULL;

	vector<Component*> components;
	mat4 transform;
	quat storged_rotation;
	vec3 storged_scale;

	vector<Object*> childs;
	Object* parent = NULL;
	bool is_new = true;

	void updateTransform();
public:
	string name;

	void onGui();
	Scene* getScene() { return _scene; };

	vec3 getTranslation();
	quat getRotation();
	vec3 getScale();
	void setTranslation(const vec3& translation);
	void setRotation(const quat& rotation);
	void setScale(const vec3& scale);

	void setTransform(const mat4& transform) {
		this->transform = transform;
		mat3 ro(1);
		ro[0] = normalize(transform[0]);
		ro[1] = normalize(transform[1]);
		ro[2] = normalize(transform[2]);
		storged_rotation = quat_cast(ro);
		storged_scale[0] = length(transform[0]);
		storged_scale[1] = length(transform[1]);
		storged_scale[2] = length(transform[2]);
	}
	mat4 getTransform() { return transform; };
	mat4 getTransformInverse() { return inverse(transform); };
	mat4 getWorldTransform();
	size_t numChildren() { return childs.size(); };
	Object* children(size_t index) { if (index >= 0 && index < childs.size())return childs[index]; else return NULL; };


	void detach();
	void setParent(Object* parent);
	void addChild(Object* child);

	template<typename T> T* getComponent();
	template<typename T> T* addComponent();

	void start();
	void update();

	Object() {
		transform = mat4(1);
	};
	~Object();

	friend class Component;
	friend class Scene;
};

template<typename T>
inline T* Object::getComponent()
{
	for (auto comp : components)
		if (typeid(T).hash_code() == comp->get_type_id())
			return (T*)comp;
	return NULL;
}

template<typename T>
inline T* Object::addComponent()
{
	T* comp = new T;
	comp->_object = this;
	components.push_back(comp);
	return comp;
}
