#include <Window.h>
#include <ViewPort.h>
#include <Renderer.h>
#include <Mesh.h>
#include "Wire2D.h"

float v = 3500;
Object* cube;
Object* canvas;
Object* focused_obj = NULL;
void MyUI(Window* wnd)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();


	ImGui::NewFrame();

	ImGui::Begin("Object Tree");
	Object* root = wnd->getRoot();
	function<void(Object*)> drawNode;

	drawNode = [&drawNode](Object* node) {
		auto num_c = node->numChildren();
		ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_FramePadding;
		if (num_c == 0) {
			base_flags |= ImGuiTreeNodeFlags_Leaf;
		}
		bool is_open = ImGui::TreeNodeEx(node->name.c_str(), base_flags);
		if (ImGui::IsItemFocused()) {
			focused_obj = node;
		}
		if (is_open) {
			for (int i = 0; i < num_c; i++) {
				drawNode(node->children(i));
			}
			ImGui::TreePop();
		}
	};
	drawNode(root);
	ImGui::End();


	ImGui::Begin("Object viewer");
	if (focused_obj) {
		ImGui::Text(focused_obj->name.c_str());
		ImGui::InputFloat4("0", (float*)&(focused_obj->getTransform()[0]));
		ImGui::InputFloat4("1", (float*)&(focused_obj->getTransform()[1]));
		ImGui::InputFloat4("2", (float*)&(focused_obj->getTransform()[2]));
		ImGui::InputFloat4("3", (float*)&(focused_obj->getTransform()[3]));
	}
	ImGui::End();


	ImGui::Begin("mass-spring");
	ImGui::SliderFloat("K", &v, 100, 40000);
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

	canvas = scene.createObject("Canvas");
	auto wire2D = canvas->addComponent<Wire2D>();
	cout << ((Component*)wire2D)->get_type_name() << endl;
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