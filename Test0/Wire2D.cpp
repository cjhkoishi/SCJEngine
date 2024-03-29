#include "Wire2D.h"
#include <Widget.h>
#include <windows.h>��
#include <Eigen/Dense>


void Wire2D::update()
{
	implicitEuler_simp(0.01);
}

void Wire2D::computeHessian(SparseMatrix<double>& H)
{
	H.setZero();

	for (int e = 0; e < edges.size(); e++) {

		auto& es = edges[e];
		dvec2 v0(X[2 * edges[e][0]], X[2 * edges[e][0] + 1]);
		dvec2 v1(X[2 * edges[e][1]], X[2 * edges[e][1] + 1]);
		dvec2 dv = v1 - v0;
		dvec2 normlized_dv = normalize(dv);
		double length_dv = std::max<double>(1e-1, length(dv));
		double coeff = K / length_dv;
		double d_length = lens[e] - length_dv;


		for (int n = 0; n < 4; n++) {
			int i = n / 2;
			int j = n % 2;

			double hess = coeff * ((i == j ? d_length : 0) - normlized_dv[i] * normlized_dv[j]);

			H.coeffRef(2 * es[0] + i, 2 * es[0] + j) += hess;
			H.coeffRef(2 * es[1] + i, 2 * es[0] + j) -= hess;
			H.coeffRef(2 * es[0] + i, 2 * es[1] + j) -= hess;
			H.coeffRef(2 * es[1] + i, 2 * es[1] + j) += hess;

		}
	}

}

void Wire2D::computeForce()
{
	F.setZero();
	for (int e = 0; e < edges.size(); e++) {
		dvec2 v0(X[2 * edges[e][0]], X[2 * edges[e][0] + 1]);
		dvec2 v1(X[2 * edges[e][1]], X[2 * edges[e][1] + 1]);
		dvec2 dv = v1 - v0;
		dvec2 normlized_dv = normalize(dv);
		double length_dv = std::max<double>(1e-3, length(dv));
		dvec2 force = K * (length_dv - lens[e]) * normlized_dv;
		F[2 * edges[e][0]] += force[0];
		F[2 * edges[e][0] + 1] += force[1];
		F[2 * edges[e][1]] -= force[0];
		F[2 * edges[e][1] + 1] -= force[1];
	}
	//gravity
	for (int i = 0; i < vertices.size(); i++) {
		F[2 * i + 1] -= 10;
	}
}
int N = 10;
void Wire2D::reset()
{
	for (auto& vel : velocities) {
		vel = dvec2(-2, 0);
	}
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			vertices[i * N + j] = dvec2(i * 0.3, j * 0.3);
		}
	}
}

void Wire2D::implicitEuler(double dt)
{
	auto V = vertices.size();
	auto E = edges.size();

	for (int i = 0; i < vertices.size(); i++) {
		velocities[i] *= 0.999;
		vertices[i] += velocities[i] * dt;
	}

	memcpy(init_x.data(), vertices.data(), 2 * V * sizeof(double));
	memcpy(X.data(), vertices.data(), 2 * V * sizeof(double));
	//ConjugateGradient<MatrixXd> solver;
	ConjugateGradient<SparseMatrix<double>> solver;
	MatrixXd Hessian_D(V * 2, V * 2);
	MatrixXd G1_D(V * 2, V * 2);
	SparseMatrix<double> Hessian(V * 2, V * 2);
	SparseMatrix<double> G1(V * 2, V * 2);
	VectorXd D(V * 2);

	for (int times = 0; times < 5; times++) {
		computeForce();
		G = X - F * dt * dt - init_x;



		//LARGE_INTEGER t1, t2, tc;
		//QueryPerformanceFrequency(&tc);
		//QueryPerformanceCounter(&t1);


		computeHessian(Hessian_D);
		G1_D.setIdentity();
		G1_D -= Hessian_D * dt * dt;



		//QueryPerformanceCounter(&t2);
		//auto time = (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;
		//cout << "time = " << time << endl;



		//computeJacobian(G1_D, dt);
		DenseToSparse(G1_D, G1);
		solver.compute(G1);
		VectorXd D = solver.solve(G);/**/

		D = G;
		for (int i = 0; i < D.rows(); i++) {
			D[i] /= G1_D(i, i);
		}


		X = X - D;

	}

	cout << G.norm() << endl;

	computeForce();
	memcpy(vertices.data(), X.data(), 2 * V * sizeof(double));

	for (int i = 0; i < V; i++) {

		velocities[i] += dvec2(F[2 * i], F[2 * i + 1]) * dt;
		if (vertices[i][1] < -2) {
			vertices[i][1] = -2;
			//velocities[i][0] *= 0.9;
			velocities[i][1] = 0;
		}
		if (vertices[i][0] < -2) {
			vertices[i][0] = -2;
			//velocities[i][0] *= 0.9;
			velocities[i][0] = 0;
		}

		if (glfwGetMouseButton(_object->getScene()->getWidget()->getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT)) {
			dvec2 pos;
			glfwGetCursorPos(_object->getScene()->getWidget()->getGLFWwindow(), (double*)&pos, (double*)&pos + 1);
			vec3 dir = _object->getScene()->getWidget()->input_system.getCursorDirection(pos);
			//cout << dir[2] << endl;
			dir *= -5 / dir[2];
			vertices[0] = dir;
			velocities[0] = dvec2(0, 0);
		}
	}
}

void Wire2D::implicitEuler_simp(double dt)
{
	auto V = vertices.size();
	auto E = edges.size();

	for (int i = 0; i < vertices.size(); i++) {
		velocities[i] *= 0.999;
		vertices[i] += velocities[i] * dt;
	}

	memcpy(init_x.data(), vertices.data(), 2 * V * sizeof(double));
	memcpy(X.data(), vertices.data(), 2 * V * sizeof(double));
	//ConjugateGradient<MatrixXd> solver;
	//ConjugateGradient<SparseMatrix<double>> solver;
	MatrixXd Hessian_D(V * 2, V * 2);
	MatrixXd G1_D(V * 2, V * 2);
	SparseMatrix<double> Hessian(V * 2, V * 2);
	SparseMatrix<double> G1(V * 2, V * 2);
	VectorXd D(V * 2);

	for (int times = 0; times < 32; times++) {
		computeForce();
		G = X - F * dt * dt - init_x;

		computeJacobian(G1_D, dt);

		D = G;
		for (int i = 0; i < D.rows(); i++) {
			D[i] /= G1_D(i, i);
		}

		X = X - D;

	}

	//cout << G.norm() << endl;

	computeForce();
	memcpy(vertices.data(), X.data(), 2 * V * sizeof(double));

	for (int i = 0; i < V; i++) {

		velocities[i] += dvec2(F[2 * i], F[2 * i + 1]) * dt;
		if (vertices[i][1] < -2) {
			vertices[i][1] = -2;
			//velocities[i][0] *= 0.9;
			velocities[i][1] = 0;
		}
		if (vertices[i][0] < -2) {
			vertices[i][0] = -2;
			//velocities[i][0] *= 0.9;
			velocities[i][0] = 0;
		}

		if (glfwGetMouseButton(_object->getScene()->getWidget()->getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT)) {
			dvec2 pos;
			glfwGetCursorPos(_object->getScene()->getWidget()->getGLFWwindow(), (double*)&pos, (double*)&pos + 1);
			vec3 dir = _object->getScene()->getWidget()->input_system.getCursorDirection(pos);
			//cout << dir[2] << endl;
			dir *= -5 / dir[2];
			vertices[0] = dir;
			velocities[0] = dvec2(0, 0);
		}
	}
}

void Wire2D::computeJacobian(MatrixXd& G1, double dt)
{
	G1.setIdentity();
	double dt2 = dt * dt;
	for (int e = 0; e < edges.size(); e++) {

		auto& es = edges[e];
		dvec2 v0(X[2 * edges[e][0]], X[2 * edges[e][0] + 1]);
		dvec2 v1(X[2 * edges[e][1]], X[2 * edges[e][1] + 1]);
		dvec2 dv = v1 - v0;
		dvec2 normlized_dv = normalize(dv);
		double length_dv = std::max<double>(1e-3, length(dv));
		double coeff = K / length_dv;
		double d_length = lens[e] - length_dv;



		for (int n = 0; n < 2; n++) {
			double hess = -coeff * (d_length - normlized_dv[n] * normlized_dv[n]) * dt2;

			G1(2 * es[0] + n, 2 * es[0] + n) += hess;
			//G1(2 * es[1] + i, 2 * es[0] + j) -= hess;
			//G1(2 * es[0] + i, 2 * es[1] + j) -= hess;
			G1(2 * es[1] + n, 2 * es[1] + n) += hess;

		}
	}
}

void Wire2D::DenseToSparse(MatrixXd& H_D, SparseMatrix<double>& H)
{
	for (auto& trip : Hessian_triplets) {
		//trip = Triplet<double>(trip.row(), trip.col(), H_D(trip.row(), trip.col()));
		trip.value() = H_D(trip.row(), trip.col());
	}
	H.setFromTriplets(Hessian_triplets.begin(), Hessian_triplets.end());
}

Wire2D::Wire2D()
{
	K = 3500;
	auto cube_index = [](int i, int j)->int {
		return i * N + j;
	};
	velocities.resize(N * N, dvec2(-2, 0));
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			vertices.push_back(dvec2(i * 0.3, j * 0.3));
			auto edge_push = [this](ivec2 edge) {
				edges.push_back(edge);
			};

			if (i != N - 1) {
				//edges.push_back(ivec2(cube_index(i, j), cube_index(i + 1, j)));
				edge_push(ivec2(cube_index(i, j), cube_index(i + 1, j)));
				if (j != N - 1) {
					//edges.push_back(ivec2(cube_index(i, j + 1), cube_index(i + 1, j)));
					//edges.push_back(ivec2(cube_index(i, j), cube_index(i + 1, j + 1)));
					//if ((i + j) % 2)
					edge_push(ivec2(cube_index(i, j + 1), cube_index(i + 1, j)));
					//else
					edge_push(ivec2(cube_index(i, j), cube_index(i + 1, j + 1)));
				}
			}
			if (j != N - 1) {
				//edges.push_back(ivec2(cube_index(i, j), cube_index(i, j + 1)));
				edge_push(ivec2(cube_index(i, j), cube_index(i, j + 1)));
			}
		}
	}
	for (auto edge : edges) {
		lens.push_back(length(vertices[edge[0]] - vertices[edge[1]]));
	}

	auto V = vertices.size();
	auto E = edges.size();

	init_x.resize(2 * V);
	X.resize(2 * V);
	G.resize(2 * V);
	F.resize(2 * V);

	Hessian_triplets.resize(2 * 4 * E + 4 * V);
	int t = 0;
	for (int n = 0; n < V; n++) {
		for (int i = 0; i < 4; i++) {
			auto nn = 2 * n;
			Hessian_triplets[t++] = Triplet<double>(nn + (i / 2), nn + (i % 2));
		}
	}
	for (auto& edge : edges) {
		for (int i = 0; i < 4; i++) {
			auto b_edge = edge * 2;
			Hessian_triplets[t++] = Triplet<double>(b_edge[0] + (i / 2), b_edge[1] + (i % 2));
			Hessian_triplets[t++] = Triplet<double>(b_edge[1] + (i / 2), b_edge[0] + (i % 2));
		}
	}

}

void Wire2D::computeHessian(MatrixXd& H)
{
	H.setZero();

	for (int e = 0; e < edges.size(); e++) {

		auto& es = edges[e];
		dvec2 v0(X[2 * edges[e][0]], X[2 * edges[e][0] + 1]);
		dvec2 v1(X[2 * edges[e][1]], X[2 * edges[e][1] + 1]);
		dvec2 dv = v1 - v0;
		dvec2 normlized_dv = normalize(dv);
		double length_dv = std::max<double>(1e-3, length(dv));
		double coeff = K / length_dv;
		double d_length = lens[e] - length_dv;


		for (int n = 0; n < 4; n++) {
			int i = n / 2;
			int j = n % 2;

			double hess = coeff * ((i == j ? d_length : 0) - normlized_dv[i] * normlized_dv[j]);

			H(2 * es[0] + i, 2 * es[0] + j) += hess;
			H(2 * es[1] + i, 2 * es[0] + j) -= hess;
			H(2 * es[0] + i, 2 * es[1] + j) -= hess;
			H(2 * es[1] + i, 2 * es[1] + j) += hess;

		}
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
		glLineWidth(2);
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
		shader.setVec4("color", vec4(1, 1, 1, 1));
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
