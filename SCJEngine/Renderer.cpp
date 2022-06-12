#include "pch.h"
#include "Renderer.h"

RenderSystem render_system;

void Renderer::update()
{
	render_system.append(this);
}

void RenderSystem::setFullWindow(GLFWwindow* window)
{
	int w, h;
	glfwGetWindowSize(window, &w, &h);
	render_target.setViewPortSize(0, 0, w, h);
	render_target.setActive();
}

void RenderSystem::run(GLFWwindow* window)
{
	setFullWindow(window);
	mat4 view = render_target.getViewMat();
	mat4 proj = render_target.getProjMat();
	for (auto it : render_query) {
		it->render(view, proj);
	}
	render_query.clear();
}
