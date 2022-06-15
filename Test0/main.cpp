#include <Window.h>
#include <ViewPort.h>
#include <Renderer.h>
#include <Mesh.h>
#include "Wire2D.h"

float v=3500;
Object* cube;
Object* canvas;
void MyUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();


	ImGui::NewFrame();
	ImGui::Begin("DDD");
	ImGui::SliderFloat("angle", &v, 100, 40000);
	canvas->getComponent<Wire2D>()->K = v;

	if (ImGui::Button("reset")) {
		canvas->getComponent<Wire2D>()->reset();
	};
	ImGui::End();

	ImGui::Render();

	auto io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MyScene(Scene& scene) {


	/*Object* cube = scene.createObject();
	cube->addComponent<Mesh>();
	cube->addComponent<MeshRenderer>();

	Object* cube_sub = scene.createObject();
	cube_sub->addComponent<Mesh>();
	cube_sub->addComponent<MeshRenderer>();
	cube_sub->setTranslation(vec3(1, 1, 1));
	cube->addChild(cube_sub);*/

	canvas = scene.createObject();
	canvas->addComponent<Wire2D>();
	canvas->addComponent<WireRenderer2D>();
}

int main() {
	Window wnd;
	ViewPort vp;

	wnd.drawUI = MyUI;
	wnd.setScene = MyScene;

	wnd.init();
	wnd.run();
	wnd.destroy();
	return 0;
}