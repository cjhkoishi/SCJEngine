#include<Widget.h>
#include<Mesh.h>
#include"Nurbs.h"

Nurbs* curve;

EnergyFairing::Constraint cons1;
EnergyFairing::Constraint cons2;
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
	curve->curve.insert(0.45);
	curve->curve.insert(0.55);
	curve->curve.insert(0.6);


	cons1.t = 0.4;
	cons1.level = 2;
	cons1.position = curve->curve.eval(0.4) + 0.1 * CAAVector<3>(-1, 1, 1);
	cons1.tangent = /*curve->curve.eval(0.4, 1) +*/ 1 * CAAVector<3>(1, 1, 0.05);
	cons1.curvature = curve->curve.curvature(0.4) * 2;
	cons1.regularize();
	curve->constraints.push_back(cons1);

	cons2.t = 0.5;
	cons2.level = 2;
	cons2.position = curve->curve.eval(0.5);
	cons2.tangent = curve->curve.eval(0.5, 1) + CAAVector<3>(1, -1, 1);
	cons2.curvature = curve->curve.curvature(0.5) * 40/*20*/;
	cons2.regularize();
	curve->constraints.push_back(cons2);
}

EnergyFairing EF;
void myUI(Widget* wnd) {

	ImGui::Begin("Curve");
	ImGui::InputDouble("alpha", &EF.alpha);
	ImGui::InputDouble("beta", &EF.beta);
	ImGui::InputDouble("gamma", &EF.gamma);
	ImGui::InputDouble("delta", &EF.delta);
	if (ImGui::Button("apply")) {
		vector<bool> var;
		EF.getVariablePts(curve->curve, curve->constraints, var);
		for_each(var.begin(), var.end(), [](const bool& item) {cout << item; });
		EF.multiGeoConstraint(curve->curve, curve->constraints, var, 0.25);
		curve->isChanged = true;
	}
	if (ImGui::Button("apply_single")) {
		int b[2]{ 0,0 };
		EF.addGeometricConstraintsOnKnot(curve->curve, 0.5, cons2.position, cons2.tangent, cons2.curvature, b);
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