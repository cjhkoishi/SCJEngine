#include <Widget.h>
#include "BVHTree.h"
#include "Grid.h"

void MyUI(Widget* wnd)
{

}

void MyScene(Scene& scene) {
	glDisable(GL_DEPTH_TEST);
	scene.findByName("Camera")->setScale(vec3(200,200,1));
	auto container = scene.createObject("cont")->addComponent<Container>();
	auto ray = scene.createObject("ray")->addComponent<Ray>();
	auto bvh = scene.createObject("Tree")->addComponent<BVHTree>();
	ray->pos = dvec2(-150, 512);
	ray->dir = dvec2(1, 0.2);
	bvh->container = container;
	bvh->ray = ray;
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