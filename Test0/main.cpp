#include <Window.h>
#include <ViewPort.h>
int main(){
	Window wnd;
	ViewPort vp;
	wnd.init();
	wnd.run();
	wnd.destroy();
	return 0;
}