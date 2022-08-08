#include "pch.h"
#include "Renderer.h"
#include "Scene.h"
#include "Widget.h"

void Renderer::update()
{
	_object->getScene()->getWidget()->render_system.append(this);
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
	glClearColor(0.1, 0.1, 0.1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setFullWindow(window);
	mat4 view = render_area.getViewMat();
	mat4 proj = render_area.getProjMat();
	for (auto it : render_query) {
		it->render(view, proj);
	}
	render_query.clear();
}
