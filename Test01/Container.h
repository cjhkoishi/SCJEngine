#pragma once
#include <Widget.h>
#include <Shader.h>
class Container :public Renderer
{
	COMPONENT
public:
	struct Ball
	{
		dvec2 pos;
		double r = 5;
		bool activated = false;
	};
	GLuint VBO, VAO, EBO;
	Shader shader;
	vector<Ball> balls;

	void start();
	void render(const mat4& view, const mat4& proj);
};

class Ray :public Renderer
{
	COMPONENT
private:
	GLuint VAO, VBO;
	Shader shader;
public:
	dvec2 pos;
	dvec2 dir;

	bool RayHitAABB(dvec2 AA, dvec2 BB, double* t = NULL);
	bool RayHitBall(dvec2 bpos, double r);

	void start();
	void onGui();
	void render(const mat4& view, const mat4& proj);
};

