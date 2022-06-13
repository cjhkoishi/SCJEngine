#include "Wire2D.h"

void Wire2D::update()
{
	double dt = 0.01;
	double k = 3500;
	for (int i = 0; i < vertices.size(); i++) {
		velocities[i][1] -= 10 * dt;
		velocities[i] *= 0.99;
		vertices[i] += dt * velocities[i];
		if (vertices[i][1] < -1) {
			vertices[i][1] = -1;
			velocities[i][0] *= 0.5;
			velocities[i][1] = 0;
		}
	}
	SparseMatrix<double> Hessian(vertices.size() * 2, vertices.size() * 2);
	SparseMatrix<double> A(vertices.size() * 2, vertices.size() * 2);

	for (int e = 0; e < edges.size(); e++) {
		auto& es = edges[e];
		dvec2& v0 = vertices[es[0]];
		dvec2& v1 = vertices[es[1]];
		dvec2 dv = v1 - v0;
		dvec2 normlized_dv = normalize(dv);
		double length_dv = length(dv);
		double coeff = k / length_dv;
		double d_length = lens[e] - length_dv;
		for (int n = 0; n < 4; n++) {
			int i = n / 2;
			int j = i % 2;

			double hess = coeff * ((i==j?d_length:0)-normlized_dv[i]* normlized_dv[j]);

			Hessian.coeffRef(2 * es[0] + i, 2 * es[0] + j)+= hess;
			Hessian.coeffRef(2 * es[1] + i, 2 * es[0] + j)-= hess;
			Hessian.coeffRef(2 * es[0] + i, 2 * es[1] + j)-= hess;
			Hessian.coeffRef(2 * es[1] + i, 2 * es[1] + j)+= hess;

		}
	}

	for (int i = 0; i < vertices.size(); i++) {
		A.coeffRef(i, i) += 1 / (dt * dt);

	}

	for (int e = 0; e < edges.size(); e++) {
		dvec2& v0 = vertices[edges[e][0]];
		dvec2& v1 = vertices[edges[e][1]];
		dvec2 dv = v1 - v0;
		dvec2 normlized_dv = normalize(dv);
		double length_dv = length(dv);
		dvec2 force = k * (length(dv) - lens[e]) * normlized_dv;
		velocities[edges[e][0]] += force * dt;
		velocities[edges[e][1]] -= force * dt;
	}



}

Wire2D::Wire2D()
{
	int N = 11;
	auto cube_index = [N](int i, int j)->int {
		return i * N + j;
	};
	velocities.resize(N * N, dvec2(-0.5, 0));
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			vertices.push_back(dvec2(i * 0.2, j * 0.2));

			if (i != N - 1) {
				edges.push_back(ivec2(cube_index(i, j), cube_index(i + 1, j)));
				if (j != N - 1) {
					edges.push_back(ivec2(cube_index(i, j + 1), cube_index(i + 1, j)));
					edges.push_back(ivec2(cube_index(i, j), cube_index(i + 1, j + 1)));
				}
			}
			if (j != N - 1) {
				edges.push_back(ivec2(cube_index(i, j), cube_index(i, j + 1)));
			}
		}
	}
	for (auto edge : edges) {
		lens.push_back(length(vertices[edge[0]] - vertices[edge[1]]));
	}
}


void WireRenderer2D::start()
{
	auto wire = _object->getComponent<Wire2D>();
	if (wire) {
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, wire->vertices.size() * sizeof(dvec2), &wire->vertices[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, wire->edges.size() * sizeof(ivec2), &wire->edges[0], GL_STATIC_DRAW);
	}
}

void WireRenderer2D::render(const mat4& view, const mat4& proj)
{
	auto wire = _object->getComponent<Wire2D>();
	if (wire) {
		glDisable(GL_DEPTH_TEST);
		shader.use();
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, wire->vertices.size() * sizeof(dvec2), &wire->vertices[0]);
		auto model = _object->getWorldTransform();
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, (GLfloat*)&model);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, (GLfloat*)&view);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, (GLfloat*)&proj);
		glDrawElements(GL_LINES, wire->edges.size() * 2, GL_UNSIGNED_INT, 0);
		glEnable(GL_DEPTH_TEST);
	}
}

WireRenderer2D::WireRenderer2D()
{
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, 2 * sizeof(double), (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	shader = shader_asset["default"];
}
