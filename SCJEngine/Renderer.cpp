#include "pch.h"
#include "Renderer.h"
#include "Scene.h"
#include "Window.h"

void Renderer::update()
{
	_object->getScene()->getWindow()->render_system.append(this);
}

void RenderSystem::setFullWindow(GLFWwindow* window)
{
	int w, h;
	glfwGetWindowSize(window, &w, &h);
	render_area.setViewPortSize(0, 0, w, h);
	render_area.setActive();
}

void RenderSystem::run(GLFWwindow* window)
{
	setFullWindow(window);
	mat4 view = render_area.getViewMat();
	mat4 proj = render_area.getProjMat();
	for (auto it : render_query) {
		it->render(view, proj);
	}
	render_query.clear();
}
