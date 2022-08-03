#include<Widget.h>
#include<Camera.h>
#include<Mesh.h>
#include "Canvas.h"

void MyScene(Scene& scene) {
	scene.createObject("Cloud")->addComponent<Cloud>();
	scene.createObject("Canvas")->addComponent<Canvas>();
	//scene.createObject("Cube")->addComponent<Mesh>()->getObject()->addComponent<MeshRenderer>();
}

int main() {
	Widget app;
	app.setScene = MyScene;
	app.init();
	app.run();
	app.destroy();
	return 0;
}