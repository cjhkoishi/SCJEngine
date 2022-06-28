#include<Widget.h>
#include<Mesh.h>
#include"Nurbs.h"

Nurbs* curve;

void MyScene(Scene& sc) {
	//Object* cube = sc.createObject("CubeRoot");
	//cube->addComponent<Mesh>();
	//cube->addComponent<MeshRenderer>();
	//Object* cube0 = sc.createObject("Cubespine");
	//cube0->addComponent<Mesh>();
	//cube0->addComponent<MeshRenderer>();
	//cube->addChild(cube0);
	Object* nb = sc.createObject("Nurbs");
	curve = nb->addComponent<Nurbs>();
	nb->addComponent<NurbsRenderer>();

	curve->curve.p = 3;
	curve->curve.isRational = true;
	curve->curve.knot_vector = { 0,0,0,0,0.25,0.3,0.5,0.75,1,1,1,1 };
	curve->curve.control_points = {
		CAAVector<3>(1, 0, 0.1),
		CAAVector<3>(1, 1, 0.2),
		CAAVector<3>(0, 1, 0.1),
		CAAVector<3>(-1, 1, 0),
		CAAVector<3>(0, 1, 1.5),
		CAAVector<3>(1, 1, 1.8),
		CAAVector<3>(1, 0, 1.2),
		CAAVector<3>(0, 0, 1),
	};
	curve->curve.weights = { 1,sqrt(2),5,sqrt(2),1,2,1,0.5 };
	curve->curve.increaseDegree(2);
	curve->curve.insert(0.4);

	EnergyFairing::Constraint cons1;
	cons1.t = 0.4;
	cons1.level = 1;
	cons1.position = curve->curve.eval(0.4);
	cons1.tangent = curve->curve.eval(0.4, 1) + CAAVector<3>(1, 1, 1);
	cons1.curvature = curve->curve.curvature(0.4) * 0;
	curve->constraints.push_back(cons1);

	EnergyFairing::Constraint cons2;
	cons2.t = 0.5;
	cons2.level = 2;
	cons2.position = curve->curve.eval(0.5);
	cons2.tangent = curve->curve.eval(0.5, 1) + CAAVector<3>(1, -1, 1);
	cons2.curvature = curve->curve.curvature(0.5)*4;
	curve->constraints.push_back(cons2);
}

void myUI(Widget* wnd) {
	ImGui::Begin("Curve");
	if (ImGui::Button("apply")) {
		EnergyFairing EF;
		EF.alpha = 100;
		//EF.gamma = 0.01;
		vector<bool> var;
		EF.getVariablePts(curve->curve, curve->constraints, var);
		for_each(var.begin(), var.end(), [](const bool& item) {cout << item; });
		EF.multiGeoConstraint(curve->curve, curve->constraints, var);
		curve->isChanged = true;
	}
	ImGui::End();
}

int main() {
	Widget wnd;
	wnd.setScene = MyScene;
	wnd.drawUI = myUI;
	wnd.init();
	wnd.run();
	wnd.destroy();
	return 0;
}