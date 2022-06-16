#pragma once
#include <Component.h>
#include <Renderer.h>
#include <Shader.h>
#include <Eigen/Sparse>
class Wire2D :public Component
{
	COMPONENT
public:
	vector<dvec2> vertices;
	vector<dvec2> velocities;
	vector<ivec2> edges;
	vector<double> lens;
	VectorXd init_x;
	VectorXd X;
	VectorXd G;
	VectorXd F;
	double K,M;
	vector<Triplet<double>> Hessian_triplets;
	

	virtual void update();
	void implicitEuler(double dt);
	void implicitEuler_simp(double dt);
	void reset();
	void computeForce();
	void computeHessian(SparseMatrix<double>& H);
	void computeHessian(MatrixXd& H);
	void computeJacobian(MatrixXd& G1,double dt);
	void DenseToSparse(MatrixXd& H_D,SparseMatrix<double>& H);

	Wire2D();
	~Wire2D() {};
};

class WireRenderer2D :public Renderer
{
	COMPONENT
public:
	Shader shader;
	GLuint VBO, VAO, EBO;

	virtual void start();
	void render(const mat4& view, const mat4& proj);

	WireRenderer2D();
	~WireRenderer2D() {};
};

