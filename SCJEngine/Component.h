#pragma once


#include <typeinfo> 

#define COMPONENT public: virtual size_t get_type_id() {return typeid(*this).hash_code();};

class Object;

class Component
{
	COMPONENT
protected:
	Object* _object;
public:
	Object* getObject() { return _object; };	
	virtual void start();
	virtual void update() = 0;
	virtual ~Component() {};

	friend class Object;
};

