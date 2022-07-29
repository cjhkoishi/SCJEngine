#pragma once
#include <Renderer.h>
#include <Shader.h>
class Canvas :
	public Renderer
{
	COMPONENT
public:
	GLuint VBO,TBO;
	ComputeShader kernel;

	void start()override;
	void render(const mat4& view, const mat4& proj) {};
};

