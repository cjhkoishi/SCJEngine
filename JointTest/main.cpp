#include<Widget.h>
#include<Mesh.h>

void MyScene(Scene& sc) {
	Object* cube = sc.createObject("CubeRoot");
	cube->addComponent<Mesh>();
	cube->addComponent<MeshRenderer>();
	Object* cube0 = sc.createObject("Cubespine");
	cube0->addComponent<Mesh>();
	cube0->addComponent<MeshRenderer>();
	cube->addChild(cube0);
}

int main() {
	Widget wnd;
	wnd.setScene = MyScene;
	wnd.init();
	wnd.run();
	wnd.destroy();
	return 0;
}