/****************************************************************************\
*						itwom.math - Math for ITWOM							 *
*****************************************************************************/

#include "stdafx.h"
#include <math.h>
#include <complex>
#include <assert.h>
#include <string.h>
#include "itwom.h"

using namespace std;


//| ------------------------------
//| 
//| FUNCTION: MinimumInteger
//| 
//| OLD NAME: mymin
//| 
//| NOTES: 
//| 
//| ------------------------------
int
MinimumInteger(
	const int& i,
	const int& j)
{
	if (i < j)
	{
		return i;
	}
	else
	{
		return j;
	}
}


//| ------------------------------
//| 
//| FUNCTION: MaximumInteger
//| 
//| OLD NAME: mymax
//| 
//| NOTES: 
//| 
//| ------------------------------
int
MaximumInteger(
	const int& i,
	const int& j)
{
	if (i > j)
	{
		return i;
	}
	else
	{
		return j;
	}
}


//| ------------------------------
//| 
//| FUNCTION: MinimumDouble
//| 
//| OLD NAME: mymin
//| 
//| NOTES: 
//| 
//| ------------------------------
double
MinimumDouble(
	const double& a,
	const double& b)
{
	if (a < b)
	{
		return a;
	}
	else
	{
		return b;
	}
}


//| ------------------------------
//| 
//| FUNCTION: MaximumDouble
//| 
//| OLD NAME: mymax
//| 
//| NOTES: 
//| 
//| ------------------------------
double
MaximumDouble(
	const double& a,
	const double& b)
{
	if (a > b)
	{
		return a;
	}
	else
	{
		return b;
	}
}


//| ------------------------------
//| 
//| FUNCTION: PositiveDifferenceOrZero
//| 
//| OLD NAME: FORTRAN_DIM
//| 
//| NOTES: 
//|   This performs the FORTRAN DIM function. Result is x-y
//|   if x is greater than y; otherwise result is 0.0
//| 
//| ------------------------------
double
PositiveDifferenceOrZero(
	const double& x,
	const double& y)
{
	if (x > y)
	{
		return x - y;
	}
	else
	{
		return 0.0;
	}
}


//| ------------------------------
//| 
//| FUNCTION: AbsoluteValueOfComplexNumber
//| 
//| OLD NAME: abq_alos
//| 
//| NOTES: 
//| 
//| ------------------------------
double
AbsoluteValueOfComplexNumber(
	complex<double> r)
{
	return r.real() * r.real() + r.imag() * r.imag();
}


//| ------------------------------
//| 
//| FUNCTION: StandardNormalTailDistributionFunctionInverse
//| 
//| OLD NAME: qerfi
//| 
//| NOTES: 
//|   The inverse of StandardNormalTailDistributionFunction, the solution for x to q = Q(x).
//|   The approximation is due to C. Hastings, Jr.
//|   ("Approximations for digital computers," Princeton
//|   Univ. Press, 1955) and the maximum error should be 4.5 x 10 e-4
//| 
//| ------------------------------
double
StandardNormalTailDistributionFunctionInverse(
	double q)
{
	double x, t, v;
	double c0 = 2.515516698;
	double c1 = 0.802853;
	double c2 = 0.010328;
	double d1 = 1.432788;
	double d2 = 0.189269;
	double d3 = 0.001308;

	x = 0.5 - q;
	t = MaximumDouble(0.5 - fabs(x), 0.000001);
	t = sqrt(-2.0 * log(t));
	v = t - ((c2 * t + c1) * t + c0) / (((d3 * t + d2) * t + d1) * t + 1.0);

	if (x < 0.0)
	{
		v = -v;
	}

	return v;
}


//| ------------------------------
//| 
//| FUNCTION: Curve
//| 
//| OLD NAME: curve
//| 
//| NOTES: 
//| 
//| ------------------------------
double
Curve(
	double const& c1,
	double const& c2,
	double const& x1,
	double const& x2,
	double const& x3,
	double const& de)
{
	//return (c1+c2/(1.0+pow((de-x2)/x3,2.0)))*pow(de/x1,2.0)/(1.0+pow(de/x1,2.0));
	double temp1, temp2;

	temp1 = (de - x2) / x3;
	temp2 = de / x1;

	temp1 *= temp1;
	temp2 *= temp2;

	return (c1 + c2 / (1.0 + temp1)) * temp2 / (1.0 + temp2);
}


//| ------------------------------
//| 
//| FUNCTION: CreateQuantile
//| 
//| OLD NAME: qtile
//| 
//| NOTES: 
//|   This routine provides a quantile. It reorders the array a so that a(j) j=1...ir are all greater than or
//|   equal to all a(i), i=ir...nn. In particular, a(ir) will have the same value it would have if "a" were
//|   completely sorted in descending order. The returned value is CreateQuantile = a(ir)
//| 
//| ------------------------------
double
CreateQuantile(
	const int& nn,
	double a[],
	const int& ir)
{
	double q = 0.0, r;					// q initialization -- KD2BD
	int m, n, i, j, j1 = 0, i0 = 0, k;	// more initializations -- KD2BD
	bool done = false;
	bool goto10 = true;

	m = 0;
	n = nn;
	k = MinimumInteger(MaximumInteger(0, ir), n);

	while (!done)
	{
		if (goto10)
		{
			q = a[k];
			i0 = m;
			j1 = n;
		}

		i = i0;

		while (i <= n && a[i] >= q)
		{
			i++;
		}

		if (i > n)
		{
			i = n;
		}

		j = j1;

		while ((j >= m) && (a[j] <= q))
		{
			j--;
		}

		if (j < m)
		{
			j = m;
		}

		if (i < j)
		{
			r = a[i];
			a[i] = a[j];
			a[j] = r;
			i0 = i + 1;
			j1 = j - 1;
			goto10 = false;
		}
		else if (i < k)
		{
			a[k] = a[i];
			a[i] = q;
			m = i + 1;
			goto10 = true;
		}
		else if (j > k)
		{
			a[k] = a[j];
			a[j] = q;
			n = j - 1;
			goto10 = true;
		}
		else
		{
			done = true;
		}
	}

	return q;
}


//| ------------------------------
//| 
//| FUNCTION: StandardNormalTailDistributionFunction
//| 
//| OLD NAME: qerf
//| 
//| NOTES: 
//|   The standard normal complementary probability Q(s).
//|   The maximum error should be 7.5 x 10e8
//| 
//| ------------------------------
double
StandardNormalTailDistributionFunction(
	const double& z)
{
	double b1 = 0.319381530, b2 = -0.356563782, b3 = 1.781477937;
	double b4 = -1.821255987, b5 = 1.330274429;
	double rp = 4.317008, rrt2pi = 0.398942280;
	double t, x, qerfv;

	x = z;
	t = fabs(x);

	if (t >= 10.0)
	{
		qerfv = 0.0;
	}
	else
	{
		t = rp / (t + rp);
		qerfv = exp(-0.5 * x * x) * rrt2pi * ((((b5 * t + b4) * t + b3) * t + b2) * t + b1) * t;
	}

	if (x < 0.0)
	{
		qerfv = 1.0 - qerfv;
	}

	return qerfv;
}


//| ------------------------------
//| 
//| FUNCTION: CalculateLinearLeastSquaresFit
//| 
//| OLD NAME: z1sq1
//| 
//| NOTES: 
//|   A linear least squares fit between x1, x2 to the function described by the array z. This array must
//|   have a special format: z(0) = en, the number of equally large intervales, z(1) = epsilon, the interval length,
//|   and z(j+2), j=0...n function values. The output consists of values of the required line, zo at 0, zn at
//|   xt = n*epsilon
//|   Changes suggested by IEEE Broadcast Technology Society Newsletter, Vol. 17, Number 1, Spring 2009
//| 
//| ------------------------------
void
CalculateLinearLeastSquaresFit(
	double z[],
	const double& x1,
	const double& x2,
	double& z0,
	double& zn)
{
	// Used only with ITM 1.2.2
	double xn, xa, xb, x, a, b;
	int n, ja, jb;

	xn = z[0];
	xa = int(PositiveDifferenceOrZero(x1 / z[1], 0.0));
	xb = xn - int(PositiveDifferenceOrZero(xn, x2 / z[1]));

	if (xb <= xa)
	{
		xa = PositiveDifferenceOrZero(xa, 1.0);
		xb = xn - PositiveDifferenceOrZero(xn, xb + 1.0);
	}

	ja = (int)xa;
	jb = (int)xb;
	n = jb - ja;
	xa = xb - xa;
	x = -0.5 * xa;
	xb += x;
	a = 0.5 * (z[ja + 2] + z[jb + 2]);
	b = 0.5 * (z[ja + 2] - z[jb + 2]) * x;

	for (int i = 2; i <= n; ++i)
	{
		++ja;
		x += 1.0;
		a += z[ja + 2];
		b += z[ja + 2] * x;
	}

	a /= xa;
	b = b * 12.0 / ((xa * xa + 2.0) * xa);
	z0 = a - b * xb;
	zn = a + b * (xn - xb);
}


//| ------------------------------
//| 
//| FUNCTION: CalculateLinearLeastSquaresFitV2
//| 
//| OLD NAME: z1sq2
//| 
//| NOTES: 
//| 
//| ------------------------------
void
CalculateLinearLeastSquaresFitV2(
	double z[],
	const double& x1,
	const double& x2,
	double& z0,
	double& zn)
{
	// corrected for use with ITWOM
	double xn, xa, xb, x, a, b, bn;
	int n, ja, jb;

	xn = z[0];
	xa = int(PositiveDifferenceOrZero(x1 / z[1], 0.0));
	xb = xn - int(PositiveDifferenceOrZero(xn, x2 / z[1]));

	if (xb <= xa)
	{
		xa = PositiveDifferenceOrZero(xa, 1.0);
		xb = xn - PositiveDifferenceOrZero(xn, xb + 1.0);
	}

	ja = (int)xa;
	jb = (int)xb;
	xa = (2 * int((xb - xa) / 2)) - 1;
	x = -0.5 * (xa + 1);
	xb += x;
	ja = jb - 1 - (int)xa;
	n = jb - ja;
	a = (z[ja + 2] + z[jb + 2]);
	b = (z[ja + 2] - z[jb + 2]) * x;
	bn = 2 * (x * x);

	for (int i = 2; i <= n; ++i)
	{
		++ja;
		x += 1.0;
		bn += (x * x);
		a += z[ja + 2];
		b += z[ja + 2] * x;
	}

	a /= (xa + 2);
	b = b / bn;
	z0 = a - (b * xb);
	zn = a + (b * (xn - xb));
}
