#pragma once

#include "pch.h"
#include <typeinfo> 

#define COMPONENT public: virtual size_t get_type_id() {return typeid(*this).hash_code();}; virtual string get_type_name(){return typeid(*this).name()+6;};

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

