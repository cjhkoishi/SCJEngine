#pragma once
#include <Component.h>
#include <Renderer.h>
#include <Shader.h>
#include <Eigen/Sparse>
class Wire2D :public Component
{
	COMPONENT
public:
	vector<dvec2> vertices;
	vector<dvec2> velocities;
	vector<ivec2> edges;
	vector<double> lens;

	virtual void update();

	Wire2D();
	~Wire2D() {};
};

class WireRenderer2D :public Renderer
{
	COMPONENT
public:
	Shader shader;
	GLuint VBO, VAO, EBO;

	virtual void start();
	void render(const mat4& view, const mat4& proj);

	WireRenderer2D();
	~WireRenderer2D() {};
};

