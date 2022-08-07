#pragma once
#include <Renderer.h>
#include "Container.h"
class Grid :
	public Renderer
{
	COMPONENT
private:
	GLuint VAO, VBO, EBO,CELLEBO;
	Shader shader;
public:
	struct StorgedData {
		vector<int> obj_indices;
	};
	double W, H, X, Y;
	int num_W, num_H;
	vector<StorgedData> data;
	list<ivec2> activated_cells;
	Container* container = NULL;
	Ray* ray;

	void construct(int num_W, int num_H);
	void Bresenham();

	void start();
	void update();
	void render(const mat4& view, const mat4& proj);
};

