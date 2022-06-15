#include "Wire2D.h"
#include <windows.h>¡¡
#include <Eigen/Dense>


void Wire2D::update()
{
	implicitEuler(0.01);
	//=========================================
/*for (int i = 0; i < vertices.size(); i++) {
velocities[i][1] -= 10 * dt;
//velocities[i] *= 0.99;
//vertices[i] += dt * velocities[i];
//if (vertices[i][1] < -1) {
//	vertices[i][1] = -1;
//	velocities[i][0] *= 0.5;
//	velocities[i][1] = 0;
//}
}
for (int e = 0; e < edges.size(); e++) {
	dvec2& v0 = vertices[edges[e][0]];
	dvec2& v1 = vertices[edges[e][1]];
	dvec2 dv = v1 - v0;
	dvec2 normlized_dv = normalize(dv);
	double length_dv = length(dv);
	dvec2 force = K * (length(dv) - lens[e]) * normlized_dv;
	velocities[edges[e][0]] += force * dt;
	velocities[edges[e][1]] -= force * dt;
}

SparseMatrix<double> Hessian(vertices.size() * 2, vertices.size() * 2);
SparseMatrix<double> A(vertices.size() * 2, vertices.size() * 2);
VectorXd b(velocities.size() * 2);

for (int e = 0; e < edges.size(); e++) {
	auto& es = edges[e];
	dvec2& v0 = vertices[es[0]];
	dvec2& v1 = vertices[es[1]];
	dvec2 dv = v1 - v0;
	dvec2 normlized_dv = normalize(dv);
	double length_dv = length(dv);
	double coeff = K / length_dv;
	double d_length = lens[e] - length_dv;
	for (int n = 0; n < 4; n++) {
		int i = n / 2;
		int j = n % 2;

		double hess = coeff * ((i == j ? d_length : 0) - normlized_dv[i] * normlized_dv[j]);

		Hessian.coeffRef(2 * es[0] + i, 2 * es[0] + j) += hess;
		Hessian.coeffRef(2 * es[1] + i, 2 * es[0] + j) -= hess;
		Hessian.coeffRef(2 * es[0] + i, 2 * es[1] + j) -= hess;
		Hessian.coeffRef(2 * es[1] + i, 2 * es[1] + j) += hess;

	}
}

memcpy(b.data(), velocities.data(), velocities.size() * sizeof(dvec2));

for (int i = 0; i < b.rows(); i++) {
	A.coeffRef(i, i) += 1;
}

A -= Hessian * dt * dt;
SimplicialLLT<SparseMatrix<double>> solver;
solver.compute(A);
VectorXd X = solver.solve(b);

cout << (A * X - b).norm() << endl;


memcpy(velocities.data(), X.data(), velocities.size() * sizeof(dvec2));

for (int i = 0; i < vertices.size(); i++) {
	vertices[i] += dt * velocities[i];
	if (vertices[i][1] < -1) {
		vertices[i][1] = -1;
		velocities[i][0] *= 0.5;
		velocities[i][1] = 0;
	}
}
*/




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
		double length_dv = std::max<double>(1e-1, length(dv));
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
		vel = dvec2(-1, 0);
	}
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			vertices[i * N + j] = dvec2(i * 0.2, j * 0.2);
		}
	}
}

void Wire2D::implicitEuler(double dt)
{
	auto V = vertices.size();
	auto E = edges.size();

	for (int i = 0; i < vertices.size(); i++) {
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


	for (int times = 0; times < 20; times++) {
		computeForce();
		G = X - F * dt * dt - init_x;



		//LARGE_INTEGER t1, t2, tc;
		//QueryPerformanceFrequency(&tc);
		//QueryPerformanceCounter(&t1);


		//computeHessian(Hessian_D);
		//G1_D.setIdentity();
		//G1_D -= Hessian_D * dt * dt;



		//QueryPerformanceCounter(&t2);
		//auto time = (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;
		//cout << "time = " << time << endl;


		/**/computeJacobian(G1_D, dt);
		hessianTransfer(G1_D, G1);
		solver.compute(G1);
		VectorXd D = solver.solve(G);



		X = X - D;

	}

	cout << G.norm() << endl;

	computeForce();
	memcpy(vertices.data(), X.data(), 2 * V * sizeof(double));

	for (int i = 0; i < V; i++) {

		velocities[i] += dvec2(F[2 * i], F[2 * i + 1]) * dt;
		if (vertices[i][1] < -2) {
			vertices[i][1] = -2;
			velocities[i][0] *= 0.9;
			velocities[i][1] = 0;
		}
	}
}

void Wire2D::computeJacobian(MatrixXd& G1, double dt)
{
	G1.setIdentity();

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

			double hess = -coeff * ((i == j ? d_length : 0) - normlized_dv[i] * normlized_dv[j]) * dt * dt;

			G1(2 * es[0] + i, 2 * es[0] + j) += hess;
			//G1(2 * es[1] + i, 2 * es[0] + j) -= hess;
			//G1(2 * es[0] + i, 2 * es[1] + j) -= hess;
			G1(2 * es[1] + i, 2 * es[1] + j) += hess;

		}
	}
}

void Wire2D::hessianTransfer(MatrixXd& H_D, SparseMatrix<double>& H)
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
	velocities.resize(N * N, dvec2(-0.5, 0));
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			vertices.push_back(dvec2(i * 0.2, j * 0.2));
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
		double length_dv = std::max<double>(1e-1, length(dv));
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
