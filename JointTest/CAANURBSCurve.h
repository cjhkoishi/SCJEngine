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
	计算输入抽象定点数组cp的deBoor算法结果，t为参数，k为指定的节点区间
	*/
	template<typename T> T deBoor(double t, int k, vector<T>& cp);

	/*
	计算输入抽象定点数组cp的order阶导数deBoor算法结果，t为参数，k为指定的节点区间
	*/
	template<typename T> T deBoor(double t, int k, int order, vector<T>& cp);

	/*
	计算有理情形下给定抽象组cp的deBoor算法结果，t为参数，k为指定的节点区间
	*/
	template<typename T> T deBoorWithWeights(double t, int k, int order, vector<T>& cp);

	/*
	计算参数t所在的节点区间序号
	*/
	int findSpan(double t);

	/*
	寻找最近的节点
	*/
	int findClosest(double t);

	/*
	获取有效参数返回
	*/
	CAAVector<2> getLimits();

	/*
	翻转参数方向
	*/
	void reverse();

	/*
	返回第k个曲线段对应的节点区间序号
	*/
	int segment(int k);

	/*
	获取指定节点区间k下，参数t处的曲线坐标
	*/
	CAAVector<dim> evalInSpan(double t, int k);

	/*
	获取指定节点区间k下，参数t处的曲线order阶导数
	*/
	CAAVector<dim> evalInSpan(double t, int k, int order);

	/*
	获取参数t处的曲线坐标
	*/
	CAAVector<dim> eval(double t);

	/*
	获取参数t处的曲线order阶导数
	*/
	CAAVector<dim> eval(double t, int order);

	/*
	获取所有非零节点区间的序号
	*/
	void getAllKnotPoints(vector<int>& ts);

	/*
	计算节点区间k的order阶导数左右极限
	*/
	CAAVector<dim> knotEval(int k, bool direction, int order);

	/*
	计算B样条基函数N_i(t)
	*/
	double N(int i, double t);

	/*
	计算B样条基函数导数N_i^(d)(t)
	*/
	double N_d(int i, double t, int d);

	/*
	计算Nurbs有理基函数导数B_i^(d)(t)，包含非有理情形
	*/
	double B_d(int i, double t, int d);

	/*
	计算曲率杆（dim=3）
	*/
	CAAVector<3> curvature(double t);

	/*
	计算平面曲线曲率公式中的分子
	*/
	double kappa(double t, int k);

	/*
	计算能量矩阵中的(i,j)项：int(B_i^(d) B_j^(d),dt)。采用10次多项式高斯积分计算
	*/
	double energyInteral(int i, int j, int d);

	/*
	t参数处插入节点
	*/
	void insert(double t);

	/*
	删除第k个节点
	*/
	bool remove(int k);

	/*
	删除t处的节点一次
	*/
	void erase(double t);

	/*
	提升次数s
	*/
	void increaseDegree(int s);

	/*
	实验函数，用于点插值实验
	*/
	void interpolation(vector<CAAVector<dim>> pts);

	//////////////////////////////////////////
	// 功能描述： 本函数用于对曲线应用能量光顺算法
	// 入口参数： alpha：一节导矢平方积分能量系数 beta：二节导矢平方积分能量系数 gamma：三节导矢平方积分能量系数 delta：控制点距离和系数
	// 出口参数： 无
	// 返回值： 无
	// 作者：陈金浩
	// 创建日期： 2022-04-20
	// 版权所有： 浙江大学-几何与拓扑计算
	//////////////////////////////////////////
	void smoothing(double alpha, double beta, double gamma, double delta = 1);

	//////////////////////////////////////////
	// 功能描述： 本函数用于寻找二维Nurbs曲线上所有的拐点
	// 入口参数： resolution：采样分辨率
	// 出口参数： 拐点位置对应参数的数组
	// 返回值： 无
	// 作者：陈金浩
	// 创建日期： 2022-04-20
	// 版权所有： 浙江大学-几何与拓扑计算
	//////////////////////////////////////////
	void findInflectionPts(vector<double>& ts, int resolution = 100);
	//CAANURBSCurve<3> addGeometricConstraints(double t,int multiplities,CAAVector<3> position,CAAVector<3> tangent,CAAVector<3> curvature);

	/*
	直接嵌入二维曲线到三维空间的xy平面
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
	// 功能描述： 本函数用于实现输出曲线光顺性报告的功能
	// 入口参数： 无
	// 出口参数： Info类引用，内含有曲线光顺性属性的基本信息
	// 返回值： 无
	// 作者：陈金浩
	// 创建日期： 2022-04-20
	// 版权所有： 浙江大学-几何与拓扑计算
	//////////////////////////////////////////
	void getReport(Info& res);

#ifdef CAA_ENV
	void getCATNurbsCurve(CATGeoFactory* piGeoFactory, CATNurbsCurve** oResult);
	//////////////////////////////////////////
	// 功能描述： 本构造函数将CATIA内建NURBS对象转换为CAANURBSCurve<3>对象
	// 入口参数： CATNurbsCurve对象引用
	// 出口参数： 无
	// 返回值： 无
	// 作者：陈金浩
	// 创建日期： 2022-05-18
	// 版权所有： 浙江大学-几何与拓扑计算
	//////////////////////////////////////////
	CAANURBSCurve(const CATNurbsCurve& nurbs);
#endif

	CAANURBSCurve();
	CAANURBSCurve(int p, vector<double> knot_vector, vector<CAAVector<dim>> control_points, bool isRational = false, vector<double> weights = vector<double>);
};

/*
曲线光顺器类，利用能量优化光顺曲线，能量函数超参数：
alpha - 一阶导平方积分
beta  - 二阶导平方积分
gamma - 三阶导平方积分
delta - 优化前后曲线L2距离
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
	// 功能描述： 本函数用于在第s个不重复结点处添加导矢约束
	// 入口参数： curve：输入曲线 s：第s个不重复结点 derivatives：{导数阶：导矢值}组成的约束 boundary：首尾各降低多少自由度
	// 出口参数： curve：经过几何约束后的新曲线
	// 返回值： 无
	// 作者：陈金浩
	// 创建日期： 2022-04-20
	// 版权所有： 浙江大学-几何与拓扑计算
	//////////////////////////////////////////
	void addDerivativeConstraintsOnKnot(CAANURBSCurve<3>& curve, int s, map<int, CAAVector<3>> derivatives, int boundary[2]);

	//////////////////////////////////////////
	// 功能描述： 本函数用于在离参数t最近的结点处添加位置，切向，曲率约束
	// 入口参数： curve：输入曲线 t：曲线参数 position：位置约束 tangent：切向约束 curvature：曲率约束 boundary：首尾各降低多少自由度
	// 出口参数： curve：经过几何约束后的新曲线
	// 返回值： 无
	// 作者：陈金浩
	// 创建日期： 2022-04-20
	// 版权所有： 浙江大学-几何与拓扑计算
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
	// 功能描述： 本函数根据约束点的参数位置给定变量顶点的下标集，可能对输入曲线进行细分处理
	// 入口参数： curve：输入曲线 ts：参数集
	// 出口参数： curve：细分后曲线 variables：变量顶点下标集
	// 返回值： 无
	// 作者：陈金浩
	// 创建日期： 2022-06-27
	// 版权所有： 浙江大学-几何与拓扑计算
	//////////////////////////////////////////
	void getVariablePts(CAANURBSCurve<3>& curve, vector<Constraint> constraints, vector<bool>& variables);

	//////////////////////////////////////////
	// 功能描述： 本函数用于在给定曲线上加入指定的多个约束
	// 入口参数： curve：输入曲线 constraints：约束 variables：作为变量的控制顶点下标集
	// 出口参数： curve：经过几何约束后的新曲线
	// 返回值： 无
	// 作者：陈金浩
	// 创建日期： 2022-06-27
	// 版权所有： 浙江大学-几何与拓扑计算
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
