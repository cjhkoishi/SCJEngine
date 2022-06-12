#pragma once
#include "Renderer.h"
#include "Shader.h"
#include "pch.h"
class Mesh :
	public Component
{
	COMPONENT
protected:
	vector<vec3> vertices;
	vector<ivec3> triangles;
	vector<vec3> normals;
public:
	virtual void update() {};
	Mesh();
	friend class MeshRenderer;
};

class MeshRenderer :
	public Renderer
{
	COMPONENT
protected:
	GLuint VBO,VAO,EBO;
	Shader shader;
public:
	virtual void start();
	void render(const mat4& view, const mat4& proj)override;

	MeshRenderer();
};