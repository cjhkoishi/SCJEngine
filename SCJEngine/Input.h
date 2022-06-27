#pragma once
#include"pch.h"
class Widget;
class InputSystem
{
protected:
	Widget* _wnd;

	dvec2 cursor_pos = dvec2(0);
	dvec2 cursor_offset = dvec2(0);

	dvec2 wheel_last = dvec2(0);
	dvec2 wheel_offset = dvec2(0);
public:
	void init();
	void run();


	inline dvec2 getCursorPos() {return cursor_pos;};
	inline dvec2 getCursorOffset() { return cursor_offset; };
	inline dvec2 getWheelOffset() { return wheel_last; };
	int getMouseButtonState(int button);	
	int getKeyState(int key) ;
	vec3 getCursorDirection(vec2 screen_pos);

	InputSystem(Widget* _wnd) :
		_wnd(_wnd)
	{
	};

	friend void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};

