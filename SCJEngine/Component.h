#pragma once

#include "pch.h"
#include <typeinfo> 
#include <regex>
using namespace std;


const regex class_name("(?:[A-Z](?:[a-z]|[0-9])*)+");

#define COMPONENT \
public: \
	virtual size_t get_type_id() {\
		return typeid(*this).hash_code();\
	}; \
	virtual string get_type_name(){\
		string full_name=typeid(*this).name();\
		smatch sm;\
		regex_search(full_name,sm,class_name);\
		return sm[0];\
	};

class Object;

class Component
{
	COMPONENT
protected:
	Object* _object;
public:
	Object* getObject() {  return _object; };
	virtual void start() {};
	virtual void onGui() {};
	virtual void update() = 0;
	virtual ~Component() {};

	friend class Object;
};

