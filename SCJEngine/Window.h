#pragma once
#include "pch.h"
#include "Scene.h"
#include "ViewPort.h"
#include "Renderer.h"
#include "Input.h"

class Window
{
private:

protected:
	GLFWwindow* window = NULL;
	Object* focused_obj = NULL;
	Scene scene;
public:
	RenderSystem render_system;
	InputSystem input_system;

	void (*drawUI)(Window*) = NULL;
	void (*setScene)(Scene&) = NULL;

	Object* getRoot();
	GLFWwindow* getGLFWwindow() { return window; };
	void objectViewerGui();
	bool init();
	void run();
	void destroy();

	Window() :scene(this),
		input_system(this)
	{
	};
};

