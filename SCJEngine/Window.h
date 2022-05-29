#pragma once
#include "pch.h"

class Window
{
private:

protected:
	GLFWwindow* window;
public:
	bool init();
	void run();
	void destroy();
};

