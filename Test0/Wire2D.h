#pragma once
#include <Component.h>
#include <Renderer.h>
#include <Shader.h>
class Wire2D :public Component
{
	COMPONENT
public:
	vector<dvec2> vertices;
	vector<dvec2> velocities;
	vector<ivec2> edges;
	vector<double> lens;

	virtual void update() {
		double dt = 0.01;
		double k = 2000;
		for (int i = 0; i < vertices.size(); i++) {
			velocities[i][1] -= 0.6 * dt;
			velocities[i] *= 0.999;
			vertices[i] += dt * velocities[i];
			if (vertices[i][1] < -1) {
				vertices[i][1] = -1;
				velocities[i][0] *= 0.7;
				velocities[i][1] = 0;
			}
		}
		for (int e = 0; e < edges.size(); e++) {
			dvec2& v0 = vertices[edges[e][0]];
			dvec2& v1 = vertices[edges[e][1]];
			dvec2 dv = v1 - v0;
			dvec2 force = k * (length(dv) - lens[e]) * normalize(dv);
			velocities[edges[e][0]] += force * dt;
			velocities[edges[e][1]] -= force * dt;
		}
	};

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

