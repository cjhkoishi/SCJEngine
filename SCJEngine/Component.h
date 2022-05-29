#pragma once
class Object;

class Component
{
protected:
	Object* _object;
public:
	Object* getObject() { return _object; };
	virtual void update() = 0;
};

