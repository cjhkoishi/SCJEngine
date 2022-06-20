#pragma once
#include "pch.h"
#include "Scene.h"
#include "ViewPort.h"

class Window
{
private:

protected:
	GLFWwindow* window = NULL;
	Object* focused_obj = NULL;
	Scene scene;
public:
	void (*drawUI)(Window*) = NULL;
	void (*setScene)(Scene&) = NULL;

	Object* getRoot();
	GLFWwindow* getGLFWwindow() { return window; };
	void objectViewerGui();
	bool init();
	void run();
	void destroy();

	Window() :scene(this) {};
};

