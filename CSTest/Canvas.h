#pragma once
#include <Renderer.h>
#include <Shader.h>
class Canvas :
	public Renderer
{
	COMPONENT
public:
	GLuint VBO, VAO, TBO;
	ComputeShader kernel;
	Shader shader;
	float a=10,b=28,c=3;

	void start()override;
	void render(const mat4& view, const mat4& proj)override;
	void onGui();
};

