#pragma once
#include "pch.h"
#include "Scene.h"
#include "ViewPort.h"

class Window
{
private:

protected:
	GLFWwindow* window = NULL;
	Scene scene;
public:
	void (*drawUI)(Window*) = NULL;
	void (*setScene)(Scene&) = NULL;

	Object* getRoot();
	bool init();
	void run();
	void destroy();
};

