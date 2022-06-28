#pragma once
#include <Component.h>
#include <Renderer.h>
#include "CAANURBSCurve.h"
class Nurbs:
	public Component
{
	COMPONENT
public:
	CAANURBSCurve<3> curve;
	vector<EnergyFairing::Constraint> constraints;
	vector<vec3> sample_data;
	bool isChanged=true;

	void update();
};

class NurbsRenderer :
	public Renderer
{
	COMPONENT
public:
	GLuint VBO,VAO,VAOD;
	int N=100;

	void start();
	virtual void render(const mat4& view, const mat4& proj);

	
};

