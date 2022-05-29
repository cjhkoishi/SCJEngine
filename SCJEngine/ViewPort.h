#pragma once
class ViewPort
{
protected:
	int x, y;
	int w, h;
public:
	void setViewPortSize(int x, int y, int w, int h) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	};
	int getW() { return w; };
	int getH() { return h; };
	int getX() { return x; };
	int getY() { return y; };
};

