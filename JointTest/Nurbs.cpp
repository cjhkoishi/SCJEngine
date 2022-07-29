#include "Nurbs.h"
#include <Shader.h>

void Nurbs::update()
{
	if (isChanged) {
		int N = 500;
		sample_data.resize(N + 1);
		CAAVector<2> limit = curve.getLimits();

		for (int i = 0; i <= N; i++) {
			double t = limit[0] * (1.0 * i / N) + limit[1] * (1 - 1.0 * i / N);
			auto pt = curve.eval(t);
			sample_data[i] = vec3(pt[0], pt[1], pt[2]);
		}
		for (auto& cons : constraints) {
			cons.regularize();
		}

		isChanged = false;
	}
}

void NurbsRenderer::start()
{
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glGenVertexArrays(1, &VAOD);
	glBindVertexArray(VAOD);
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(double), (void*)0);
	glEnableVertexAttribArray(0);
}

void NurbsRenderer::render(const mat4& view, const mat4& proj)
{
	Shader def = shader_asset["default"];
	def.use();

	auto nurbs = _object->getComponent<Nurbs>();
	auto model = _object->getWorldTransform();
	def.setMat4("model", model);
	def.setMat4("view", view);
	def.setMat4("projection", proj);
	def.setVec4("color", vec4(1, 1, 1, 1));

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nurbs->sample_data.size() * sizeof(vec3), &nurbs->sample_data[0], GL_DYNAMIC_DRAW);
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINE_STRIP, 0, nurbs->sample_data.size());

	glBindVertexArray(VAOD);
	glBufferData(GL_ARRAY_BUFFER, nurbs->curve.control_points.size() * sizeof(CAAVector<3>), &nurbs->curve.control_points[0], GL_DYNAMIC_DRAW);
	def.setVec4("color", vec4(1, 0, 0, 1));
	glDrawArrays(GL_LINE_STRIP, 0, nurbs->curve.control_points.size());
	vector<CAAVector<3>> arc_pts;
	vector<int> ts;
	nurbs->curve.getAllKnotPoints(ts);
	for (auto k : ts) {
		arc_pts.push_back(nurbs->curve.eval(nurbs->curve.knot_vector[k]));
	}
	glBufferData(GL_ARRAY_BUFFER, arc_pts.size() * sizeof(CAAVector<3>), &arc_pts[0], GL_DYNAMIC_DRAW);
	def.setVec4("color", vec4(0, 1, 0, 1));
	glPointSize(8);
	glDrawArrays(GL_POINTS, 0, arc_pts.size());

	for (auto& cons : nurbs->constraints) {
		CAAVector<3> line[2];
		line[0] = cons.position;
		line[1] = cons.position + cons.tangent;
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(double), line, GL_DYNAMIC_DRAW);
		def.setVec4("color", vec4(0, 1, 1, 1));
		glDrawArrays(GL_LINES, 0, 2);
		if (cons.level < 2)
			continue;
		auto T = cons.tangent / cons.curvature.norm();
		auto K = cons.curvature / pow(cons.curvature.norm(), 2);
		double circle[101][3];
		for (int i = 0; i <= 100; i++) {
			double theta = (i / 100.0) * 2 * pi<double>();
			auto P = cons.position + T * sin(theta) + K * (1 + cos(theta));
			circle[i][0] = P[0];
			circle[i][1] = P[1];
			circle[i][2] = P[2];
		}
		glBufferData(GL_ARRAY_BUFFER, 101 * 3 * sizeof(double), circle, GL_DYNAMIC_DRAW);
		def.setVec4("color", vec4(1, 0, 1, 1));
		glDrawArrays(GL_LINE_STRIP, 0, 101);
	}
}