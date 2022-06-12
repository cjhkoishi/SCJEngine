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
	void (*drawUI)(void) = NULL;
	void (*setScene)(Scene&) = NULL;

	bool init();
	void run();
	void destroy();
};

