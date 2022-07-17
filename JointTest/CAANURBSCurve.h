#pragma once
#include<vector>
#include<map>
#include<set>
#include <cassert>
#include <math.h>

#ifdef CAA_ENV
#include "CATNurbsCurve.h"
#include "CATKnotVector.h"
#include "CATGeoFactory.h"
#endif // CAA_ENV


using namespace std;

template<int n>
class CAAVector
{
public:
	double data[n];

	CAAVector()
	{
		for (int i = 0; i < n; i++)
			data[i] = 0;
	}

	CAAVector(double data[n])
	{
		memcpy(this->data, data, n * sizeof(double));
	}

	CAAVector(double x, double y)
	{
		data[0] = x;
		data[1] = y;
	}

	CAAVector(double x, double y, double z)
	{
		data[0] = x;
		data[1] = y;
		data[2] = z;
	}

	inline double& operator[](int index) const
	{
		return ((double*)data)[index];
	}

	inline CAAVector operator+(const CAAVector& rhs) const
	{
		CAAVector res;
		for (int i = 0; i < n; i++)
			res[i] = data[i] + rhs[i];
		return res;
	}

	inline CAAVector operator-(const CAAVector& rhs)const
	{
		CAAVector res;
		for (int i = 0; i < n; i++)
			res[i] = data[i] - rhs[i];
		return res;
	}

	inline CAAVector operator*(double rhs)const
	{
		CAAVector res;
		for (int i = 0; i < n; i++)
			res[i] = data[i] * rhs;
		return res;
	}

	inline CAAVector operator/(double rhs)const
	{
		CAAVector res;
		for (int i = 0; i < n; i++)
			res[i] = data[i] / rhs;
		return res;
	}

	inline double norm()const
	{
		double res = 0;
		for (int i = 0; i < n; i++)
			res += data[i] * data[i];
		return sqrt(res);
	}

	friend inline CAAVector operator*(double lhs, const CAAVector& rhs) {
		CAAVector res;
		for (int i = 0; i < n; i++)
			res[i] = rhs[i] * lhs;
		return res;
	}

	inline double dot(const CAAVector& rhs)const
	{
		double res = 0;
		for (int i = 0; i < n; i++)
			res += rhs[i] * data[i];
		return res;
	}

	inline CAAVector<3> cross(const CAAVector<3>& rhs)const
	{
		return CAAVector<3>(
			data[1] * rhs[2] - data[2] * rhs[1],
			data[2] * rhs[0] - data[0] * rhs[2],
			data[0] * rhs[1] - data[1] * rhs[0]
			);
	}

	inline CAAVector<3> cross(const CAAVector<2>& rhs)const
	{
		return CAAVector<3>(0, 0, data[0] * rhs[1] - data[1] * rhs[0]);
	}
};

class CAADynamicMatrix {

private:
	double* data;
	int m, n;
public:
	CAADynamicMatrix() :
		data(NULL),
		m(0),
		n(0)
	{}

	CAADynamicMatrix(int m, int n) :
		data(new double[m * n]),
		m(m),
		n(n)
	{
		memset(data, 0, m * n * sizeof(double));
	}

	CAADynamicMatrix(vector<double> coords, int m, int n) :
		data(new double[m * n]),
		m(m),
		n(n)
	{
		memcpy(data, (double*)&coords, m * n * sizeof(double));
	}

	CAADynamicMatrix(vector<double> coords) :
		data(new double[coords.size()]),
		m(coords.size()),
		n(1)
	{
		memcpy(data, (double*)&coords, coords.size() * sizeof(double));
	}

	CAADynamicMatrix(const CAADynamicMatrix& obj) :
		data(NULL)
	{
		resize(obj.m, obj.n);
		memcpy(data, obj.data, obj.size() * sizeof(double));
	}

	~CAADynamicMatrix() {
		if (data != NULL)
			delete[] data;
		data = NULL;
	}

	CAADynamicMatrix& operator=(const CAADynamicMatrix& obj) {
		resize(obj.m, obj.n);
		memcpy(data, obj.data, obj.size() * sizeof(double));
		return *this;
	}

	inline void resize(int m, int n) {
		this->m = m;
		this->n = n;
		if (data != NULL)
			delete[] data;
		data = new double[m * n];
		memset(data, 0, m * n * sizeof(double));
	}

	inline int size() const {
		return m * n;
	}

	inline double& operator[](int index)const {
		//assert(m > 0 && n > 0);
		return data[index];
	};

	inline double& operator()(int i, int j)const {
		return data[i * n + j];
	}

	inline double norm() {
		double res = 0;
		int k = m * n;
		for (int i = 0; i < k; i++) {
			res += data[i] * data[i];
		}
		return sqrt(res);
	}

	inline CAADynamicMatrix operator+(const CAADynamicMatrix& rhs) {
		assert(m == rhs.m && n == rhs.n);
		int k = m * n;
		CAADynamicMatrix res(m, n);
		for (int i = 0; i < k; i++) {
			res[i] = data[i] + rhs[i];
		}
		return res;
	}

	inline CAADynamicMatrix operator-(const CAADynamicMatrix& rhs) {
		assert(m == rhs.m && n == rhs.n);
		int k = m * n;
		CAADynamicMatrix res(m, n);
		for (int i = 0; i < k; i++) {
			res[i] = data[i] - rhs[i];
		}
		return res;
	}

	inline CAADynamicMatrix operator*(const CAADynamicMatrix& rhs)
	{
		assert(n == rhs.m);
		CAADynamicMatrix res(m, rhs.n);
		for (int i = 0; i < m; i++) {
			for (int j = 0; j < rhs.n; j++) {
				res(i, j) = 0;
				for (int p = 0; p < n; p++) {
					res(i, j) += (*this)(i, p) * rhs(p, j);
				}

			}
		}
		return res;
	}

	inline CAADynamicMatrix operator*(double scalar)const {
		CAADynamicMatrix res(m, n);
		int k = m * n;
		for (int i = 0; i < k; i++) {
			res[i] = (*this)[i] * scalar;
		}
		return res;
	}

	friend CAADynamicMatrix operator*(double scalar, const CAADynamicMatrix& rhs) {
		CAADynamicMatrix res(rhs.m, rhs.n);
		int k = rhs.m * rhs.n;
		for (int i = 0; i < k; i++) {
			res[i] = rhs[i] * scalar;
		}
		return res;
	}

	CAADynamicMatrix transpose() const {
		assert(data != NULL);
		CAADynamicMatrix res(n, m);
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				res(i, j) = (*this)(j, i);
			}
		}
		return res;
	}

	CAADynamicMatrix LUPSolve(CAADynamicMatrix b);
};

template<int dim = 3>
class CAANURBSCurve
{
public:
	int p;
	vector<double> knot_vector;
	vector<CAAVector<dim>> control_points;
	vector<double> weights;
	bool isRational;

	bool isValid();//working

	/*
	����������󶨵�����cp��deBoor�㷨�����tΪ������kΪָ���Ľڵ�����
	*/
	template<typename T> T deBoor(double t, int k, vector<T>& cp);

	/*
	����������󶨵�����cp��order�׵���deBoor�㷨�����tΪ������kΪָ���Ľڵ�����
	*/
	template<typename T> T deBoor(double t, int k, int order, vector<T>& cp);

	/*
	�������������¸���������cp��deBoor�㷨�����tΪ������kΪָ���Ľڵ�����
	*/
	template<typename T> T deBoorWithWeights(double t, int k, int order, vector<T>& cp);

	/*
	�������t���ڵĽڵ��������
	*/
	int findSpan(double t);

	/*
	Ѱ������Ľڵ�
	*/
	int findClosest(double t);

	/*
	��ȡ��Ч��������
	*/
	CAAVector<2> getLimits();

	/*
	��ת��������
	*/
	void reverse();

	/*
	���ص�k�����߶ζ�Ӧ�Ľڵ��������
	*/
	int segment(int k);

	/*
	��ȡָ���ڵ�����k�£�����t������������
	*/
	CAAVector<dim> evalInSpan(double t, int k);

	/*
	��ȡָ���ڵ�����k�£�����t��������order�׵���
	*/
	CAAVector<dim> evalInSpan(double t, int k, int order);

	/*
	��ȡ����t������������
	*/
	CAAVector<dim> eval(double t);

	/*
	��ȡ����t��������order�׵���
	*/
	CAAVector<dim> eval(double t, int order);

	/*
	��ȡ���з���ڵ���������
	*/
	void getAllKnotPoints(vector<int>& ts);

	/*
	����ڵ�����k��order�׵������Ҽ���
	*/
	CAAVector<dim> knotEval(int k, bool direction, int order);

	/*
	����B����������N_i(t)
	*/
	double N(int i, double t);

	/*
	����B��������������N_i^(d)(t)
	*/
	double N_d(int i, double t, int d);

	/*
	����Nurbs�������������B_i^(d)(t)����������������
	*/
	double B_d(int i, double t, int d);

	/*
	�������ʸˣ�dim=3��
	*/
	CAAVector<3> curvature(double t);

	/*
	����ƽ���������ʹ�ʽ�еķ���
	*/
	double kappa(double t, int k);

	/*
	�������������е�(i,j)�int(B_i^(d) B_j^(d),dt)������10�ζ���ʽ��˹���ּ���
	*/
	double energyInteral(int i, int j, int d);

	/*
	t����������ڵ�
	*/
	void insert(double t);

	/*
	ɾ����k���ڵ�
	*/
	bool remove(int k);

	/*
	ɾ��t���Ľڵ�һ��
	*/
	void erase(double t);

	/*
	��������s
	*/
	void increaseDegree(int s);

	/*
	ʵ�麯�������ڵ��ֵʵ��
	*/
	void interpolation(vector<CAAVector<dim>> pts);

	//////////////////////////////////////////
	// ���������� ���������ڶ�����Ӧ��������˳�㷨
	// ��ڲ����� alpha��һ�ڵ�ʸƽ����������ϵ�� beta�����ڵ�ʸƽ����������ϵ�� gamma�����ڵ�ʸƽ����������ϵ�� delta�����Ƶ�����ϵ��
	// ���ڲ����� ��
	// ����ֵ�� ��
	// ���ߣ��½��
	// �������ڣ� 2022-04-20
	// ��Ȩ���У� �㽭��ѧ-���������˼���
	//////////////////////////////////////////
	void smoothing(double alpha, double beta, double gamma, double delta = 1);

	//////////////////////////////////////////
	// ���������� ����������Ѱ�Ҷ�άNurbs���������еĹյ�
	// ��ڲ����� resolution�������ֱ���
	// ���ڲ����� �յ�λ�ö�Ӧ����������
	// ����ֵ�� ��
	// ���ߣ��½��
	// �������ڣ� 2022-04-20
	// ��Ȩ���У� �㽭��ѧ-���������˼���
	//////////////////////////////////////////
	void findInflectionPts(vector<double>& ts, int resolution = 100);
	//CAANURBSCurve<3> addGeometricConstraints(double t,int multiplities,CAAVector<3> position,CAAVector<3> tangent,CAAVector<3> curvature);

	/*
	ֱ��Ƕ���ά���ߵ���ά�ռ��xyƽ��
	*/
	CAANURBSCurve<3> embed();

	void removeUnfairingPt(double t);
	void UnfRm();

	struct Info {
		double energy[3];
		map<double, int> pts;
		set<double> ipts;
	};

	//////////////////////////////////////////
	// ���������� ����������ʵ��������߹�˳�Ա���Ĺ���
	// ��ڲ����� ��
	// ���ڲ����� Info�����ã��ں������߹�˳�����ԵĻ�����Ϣ
	// ����ֵ�� ��
	// ���ߣ��½��
	// �������ڣ� 2022-04-20
	// ��Ȩ���У� �㽭��ѧ-���������˼���
	//////////////////////////////////////////
	void getReport(Info& res);

#ifdef CAA_ENV
	void getCATNurbsCurve(CATGeoFactory* piGeoFactory, CATNurbsCurve** oResult);
	//////////////////////////////////////////
	// ���������� �����캯����CATIA�ڽ�NURBS����ת��ΪCAANURBSCurve<3>����
	// ��ڲ����� CATNurbsCurve��������
	// ���ڲ����� ��
	// ����ֵ�� ��
	// ���ߣ��½��
	// �������ڣ� 2022-05-18
	// ��Ȩ���У� �㽭��ѧ-���������˼���
	//////////////////////////////////////////
	CAANURBSCurve(const CATNurbsCurve& nurbs);
#endif

	CAANURBSCurve();
	CAANURBSCurve(int p, vector<double> knot_vector, vector<CAAVector<dim>> control_points, bool isRational = false, vector<double> weights = vector<double>);
};

/*
���߹�˳���࣬���������Ż���˳���ߣ�����������������
alpha - һ�׵�ƽ������
beta  - ���׵�ƽ������
gamma - ���׵�ƽ������
delta - �Ż�ǰ������L2����
*/
class EnergyFairing {
public:
	double alpha;
	double beta;
	double gamma;
	double delta;

	EnergyFairing() :
		alpha(0),
		beta(0),
		gamma(0),
		delta(1)
	{};
	//////////////////////////////////////////
	// ���������� �����������ڵ�s�����ظ���㴦��ӵ�ʸԼ��
	// ��ڲ����� curve���������� s����s�����ظ���� derivatives��{�����ף���ʸֵ}��ɵ�Լ�� boundary����β�����Ͷ������ɶ�
	// ���ڲ����� curve����������Լ�����������
	// ����ֵ�� ��
	// ���ߣ��½��
	// �������ڣ� 2022-04-20
	// ��Ȩ���У� �㽭��ѧ-���������˼���
	//////////////////////////////////////////
	void addDerivativeConstraintsOnKnot(CAANURBSCurve<3>& curve, int s, map<int, CAAVector<3>> derivatives, int boundary[2]);

	//////////////////////////////////////////
	// ���������� �����������������t����Ľ�㴦���λ�ã���������Լ��
	// ��ڲ����� curve���������� t�����߲��� position��λ��Լ�� tangent������Լ�� curvature������Լ�� boundary����β�����Ͷ������ɶ�
	// ���ڲ����� curve����������Լ�����������
	// ����ֵ�� ��
	// ���ߣ��½��
	// �������ڣ� 2022-04-20
	// ��Ȩ���У� �㽭��ѧ-���������˼���
	//////////////////////////////////////////
	void addGeometricConstraintsOnKnot(CAANURBSCurve<3>& curve, double t, CAAVector<3> position, CAAVector<3> tangent, CAAVector<3> curvature, int boundary[2]);

	struct Constraint {
	public:
		double t;
		int level = 0;
		CAAVector<3> position;
		CAAVector<3> tangent;
		CAAVector<3> curvature;

		void regularize();
	};

	//////////////////////////////////////////
	// ���������� ����������Լ����Ĳ���λ�ø�������������±꼯�����ܶ��������߽���ϸ�ִ���
	// ��ڲ����� curve���������� ts��������
	// ���ڲ����� curve��ϸ�ֺ����� variables�����������±꼯
	// ����ֵ�� ��
	// ���ߣ��½��
	// �������ڣ� 2022-06-27
	// ��Ȩ���У� �㽭��ѧ-���������˼���
	//////////////////////////////////////////
	void getVariablePts(CAANURBSCurve<3>& curve, vector<Constraint> constraints, vector<bool>& variables);

	//////////////////////////////////////////
	// ���������� �����������ڸ��������ϼ���ָ���Ķ��Լ��
	// ��ڲ����� curve���������� constraints��Լ�� variables����Ϊ�����Ŀ��ƶ����±꼯
	// ���ڲ����� curve����������Լ�����������
	// ����ֵ�� ��
	// ���ߣ��½��
	// �������ڣ� 2022-06-27
	// ��Ȩ���У� �㽭��ѧ-���������˼���
	//////////////////////////////////////////
	void multiGeoConstraint(CAANURBSCurve<3>& curve, vector<Constraint> constraints, vector<bool> variables, double lambda = 1);
};

class CAANURBSWire {
public:
	vector<CAANURBSCurve<3>> curves;

	void reparameterized();
	CAANURBSCurve<3> merge();
};

bool checkGeometricContinuity(CAAVector<3> vecs1[4], CAAVector<3> vecs2[4], int order, double err);

//extern template class CAANURBSCurve<2>;
//extern template class CAANURBSCurve<3>;
