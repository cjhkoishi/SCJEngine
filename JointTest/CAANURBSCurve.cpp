//////////////////////////////////////////
// 文件名：CAANURBSCurve.cpp
// 功能说明：nurbs曲线类的函数实现
// 作者：陈金浩
// 创建日期： 2022-03-15
// 版本： 0.1.1
// 版权所有: 浙江大学-几何与拓扑计算
//////////////////////////////////////////

#include "CAANURBSCurve.h"
#include <functional>
#include <algorithm>
#include <map>
#include <iostream>

double xs_10i[10] = {
	-0.1488743389816312,
	0.1488743389816312,
	-0.4333953941292472,
	0.4333953941292472,
	-0.6794095682990244,
	0.6794095682990244,
	-0.8650633666889845,
	0.8650633666889845,
	-0.9739065285171717,
	0.9739065285171717 };
double ws_10i[10] = {
	0.2955242247147529,
	0.2955242247147529,
	0.2692667193099963,
	0.2692667193099963,
	0.2190863625159820,
	0.2190863625159820,
	0.1494513491505806,
	0.1494513491505806,
	0.0666713443086881,
	0.0666713443086881 };

template <int dim>
bool checkGeometricContinuity(CAAVector<dim> vecs1[4], CAAVector<dim> vecs2[4], int order, double err);

template <int dim>
template <typename T>
T CAANURBSCurve<dim>::deBoor(double t, int k, vector<T>& cp)
{
	vector<T> P(p + 1);

	for (int i = 0; i < p + 1; i++)
	{
		P[i] = cp[k - p + i];
	}

	for (int r = 1; r < p + 1; r++)
	{
		for (int i = 0; i <= p - r; i++)
		{
			double L = knot_vector[k + i + 1] - knot_vector[k + i + r - p];
			double tau;
			if (L <= 1e-8)
				tau = 0;
			else
				tau = (t - knot_vector[k + i + r - p]) / L;
			P[i] = (1 - tau) * P[i] + tau * P[i + 1];
		}
	}
	return P[0];
}

template <int dim>
template <typename T>
T CAANURBSCurve<dim>::deBoor(double t, int k, int order, vector<T>& cp)
{
	vector<T> P(p + 1);
	//
	for (int i = 0; i < p + 1; i++)
	{
		P[i] = cp[k - p + i];
	}
	// deboor
	for (int r = 1; r < p + 1 - order; r++)
	{
		for (int i = 0; i <= p - r; i++)
		{
			double L = knot_vector[k + i + 1] - knot_vector[k + i + r - p];
			double tau = (t - knot_vector[k + i + r - p]) / L;
			P[i] = (1 - tau) * P[i] + tau * P[i + 1];
		}
	}

	for (int r = p + 1 - order; r < p + 1; r++)
	{
		for (int i = 0; i <= p - r; i++)
		{
			double L = knot_vector[k + i + 1] - knot_vector[k + i + r - p];
			P[i] = r / L * (P[i + 1] - P[i]);
		}
	}

	return P[0];
}

template <int dim>
template <typename T>
T CAANURBSCurve<dim>::deBoorWithWeights(double t, int k, int order, vector<T>& cp)
{
	vector<T> wP = cp;
	for (int i = 0; i < wP.size(); i++)
	{
		wP[i] = weights[i] * cp[i];
	}

	vector<T> affine_curve_d(order + 1);
	vector<T> projected_curve_d(order + 1);
	vector<double> total_weight_d(order + 1);
	vector<int> bin(order + 1, 0);

	affine_curve_d[0] = deBoor<T>(t, k, wP);
	total_weight_d[0] = deBoor<double>(t, k, weights);
	projected_curve_d[0] = affine_curve_d[0] / total_weight_d[0];
	bin[0] = 1;

	for (int i = 1; i <= order; i++)
	{
		for (int j = i; j >= 1; j--)
		{
			bin[j] = bin[j - 1] + bin[j];
		}
		affine_curve_d[i] = deBoor<T>(t, k, i, wP);
		total_weight_d[i] = deBoor<double>(t, k, i, weights);
		T sum = bin[1] * total_weight_d[1] * projected_curve_d[i - 1];
		;
		for (int j = 2; j <= i; j++)
		{
			sum = sum + bin[j] * total_weight_d[j] * projected_curve_d[i - j];
		}
		projected_curve_d[i] = (affine_curve_d[i] - sum) / total_weight_d[0];
	}
	return projected_curve_d[order];
}

template <int dim>
CAANURBSCurve<dim>::CAANURBSCurve(
	int p,
	vector<double> knot_vector,
	vector<CAAVector<dim>> control_points,
	bool isRational, vector<double> weights) : p(p),
	knot_vector(knot_vector),
	control_points(control_points),
	isRational(isRational),
	weights(weights)
{
}

template <int dim>
void CAANURBSCurve<dim>::insert(double t)
{
	int k = findSpan(t);
	vector<CAAVector<dim>> new_points(p);
	if (!isRational)
	{
		for (size_t i = k - p; i < k; i++)
		{
			double L = knot_vector[i + p + 1] - knot_vector[i + 1];
			double tau = (t - knot_vector[i + 1]) / L;
			new_points[i - k + p] = (1 - tau) * control_points[i] + tau * control_points[i + 1];
		}
		control_points.erase(control_points.begin() + (k - p + 1), control_points.begin() + k);
		control_points.insert(control_points.begin() + (k - p + 1), new_points.begin(), new_points.end());
		knot_vector.insert(knot_vector.begin() + (k + 1), t);
	}
	else
	{
		vector<CAAVector<dim>> affine_points(control_points.size());
		vector<double> new_weights(p);
		for (int i = 0; i < affine_points.size(); i++)
		{
			affine_points[i] = control_points[i] * weights[i];
		}
		for (size_t i = k - p; i < k; i++)
		{
			double L = knot_vector[i + p + 1] - knot_vector[i + 1];
			double tau = (t - knot_vector[i + 1]) / L;
			new_weights[i - k + p] = (1 - tau) * weights[i] + tau * weights[i + 1];
			new_points[i - k + p] = ((1 - tau) * affine_points[i] + tau * affine_points[i + 1]) / new_weights[i - k + p];
		}
		control_points.erase(control_points.begin() + (k - p + 1), control_points.begin() + k);
		control_points.insert(control_points.begin() + (k - p + 1), new_points.begin(), new_points.end());
		weights.erase(weights.begin() + (k - p + 1), weights.begin() + k);
		weights.insert(weights.begin() + (k - p + 1), new_weights.begin(), new_weights.end());
		knot_vector.insert(knot_vector.begin() + (k + 1), t);
	}
}

double divide(double f, double g)
{
	return g == 0 ? 0 : (f / g);
};

template <int dim>
bool CAANURBSCurve<dim>::remove(int k)
{

	double t = knot_vector[k];
	int multies = 0;
	for (int i = k; i >= 0; i--)
		if (knot_vector[i] == t)
			multies++;
		else
			break;

	if (k <= p || k >= knot_vector.size() - p - 1)
		return false;
	vector<CAAVector<dim>> new_pts(p + 2 - multies);
	if (!isRational)
	{

		new_pts[0] = control_points[k - p - 1];
		new_pts[p + 1 - multies] = control_points[k + 1 - multies];
		int i = k - p, j = k - 1 - multies;
		while (i <= j)
		{
			double ai = (t - knot_vector[i]) / (knot_vector[i + p + 1] - knot_vector[i]);
			double aj = (t - knot_vector[j + 1]) / (knot_vector[j + p + 2] - knot_vector[j + 1]);
			if (ai != 0.0)
			{
				new_pts[i - k + p + 1] = (control_points[i] - (1 - ai) * new_pts[i - k + p]) / ai;
				i++;
			}
			if (aj != 1.0)
			{
				new_pts[j - k + p + 1] = (control_points[j + 1] - aj * new_pts[j - k + p + 2]) / (1 - aj);
				j--;
			}
		}
		if (new_pts.size() > 1)
		{
			control_points.erase(control_points.begin() + (k - p), control_points.begin() + (k + 1 - multies));
			control_points.insert(control_points.begin() + (k - p), new_pts.begin() + 1, new_pts.end() - 1);
		}
		else
		{
			control_points.erase(control_points.begin() + (k - p));
		}

		knot_vector.erase(knot_vector.begin() + k);
	}
	else
	{
		vector<CAAVector<dim>> new_pts_w(p + 2 - multies);
		vector<double> new_weights(p + 2 - multies);
		vector<CAAVector<dim>> cp_w = control_points;
		for (int i = 0; i < cp_w.size(); i++)
		{
			cp_w[i] = control_points[i] * weights[i];
		}
		new_pts_w[0] = cp_w[k - p - 1];
		new_weights[0] = weights[k - p - 1];
		new_pts_w[p + 1 - multies] = cp_w[k + 1 - multies];
		new_weights[p + 1 - multies] = weights[k + 1 - multies];
		int i = k - p, j = k - 1 - multies;
		while (i <= j)
		{
			double ai = (t - knot_vector[i]) / (knot_vector[i + p + 1] - knot_vector[i]);
			double aj = (t - knot_vector[j + 1]) / (knot_vector[j + p + 2] - knot_vector[j + 1]);
			if (ai != 0.0)
			{
				new_pts_w[i - k + p + 1] = (cp_w[i] - (1 - ai) * new_pts_w[i - k + p]) / ai;
				new_weights[i - k + p + 1] = (weights[i] - (1 - ai) * new_weights[i - k + p]) / ai;
				i++;
			}
			if (aj != 1.0)
			{
				new_pts_w[j - k + p + 1] = (cp_w[j + 1] - aj * new_pts_w[j - k + p + 2]) / (1 - aj);
				new_weights[j - k + p + 1] = (weights[j + 1] - aj * new_weights[j - k + p + 2]) / (1 - aj);
				j--;
			}
		}
		for (int i = 0; i < new_pts.size(); i++)
		{
			new_pts[i] = new_pts_w[i] / new_weights[i];
		}
		if (new_pts.size() > 1)
		{
			control_points.erase(control_points.begin() + (k - p), control_points.begin() + (k + 1 - multies));
			control_points.insert(control_points.begin() + (k - p), new_pts.begin() + 1, new_pts.end() - 1);

			weights.erase(weights.begin() + (k - p), weights.begin() + (k + 1 - multies));
			weights.insert(weights.begin() + (k - p), new_weights.begin() + 1, new_weights.end() - 1);
		}
		else
		{
			control_points.erase(control_points.begin() + (k - p));
			weights.erase(weights.begin() + (k - p));
		}
		knot_vector.erase(knot_vector.begin() + k);
	} /**/

	return true;
}

template <int dim>
void CAANURBSCurve<dim>::erase(double t)
{
	int k = findClosest(t);
	remove(k);
}

template <int dim>
void CAANURBSCurve<dim>::increaseDegree(int s)
{
	map<double, int> multiplies;
	for (int i = 0; i < knot_vector.size(); i++)
	{
		multiplies[knot_vector[i]]++;
	}
	if (multiplies.begin()->second != p + 1 || multiplies.rbegin()->second != p + 1)
		return;
	// splited into beziers
	int m = knot_vector.size();
	for (int i = m - p - 2; i > p; i -= multiplies[knot_vector[i]])
	{
		for (int times = p - multiplies[knot_vector[i]] + 1; times > 0; times--)
		{
			insert(knot_vector[i]);
		}
	}

	// deal with every beziers
	if (!isRational)
	{
		vector<CAAVector<dim>> new_pts(p + s + 1);
		for (int index = 0; index < control_points.size() - 1; index += p + s + 1)
		{
			for (int i = 0; i < p + 1; i++)
			{
				new_pts[i] = control_points[index + i];
			}
			for (int i = p + 1; i < p + s + 1; i++)
			{
				new_pts[i] = control_points[index + p];
			}
			for (int r = 0; r < s; r++)
				for (int i = p + r; i > 0; i--)
				{
					double a = ((double)i) / (p + r + 1);
					new_pts[i] = (1 - a) * new_pts[i] + a * new_pts[i - 1];
				}
			control_points.erase(control_points.begin() + (index + 1), control_points.begin() + (index + p));
			control_points.insert(control_points.begin() + (index + 1), new_pts.begin() + 1, new_pts.end() - 1);
		}
	}
	else
	{
		vector<CAAVector<dim>> new_pts(p + s + 1);
		vector<double> new_weights(p + s + 1);
		for (int index = 0; index < control_points.size() - 1; index += p + s + 1)
		{
			for (int i = 0; i < p + 1; i++)
			{
				new_pts[i] = control_points[index + i] * weights[index + i];
				new_weights[i] = weights[index + i];
			}
			for (int i = p + 1; i < p + s + 1; i++)
			{
				new_pts[i] = new_pts[p];
				new_weights[i] = new_weights[p];
			}

			for (int r = 0; r < s; r++)
				for (int i = p + r; i > 0; i--)
				{
					double a = ((double)i) / (p + r + 1);
					new_pts[i] = (1 - a) * new_pts[i] + a * new_pts[i - 1];
					new_weights[i] = (1 - a) * new_weights[i] + a * new_weights[i - 1];
				}
			for (int i = 0; i < p + s + 1; i++)
				new_pts[i] = new_pts[i] / new_weights[i];
			control_points.erase(control_points.begin() + (index + 1), control_points.begin() + (index + p));
			control_points.insert(control_points.begin() + (index + 1), new_pts.begin() + 1, new_pts.end() - 1);
			weights.erase(weights.begin() + (index + 1), weights.begin() + (index + p));
			weights.insert(weights.begin() + (index + 1), new_weights.begin() + 1, new_weights.end() - 1);
		}
	}

	// restore the multiplies
	p += s;
	for (int index = 0; index < knot_vector.size() - 1; index += p + 1)
	{
		knot_vector.insert(knot_vector.begin() + index, s, knot_vector[index]);
	}
	for (int index = knot_vector.size() - p - 1; index > 0; index -= p + 1)
	{
		for (int times = p + 1 - multiplies[knot_vector[index]] - s; times > 0; times--)
		{
			remove(index);
		}
	}
}

template <int dim>
double CAANURBSCurve<dim>::N(int i, double t)
{
	int k = findSpan(t);
	vector<double> one(control_points.size(), 0);
	one[i] = 1;
	return deBoor<double>(t, k, one);
}

template <int dim>
double CAANURBSCurve<dim>::N_d(int i, double t, int d)
{
	int k = findSpan(t);
	vector<double> one(control_points.size(), 0);
	one[i] = 1;
	return deBoor<double>(t, k, d, one);
}

template <int dim>
double CAANURBSCurve<dim>::energyInteral(int i, int j, int d)
{
	if (i > j)
		return energyInteral(j, i, d);
	if (j - i > p)
		return 0.0;
	double res = 0;
	for (int k = j; k < i + p + 1; k++)
	{
		double span_res = 0;
		double a = (knot_vector[k] + knot_vector[k + 1]) * 0.5;
		double b = (knot_vector[k + 1] - knot_vector[k]) * 0.5;
		for (int m = 0; m < 10; m++)
		{
			double t = a + xs_10i[m] * b;
			span_res += ws_10i[m] * B_d(i, t, d) * B_d(j, t, d);
		}

		res += b * span_res;
	}
	return res;
}

template <int dim>
void CAANURBSCurve<dim>::interpolation(vector<CAAVector<dim>> pts)
{
	knot_vector.clear();
	control_points.clear();
	p = 5;
	isRational = false;
	int n_pt = pts.size();
	vector<double> Ls(n_pt);
	Ls[0] = 0;
	for (size_t i = 0; i < Ls.size() - 1; i++)
	{
		Ls[i + 1] = Ls[i] + (pts[i + 1] - pts[i]).norm();
	}
	double t_end = *(Ls.end() - 1);
	// knot_vector = vector<double>{ 0,0,0,0,0,0,t_end,t_end,t_end,t_end,t_end,t_end };
	knot_vector.resize(12);
	for (int i = 0; i < 12; i++)
	{
		if (i < 6)
			knot_vector[i] = 0;
		else
			knot_vector[i] = t_end;
	}
	for (int i = Ls.size() - 2; i > 0; i--)
	{
		knot_vector.insert(knot_vector.begin() + p + 1, Ls[i]);
		knot_vector.insert(knot_vector.begin() + p + 1, Ls[i]);
	}
	control_points.resize(knot_vector.size() - 1 - p);
	int n = control_points.size();

	int M = n_pt + n;
	CAADynamicMatrix Lag(M, M);
	CAADynamicMatrix X(M, 1), Y(M, 1), Z(M, 1);

	for (int i = 0; i < n_pt; i++)
	{
		X[i + n] = pts[i][0];
		Y[i + n] = pts[i][1];
		Z[i + n] = pts[i][2];
	}

	for (int i = 0; i < n; i++)
	{
		for (int j = i; j < n; j++)
			Lag(i, j) = Lag(j, i) = 2 * energyInteral(i, j, 3);
		for (int j = n; j < n + n_pt; j++)
			Lag(i, j) = Lag(j, i) = B_d(i, Ls[j - n], 0);
	}

	CAADynamicMatrix SX = Lag.LUPSolve(X);
	CAADynamicMatrix SY = Lag.LUPSolve(Y);
	CAADynamicMatrix SZ = Lag.LUPSolve(Z);
	for (int i = 0; i < n; i++)
	{
		control_points[i] = CAAVector<dim>(SX[i], SY[i], SZ[i]);
	}
}

template <int dim>
CAANURBSCurve<dim>::CAANURBSCurve() : p(3),
isRational(false)
{
	knot_vector.resize(8);
	knot_vector[0] = 0;
	knot_vector[1] = 0;
	knot_vector[2] = 0;
	knot_vector[3] = 0;
	knot_vector[4] = 1;
	knot_vector[5] = 1;
	knot_vector[6] = 1;
	knot_vector[7] = 1;
	control_points.push_back(CAAVector<dim>());
	control_points.push_back(CAAVector<dim>());
	control_points.push_back(CAAVector<dim>());
	control_points.push_back(CAAVector<dim>());
}

template <int dim>
void CAANURBSCurve<dim>::smoothing(double alpha, double beta, double gamma, double delta)
{
	int n = control_points.size();
	CAADynamicMatrix E_2(n, n), B(n, n);
	CAADynamicMatrix X(n, 1), Y(n, 1), Z(n, 1);

	for (int i = 0; i < n; i++)
	{
		CAAVector<dim> Qi = eval(knot_vector[i + p]);
		X[i] = Qi[0];
		Y[i] = Qi[1];
		Z[i] = Qi[2];
	}
	for (int i = 0; i < n; i++)
		for (int j = i; j < n; j++)
			E_2(i, j) = E_2(j, i) = 2 * ((alpha == 0 ? 0 : (alpha * energyInteral(i, j, 1))) +
				(beta == 0 ? 0 : (beta * energyInteral(i, j, 2))) +
				(gamma == 0 ? 0 : (gamma * energyInteral(i, j, 3))));

	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
		{
			B(i, j) = B_d(j, knot_vector[i + p], 0);
		}

	CAADynamicMatrix BT = B.transpose();
	CAADynamicMatrix BTB = BT * B;
	CAADynamicMatrix E = E_2 + BTB;
	X = BT * X;
	Y = BT * Y;
	Z = BT * Z;
	CAADynamicMatrix SX = E.LUPSolve(X);
	CAADynamicMatrix SY = E.LUPSolve(Y);
	CAADynamicMatrix SZ = E.LUPSolve(Z);

	for (int i = 0; i < n; i++)
	{
		control_points[i] = CAAVector<dim>(SX[i], SY[i], SZ[i]);
	}
}

template <>
void CAANURBSCurve<2>::findInflectionPts(vector<double>& ts, int resolution)
{
	int total_resulotion = (knot_vector.size() - 2 * p) * resolution;
	vector<double> samples(total_resulotion + 1);
	vector<double> kappas(total_resulotion + 1);
	vector<int> signs(total_resulotion + 1);
	CAAVector<2> limits = getLimits();
	double a = limits[0];
	double b = limits[1];
	for (int i = 0; i <= total_resulotion; i++)
	{
		samples[i] = a + (b - a) * i / total_resulotion;
		int k = findSpan(samples[i]);
		kappas[i] = kappa(samples[i], k);
		signs[i] = kappas[i] > 1e-8 ? 1 : (kappas[i] < -1e-8 ? -1 : 0);
	}
	for (int i = 0; i < total_resulotion; i++)
	{
		// First, use Newton method, if failed, use midpoint method
		int k = findSpan(samples[i]);
		if (signs[i] * signs[i + 1] < 0)
		{
			double t = 0;
			double t_ = (samples[i] + samples[i + 1]) * 0.5;
			int iters = 0;
			do
			{
				t = t_;
				CAAVector<2> C_d = evalInSpan(t, k, 1);
				CAAVector<2> C_dd = evalInSpan(t, k, 2);
				CAAVector<2> C_ddd = evalInSpan(t, k, 3);
				t_ = t - (C_d[0] * C_dd[1] - C_d[1] * C_dd[0]) / (C_d[0] * C_ddd[1] - C_d[1] * C_ddd[0]);
				t_ = min(max(t_, samples[i]), samples[i + 1]);
				iters++;
			} while (abs(t - t_) > 1e-13 && iters < 100);
			// midpoint
			if (abs(kappa(t_, k)) > 1e-8)
			{
				t_ = (samples[i] + samples[i + 1]) * 0.5;
				double t_0 = samples[i];
				double t_1 = samples[i + 1];
				iters = 0;
				while (abs(kappa(t_, k)) > 1e-8 && iters < 100)
				{
					bool s_0 = kappa(t_0, k) > 0;
					bool s_1 = kappa(t_1, k) > 0;
					bool s_ = kappa(t_, k) > 0;
					if (s_0 ^ s_1)
					{
						if (s_ ^ s_0)
						{
							t_1 = t_;
							t_ = (t_0 + t_) * 0.5;
						}
						else
						{
							t_0 = t_;
							t_ = (t_1 + t_) * 0.5;
						}
					}
					else
					{
						break;
					}
					iters++;
				}
			}
			ts.push_back(t_);
		}
		else if (signs[i] != 0 && signs[i + 1] == 0)
		{
			int j = i + 2;
			while (j < total_resulotion)
			{
				if (signs[j] != 0)
					break;
				j++;
			}
			if (j == total_resulotion || !(signs[i] ^ signs[j]))
				continue;

			double t_ = 0;
			int iters = 0;

			//[i,i+1] midpoint
			t_ = (samples[i] + samples[i + 1]) * 0.5;
			double t_0 = samples[i];
			double t_1 = samples[i + 1];
			iters = 0;
			while (abs(kappa(t_, k)) > 1e-8 && iters < 100)
			{
				bool s_0 = kappa(t_0, k) > 0;
				bool s_1 = kappa(t_1, k) > 0;
				bool s_ = kappa(t_, k) > 0;
				if (s_0 ^ s_1)
				{
					if (s_ ^ s_0)
					{
						t_0 = t_;
						t_ = (t_1 + t_) * 0.5;
					}
					else
					{
						t_1 = t_;
						t_ = (t_0 + t_) * 0.5;
					}
				}
				else
				{
					break;
				}
				iters++;
			}
			ts.push_back(t_);

			//[j-1,j] midpoint
			t_ = (samples[j] + samples[j - 1]) * 0.5;
			t_0 = samples[j - 1];
			t_1 = samples[j];
			iters = 0;
			while (abs(kappa(t_, k)) > 1e-8 && iters < 100)
			{
				bool s_0 = kappa(t_0, k) > 0;
				bool s_1 = kappa(t_1, k) > 0;
				bool s_ = kappa(t_, k) > 0;
				if (s_0 ^ s_1)
				{
					if (s_ ^ s_0)
					{
						t_0 = t_;
						t_ = (t_1 + t_) * 0.5;
					}
					else
					{
						t_1 = s_;
						t_ = (t_0 + t_) * 0.5;
					}
				}
				else
				{
					break;
				}
				iters++;
			}
			ts.push_back(t_);

			i = j;
		}
	}
}

template <>
void CAANURBSCurve<3>::findInflectionPts(vector<double>& ts, int resolution)
{
	ts.clear();
}

void EnergyFairing::addDerivativeConstraintsOnKnot(CAANURBSCurve<3>& curve, int s, map<int, CAAVector<3>> derivatives, int boundary[2])
{
	assert(boundary[0] >= 0 && boundary[1] >= 0 && boundary[0] + boundary[1] < curve.p);

	int k = curve.segment(s);
	double t = curve.knot_vector[k];
	int n = curve.control_points.size();
	int multis = 0;
	for (int i = k; i >= 0; i--)
		if (curve.knot_vector[i] == t)
			multis++;
		else
			break;

	int p_begin = k - curve.p - multis + 1 + boundary[0];
	int p_end = k - boundary[1];
	assert(p_end <= curve.control_points.size() && p_begin >= 0);
	assert(p_end - p_begin >= derivatives.size());

	vector<int> var_idx, cst_idx;
	for (int i = 0; i < n; i++)
	{
		if (i < p_end && i >= p_begin)
			var_idx.push_back(i);
		else
			cst_idx.push_back(i);
	}
	int vn = var_idx.size();
	int cn = cst_idx.size();
	int ds = derivatives.size();
	int M = vn + ds;

	CAADynamicMatrix Lag(M, M);
	CAADynamicMatrix X(M, 1), Y(M, 1), Z(M, 1);
	// calculate the constant term in derivation
	CAADynamicMatrix Bij(var_idx.size(), cst_idx.size());
	CAADynamicMatrix Xc(cst_idx.size(), 1), Yc(cst_idx.size(), 1), Zc(cst_idx.size(), 1);
	CAADynamicMatrix Bin(var_idx.size(), n);
	CAADynamicMatrix Xq(n, 1), Yq(n, 1), Zq(n, 1);

	for (int i = 0; i < vn; i++)
		for (int j = 0; j < cn; j++)
			Bij(i, j) = (alpha == 0 ? 0 : alpha * curve.energyInteral(var_idx[i], cst_idx[j], 1)) +
			(beta == 0 ? 0 : beta * curve.energyInteral(var_idx[i], cst_idx[j], 2)) +
			(gamma == 0 ? 0 : gamma * curve.energyInteral(var_idx[i], cst_idx[j], 3)) +
			(delta == 0 ? 0 : delta * curve.energyInteral(var_idx[i], cst_idx[j], 0));

	for (int j = 0; j < cn; j++)
	{
		CAAVector<3>& Pc = curve.control_points[cst_idx[j]];
		Xc[j] = Pc[0];
		Yc[j] = Pc[1];
		Zc[j] = Pc[2];
	}
	for (int i = 0; i < vn; i++)
		for (int j = 0; j < n; j++)
			Bin(i, j) = delta == 0 ? 0 : delta * curve.energyInteral(var_idx[i], j, 0);

	for (int j = 0; j < n; j++)
	{
		CAAVector<3> Pc = curve.control_points[j];
		Xq[j] = Pc[0];
		Yq[j] = Pc[1];
		Zq[j] = Pc[2];
	}

	CAADynamicMatrix QX = Bij * Xc - Bin * Xq;
	CAADynamicMatrix QY = Bij * Yc - Bin * Yq;
	CAADynamicMatrix QZ = Bij * Zc - Bin * Zq;

	// set the values of Lag matrix
	for (int i = 0; i < vn; i++)
		for (int j = i; j < vn; j++)
			Lag(i, j) = Lag(j, i) =
			(alpha == 0 ? 0 : alpha * curve.energyInteral(var_idx[i], var_idx[j], 1)) +
			(beta == 0 ? 0 : beta * curve.energyInteral(var_idx[i], var_idx[j], 2)) +
			(gamma == 0 ? 0 : gamma * curve.energyInteral(var_idx[i], var_idx[j], 3)) +
			(delta == 0 ? 0 : delta * curve.energyInteral(var_idx[i], var_idx[j], 0));

	for (int j = 0; j < vn; j++)
	{
		int index = 0;
		for (map<int, CAAVector<3>>::iterator it = derivatives.begin(); it != derivatives.end(); it++, index++)
			Lag(vn + index, j) = Lag(j, vn + index) = curve.B_d(var_idx[j], t, it->first);
	}

	// set the values of b
	for (int j = 0; j < vn; j++)
	{
		X[j] = -QX[j];
		Y[j] = -QY[j];
		Z[j] = -QZ[j];
	}
	int index = 0;
	for (map<int, CAAVector<3>>::iterator it = derivatives.begin(); it != derivatives.end(); it++, index++)
	{
		CAAVector<3> constraint = it->second;
		X[vn + index] = constraint[0];
		Y[vn + index] = constraint[1];
		Z[vn + index] = constraint[2];
		for (int j = 0; j < cn; j++)
		{
			CAAVector<3> cst_P = curve.B_d(cst_idx[j], t, it->first) * curve.control_points[cst_idx[j]];
			X[vn + index] -= cst_P[0];
			Y[vn + index] -= cst_P[1];
			Z[vn + index] -= cst_P[2];
		}
	}

	// solve linear system
	CAADynamicMatrix SX = Lag.LUPSolve(X);
	CAADynamicMatrix SY = Lag.LUPSolve(Y);
	CAADynamicMatrix SZ = Lag.LUPSolve(Z);

	// give the result
	for (int i = 0; i < vn; i++)
	{
		CAAVector<3>& P = curve.control_points[var_idx[i]];
		P[0] = SX[i];
		P[1] = SY[i];
		P[2] = SZ[i];
	}
}

template <>
CAANURBSCurve<3> CAANURBSCurve<2>::embed()
{
	CAANURBSCurve<3> embedded;
	embedded.p = p;
	embedded.knot_vector = knot_vector;
	embedded.isRational = isRational;
	embedded.weights = weights;

	embedded.control_points.resize(control_points.size());
	for (int i = 0; i < control_points.size(); i++)
	{
		embedded.control_points[i] = CAAVector<3>(control_points[i][0], control_points[i][1], 0);
	}
	return embedded;
}

template <int dim>
void CAANURBSCurve<dim>::removeUnfairingPt(double t)
{
	if (p < 5)
	{
		increaseDegree(5 - p);
	}

	int k = findClosest(t);
	if (k <= p || k >= knot_vector.size() - p - 1)
		return;

	double s = knot_vector[k + 1];

	int index = k - 1;
	int multies = 1;
	double m = knot_vector[index];
	while (t == m && index >= 0)
	{
		index--;
		multies++;
		m = knot_vector[index];
	}
	s = s * 0.1 + t * 0.9;
	m = m * 0.1 + t * 0.9;
	insert(s);
	insert(s);
	insert(m);
	insert(m);
	if (multies == 7)
	{
		double ss = t + 2 * (s - t);
		double mm = t + 2 * (m - t);
		insert(ss);
		insert(ss);
		insert(mm);
		insert(mm);
		for (int i = 0; i < multies - 3; i++)
			erase(t);
	}
	else
	{
		for (int i = 0; i < multies - 2; i++)
			erase(t);
	}
}

template <int dim>
void CAANURBSCurve<dim>::UnfRm()
{
	vector<int> ts;
	getAllKnotPoints(ts);
	map<double, int> pts;
	for (int i = 1; i < ts.size() - 1; i++)
	{
		CAAVector<dim> d[2][4];
		d[0][0] = knotEval(i - 1, true, 0);
		d[0][1] = knotEval(i - 1, true, 1);
		d[0][2] = knotEval(i - 1, true, 2);
		d[0][3] = knotEval(i - 1, true, 3);

		d[1][0] = knotEval(i, false, 0);
		d[1][1] = knotEval(i, false, 1);
		d[1][2] = knotEval(i, false, 2);
		d[1][3] = knotEval(i, false, 3);

		for (int j = 0; j < 4; j++)
		{
			if (!checkGeometricContinuity<dim>(d[0], d[1], j, 1e-6))
			{
				pts[knot_vector[ts[i]]] = j;
				break;
			}
		}
	}

	for (map<double, int>::iterator it = pts.begin(); it != pts.end(); it++)
	{
		double t = it->first;
		removeUnfairingPt(t);
	}
}

template <int dim>
bool CAANURBSCurve<dim>::isValid()
{
	if (p <= 0)
		return false;
	int m = knot_vector.size();
	int n = control_points.size();
	if (isRational)
	{
		if (weights.size() != n)
			return false;
		for (int i = 0; i < n; i++)
			if (weights[i] <= 0)
				return false;
	}
	if (m - n - 1 != p)
		return false;
	for (int i = 0; i < m - 1; i++)
		if (knot_vector[i] > knot_vector[i + 1])
			return false;
	for (int i = 1; i <= p; i++)
		if (knot_vector[i] != knot_vector[0] || knot_vector[m - i - 1] != knot_vector[m - 1])
			return false;
	return true;
}

template <int dim>
int CAANURBSCurve<dim>::findSpan(double t)
{
	int _end = knot_vector.size() - p - 2;
	if (t < knot_vector[p])
		return p;
	else if (t >= knot_vector[_end])
		return _end;
	for (size_t i = p; i < knot_vector.size() - p - 1; i++)
	{
		if (knot_vector[i] <= t && knot_vector[i + 1] > t)
			return i;
	}
	return 0;
}

template <int dim>
int CAANURBSCurve<dim>::findClosest(double t)
{
	double min;
	int min_index = 0;
	for (int i = 0; i < knot_vector.size(); i++)
	{
		if (i == 0)
			min = abs(knot_vector[0] - t);
		double cur_t = abs(knot_vector[i] - t);
		if (min >= cur_t)
		{
			min = cur_t;
			min_index = i;
		}
	}
	return min_index;
}

template <int dim>
CAAVector<2> CAANURBSCurve<dim>::getLimits()
{
	CAAVector<2> limits;
	limits[0] = knot_vector[p];
	limits[1] = knot_vector[knot_vector.size() - p - 1];
	return limits;
}

template <int dim>
void CAANURBSCurve<dim>::reverse()
{
	for (int i = 0; i < knot_vector.size(); i++)
	{
		knot_vector[i] *= -1;
	}
	for (int i = 0; i < knot_vector.size() / 2; i++)
	{
		swap(knot_vector[i], knot_vector[knot_vector.size() - i - 1]);
	}
	for (int i = 0; i < control_points.size() / 2; i++)
	{
		swap(control_points[i], control_points[control_points.size() - i - 1]);
		if (isRational)
			swap(weights[i], weights[control_points.size() - i - 1]);
	}
}

template <int dim>
int CAANURBSCurve<dim>::segment(int k)
{
	for (int i = p; i < knot_vector.size() - p - 1; i++)
	{
		if (knot_vector[i] != knot_vector[i + 1])
		{
			k--;
			if (k < 0)
				return i;
		}
	}
	return knot_vector.size() - p - 1;
}

template <int dim>
CAAVector<dim> CAANURBSCurve<dim>::eval(double t)
{
	int k = findSpan(t);
	return evalInSpan(t, k);
}

template <int dim>
CAAVector<dim> CAANURBSCurve<dim>::evalInSpan(double t, int k, int order)
{
	if (order <= 0)
		return evalInSpan(t, k);
	if (!isRational)
		return deBoor<CAAVector<dim>>(t, k, order, control_points);
	else
		return deBoorWithWeights(t, k, order, control_points);
}

template <int dim>
CAAVector<dim> CAANURBSCurve<dim>::knotEval(int k, bool direction, int order)
{
	CAAVector<dim> res;
	if (order > p)
		return res;
	vector<int> ts;
	getAllKnotPoints(ts);
	res = deBoor(knot_vector[ts[k] + (direction ? 1 : 0)], ts[k], order, control_points);
	return res;
}

template <int dim>
double CAANURBSCurve<dim>::B_d(int i, double t, int d)
{
	if (d > p)
		return 0;

	if (!isRational)
	{
		return N_d(i, t, d);
	}
	else
	{
		int k = findSpan(t);
		vector<double> one(control_points.size(), 0);
		one[i] = 1;
		return deBoorWithWeights(t, k, d, one);
	}
}

template <int dim>
CAAVector<dim> CAANURBSCurve<dim>::eval(double t, int order)
{
	int k = findSpan(t);
	return evalInSpan(t, k, order);
}

template <int dim>
CAAVector<dim> CAANURBSCurve<dim>::evalInSpan(double t, int k)
{
	if (!isRational)
	{
		return deBoor<CAAVector<dim>>(t, k, control_points);
	}
	else
	{
		vector<CAAVector<dim>> wP = control_points;
		for (int i = 0; i < wP.size(); i++)
		{
			wP[i] = weights[i] * control_points[i];
		}
		CAAVector<dim> affine_coord = deBoor<CAAVector<dim>>(t, k, wP);
		double total_weight = deBoor<double>(t, k, weights);
		return affine_coord / total_weight;
	}
}

template <int dim>
void CAANURBSCurve<dim>::getAllKnotPoints(vector<int>& ts)
{
	double current = knot_vector[0];
	ts.resize(1);
	ts[0] = 0;
	int r = 0;
	for (int i = 1; i < knot_vector.size(); i++)
	{
		if (knot_vector[i] != current)
		{
			current = knot_vector[i];
			ts.push_back(i);
			r++;
		}
		else
		{
			ts[r] = i;
		}
	}
}

template <>
CAAVector<3> CAANURBSCurve<2>::curvature(double t)
{
	return CAAVector<3>();
}

template <>
double CAANURBSCurve<2>::kappa(double t, int k)
{
	k = findSpan(t);
	CAAVector<2> C_d = evalInSpan(t, k, 1);
	CAAVector<2> C_dd = evalInSpan(t, k, 2);

	return C_d[0] * C_dd[1] - C_d[1] * C_dd[0];
}

template <>
CAAVector<3> CAANURBSCurve<3>::curvature(double t)
{
	int k = findSpan(t);
	CAAVector<3> C_d = eval(t, 1);
	CAAVector<3> C_dd = eval(t, 2);
	CAAVector<3> kappa = C_d.cross(C_dd) / pow(C_d.norm(), 3);

	return kappa.cross(C_d / C_d.norm());
}

CAADynamicMatrix CAADynamicMatrix::LUPSolve(CAADynamicMatrix b)
{
	if (m != n || b.m != m || b.n != 1)
		return CAADynamicMatrix();
	int i = 0, j = 0, k = 0;
	CAADynamicMatrix L(m, m), U(m, m), A(*this);
	CAADynamicMatrix X(m, 1);
	for (i = 0; i < m; i++)
	{
		for (j = 0; j < m; j++)
		{
			if (j < i)
				L(j, i) = 0;
			else
			{
				L(j, i) = A(j, i);
				for (k = 0; k < i; k++)
				{
					L(j, i) = L(j, i) - L(j, k) * U(k, i);
				}
			}
		}

		double max_pivot = 0;
		int max_index = i;
		for (int p = i + 1; p < m; p++)
		{
			if (abs(L(p, i)) > max_pivot)
			{
				max_pivot = L(p, i);
				max_index = p;
			}
		}
		if (/*true*/ abs(L(i, i) / max_pivot) < 0.1)
		{

			for (int p = 0; p < m; p++)
			{
				swap(L(i, p), L(max_index, p));
				swap(A(i, p), A(max_index, p));
			}
			swap(b[i], b[max_index]);
		}
		for (j = 0; j < m; j++)
		{
			if (j < i)
				U(i, j) = 0;
			else if (j == i)
				U(i, j) = 1;
			else
			{
				U(i, j) = A(i, j) / L(i, i);
				for (k = 0; k < i; k++)
				{
					U(i, j) = U(i, j) - ((L(i, k) * U(k, j)) / L(i, i));
				}
			}
		}
	}
	// solve Lx=b
	for (i = 0; i < m; i++)
	{
		double R = 0;
		for (int p = 0; p < i; p++)
			R = R + L(i, p) * X[p];
		X[i] = (b[i] - R) / L(i, i);
	}
	b = X;
	// solve Ux=b
	for (i = m - 1; i >= 0; i--)
	{
		double R = 0;
		for (int p = m - 1; p > i; p--)
			R = R + U(i, p) * X[p];
		X[i] = b[i] - R;
	}
	return X;
}

template <int dim>
bool checkGeometricContinuity(CAAVector<dim> vecs1[4], CAAVector<dim> vecs2[4], int order, double thershold)
{
	double err = 0;
	CAAVector<dim> t1, t2;
	CAAVector<3> k1, k2;
	double b1, b2, b3;

	switch (order)
	{
	case 0:
		err = (vecs1[0] - vecs2[0]).norm();
		break;
	case 1:
		t1 = vecs1[1] / vecs1[1].norm();
		t2 = vecs2[1] / vecs2[1].norm();
		err = acos(t1.dot(t2));
		break;
	case 2:
		k1 = (vecs1[1].cross(vecs1[2]).cross(vecs1[1])) / pow(vecs1[1].norm(), 4);
		k2 = (vecs2[1].cross(vecs2[2]).cross(vecs2[1])) / pow(vecs2[1].norm(), 4);
		err = (k1 - k2).norm();
		break;
	case 3:
		b1 = vecs2[1].norm() / vecs1[1].norm();
		b2 = (vecs2[2] - b1 * b1 * vecs1[2]).norm() / vecs1[1].norm();
		b3 = (vecs2[3] - 3 * b1 * b2 * vecs1[2] - b1 * b1 * b1 * vecs1[3]).norm() / vecs1[1].norm();
		err = (vecs2[3] - b1 * b1 * b1 * vecs1[3] - 3 * b1 * b2 * vecs1[2] - b3 * vecs1[1]).norm();
		break;
	default:
		break;
	}
	if (err > thershold)
		return false;
	else
		return true;
}

void CAANURBSWire::reparameterized()
{
	for (int i = 1; i < curves.size(); i++)
	{
		double lift = *(curves[i - 1].knot_vector.end() - 1);
		int K = curves[i].knot_vector.size();
		double base = curves[i].knot_vector[0];
		for (int k = 0; k < K; k++)
		{
			curves[i].knot_vector[k] = lift + (curves[i].knot_vector[k] - base);
		}
	}
}

CAANURBSCurve<3> CAANURBSWire::merge()
{
	reparameterized();
	CAANURBSCurve<3> res;
	res.knot_vector.clear();
	res.control_points.clear();

	vector<CAANURBSCurve<3>> buffer = curves;
	int max_p = 0;
	bool is_r = false;
	for (int i = 0; i < curves.size(); i++)
	{
		max_p = max_p > curves[i].p ? max_p : curves[i].p;
		is_r |= curves[i].isRational;
	}
	for (int i = 0; i < buffer.size(); i++)
	{
		buffer[i].increaseDegree(max_p - curves[i].p);
		if (i != 0)
			res.knot_vector.pop_back();
		res.knot_vector.insert(res.knot_vector.end(), buffer[i].knot_vector.begin() + (i == 0 ? 0 : max_p + 1), buffer[i].knot_vector.end());
		res.control_points.insert(res.control_points.end(), buffer[i].control_points.begin() + (i == 0 ? 0 : 1), buffer[i].control_points.end());
		if (is_r)
		{
			int n = buffer[i].control_points.size();
			int m = buffer[i].weights.size();
			vector<double> seg_w = buffer[i].weights;
			if (!buffer[i].isRational)
				buffer[i].weights.clear();
			if (m < n)
			{

				seg_w.insert(seg_w.end(), n - m, 1);
			}
			else if (n > m)
			{
				seg_w.assign(seg_w.begin(), seg_w.begin() + n);
			}
			res.weights.insert(res.weights.end(), seg_w.begin() + (i == 0 ? 0 : 1), seg_w.end());
		}
	}
	res.p = max_p;
	res.isRational = is_r;
	return res;
}

void EnergyFairing::addGeometricConstraintsOnKnot(CAANURBSCurve<3>& curve, double t, CAAVector<3> position, CAAVector<3> tangent, CAAVector<3> curvature, int boundary[2])
{
	/*

	*/
	assert(boundary[0] >= 0 && boundary[1] >= 0 && boundary[0] + boundary[1] < curve.p);

	curvature = curvature - (tangent.dot(curvature)) / (tangent.dot(tangent)) * tangent;

	cout << curvature[0] << " " << curvature[1] << " " << curvature[2] << endl
		<< endl;

	int k = curve.findClosest(t);
	t = curve.knot_vector[k];
	int n = curve.control_points.size();
	int multis = 0;
	for (int i = k; i >= 0; i--)
		if (curve.knot_vector[i] == t)
			multis++;
		else
			break;

	vector<int> var_idx, cst_idx;
	for (int i = 0; i < n; i++)
	{
		if (i < k - boundary[1] && i >= k - curve.p - multis + 1 + boundary[0])
			var_idx.push_back(i);
		else
			cst_idx.push_back(i);
	}
	int vn = var_idx.size();
	int cn = cst_idx.size();
	int M = 3 * vn + 7; // varible:3*vn|position:3|tangent:2|curvature:2

	//设置坐标映射使Z坐标非零，避免拉格朗日矩阵奇异
	int coord_map[3] = { 0, 1, 2 };
	double len = tangent.norm();
	int nonzero_index = abs(tangent[2] / len) > 0.1 ? 2 : (abs(tangent[1] / len) > 0.1 ? 1 : 0);
	swap(coord_map[nonzero_index], coord_map[2]);

	CAAVector<3> current_C_d = tangent;
	for (int iter = 0; iter < 3; iter++)
	{
		double norm4_C_d = pow(current_C_d.norm(), 4);
		CAADynamicMatrix Lag(M, M);
		CAADynamicMatrix b(M, 1);
		// calculate the constant term in derivation
		CAADynamicMatrix Bij(vn, cn);
		CAADynamicMatrix Xc(cn, 1), Yc(cn, 1), Zc(cn, 1);
		CAADynamicMatrix Bin(vn, n);
		CAADynamicMatrix Xq(n, 1), Yq(n, 1), Zq(n, 1);

		for (int i = 0; i < vn; i++)
			for (int j = 0; j < cn; j++)
				Bij(i, j) =
				(alpha == 0 ? 0 : alpha * curve.energyInteral(var_idx[i], cst_idx[j], 1)) +
				(beta == 0 ? 0 : beta * curve.energyInteral(var_idx[i], cst_idx[j], 2)) +
				(gamma == 0 ? 0 : gamma * curve.energyInteral(var_idx[i], cst_idx[j], 3)) +
				(delta == 0 ? 0 : delta * curve.energyInteral(var_idx[i], cst_idx[j], 0));

		for (int j = 0; j < cn; j++)
		{
			CAAVector<3>& Pc = curve.control_points[cst_idx[j]];
			Xc[j] = Pc[0];
			Yc[j] = Pc[1];
			Zc[j] = Pc[2];
		}
		for (int i = 0; i < vn; i++)
			for (int j = 0; j < n; j++)
				Bin(i, j) = delta == 0 ? 0 : delta * curve.energyInteral(var_idx[i], j, 0);

		for (int j = 0; j < n; j++)
		{
			CAAVector<3> Pc = curve.control_points[j];
			Xq[j] = Pc[0];
			Yq[j] = Pc[1];
			Zq[j] = Pc[2];
		}

		CAADynamicMatrix QX = Bij * Xc - Bin * Xq;
		CAADynamicMatrix QY = Bij * Yc - Bin * Yq;
		CAADynamicMatrix QZ = Bij * Zc - Bin * Zq;

		// set the values of Lag matrix
		for (int i = 0; i < vn; i++)
			for (int j = i; j < vn; j++)
			{
				int I = 3 * i, J = 3 * j;
				Lag(I, J) = Lag(J, I) =
					Lag(I + 1, J + 1) = Lag(J + 1, I + 1) =
					Lag(I + 2, J + 2) = Lag(J + 2, I + 2) =
					(alpha == 0 ? 0 : alpha * curve.energyInteral(var_idx[i], var_idx[j], 1)) +
					(beta == 0 ? 0 : beta * curve.energyInteral(var_idx[i], var_idx[j], 2)) +
					(gamma == 0 ? 0 : gamma * curve.energyInteral(var_idx[i], var_idx[j], 3)) +
					(delta == 0 ? 0 : delta * curve.energyInteral(var_idx[i], var_idx[j], 0));
			}
		// set position constraint in Lag
		for (int j = 0, I = 3 * vn; j < vn; j++)
		{
			int J = 3 * j;
			Lag(I, J) = Lag(J, I) =
				Lag(I + 1, J + 1) = Lag(J + 1, I + 1) =
				Lag(I + 2, J + 2) = Lag(J + 2, I + 2) = curve.B_d(var_idx[j], t, 0);
		}
		// set tangent constraint in Lag
		for (int j = 0, I = 3 * vn + 3; j < vn; j++)
		{
			int J = 3 * j;
			double N_dj = curve.B_d(var_idx[j], t, 1);

			Lag(J + coord_map[0], I) = Lag(I, J + coord_map[0]) = N_dj * tangent[coord_map[2]];
			Lag(J + coord_map[2], I) = Lag(I, J + coord_map[2]) = -N_dj * tangent[coord_map[0]];

			Lag(J + coord_map[1], I + 1) = Lag(I + 1, J + coord_map[1]) = N_dj * tangent[coord_map[2]];
			Lag(J + coord_map[2], I + 1) = Lag(I + 1, J + coord_map[2]) = -N_dj * tangent[coord_map[1]];
		}

		// set curvature contraint in Lag
		double dx = current_C_d[0], dy = current_C_d[1], dz = current_C_d[2];
		double TxTx[3][3] = {
			{dy * dy + dz * dz, -dx * dy, -dx * dz},
			{-dx * dy, dx * dx + dz * dz, -dy * dz},
			{-dz * dx, -dz * dy, dx * dx + dy * dy} };
		for (int j = 0, I = 3 * vn + 5; j < vn; j++)
		{
			int J = 3 * j;
			double N_dj = curve.B_d(var_idx[j], t, 2);
			Lag(J, I) = Lag(I, J) = N_dj * TxTx[coord_map[0]][0];
			Lag(J + 1, I) = Lag(I, J + 1) = N_dj * TxTx[coord_map[0]][1];
			Lag(J + 2, I) = Lag(I, J + 2) = N_dj * TxTx[coord_map[0]][2];

			Lag(J, I + 1) = Lag(I + 1, J) = N_dj * TxTx[coord_map[1]][0];
			Lag(J + 1, I + 1) = Lag(I + 1, J + 1) = N_dj * TxTx[coord_map[1]][1];
			Lag(J + 2, I + 1) = Lag(I + 1, J + 2) = N_dj * TxTx[coord_map[1]][2];
		}

		// set the energy part of b
		for (int j = 0; j < vn; j++)
		{
			int J = 3 * j;
			b[J] = -QX[j];
			b[J + 1] = -QY[j];
			b[J + 2] = -QZ[j];
		}
		// set the position part of b
		b[3 * vn] = position[0];
		b[3 * vn + 1] = position[1];
		b[3 * vn + 2] = position[2];
		for (int j = 0; j < cn; j++)
		{
			CAAVector<3> cst_P = curve.B_d(cst_idx[j], t, 0) * curve.control_points[cst_idx[j]];
			b[3 * vn] -= cst_P[0];
			b[3 * vn + 1] -= cst_P[1];
			b[3 * vn + 2] -= cst_P[2];
		}
		// set the tangent part of b
		b[3 * vn + 3] = 0;
		b[3 * vn + 4] = 0;
		for (int j = 0; j < cn; j++)
		{
			CAAVector<3> cst_P = curve.B_d(cst_idx[j], t, 1) * curve.control_points[cst_idx[j]];
			b[3 * vn + 3] -= cst_P[coord_map[0]] * tangent[coord_map[2]] - cst_P[coord_map[2]] * tangent[coord_map[0]];
			b[3 * vn + 4] -= cst_P[coord_map[1]] * tangent[coord_map[2]] - cst_P[coord_map[2]] * tangent[coord_map[1]];
		}
		// set the curvature part of b
		b[3 * vn + 5] = curvature[coord_map[0]] * norm4_C_d;
		b[3 * vn + 6] = curvature[coord_map[1]] * norm4_C_d;
		for (int j = 0; j < cn; j++)
		{
			CAAVector<3> cst_P = curve.B_d(cst_idx[j], t, 1) * curve.control_points[cst_idx[j]];
			b[3 * vn + 5] -=
				cst_P[0] * TxTx[coord_map[0]][0] + cst_P[1] * TxTx[coord_map[0]][1] + cst_P[2] * TxTx[coord_map[0]][2];
			b[3 * vn + 6] -=
				cst_P[0] * TxTx[coord_map[1]][0] + cst_P[1] * TxTx[coord_map[1]][1] + cst_P[2] * TxTx[coord_map[1]][2];
		}

		// solve linear system
		CAADynamicMatrix Sol = Lag.LUPSolve(b);

		// give the result
		for (int i = 0; i < vn; i++)
		{
			int I = 3 * i;
			CAAVector<3>& P = curve.control_points[var_idx[i]];
			P[0] = Sol[I];
			P[1] = Sol[I + 1];
			P[2] = Sol[I + 2];
		}
		current_C_d = curve.eval(t, 1);

		// auto test_0 = curve.eval(t);
		// auto test_1 = curve.eval(t, 1);
		// auto test_2 = curve.curvature(t);
		// cout << test_0[0] << " " << test_0[1] << " " << test_0[2] << endl;
		// cout << test_1[0] << " " << test_1[1] << " " << test_1[2] << endl;
		// cout << test_2[0] << " " << test_2[1] << " " << test_2[2] << endl << endl;
	}
}

void EnergyFairing::getVariablePts(CAANURBSCurve<3>& curve, vector<Constraint> constraints, vector<bool>& variables)
{
	int n = curve.control_points.size();

	variables.resize(n, false);
	for (auto& cons : constraints) {
		int k = curve.findClosest(cons.t);
		int multis = 0;
		for (int i = k; i >= 0; i--)
			if (curve.knot_vector[i] == cons.t)
				multis++;
			else
				break;
		for (int i = 0; i < n; i++) {
			if (i < k && i >= k - curve.p - multis + 1)
				variables[i] = true;
		}
	}
}

void EnergyFairing::multiGeoConstraint(CAANURBSCurve<3>& curve, vector<Constraint> constraints, vector<bool> variables)
{
	//预处理
	int n = curve.control_points.size();
	assert(n == variables.size());
	vector<int> var_idx, cst_idx;
	for (int i = 0; i < n; i++)
	{
		if (variables[i])
			var_idx.push_back(i);
		else
			cst_idx.push_back(i);
	}
	int vn = var_idx.size();
	int cn = cst_idx.size();

	//处理输入约束，预计算矩阵尺寸
	int degree_map[3] = { 3,5,7 };
	int total_degree = 0;
	for (auto& cons : constraints) {
		total_degree += degree_map[cons.level];
	}
	assert(vn * 3 >= total_degree);//检查变量顶点自由度是否不小于约束自由度
	int M = 3 * vn + total_degree; // varible:3*vn|position:3|tangent:2|curvature:2

	//线性化迭代
	for (int iter = 0; iter < 3; iter++) {
		CAADynamicMatrix Lag(M, M);
		CAADynamicMatrix b(M, 1);
		// calculate the constant term in derivation
		CAADynamicMatrix Bij(vn, cn);
		CAADynamicMatrix Xc(cn, 1), Yc(cn, 1), Zc(cn, 1);
		CAADynamicMatrix Bin(vn, n);
		CAADynamicMatrix Xq(n, 1), Yq(n, 1), Zq(n, 1);

		for (int i = 0; i < vn; i++)
			for (int j = 0; j < cn; j++)
				Bij(i, j) =
				(alpha == 0 ? 0 : alpha * curve.energyInteral(var_idx[i], cst_idx[j], 1)) +
				(beta == 0 ? 0 : beta * curve.energyInteral(var_idx[i], cst_idx[j], 2)) +
				(gamma == 0 ? 0 : gamma * curve.energyInteral(var_idx[i], cst_idx[j], 3)) +
				(delta == 0 ? 0 : delta * curve.energyInteral(var_idx[i], cst_idx[j], 0));
		for (int j = 0; j < cn; j++)
		{
			CAAVector<3>& Pc = curve.control_points[cst_idx[j]];
			Xc[j] = Pc[0];
			Yc[j] = Pc[1];
			Zc[j] = Pc[2];
		}
		for (int i = 0; i < vn; i++)
			for (int j = 0; j < n; j++)
				Bin(i, j) = delta == 0 ? 0 : delta * curve.energyInteral(var_idx[i], j, 0);

		for (int j = 0; j < n; j++)
		{
			CAAVector<3> Pc = curve.control_points[j];
			Xq[j] = Pc[0];
			Yq[j] = Pc[1];
			Zq[j] = Pc[2];
		}

		CAADynamicMatrix QX = Bij * Xc - Bin * Xq;
		CAADynamicMatrix QY = Bij * Yc - Bin * Yq;
		CAADynamicMatrix QZ = Bij * Zc - Bin * Zq;

		// 设置Lag矩阵的能量部分
		for (int i = 0; i < vn; i++)
			for (int j = i; j < vn; j++)
			{
				int I = 3 * i, J = 3 * j;
				Lag(I, J) = Lag(J, I) =
					Lag(I + 1, J + 1) = Lag(J + 1, I + 1) =
					Lag(I + 2, J + 2) = Lag(J + 2, I + 2) =
					(alpha == 0 ? 0 : alpha * curve.energyInteral(var_idx[i], var_idx[j], 1)) +
					(beta == 0 ? 0 : beta * curve.energyInteral(var_idx[i], var_idx[j], 2)) +
					(gamma == 0 ? 0 : gamma * curve.energyInteral(var_idx[i], var_idx[j], 3)) +
					(delta == 0 ? 0 : delta * curve.energyInteral(var_idx[i], var_idx[j], 0));
			}
		// 设置b的能量部分
		for (int j = 0; j < vn; j++)
		{
			int J = 3 * j;
			b[J] = -QX[j];
			b[J + 1] = -QY[j];
			b[J + 2] = -QZ[j];
		}

		//约束部分
		int constraint_idx = vn * 3;
		for (auto& cons : constraints) {
			// 当前约束的预处理
			CAAVector<3> current_C_d = iter == 0 ? cons.tangent : curve.eval(cons.t, 1);
			double norm4_C_d = pow(current_C_d.norm(), 4);
			// 设置坐标映射使Z坐标非零，避免拉格朗日矩阵奇异
			int coord_map[3] = { 0, 1, 2 };
			double len = cons.tangent.norm();
			int nonzero_index = abs(cons.tangent[2] / len) > 0.1 ? 2 : (abs(cons.tangent[1] / len) > 0.1 ? 1 : 0);
			swap(coord_map[nonzero_index], coord_map[2]);

			// 设置Lag矩阵的位置约束
			for (int j = 0, I = constraint_idx; j < vn; j++)
			{
				int J = 3 * j;
				Lag(I, J) = Lag(J, I) =
					Lag(I + 1, J + 1) = Lag(J + 1, I + 1) =
					Lag(I + 2, J + 2) = Lag(J + 2, I + 2) = curve.B_d(var_idx[j], cons.t, 0);
			}

			if (cons.level >= 1) {
				// 设置Lag矩阵的切向约束
				for (int j = 0, I = constraint_idx + 3; j < vn; j++)
				{
					int J = 3 * j;
					double N_dj = curve.B_d(var_idx[j], cons.t, 1);

					Lag(J + coord_map[0], I) = Lag(I, J + coord_map[0]) = N_dj * cons.tangent[coord_map[2]];
					Lag(J + coord_map[2], I) = Lag(I, J + coord_map[2]) = -N_dj * cons.tangent[coord_map[0]];

					Lag(J + coord_map[1], I + 1) = Lag(I + 1, J + coord_map[1]) = N_dj * cons.tangent[coord_map[2]];
					Lag(J + coord_map[2], I + 1) = Lag(I + 1, J + coord_map[2]) = -N_dj * cons.tangent[coord_map[1]];
				}
			}
			double dx = current_C_d[0], dy = current_C_d[1], dz = current_C_d[2];
			double TxTx[3][3] = {
				{dy * dy + dz * dz, -dx * dy, -dx * dz},
				{-dx * dy, dx * dx + dz * dz, -dy * dz},
				{-dz * dx, -dz * dy, dx * dx + dy * dy} };
			if (cons.level >= 2) {
				// 设置Lag矩阵的线性化曲率约束

				for (int j = 0, I = constraint_idx + 5; j < vn; j++)
				{
					int J = 3 * j;
					double N_dj = curve.B_d(var_idx[j], cons.t, 2);
					Lag(J, I) = Lag(I, J) = N_dj * TxTx[coord_map[0]][0];
					Lag(J + 1, I) = Lag(I, J + 1) = N_dj * TxTx[coord_map[0]][1];
					Lag(J + 2, I) = Lag(I, J + 2) = N_dj * TxTx[coord_map[0]][2];

					Lag(J, I + 1) = Lag(I + 1, J) = N_dj * TxTx[coord_map[1]][0];
					Lag(J + 1, I + 1) = Lag(I + 1, J + 1) = N_dj * TxTx[coord_map[1]][1];
					Lag(J + 2, I + 1) = Lag(I + 1, J + 2) = N_dj * TxTx[coord_map[1]][2];
				}
			}

			// 设置b的位置约束
			b[constraint_idx] = cons.position[0];
			b[constraint_idx + 1] = cons.position[1];
			b[constraint_idx + 2] = cons.position[2];
			for (int j = 0; j < cn; j++)
			{
				CAAVector<3> cst_P = curve.B_d(cst_idx[j], cons.t, 0) * curve.control_points[cst_idx[j]];
				b[constraint_idx] -= cst_P[0];
				b[constraint_idx + 1] -= cst_P[1];
				b[constraint_idx + 2] -= cst_P[2];
			}
			if (cons.level >= 1) {
				// 设置b的切向约束
				b[constraint_idx + 3] = 0;
				b[constraint_idx + 4] = 0;
				for (int j = 0; j < cn; j++)
				{
					CAAVector<3> cst_P = curve.B_d(cst_idx[j], cons.t, 1) * curve.control_points[cst_idx[j]];
					b[constraint_idx + 3] -= cst_P[coord_map[0]] * cons.tangent[coord_map[2]] - cst_P[coord_map[2]] * cons.tangent[coord_map[0]];
					b[constraint_idx + 4] -= cst_P[coord_map[1]] * cons.tangent[coord_map[2]] - cst_P[coord_map[2]] * cons.tangent[coord_map[1]];
				}
			}
			if (cons.level >= 2) {
				// 设置b的线性化曲率约束
				b[constraint_idx + 5] = cons.curvature[coord_map[0]] * norm4_C_d;
				b[constraint_idx + 6] = cons.curvature[coord_map[1]] * norm4_C_d;
				for (int j = 0; j < cn; j++)
				{
					CAAVector<3> cst_P = curve.B_d(cst_idx[j], cons.t, 1) * curve.control_points[cst_idx[j]];
					b[constraint_idx + 5] -=
						cst_P[0] * TxTx[coord_map[0]][0] + cst_P[1] * TxTx[coord_map[0]][1] + cst_P[2] * TxTx[coord_map[0]][2];
					b[constraint_idx + 6] -=
						cst_P[0] * TxTx[coord_map[1]][0] + cst_P[1] * TxTx[coord_map[1]][1] + cst_P[2] * TxTx[coord_map[1]][2];
				}
			}

			//设置定位指标
			constraint_idx += degree_map[cons.level];

		}
		// solve linear system
		CAADynamicMatrix Sol = Lag.LUPSolve(b);

		// give the result
		for (int i = 0; i < vn; i++)
		{
			int I = 3 * i;
			CAAVector<3>& P = curve.control_points[var_idx[i]];
			P[0] = Sol[I];
			P[1] = Sol[I + 1];
			P[2] = Sol[I + 2];
		}
	}
}

template <int dim>
void CAANURBSCurve<dim>::getReport(Info& res)
{
	vector<int> ts;
	getAllKnotPoints(ts);
	res.energy[0] = res.energy[1] = res.energy[2] = 0;
	res.pts.clear();
	int n = control_points.size();

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			res.energy[0] += energyInteral(i, j, 1) * control_points[i].dot(control_points[j]);
			res.energy[1] += energyInteral(i, j, 2) * control_points[i].dot(control_points[j]);
			res.energy[2] += energyInteral(i, j, 3) * control_points[i].dot(control_points[j]);
		}
	}
	res.energy[1] *= 0.01;
	res.energy[2] *= 0.0001;

	for (int i = 1; i < ts.size() - 1; i++)
	{
		CAAVector<dim> d[2][4];
		d[0][0] = knotEval(i - 1, true, 0);
		d[0][1] = knotEval(i - 1, true, 1);
		d[0][2] = knotEval(i - 1, true, 2);
		d[0][3] = knotEval(i - 1, true, 3);

		d[1][0] = knotEval(i, false, 0);
		d[1][1] = knotEval(i, false, 1);
		d[1][2] = knotEval(i, false, 2);
		d[1][3] = knotEval(i, false, 3);

		for (int j = 0; j < 4; j++)
		{
			if (!checkGeometricContinuity<dim>(d[0], d[1], j, 1e-6))
			{
				res.pts[knot_vector[ts[i]]] = j;
				break;
			}
		}
	}

	if (dim == 2)
	{
		vector<double> ipts;
		findInflectionPts(ipts, 100);
		cout << ipts.size() << "!" << endl;
		for (int i = 0; i < ipts.size(); i++)
		{
			res.ipts.insert(ipts[i]);
		}
	}
}

#ifdef CAA_ENV
template <>
void CAANURBSCurve<3>::getCATNurbsCurve(CATGeoFactory* piGeoFactory, CATNurbsCurve** oResult)
{
	cout << "========recovering=========" << endl;
	vector<double> ts;
	vector<long> ms;
	CATMathSetOfPoints pts(control_points.size());
	vector<double> ws;
	double current = knot_vector[0];
	ts.push_back(current);
	ms.push_back(1);
	for (int i = 1; i < knot_vector.size(); i++)
	{
		if (current != knot_vector[i])
		{
			current = knot_vector[i];
			ts.push_back(current);
			ms.push_back(1);
		}
		else
		{
			int fin = ms.size() - 1;
			ms[fin] = ms[fin] + 1;
		}
	}
	for (int i = 0; i < control_points.size(); i++)
	{
		CATMathPoint cpt(control_points[i][0], control_points[i][1], control_points[i][2]);
		pts.SetPoint(cpt, i);
	}
	CATKnotVector new_knots((long)p, 0L, (long)ts.size(), &ts[0], &ms[0]);
	*oResult = piGeoFactory->CreateNurbsCurve(new_knots, isRational, pts, &weights[0], CatKeepParameterization);
	cout << "========end=========" << endl;
}

template <>
CAANURBSCurve<3>::CAANURBSCurve(const CATNurbsCurve& nurbs)
{
	cout << "========converting=========" << endl;
	const CATKnotVector* knot = nurbs.GetKnotVector();
	int num = knot->GetNumberOfKnots();
	const double* knots = NULL;
	knot->GetKnots(knots);
	p = knot->GetDegree();
	cout << "p: " << p << endl;
	knot_vector.clear();
	for (int i = 0; i < num; i++)
	{
		int m = knot->GetKnotMultiplicity(i + 1);
		for (int j = 0; j < m; j++)
		{
			cout << knots[i] << " ";
			knot_vector.push_back(knots[i]);
		}
		cout << endl;
	}
	int n = knot_vector.size() - p - 1;
	control_points.clear();
	for (int i = 1; i <= n; i++)
	{
		CATMathPoint pt = nurbs.GetOneControlPoint(i);
		CAAVector<3> my_pt(pt.GetX(), pt.GetY(), pt.GetZ());

		control_points.push_back(my_pt);
	}
	isRational = nurbs.IsRational();
	if (isRational)
	{
		weights.clear();
		for (int i = 1; i <= n; i++)
		{
			double w = nurbs.GetOneWeight(i);
			weights.push_back(w);
		}
	}
	cout << "========end=========" << endl;
}
#endif
template class CAANURBSCurve<2>;
template class CAANURBSCurve<3>;

void EnergyFairing::Constraint::regularize()
{
	if (level >= 1) {
		tangent = tangent / tangent.norm();
		if (level >= 2) {
			curvature = curvature - (tangent.dot(curvature)) / (tangent.dot(tangent)) * tangent;
		}
	}
}
