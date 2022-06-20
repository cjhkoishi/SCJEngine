#pragma once
#include "pch.h"
#include "Component.h"
#include "ViewPort.h"

class Renderer :
	public Component
{
	COMPONENT
public:
	void update() override;
	virtual void render(const mat4& view, const mat4& proj) = 0;
	virtual ~Renderer() {};
};

class RenderSystem
{
protected:
	list<Renderer*> render_query;
	ViewPort render_area;
public:
	ViewPort getViewPort() { return render_area; };
	void clear() { render_query.clear(); };
	void append(Renderer* renderer) { render_query.push_back(renderer); };

	void setFullWindow(GLFWwindow* window);
	void setCamera(Object* camera_obj) { render_area.setActiveCamera(camera_obj); };
	void run(GLFWwindow* window);
};
