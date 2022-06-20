#include "pch.h"
#include "Input.h"
#include "Window.h"
#include "Renderer.h"

int InputSystem::getKeyState()
{
	return false;
}

vec3 InputSystem::getCursorDirection(vec2 screen_pos)
{
	return  _wnd->render_system.getViewPort().getDirection(screen_pos);
}
