#include <Widget.h>
#include <ViewPort.h>
#include <Renderer.h>
#include <Mesh.h>
#include "Wire2D.h"


Object* cube;
Object* canvas;
float v=3500;

void MyUI(Widget* wnd)
{

	ImGui::Begin("mass-spring");
	ImGui::SliderFloat("K", &v, 100, 40000);
	canvas->getComponent<Wire2D>()->K = v;

	if (ImGui::Button("reset")) {
		canvas->getComponent<Wire2D>()->reset();
	};
	ImGui::End();


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

	canvas = scene.createObject("Cloud");
	auto wire2D = canvas->addComponent<Wire2D>();
	cout << ((Component*)wire2D)->get_type_name() << endl;
	canvas->addComponent<WireRenderer2D>();

	Object* camera = scene.findByName("Camera");
	camera->getComponent<Camera>()->controllable = false;
}

int main() {
	Widget wnd;

	wnd.drawUI = MyUI;
	wnd.setScene = MyScene;

	wnd.init();
	wnd.run();
	wnd.destroy();
	return 0;
}