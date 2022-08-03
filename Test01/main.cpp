#include <Widget.h>
#include <ViewPort.h>
#include <Renderer.h>
#include <Mesh.h>


Object* cube;
Object* canvas;
float v=3500;

void MyUI(Widget* wnd)
{


}

void MyScene(Scene& scene) {


	canvas = scene.createObject("Cloud");
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