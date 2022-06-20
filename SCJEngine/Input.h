#pragma once
class Input
{
protected:
	static bool key_state[256];
	static bool mouse_left;
	static bool mouse_right;
public:
	int getKeyState();
};

