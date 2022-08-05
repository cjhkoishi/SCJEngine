#pragma once
#include <Renderer.h>
#include <Shader.h>
class Cloud :
	public Renderer
{
	COMPONENT
public:
	GLuint VBO, VAO, TBO;
	ComputeShader kernel;
	Shader shader;
	bool is_running = false;
	float a = 10, b = 28, c = 3;

	void start()override;
	void render(const mat4& view, const mat4& proj)override;
	void onGui();
};

class Canvas :public Renderer
{
	COMPONENT
public:
	GLuint VBO, VAO, EBO, TEX0, TEX1;
	Shader shader;
	ComputeShader kernel;

	void start();
	void render(const mat4& view, const mat4& proj)override;
	void onGui();
};

