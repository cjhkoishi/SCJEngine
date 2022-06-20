#pragma once
#include"pch.h"
class Window;
class InputSystem
{
protected:
	Window* _wnd;
public:
	int getKeyState();
	vec3 getCursorDirection(vec2 screen_pos);

	InputSystem(Window* _wnd) :
		_wnd(_wnd)
	{
	
	};
};

