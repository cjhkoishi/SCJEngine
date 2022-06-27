#include "pch.h"
#include "Input.h"
#include "Widget.h"
#include "Renderer.h"

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	Widget* app = (Widget*)glfwGetWindowUserPointer(window);
	app->input_system.wheel_offset += dvec2(xoffset, yoffset);
}

void InputSystem::init()
{
	glfwGetCursorPos(_wnd->getGLFWwindow(), (double*)&cursor_pos, (double*)&cursor_pos + 1);
	glfwSetScrollCallback(_wnd->getGLFWwindow(),ScrollCallback);
}

void InputSystem::run()
{
	//cursor
	dvec2 cursor_current;
	glfwGetCursorPos(_wnd->getGLFWwindow(), (double*)&cursor_current, (double*)&cursor_current + 1);
	cursor_offset = cursor_current - cursor_pos;
	cursor_pos = cursor_current;

	//scroll
	wheel_last = wheel_offset;
	wheel_offset = dvec2(0);


}

int InputSystem::getMouseButtonState(int button)
{
	 return glfwGetMouseButton(_wnd->getGLFWwindow(), button); 
}

int InputSystem::getKeyState(int key)
{
	{ return glfwGetKey(_wnd->getGLFWwindow(), key); }
}

vec3 InputSystem::getCursorDirection(vec2 screen_pos)
{
	return  _wnd->render_system.getViewPort().getDirection(screen_pos);
}


