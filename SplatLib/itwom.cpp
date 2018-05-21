/********************************************************************************
* ITWOM version 3.0a, January 20, 2011  File: itwom3.0a.cpp                     *
* Provenance:   Further test version of itwom2.0m re adj to Hrzn range factors  *
* 1. This file is based on a thorough debugging, completion, and update of the  *
* ITM, based on an original, public domain version of this file obtained from:  *
* ftp://flattop.its.bldrdoc.gov/itm/ITMDLL.cpp prior to May, 2007. C++ routines *
* for this program are taken from a translation of the FORTRAN code written by  *
* U.S. Department of Commerce NTIA/ITS Institute for Telecommunication Sciences	*
* Irregular Terrain Model (ITM) (Longley-Rice).                                 *
* 2. The Linux version of this file incorporates improvements suggested by a    *
* study of changes made to file itm.cpp by J. D. McDonald to remove Microsoft   *
* Windows dll-isms and to debug an ambguity in overloaded calls.                *
* 3. The Linux version of this file also incorporates improvements suggested by *
* a study of further modifications made to itm.cpp by John A. Magliacane to     *
* remove unused variables, unneeded #includes, and to replace pow() statements 	*
* with explicit multiplications to improve execution speed and accuracy.        *
* 4. On August 19, 2007 this file was modified by Sid Shumate to include        *
* changes and updates included in version 7.0 of ITMDLL.cpp, which was released *
* by the NTIA/ITS on June 26, 2007. With correction set SS1 and SS2: itm71.cpp.	*
* 5. On Feb. 5, 2008 this file became v.1.0 of the ITWOM with the addition, by 	*
* Sid Shumate, of multiple corrections, the replacement of subroutines lrprop   *
* and alos with lrprop2 and alos2, and the addition of subroutine saalos to     *
* incorporate Radiative Transfer Engine (RTE) computations in the line of sight *
* range.																		*
* Update 8 Jun 2010 to modify alos to match 2010 series of IEEE-BTS             *
* newsletter articles                                                           *
* Update June 12, 2010 to z version to change test outputs                      *
* Offshoot start date June 23, 2010 to start itwom2.0 dual version for FCC.     *
* Update to 2.0b July 25 to correct if statement errors in adiff2 re two peak   *
* calculations starting at line 525                                             *
* Development to 2.0c 8 Aug 2010 after modifying saalos and adiff for full      *
* addition of saalos treatment to post obstruction calculations and debugging.  *
* Modified to make 1st obs loss=5.8 only, no clutter loss considered            *
*                                                                               *
* Commented out unused variables and calculations to eliminate gcc warnings     *
*    (-Wunused-but-set-variable)  -- John A. Magliacane -- July 25, 2013        *
********************************************************************************/

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
MinimumInteger
   (const int &i, 
	const int &j)
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
MaximumInteger
   (const int &i, 
	const int &j)
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
MinimumDouble
   (const double &a, 
	const double &b)
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
MaximumDouble
   (const double &a, 
	const double &b)
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
PositiveDifferenceOrZero
   (const double &x, 
	const double &y)
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
//| FUNCTION: AttenuationFromSingleKnifeEdge
//| 
//| OLD NAME: aknfe
//| 
//| NOTES: 
//|   The attenuation due to a single knife edge, the Fresnel 
//|   integral (in decibels) as a function of v2.
//|   The approximation is given in Alg 6.1.
//| 
//| ------------------------------
double 
AttenuationFromSingleKnifeEdge
   (const double &v2)
{
	double a;

	if (v2 < 5.76)
	{
		a = 6.02 + 9.11*sqrt(v2) - 1.27*v2;
	}
	else
	{
		a = 12.953 + 10*log10(v2);
	}

	return a;
}


//| ------------------------------
//| 
//| FUNCTION: HeightGainOverSmoothSphere
//| 
//| OLD NAME: fht
//| 
//| NOTES: 
//|   The height-gain over a smooth sphere, 
//|   to be used in the "three radii" method.
//|   The approximation is that given in Alg 6.4.
//| 
//| ------------------------------
double 
HeightGainOverSmoothSphere
   (const double &x, 
	const double &pk)
{
	double w, fhtv;

	if (x < 200.0)
	{
		w = -log(pk);

		if ((pk < 1.0e-5) || (x * w*w*w > 5495.0))
		{
			fhtv = -117.0;

			if (x > 1.0)
			{
				fhtv = 40.0*log10(x) + fhtv;
			}
		}
		else
		{
			fhtv = 2.5e-5*x*x / pk - 8.686*w - 15.0;
		}
	}
	else
	{
		fhtv = 0.05751*x - 10.0*log10(x);

		if (x < 2000.0)
		{
			w = 0.0134 * x * exp(-0.005 * x);
			fhtv = (1.0 - w)*fhtv + w*(40.0*log10(x) - 117.0);
		}
	}
	return fhtv;
}


//| ------------------------------
//| 
//| FUNCTION: H01ScatterFields
//| 
//| OLD NAME: h0f
//| 
//| NOTES: 
//|   This is the H01 function for scatter fields as defined in Alg 6.
//| 
//| ------------------------------
double 
H01ScatterFields
   (double r, 
	double et)
{
	double a[5] = { 25.0, 80.0, 177.0, 395.0, 705.0 };
	double b[5] = { 24.0, 45.0,  68.0,  80.0, 105.0 };
	double q, x;
	double h0fv, temp;
	int it;

	it = (int)et;

	if (it <= 0)
	{
		it = 1;
		q = 0.0;
	}
	else if (it >= 5)
	{
		it = 5;
		q = 0.0;
	}
	else
	{
		q = et - it;
	}

	//x = pow(1.0/r, 2.0);
	temp = 1.0 / r;
	x = temp * temp;

	h0fv = 4.343*log((a[it - 1] * x + b[it - 1])*x + 1.0);

	if (q != 0.0)
	{
		h0fv = (1.0 - q)*h0fv + q * 4.343*log((a[it] * x + b[it])*x + 1.0);
	}

	return h0fv;
}


//| ------------------------------
//| 
//| FUNCTION: FThetaDScatterFields
//| 
//| OLD NAME: ahd
//| 
//| NOTES: 
//|   This is the F(theta d) function for scatter fields.
//| 
//| ------------------------------
double 
FThetaDScatterFields
   (double td)
{
	int i;
	double a[3] = { 133.4,    104.6,     71.8 };
	double b[3] = { 0.332e-3, 0.212e-3, 0.157e-3 };
	double c[3] = { -4.343,   -1.086,    2.171 };

	if (td <= 10e3)
	{
		i = 0;
	}
	else if (td <= 70e3)
	{
		i = 1;
	}
	else
	{
		i = 2;
	}

	return a[i] + b[i] * td + c[i] * log(td);
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
AbsoluteValueOfComplexNumber
   (complex<double> r)
{
	return r.real()*r.real() + r.imag()*r.imag();
}


//| ------------------------------
//| 
//| FUNCTION: CalculateClutterLoss
//| 
//| OLD NAME: saalos
//| 
//| NOTES: 
//| 
//| ------------------------------
double 
CalculateClutterLoss
   (double d, 
	prop_type &prop, 
	propa_type &propa)
{
	double ensa, encca, q, dp, dx, tde, hc, ucrpc, ctip, tip, tic, stic, ctic, sta;
	double ttc, cttc, crpc, ssnps, d1a, rsp, tsp, arte, zi, pd, pdk, hone, tvsr;
	double saalosv = 0.0;

	q = 0.0;

	if (d == 0.0)
	{
		tsp = 1.0;
		rsp = 0.0;
		d1a = 50.0;
		saalosv = 0.0;
	}
	else if (prop.antennaHeightAboveGroundLevel[1] > prop.canopyClutterHeight)
	{
		saalosv = 0.0;
	}
	else
	{
		pd = d;
		pdk = pd / 1000.0;
		tsp = 1.0;
		rsp = 0.0;
		d1a = pd;
		//| at first, hone is transmitter antenna height
		//| relative to receive site ground level.
		hone = prop.transmitterGroundHeight + prop.tsgh - (prop.antennaHeightAndPathElevation[1] - prop.antennaHeightAboveGroundLevel[1]);

		if (prop.transmitterGroundHeight > prop.canopyClutterHeight)  //| for TX ant above all clutter height
		{
			ensa = 1 + prop.surfaceRefractivity*0.000001;
			encca = 1 + prop.encc*0.000001;
			dp = pd;

			for (int j = 0; j < 5; ++j)
			{
				tde = dp / 6378137.0;
				hc = (prop.canopyClutterHeight + 6378137.0)*(1 - cos(tde));
				dx = (prop.canopyClutterHeight + 6378137.0)*sin(tde);
				ucrpc = sqrt((hone - prop.canopyClutterHeight + hc)*(hone - prop.canopyClutterHeight + hc) + (dx*dx));
				ctip = (hone - prop.canopyClutterHeight + hc) / ucrpc;
				tip = acos(ctip);
				tic = tip + tde;
				tic = MaximumDouble(0.0, tic);
				stic = sin(tic);
				sta = (ensa / encca)*stic;
				ttc = asin(sta);
				cttc = sqrt(1 - (sin(ttc))*(sin(ttc)));
				crpc = (prop.canopyClutterHeight - prop.antennaHeightAboveGroundLevel[1]) / cttc;
				if (crpc >= dp)
				{
					crpc = dp - 1 / dp;
				}

				ssnps = (3.1415926535897 / 2) - tic;
				d1a = (crpc*sin(ttc)) / (1 - 1 / 6378137.0);
				dp = pd - d1a;
			}

			ctic = cos(tic);

			//| if the ucrpc path touches the canopy before reaching the
			//| end of the ucrpc, the entry point moves toward the
			//| transmitter, extending the crpc and d1a. Estimating the d1a:

			if (ssnps <= 0.0)
			{
				d1a = MinimumDouble(0.1*pd, 600.0);
				crpc = d1a;
				//| hone must be redefined as being barely above
				//| the canopy height with respect to the receiver
				//| canopy height, which despite the earth curvature
				//| is at or above the transmitter antenna height.
				hone = prop.canopyClutterHeight + 1;
				rsp = .997;
				tsp = 1 - rsp;
			}
			else
			{
				if (prop.transmitterPolarization >= 1)  //|vertical or circular
				{
					q = ((ensa*cttc - encca * ctic) / (ensa*cttc + encca * ctic));
					rsp = q * q;
					tsp = 1 - rsp;

					if (prop.transmitterPolarization == 2)  //| circular - new
					{
						q = ((ensa*ctic - encca * cttc) / (ensa*ctic + encca * cttc));
						rsp = ((ensa*cttc - encca * ctic) / (ensa*cttc + encca * ctic));
						rsp = (q*q + rsp * rsp) / 2;
						tsp = 1 - rsp;
					}
				}
				else	//| horizontal, or undefined
				{
					q = ((ensa*ctic - encca * cttc) / (ensa*ctic + encca * cttc));
					rsp = q * q;
					tsp = 1 - rsp;
				}
			}
			//| tvsr is defined as tx ant height above receiver ant height
			tvsr = MaximumDouble(0.0, prop.transmitterGroundHeight + prop.tsgh - prop.antennaHeightAndPathElevation[1]);

			if (d1a < 50.0)
			{
				arte = 0.0195*crpc - 20 * log10(tsp);
			}
			else
			{
				if (d1a < 225.0)
				{
					if (tvsr > 1000.0)
					{
						q = d1a * (0.03*exp(-0.14*pdk));
					}
					else
					{
						q = d1a * (0.07*exp(-0.17*pdk));
					}

					arte = q + (0.7*pdk - MaximumDouble(0.01, log10(prop.waveNumber * 47.7) - 2))*(prop.antennaHeightAboveGroundLevel[1] / hone);
				}
				else
				{
					q = 0.00055*(pdk)+log10(pdk)*(0.041 - 0.0017*sqrt(hone) + 0.019);

					arte = d1a * q - (18 * log10(rsp)) / (exp(hone / 37.5));

					zi = 1.5*sqrt(hone - prop.canopyClutterHeight);

					if (pdk > zi)
					{
						q = (pdk - zi)*10.2*((sqrt(MaximumDouble(0.01, log10(prop.waveNumber * 47.7) - 2.0))) / (100 - zi));
					}
					else
					{
						q = ((zi - pdk) / zi)*(-20.0*MaximumDouble(0.01, log10(prop.waveNumber * 47.7) - 2.0)) / sqrt(hone);
					}

					arte = arte + q;
				}
			}
		}
		else  //| for TX at or below clutter height
		{
			q = (prop.canopyClutterHeight - prop.transmitterGroundHeight)*(2.06943 - 1.56184*exp(1 / prop.canopyClutterHeight - prop.transmitterGroundHeight));
			q = q + (17.98 - 0.84224*(prop.canopyClutterHeight - prop.transmitterGroundHeight))*exp(-0.00000061*pd);
			arte = q + 1.34795 * 20 * log10(pd + 1.0);
			arte = arte - (MaximumDouble(0.01, log10(prop.waveNumber * 47.7) - 2))*(prop.antennaHeightAboveGroundLevel[1] / prop.transmitterGroundHeight);
		}
		saalosv = arte;
	}
	return saalosv;
}


//| ------------------------------
//| 
//| FUNCTION: CalculateDiffractionAttenuation
//| 
//| OLD NAME: adiff
//| 
//| NOTES: 
//|   The function finds the "diffraction attenuation"
//|   at the distance d. It uses a convex combination
//|   of smooth sphere diffraction and double knife-edge
//|   diffraction. A call with d = 0 sets up initial constants.
//| 
//| ------------------------------
double 
CalculateDiffractionAttenuation
   (double d, 
	prop_type &prop, 
	propa_type &propa)
{
	complex<double> prop_zgnd(prop.surfaceImpedanceReal, prop.surfaceImpedanceImag);
	static double wd1, xd1, afo, qk, aht, xht;
	double a, q, pk, ds, th, wa, ar, wd, adiffv;

	if (d == 0)
	{
		q = prop.antennaHeightAboveGroundLevel[0] * prop.antennaHeightAboveGroundLevel[1];
		qk = prop.antennaEffectiveHeight[0] * prop.antennaEffectiveHeight[1] - q;

		if (prop.analysisMode < 0)
		{
			q += 10.0;
		}

		wd1 = sqrt(1.0 + qk / q);
		xd1 = propa.dla + propa.tha / prop.relativeCurvature;
		q = (1.0 - 0.8*exp(-propa.dlsa / 50e3)) * prop.interdecileElevationRangeBetweenPoints;
		q *= 0.78*exp(-pow(q / 16.0, 0.25));
		afo = MinimumDouble(15.0, 2.171*log(1.0 + 4.77e-4*prop.antennaHeightAboveGroundLevel[0] * prop.antennaHeightAboveGroundLevel[1] * prop.waveNumber * q));
		qk = 1.0 / abs(prop_zgnd);
		aht = 20.0;
		xht = 0.0;

		for (int j = 0; j < 2; ++j)
		{
			//a = 0.5*pow(prop.horizonDistance[j],2.0)/prop.antennaEffectiveHeight[j];
			a = 0.5*(prop.horizonDistance[j] * prop.horizonDistance[j]) / prop.antennaEffectiveHeight[j];
			wa = pow(a * prop.waveNumber, ONE_THIRD);
			pk = qk / wa;
			q = (1.607 - pk) * 151.0 * wa * prop.horizonDistance[j] / a;
			xht += q;
			aht += HeightGainOverSmoothSphere(q, pk);
		}

		adiffv = 0.0;
	}
	else
	{
		th = propa.tha + d * prop.relativeCurvature;
		ds = d - propa.dla;
		//q = 0.0795775*prop.waveNumber*ds*pow(th,2.0);
		q = 0.0795775*prop.waveNumber*ds*th*th;
		adiffv = AttenuationFromSingleKnifeEdge(q*prop.horizonDistance[0] / (ds + prop.horizonDistance[0])) + AttenuationFromSingleKnifeEdge(q*prop.horizonDistance[1] / (ds + prop.horizonDistance[1]));
		a = ds / th;
		wa = pow(a * prop.waveNumber, ONE_THIRD);
		pk = qk / wa;
		q = (1.607 - pk)*151.0*wa*th + xht;
		ar = 0.05751*q - 4.343*log(q) - aht;
		q = (wd1 + xd1 / d)*MinimumDouble(((1.0 - 0.8*exp(-d / 50e3)) * prop.interdecileElevationRangeBetweenPoints * prop.waveNumber), 6283.2);
		wd = 25.1 / (25.1 + sqrt(q));
		adiffv = ar * wd + (1.0 - wd)*adiffv + afo;
	}

	return adiffv;
}


//| ------------------------------
//| 
//| FUNCTION: CalculateDiffractionAttenuationV2
//| 
//| OLD NAME: adiff2
//| 
//| NOTES: 
//| 
//| ------------------------------
double 
CalculateDiffractionAttenuationV2
   (double d, 
	prop_type &prop, 
	propa_type &propa)
{
	complex<double> prop_zgnd(prop.surfaceImpedanceReal, prop.surfaceImpedanceImag);
	static double wd1, xd1, qk, aht, xht, toh, toho, roh, roho, dto, dto1, dtro, dro,
		dro2, drto, dtr, dhh1, dhh2, dtof, dto1f, drof, dro2f;
	double a, q, pk, rd, ds, dsl, th, wa, sf2, vv, kedr = 0.0, arp = 0.0,
		sdr = 0.0, pd = 0.0, srp = 0.0, kem = 0.0, csd = 0.0, sdl = 0.0, adiffv2 = 0.0, closs = 0.0;
	//static double dhec;
	//double dfdh, ar, wd, sf1, ec;

	//sf1 = 1.0;	//| average empirical hilltop foliage scatter factor for 1 obstruction
	sf2 = 1.0;		//| average empirical hilltop foliage scatter factor for 2 obstructions

	//dfdh = prop.interdecileElevationRangeBetweenPoints;
	//ec = 0.5*prop.relativeCurvature;

	//| CalculateDiffractionAttenuationV2 must first be run with d == 0.0 to set up coefficients
	if (d == 0)
	{
		q = prop.antennaHeightAboveGroundLevel[0] * prop.antennaHeightAboveGroundLevel[1];
		qk = prop.antennaEffectiveHeight[0] * prop.antennaEffectiveHeight[1] - q;
		//dhec = 2.73;

		if (prop.analysisMode < 0)
		{
			q += 10.0;
		}

		//| coefficients for a standard four radii, rounded earth computation are prepared
		wd1 = sqrt(1.0 + qk / q);
		xd1 = propa.dla + propa.tha / prop.relativeCurvature;
		q = (1.0 - 0.8*exp(-propa.dlsa / 50e3)) * prop.interdecileElevationRangeBetweenPoints;
		q *= 0.78 * exp(-pow(q / 16.0, 0.25));
		qk = 1.0 / abs(prop_zgnd);
		aht = 20.0;
		xht = 0.0;
		a = 0.5*(prop.horizonDistance[0] * prop.horizonDistance[0]) / prop.antennaEffectiveHeight[0];
		wa = pow(a*prop.waveNumber, ONE_THIRD);
		pk = qk / wa;
		q = (1.607 - pk) * 151.0 * wa * prop.horizonDistance[0] / a;
		xht = q;
		aht += HeightGainOverSmoothSphere(q, pk);

		if ((int(prop.horizonDistance[1]) == 0.0) || (prop.grazingAngle[1] > 0.2))
		{
			xht += xht;
			aht += (aht - 20.0);
		}
		else
		{
			a = 0.5*(prop.horizonDistance[1] * prop.horizonDistance[1]) / prop.antennaEffectiveHeight[1];
			wa = pow(a*prop.waveNumber, ONE_THIRD);
			pk = qk / wa;
			q = (1.607 - pk) * 151.0 * wa * prop.horizonDistance[1] / a;
			xht += q;
			aht += HeightGainOverSmoothSphere(q, pk);
		}
		adiffv2 = 0.0;
	}
	else
	{
		th = propa.tha + d * prop.relativeCurvature;

		dsl = MaximumDouble(d - propa.dla, 0.0);
		ds = d - propa.dla;
		a = ds / th;
		wa = pow(a*prop.waveNumber, ONE_THIRD);
		pk = qk / wa;
		toh = prop.hht - (prop.antennaHeightAndPathElevation[0] - prop.horizonDistance[0] * ((prop.antennaHeightAndPathElevation[1] - prop.antennaHeightAndPathElevation[0]) / prop.distance));
		roh = prop.hhr - (prop.antennaHeightAndPathElevation[0] - (prop.distance - prop.horizonDistance[1])*((prop.antennaHeightAndPathElevation[1] - prop.antennaHeightAndPathElevation[0]) / prop.distance));
		toho = prop.hht - (prop.antennaHeightAndPathElevation[0] - (prop.horizonDistance[0] + dsl)*((prop.hhr - prop.antennaHeightAndPathElevation[0]) / (prop.distance - prop.horizonDistance[1])));
		roho = prop.hhr - (prop.hht - dsl * ((prop.antennaHeightAndPathElevation[1] - prop.hht) / dsl));
		dto = sqrt(prop.horizonDistance[0] * prop.horizonDistance[0] + toh * toh);
		dto += prop.relativeCurvature * prop.horizonDistance[0];
		dto1 = sqrt(prop.horizonDistance[0] * prop.horizonDistance[0] + toho * toho);
		dto1 += prop.relativeCurvature * prop.horizonDistance[0];
		dtro = sqrt((prop.horizonDistance[0] + dsl)*(prop.horizonDistance[0] + dsl) + prop.hhr*prop.hhr);
		dtro += prop.relativeCurvature * (prop.horizonDistance[0] + dsl);
		drto = sqrt((prop.horizonDistance[1] + dsl)*(prop.horizonDistance[1] + dsl) + prop.hht*prop.hht);
		drto += prop.relativeCurvature * (prop.horizonDistance[1] + dsl);
		dro = sqrt(prop.horizonDistance[1] * prop.horizonDistance[1] + roh * roh);
		dro += prop.relativeCurvature * (prop.horizonDistance[1]);
		dro2 = sqrt(prop.horizonDistance[1] * prop.horizonDistance[1] + roho * roho);
		dro2 += prop.relativeCurvature * (prop.horizonDistance[1]);
		dtr = sqrt(prop.distance*prop.distance + (prop.antennaHeightAndPathElevation[0] - prop.antennaHeightAndPathElevation[1])*(prop.antennaHeightAndPathElevation[0] - prop.antennaHeightAndPathElevation[1]));
		dtr += prop.relativeCurvature * prop.distance;
		dhh1 = sqrt((prop.distance - propa.dla)*(prop.distance - propa.dla) + toho * toho);
		dhh1 += prop.relativeCurvature * (prop.distance - propa.dla);
		dhh2 = sqrt((prop.distance - propa.dla)*(prop.distance - propa.dla) + roho * roho);
		dhh2 += prop.relativeCurvature * (prop.distance - propa.dla);

		//| for 1 obst tree base path
		dtof = sqrt(prop.horizonDistance[0] * prop.horizonDistance[0] + (toh - prop.canopyClutterHeight)*(toh - prop.canopyClutterHeight));
		dtof += prop.relativeCurvature * prop.horizonDistance[0];
		dto1f = sqrt(prop.horizonDistance[0] * prop.horizonDistance[0] + (toho - prop.canopyClutterHeight)*(toho - prop.canopyClutterHeight));
		dto1f += prop.relativeCurvature * prop.horizonDistance[0];
		drof = sqrt(prop.horizonDistance[1] * prop.horizonDistance[1] + (roh - prop.canopyClutterHeight)*(roh - prop.canopyClutterHeight));
		drof += prop.relativeCurvature * prop.horizonDistance[1];
		dro2f = sqrt(prop.horizonDistance[1] * prop.horizonDistance[1] + (roho - prop.canopyClutterHeight)*(roho - prop.canopyClutterHeight));
		dro2f += prop.relativeCurvature * prop.horizonDistance[1];

		//| CalculateClutterLoss coefficients preset for post-obstacle receive path
		prop.transmitterGroundHeight = prop.canopyClutterHeight + 1.0;
		prop.tsgh = prop.hhr;
		rd = prop.horizonDistance[1];

		//| two obstacle diffraction calculation
		if (int(ds) > 0)	//| there are 2 obstacles
		{
			if (int(prop.horizonDistance[1]) > 0.0)	//| receive site past 2nd peak
			{
				//| rounding attenuation
				q = (1.607 - pk)*151.0*wa*th + xht;
				//ar = 0.05751*q - 10*log10(q) - aht;

				//| knife edge vs round weighting
				q = (1.0 - 0.8*exp(-d / 50e3))*prop.interdecileElevationRangeBetweenPoints;
				q = (wd1 + xd1 / d)*MinimumDouble((q * prop.waveNumber), 6283.2);
				//wd = 25.1/(25.1+sqrt(q));

				q = 0.6365 * prop.waveNumber;

				if (prop.grazingAngle[1] < 0.2)  //| receive grazing angle below 0.2 rad
				{
					//| knife edge attenuation for two obstructions

					if (prop.hht < 3400)	//| if below tree line, foliage top loss
					{
						vv = q * abs(dto1 + dhh1 - dtro);
						adiffv2 = -18.0 + sf2 * AttenuationFromSingleKnifeEdge(vv);
					}
					else
					{
						vv = q * abs(dto1 + dhh1 - dtro);
						adiffv2 = AttenuationFromSingleKnifeEdge(vv);
					}

					if (prop.hhr < 3400)
					{
						vv = q * abs(dro2 + dhh2 - drto);
						adiffv2 += (-18.0 + sf2 * AttenuationFromSingleKnifeEdge(vv));
					}
					else
					{
						vv = q * abs(dro2 + dhh2 - drto);
						adiffv2 += AttenuationFromSingleKnifeEdge(vv);
					}

					//| finally, add clutter loss
					closs = CalculateClutterLoss(rd, prop, propa);
					adiffv2 += MinimumDouble(22.0, closs);
				}
				else	//| rcvr site too close to 2nd obs
				{
					//| knife edge attenuation for 1st obs

					if (prop.hht < 3400)
					{
						vv = q * abs(dto1 + dhh1 - dtro);
						adiffv2 = -18.0 + sf2 * AttenuationFromSingleKnifeEdge(vv);
					}
					else
					{
						vv = q * abs(dto1 + dhh1 - dtro);
						adiffv2 = AttenuationFromSingleKnifeEdge(vv);
					}

					//| weighted calc. of knife vs rounded edge
					//adiffv2 = ar*wd + (1.0 - wd)*adiffv2;

					//| clutter path loss past 2nd peak
					if (prop.grazingAngle[1] < 1.22)
					{
						rd = prop.horizonDistance[1];

						if (prop.grazingAngle[1] > 0.6)	//| through foliage downhill
						{
							prop.transmitterGroundHeight = prop.canopyClutterHeight;
						}
						else	//| close to foliage, rcvr in foliage downslope
						{
							vv = 0.6365 * prop.waveNumber * abs(dro2 + dhh2 - drto);
						}
						adiffv2 += AttenuationFromSingleKnifeEdge(vv);
						closs = CalculateClutterLoss(rd, prop, propa);
						adiffv2 += MinimumDouble(closs, 22.0);
					}
					else	//| rcvr very close to bare cliff or skyscraper
					{
						adiffv2 = 5.8 + 25.0;
					}
				}
			}
			else	//| receive site is atop a 2nd peak
			{
				vv = 0.6365 * prop.waveNumber * abs(dto + dro - dtr);
				adiffv2 = 5.8 + AttenuationFromSingleKnifeEdge(vv);
			}
		}
		else	//| for single obstacle
		{
			if (int(prop.horizonDistance[1]) > 0.0)	//| receive site past 1st peak
			{
				if (prop.grazingAngle[1] < 0.2)	//| receive grazing angle less than 0.2 radians
				{
					vv = 0.6365 * prop.waveNumber * abs(dto + dro - dtr);

					if (prop.hht < 3400)
					{
						sdl = 18.0;
						sdl = pow(10, (-sdl / 20));
						//| ke phase difference with respect to direct t-r line
						kedr = 0.159155 * prop.waveNumber * abs(dto + dro - dtr);
						arp = abs(kedr - (int(kedr)));
						kem = AttenuationFromSingleKnifeEdge(vv);
						kem = pow(10, (-kem / 20));
						//| scatter path phase with respect to direct t-r line
						sdr = 0.5 + 0.159155*prop.waveNumber*abs(dtof + drof - dtr);
						srp = abs(sdr - (int(sdr)));
						//| difference between scatter and ke phase in radians
						pd = 6.283185307*abs(srp - arp);
						//| report pd prior to restriction
						//| keep pd between 0 and pi radians and adjust for 3&4 quadrant
						if (pd >= 3.141592654)
						{
							pd = 6.283185307 - pd;
							csd = AbsoluteValueOfComplexNumber(complex<double>(sdl, 0) + complex<double>(kem*-cos(pd), kem*-sin(pd)));
						}
						else
						{
							csd = AbsoluteValueOfComplexNumber(complex<double>(sdl, 0) + complex<double>(kem*cos(pd), kem*sin(pd)));
						}
						//csd = mymax(csd, 0.0009);	//| limits maximum loss value to 30.45 db
						adiffv2 = -3.71 - 10 * log10(csd);
					}
					else
					{
						adiffv2 = AttenuationFromSingleKnifeEdge(vv);
					}
					//| finally, add clutter loss
					closs = CalculateClutterLoss(rd, prop, propa);
					adiffv2 += MinimumDouble(closs, 22.0);
				}
				else	//| receive grazing angle too high
				{
					if (prop.grazingAngle[1] < 1.22)
					{
						rd = prop.horizonDistance[1];

						if (prop.grazingAngle[1] > 0.6)	//| through foliage downhill
						{
							prop.transmitterGroundHeight = prop.canopyClutterHeight;
						}
						else	//| downhill slope just above foliage
						{
							vv = 0.6365 * prop.waveNumber * abs(dto + dro - dtr);
							adiffv2 = AttenuationFromSingleKnifeEdge(vv);
						}
						closs = CalculateClutterLoss(rd, prop, propa);
						adiffv2 += MinimumDouble(22.0, closs);
					}
					else	//| receiver very close to bare cliff or skyscraper
					{
						adiffv2 = 5.8 + 25.0;
					}
				}
			}
			else	//| if occurs, receive site atop first peak
			{
				adiffv2 = 5.8;
			}
		}
	}
	return adiffv2;
}


//| ------------------------------
//| 
//| FUNCTION: CalculateScatterAttenuation
//| 
//| OLD NAME: ascat
//| 
//| NOTES: 
//|   The function finds the "scatter attenuation"
//|   at the distance d. It uses an approximation to the
//|   methods of NBS TN101 with checks for inadmissible
//|   situations. For proper operation, the larger distance
//|   d = d6 must be the first called.  
//|   A call with d=0 sets up initial constants.
//| 
//| ------------------------------
double 
CalculateScatterAttenuation
   (double d, 
	prop_type &prop, 
	propa_type &propa)
{
	static double ad, rr, etq, h0s;
	double h0, r1, r2, z0, ss, et, ett, th, q;
	double ascatv, temp;

	if (d == 0.0)
	{
		ad = prop.horizonDistance[0] - prop.horizonDistance[1];
		rr = prop.antennaEffectiveHeight[1] / prop.antennaHeightAndPathElevation[0];

		if (ad < 0.0)
		{
			ad = -ad;
			rr = 1.0 / rr;
		}

		etq = (5.67e-6*prop.surfaceRefractivity - 2.32e-3)*prop.surfaceRefractivity + 0.031;
		h0s = -15.0;
		ascatv = 0.0;
	}
	else
	{
		if (h0s > 15.0)
		{
			h0 = h0s;
		}
		else
		{
			th = prop.grazingAngle[0] + prop.grazingAngle[1] + d * prop.relativeCurvature;
			r2 = 2.0 * prop.waveNumber * th;
			r1 = r2 * prop.antennaEffectiveHeight[0];
			r2 *= prop.antennaEffectiveHeight[1];

			if ((r1 < 0.2) && (r2 < 0.2))
			{
				return 1001.0;
			}

			ss = (d - ad) / (d + ad);
			q = rr / ss;
			ss = MaximumDouble(0.1, ss);
			q = MinimumDouble(MaximumDouble(0.1, q), 10.0);
			z0 = (d - ad)*(d + ad)*th*0.25 / d;
			//et = (etq*exp(-pow(mymin(1.7, z0/8.0e3),6.0)) + 1.0) * z0/1.7556e3;

			temp = MinimumDouble(1.7, z0 / 8.0e3);
			temp = temp * temp*temp*temp*temp*temp;
			et = (etq*exp(-temp) + 1.0)*z0 / 1.7556e3;

			ett = MaximumDouble(et, 1.0);
			h0 = (H01ScatterFields(r1, ett) + H01ScatterFields(r2, ett))*0.5;
			h0 += MinimumDouble(h0, (1.38 - log(ett))*log(ss)*log(q)*0.49);
			h0 = PositiveDifferenceOrZero(h0, 0.0);

			if (et < 1.0)
			{
				//h0 = et*h0 + (1.0 - et)*4.343*log(pow((1.0 + 1.4142/r1)*(1.0 + 1.4142/r2), 2.0)*(r1+r2)/(r1+r2+2.8284));
				temp = ((1.0 + 1.4142 / r1)*(1.0 + 1.4142 / r2));
				h0 = et * h0 + (1.0 - et)*4.343*log((temp*temp)*(r1 + r2) / (r1 + r2 + 2.8284));
			}

			if ((h0 > 15.0) && (h0s >= 0.0))
			{
				h0 = h0s;
			}
		}

		h0s = h0;
		th = propa.tha + d * prop.relativeCurvature;
		//ascatv=FThetaDScatterFields(th*d)+4.343*log(47.7*prop.waveNumber*pow(th,4.0))-0.1*(prop.surfaceRefractivity-301.0)*exp(-th*d/40e3)+h0;
		ascatv = FThetaDScatterFields(th*d) + 4.343*log(47.7*prop.waveNumber*(th*th*th*th)) - 0.1*(prop.surfaceRefractivity - 301.0)*exp(-th * d / 40e3) + h0;
	}

	return ascatv;
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
StandardNormalTailDistributionFunctionInverse
   (double q)
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
	t = sqrt(-2.0*log(t));
	v = t - ((c2*t + c1)*t + c0) / (((d3*t + d2)*t + d1)*t + 1.0);

	if (x < 0.0)
	{
		v = -v;
	}

	return v;
}


//| ------------------------------
//| 
//| FUNCTION: Qlrps
//| 
//| OLD NAME: qlrps
//| 
//| NOTES: 
//|   The routine converts fmhz, eno, zsys, eps, sgm to waveNumber, surfaceRefractivity, relativeCurvature, zgnd.
//|   Not sure of a better name to give it yet.
//|   Inputs:
//|     fmhz - frequency (mhz)
//|     eno - surface refractivity reduced to sea level
//|     zsys - general system elevation
//|     ipol - polarity
//|     eps - dielectric constant
//|     sgm - ground conductivity
//|   Outputs:
//|     waveNumber - wave number
//|     surfaceRefractivity - surface refractivity
//|     relativeCurvature - relative earth curvature
//|     zgnd - surface impedance
//| 
//| ------------------------------
void 
Qlrps
   (double fmhz, 
	double zsys, 
	double en0, 
	int ipol, 
	double eps, 
	double sgm, 
	prop_type &prop)
{
	double gma = 157e-9;

	prop.waveNumber = fmhz / 47.7;
	prop.surfaceRefractivity = en0;

	if (zsys != 0.0)
	{
		prop.surfaceRefractivity *= exp(-zsys / 9460.0);
	}

	prop.relativeCurvature = gma * (1.0 - 0.04665*exp(prop.surfaceRefractivity / 179.3));
	complex<double> zq, prop_zgnd(prop.surfaceImpedanceReal, prop.surfaceImpedanceImag);
	zq = complex<double>(eps, 376.62 * sgm / prop.waveNumber);
	prop_zgnd = sqrt(zq - 1.0);

	if (ipol != 0.0)
	{
		prop_zgnd = prop_zgnd / zq;
	}

	prop.surfaceImpedanceReal = prop_zgnd.real();
	prop.surfaceImpedanceImag = prop_zgnd.imag();
}


//| ------------------------------
//| 
//| FUNCTION: CalculateLineOfSightAttenuation
//| 
//| OLD NAME: alos
//| 
//| NOTES: 
//|   The function finds the "line-of-sight attenuation"
//|   at the distance d. It uses a convex combination of plane earth
//|   fields and diffracted fields. A call with d = 0 sets up initial constants.
//| 
//| ------------------------------
double 
CalculateLineOfSightAttenuation
   (double d, 
	prop_type &prop, 
	propa_type &propa)
{
	complex<double> prop_zgnd(prop.surfaceImpedanceReal, prop.surfaceImpedanceImag);
	static double wls;
	complex<double> r;
	double s, sps, q;
	double alosv;

	if (d == 0.0)
	{
		wls = 0.021 / (0.021 + prop.waveNumber*prop.interdecileElevationRangeBetweenPoints / MaximumDouble(10e3, propa.dlsa));
		alosv = 0.0;
	}
	else
	{
		q = (1.0 - 0.8*exp(-d / 50e3)) * prop.interdecileElevationRangeBetweenPoints;
		s = 0.78*q*exp(-pow(q / 16.0, 0.25));
		q = prop.antennaEffectiveHeight[0] + prop.antennaEffectiveHeight[1];
		sps = q / sqrt(d*d + q * q);
		r = (sps - prop_zgnd) / (sps + prop_zgnd)*exp(-MinimumDouble(10.0, prop.waveNumber*s*sps));
		q = AbsoluteValueOfComplexNumber(r);

		if ((q < 0.25) || (q < sps))
		{
			r = r * sqrt(sps / q);
		}

		alosv = propa.emd*d + propa.aed;
		q = prop.waveNumber*prop.antennaEffectiveHeight[0] * prop.antennaEffectiveHeight[1] * 2.0 / d;

		if (q > 1.57)
		{
			q = 3.14 - 2.4649 / q;
		}

		alosv = (-4.343*log(AbsoluteValueOfComplexNumber(complex<double>(cos(q), -sin(q)) + r)) - alosv)*wls + alosv;
	}

	return alosv;
}


//| ------------------------------
//| 
//| FUNCTION: CalculateLineOfSightAttenuationV2
//| 
//| OLD NAME: alos2
//| 
//| NOTES: 
//| 
//| ------------------------------
double 
CalculateLineOfSightAttenuationV2
   (double d, 
	prop_type &prop, 
	propa_type &propa)
{
	complex<double> prop_zgnd(prop.surfaceImpedanceReal, prop.surfaceImpedanceImag);
	complex<double> r;
	double clutterDensity, cr, dr, hr, hrg, ht, htg, hrp, re, s, sps, q, pd, drh;
	//int rp;
	double alosv;

	clutterDensity = 0.0;
	cr = 0.0;
	htg = prop.antennaHeightAboveGroundLevel[0];
	hrg = prop.antennaHeightAboveGroundLevel[1];
	ht = prop.transmitterTotalHeight;
	hr = prop.receiverTotalHeight;
	//rp = prop.horizonHeightIndex;
	hrp = prop.horizonHeight;
	pd = prop.distance;

	if (d == 0.0)
	{
		alosv = 0.0;
	}
	else
	{
		q = prop.antennaEffectiveHeight[0] + prop.antennaEffectiveHeight[1];
		sps = q / sqrt(pd*pd + q * q);
		q = (1.0 - 0.8*exp(-pd / 50e3)) * prop.interdecileElevationRangeBetweenPoints;

		if (prop.analysisMode < 0)
		{
			dr = pd / (1 + hrg / htg);

			if (dr < (0.5 * pd))
			{
				drh = 6378137.0 - sqrt(-(0.5*pd)*(0.5*pd) + 6378137.0*6378137.0 + (0.5*pd - dr)*(0.5*pd - dr));
			}
			else
			{
				drh = 6378137.0 - sqrt(-(0.5*pd)*(0.5*pd) + 6378137.0*6378137.0 + (dr - 0.5*pd)*(dr - 0.5*pd));
			}

			if ((sps < 0.05) && (prop.canopyClutterHeight > hrg) && (prop.distance < prop.horizonDistance[0]))	//| if far from transmitter and receiver below canopy
			{
				clutterDensity = MaximumDouble(0.01, pd*(prop.canopyClutterHeight - hrg) / (htg - hrg));
				cr = MaximumDouble(0.01, pd - dr + dr * (prop.canopyClutterHeight - drh) / htg);
				q = ((1.0 - 0.8*exp(-pd / 50e3)) * prop.interdecileElevationRangeBetweenPoints * (MinimumDouble(-20 * log10(clutterDensity / cr), 1.0)));
			}
		}

		s = 0.78*q*exp(-pow(q / 16.0, 0.25));
		q = exp(-MinimumDouble(10.0, prop.waveNumber*s*sps));
		r = q * (sps - prop_zgnd) / (sps + prop_zgnd);
		q = AbsoluteValueOfComplexNumber(r);
		q = MinimumDouble(q, 1.0);

		if ((q < 0.25) || (q < sps))
		{
			r = r * sqrt(sps / q);
		}
		q = prop.waveNumber * prop.antennaEffectiveHeight[0] * prop.antennaEffectiveHeight[1] / (pd * 3.1415926535897);

		if (prop.analysisMode < 0)
		{
			q = prop.waveNumber * ((ht - hrp)*(hr - hrp)) / (pd * 3.1415926535897);
		}
		q -= floor(q);

		if (q < 0.5)
		{
			q *= 3.1415926535897;
		}
		else
		{
			q = (1 - q)*3.1415926535897;
		}

		//| no longer valid complex conjugate removed
		//| by removing minus sign from in front of sin function
		re = AbsoluteValueOfComplexNumber(complex<double>(cos(q), sin(q)) + r);
		alosv = -10 * log10(re);
		prop.transmitterGroundHeight = prop.antennaHeightAboveGroundLevel[0];					//| tx above gnd hgt set to antenna height AGL
		prop.tsgh = prop.antennaHeightAndPathElevation[0] - prop.antennaHeightAboveGroundLevel[0];	//| tsgh set to tx site gl AMSL

		if ((prop.antennaHeightAboveGroundLevel[1] < prop.canopyClutterHeight) && (prop.thera < 0.785) && (prop.thenr < 0.785))
		{
			if (sps < 0.05)
			{
				alosv = alosv + CalculateClutterLoss(pd, prop, propa);
			}
			else
			{
				alosv = CalculateClutterLoss(pd, prop, propa);
			}
		}
	}
	alosv = MinimumDouble(22.0, alosv);
	return alosv;
}


//| ------------------------------
//| 
//| FUNCTION: Qlra
//| 
//| OLD NAME: qlra
//| 
//| NOTES: 
//| 
//| ------------------------------
void 
Qlra
   (int kst[], 
	int klimx, 
	int mdvarx, 
	prop_type &prop, 
	propv_type &propv)
{
	double q;

	for (int j = 0; j < 2; ++j)
	{
		if (kst[j] <= 0)
		{
			prop.antennaEffectiveHeight[j] = prop.antennaHeightAboveGroundLevel[j];
		}
		else
		{
			q = 4.0;

			if (kst[j] != 1)
			{
				q = 9.0;
			}

			if (prop.antennaHeightAboveGroundLevel[j] < 5.0)
			{
				q *= sin(0.3141593*prop.antennaHeightAboveGroundLevel[j]);
			}

			prop.antennaEffectiveHeight[j] = prop.antennaHeightAboveGroundLevel[j] + (1.0 + q)*exp(-MinimumDouble(20.0, 2.0*prop.antennaHeightAboveGroundLevel[j] / MaximumDouble(1e-3, prop.interdecileElevationRangeBetweenPoints)));
		}

		q = sqrt(2.0*prop.antennaEffectiveHeight[j] / prop.relativeCurvature);
		prop.horizonDistance[j] = q * exp(-0.07*sqrt(prop.interdecileElevationRangeBetweenPoints / MaximumDouble(prop.antennaEffectiveHeight[j], 5.0)));
		prop.grazingAngle[j] = (0.65 * prop.interdecileElevationRangeBetweenPoints * (q / prop.horizonDistance[j] - 1.0) - 2.0*prop.antennaEffectiveHeight[j]) / q;
	}

	prop.analysisMode = 1;
	propv.recomputeParameters = MaximumInteger(propv.recomputeParameters, 3);

	if (mdvarx >= 0)
	{
		propv.variabilityMode = mdvarx;
		propv.recomputeParameters = MaximumInteger(propv.recomputeParameters, 4);
	}

	if (klimx > 0)
	{
		propv.radioClimate = klimx;
		propv.recomputeParameters = 5;
	}
}


//| ------------------------------
//| 
//| FUNCTION: CalculateLongleyRicePropagation
//| 
//| OLD NAME: lrprop
//| 
//| NOTES: 
//|   Longley Rice Propagation Program
//|   The value of analysisMode controls some of the program flow. When it equals -1 we are in the point-to point
//|   mode, when 1 we are beginning the area mode, and when 0 we are continuing the area mode. The 
//|   assumption is that when one uses the area mode, one will want a sequence of results for varying
//|   distances.
//| 
//| ------------------------------
void 
CalculateLongleyRicePropagation
   (double d, 
	prop_type &prop, 
	propa_type &propa)
{
	//| PaulM_lrprop used for ITM
	static bool wlos, wscat;
	static double dmin, xae;
	complex<double> prop_zgnd(prop.surfaceImpedanceReal, prop.surfaceImpedanceImag);
	double a0, a1, a2, a3, a4, a5, a6;
	double d0, d1, d2, d3, d4, d5, d6;
	bool wq;
	double q;
	int j;

	if (prop.analysisMode != 0)
	{
		for (j = 0; j < 2; j++)
		{
			propa.dls[j] = sqrt(2.0*prop.antennaEffectiveHeight[j] / prop.relativeCurvature);
		}

		propa.dlsa = propa.dls[0] + propa.dls[1];
		propa.dla = prop.horizonDistance[0] + prop.horizonDistance[1];
		propa.tha = MaximumDouble(prop.grazingAngle[0] + prop.grazingAngle[1], -propa.dla * prop.relativeCurvature);
		wlos = false;
		wscat = false;

		if ((prop.waveNumber < 0.838) || (prop.waveNumber > 210.0))
		{
			prop.errorCode = MaximumInteger(prop.errorCode, 1);
		}

		for (j = 0; j < 2; j++)
		{
			if ((prop.antennaHeightAboveGroundLevel[j] < 1.0) || (prop.antennaHeightAboveGroundLevel[j] > 1000.0))
			{
				prop.errorCode = MaximumInteger(prop.errorCode, 1);
			}
		}

		for (j = 0; j < 2; j++)
		{
			if ((abs(prop.grazingAngle[j]) > 200e-3) || (prop.horizonDistance[j] < (0.1 * propa.dls[j])) || (prop.horizonDistance[j] > (3.0 * propa.dls[j])))
			{
				prop.errorCode = MaximumInteger(prop.errorCode, 3);
			}
		}

		if ((prop.surfaceRefractivity < 250.0) || (prop.surfaceRefractivity > 400.0) || (prop.relativeCurvature < 75e-9) || (prop.relativeCurvature > 250e-9) || (prop_zgnd.real() <= abs(prop_zgnd.imag())) || (prop.waveNumber < 0.419) || (prop.waveNumber > 420.0))
		{
			prop.errorCode = 4;
		}

		for (j = 0; j < 2; j++)
		{
			if ((prop.antennaHeightAboveGroundLevel[j] < 0.5) || (prop.antennaHeightAboveGroundLevel[j] > 3000.0))
			{
				prop.errorCode = 4;
			}
		}

		dmin = abs(prop.antennaEffectiveHeight[0] - prop.antennaEffectiveHeight[1]) / 200e-3;
		q = CalculateDiffractionAttenuation(0.0, prop, propa);
		//xae = pow(prop.waveNumber * pow(prop.relativeCurvature, 2.), -ONE_THIRD);	//| JDM made argument 2 a double
		xae = pow(prop.waveNumber * (prop.relativeCurvature * prop.relativeCurvature), -ONE_THIRD);		//| No 2nd pow()
		d3 = MaximumDouble(propa.dlsa, 1.3787*xae + propa.dla);
		d4 = d3 + 2.7574*xae;
		a3 = CalculateDiffractionAttenuation(d3, prop, propa);
		a4 = CalculateDiffractionAttenuation(d4, prop, propa);
		propa.emd = (a4 - a3) / (d4 - d3);
		propa.aed = a3 - propa.emd*d3;
	}

	if (prop.analysisMode >= 0)
	{
		prop.analysisMode = 0;
		prop.distance = d;
	}

	if (prop.distance > 0.0)
	{
		if (prop.distance > 1000e3)
		{
			prop.errorCode = MaximumInteger(prop.errorCode, 1);
		}

		if (prop.distance < dmin)
		{
			prop.errorCode = MaximumInteger(prop.errorCode, 3);
		}

		if ((prop.distance < 1e3) || (prop.distance > 2000e3))
		{
			prop.errorCode = 4;
		}
	}

	if (prop.distance < propa.dlsa)
	{
		if (!wlos)
		{
			q = CalculateLineOfSightAttenuation(0.0, prop, propa);
			d2 = propa.dlsa;
			a2 = propa.aed + d2 * propa.emd;
			d0 = 1.908*prop.waveNumber*prop.antennaEffectiveHeight[0] * prop.antennaEffectiveHeight[1];

			if (propa.aed >= 0.0)
			{
				d0 = MinimumDouble(d0, 0.5*propa.dla);
				d1 = d0 + 0.25*(propa.dla - d0);
			}
			else
			{
				d1 = MaximumDouble(-propa.aed / propa.emd, 0.25*propa.dla);
			}

			a1 = CalculateLineOfSightAttenuation(d1, prop, propa);
			wq = false;

			if (d0 < d1)
			{
				a0 = CalculateLineOfSightAttenuation(d0, prop, propa);
				q = log(d2 / d0);
				propa.ak2 = MaximumDouble(0.0, ((d2 - d0)*(a1 - a0) - (d1 - d0)*(a2 - a0)) / ((d2 - d0)*log(d1 / d0) - (d1 - d0)*q));
				wq = propa.aed >= 0.0 || propa.ak2>0.0;

				if (wq)
				{
					propa.ak1 = (a2 - a0 - propa.ak2*q) / (d2 - d0);

					if (propa.ak1 < 0.0)
					{
						propa.ak1 = 0.0;
						propa.ak2 = PositiveDifferenceOrZero(a2, a0) / q;

						if (propa.ak2 == 0.0)
						{
							propa.ak1 = propa.emd;
						}
					}
				}
				else
				{
					propa.ak2 = 0.0;
					propa.ak1 = (a2 - a1) / (d2 - d1);

					if (propa.ak1 <= 0.0)
					{
						propa.ak1 = propa.emd;
					}
				}
			}
			else
			{
				propa.ak1 = (a2 - a1) / (d2 - d1);
				propa.ak2 = 0.0;

				if (propa.ak1 <= 0.0)
				{
					propa.ak1 = propa.emd;
				}
			}

			propa.ael = a2 - propa.ak1*d2 - propa.ak2*log(d2);
			wlos = true;
		}

		if (prop.distance > 0.0)
		{
			prop.referenceAttenuation = propa.ael + propa.ak1*prop.distance + propa.ak2*log(prop.distance);
		}
	}

	if ((prop.distance <= 0.0) || (prop.distance >= propa.dlsa))
	{
		if (!wscat)
		{
			q = CalculateScatterAttenuation(0.0, prop, propa);
			d5 = propa.dla + 200e3;
			d6 = d5 + 200e3;
			a6 = CalculateScatterAttenuation(d6, prop, propa);
			a5 = CalculateScatterAttenuation(d5, prop, propa);

			if (a5 < 1000.0)
			{
				propa.ems = (a6 - a5) / 200e3;
				propa.dx = MaximumDouble(propa.dlsa, MaximumDouble(propa.dla + 0.3*xae*log(47.7*prop.waveNumber), (a5 - propa.aed - propa.ems*d5) / (propa.emd - propa.ems)));
				propa.aes = (propa.emd - propa.ems)*propa.dx + propa.aed;
			}
			else
			{
				propa.ems = propa.emd;
				propa.aes = propa.aed;
				propa.dx = 10.e6;
			}

			wscat = true;
		}

		if (prop.distance > propa.dx)
		{
			prop.referenceAttenuation = propa.aes + propa.ems*prop.distance;
		}
		else
		{
			prop.referenceAttenuation = propa.aed + propa.emd*prop.distance;
		}
	}

	prop.referenceAttenuation = MaximumDouble(prop.referenceAttenuation, 0.0);
}


//| ------------------------------
//| 
//| FUNCTION: CalculateLongleyRicePropagationV2
//| 
//| OLD NAME: lrprop2
//| 
//| NOTES: 
//| 
//| ------------------------------
void 
CalculateLongleyRicePropagationV2
   (double d, 
	prop_type &prop, 
	propa_type &propa)
{
	//| ITWOM_lrprop2
	static bool wlos, wscat;
	static double dmin, xae;
	complex<double> prop_zgnd(prop.surfaceImpedanceReal, prop.surfaceImpedanceImag);
	double pd1;
	double a0, a1, a2, a3, a4, a5, a6, iw;
	double d0, d1, d2, d3, d4, d5, d6;
	bool wq;
	double q;
	int j;

	iw = prop.transmitterIntervalWidth;
	pd1 = prop.distance;
	propa.dx = 2000000.0;

	if (prop.analysisMode != 0)	//| if oper. mode is not 0, i.e. not area mode ongoing
	{
		for (j = 0; j < 2; j++)
		{
			propa.dls[j] = sqrt(2.0*prop.antennaEffectiveHeight[j] / prop.relativeCurvature);
		}

		propa.dlsa = propa.dls[0] + propa.dls[1];
		propa.dlsa = MinimumDouble(propa.dlsa, 1000000.0);
		propa.dla = prop.horizonDistance[0] + prop.horizonDistance[1];
		propa.tha = MaximumDouble(prop.grazingAngle[0] + prop.grazingAngle[1], -propa.dla * prop.relativeCurvature);
		wlos = false;
		wscat = false;

		//| checking for parameters-in-range, error codes set if not

		if ((prop.waveNumber < 0.838) || (prop.waveNumber > 210.0))
		{
			prop.errorCode = MaximumInteger(prop.errorCode, 1);
		}

		for (j = 0; j < 2; j++)
		{
			if ((prop.antennaHeightAboveGroundLevel[j] < 1.0) || (prop.antennaHeightAboveGroundLevel[j] > 1000.0))
			{
				prop.errorCode = MaximumInteger(prop.errorCode, 1);
			}
		}

		if (abs(prop.grazingAngle[0]) > 200e-3)
		{
			prop.errorCode = MaximumInteger(prop.errorCode, 3);
		}

		if (abs(prop.grazingAngle[1]) > 1.220)
		{
			prop.errorCode = MaximumInteger(prop.errorCode, 3);
		}

		//for (j = 0; j < 2; j++)
		//{
		//	if ((prop.horizonDistance[j] < (0.1 * propa.dls[j])) || (prop.horizonDistance[j] > (3.0 * propa.dls[j])))
		//	{
		//		prop.errorCode = mymax(prop.errorCode, 3);
		//	}
		//}

		if ((prop.surfaceRefractivity < 250.0) || (prop.surfaceRefractivity > 400.0) || (prop.relativeCurvature < 75e-9) || (prop.relativeCurvature > 250e-9) || (prop_zgnd.real() <= abs(prop_zgnd.imag())) || (prop.waveNumber < 0.419) || (prop.waveNumber > 420.0))
		{
			prop.errorCode = 4;
		}

		for (j = 0; j < 2; j++)
		{
			if ((prop.antennaHeightAboveGroundLevel[j] < 0.5) || (prop.antennaHeightAboveGroundLevel[j] > 3000.0))
			{
				prop.errorCode = 4;
			}
		}

		dmin = abs(prop.antennaEffectiveHeight[0] - prop.antennaEffectiveHeight[1]) / 200e-3;
		q = CalculateDiffractionAttenuationV2(0.0, prop, propa);
		xae = pow(prop.waveNumber * (prop.relativeCurvature * prop.relativeCurvature), -ONE_THIRD);
		d3 = MaximumDouble(propa.dlsa, 1.3787*xae + propa.dla);
		d4 = d3 + 2.7574*xae;
		a3 = CalculateDiffractionAttenuationV2(d3, prop, propa);
		a4 = CalculateDiffractionAttenuationV2(d4, prop, propa);
		propa.emd = (a4 - a3) / (d4 - d3);
		propa.aed = a3 - propa.emd*d3;
	}

	if (prop.analysisMode >= 0)	//| if initializing the area mode
	{
		prop.analysisMode = 0;	//| area mode is initialized
		prop.distance = d;
	}

	if (prop.distance > 0.0)
	{
		if (prop.distance > 1000e3)	//| prop.dist being in meters, if greater than 1000 km, errorCode = 1
		{
			prop.errorCode = MaximumInteger(prop.errorCode, 1);
		}

		if (prop.distance < dmin)
		{
			prop.errorCode = MaximumInteger(prop.errorCode, 3);
		}

		if ((prop.distance < 1e3) || (prop.distance > 2000e3))
		{
			prop.errorCode = 4;
		}
	}

	if (prop.distance < propa.dlsa)
	{
		if (iw <= 0.0)	//| if interval width is zero or less, used for area mode
		{
			if (!wlos)
			{
				q = CalculateLineOfSightAttenuationV2(0.0, prop, propa);
				d2 = propa.dlsa;
				a2 = propa.aed + d2 * propa.emd;
				d0 = 1.908 * prop.waveNumber * prop.antennaEffectiveHeight[0] * prop.antennaEffectiveHeight[1];

				if (propa.aed > 0.0)
				{
					prop.referenceAttenuation = propa.aed + propa.emd*prop.distance;
				}
				else
				{
					if (propa.aed == 0.0)
					{
						d0 = MinimumDouble(d0, 0.5*propa.dla);
						d1 = d0 + 0.25*(propa.dla - d0);
					}
					else
					{
						d1 = MaximumDouble(-propa.aed / propa.emd, 0.25*propa.dla);
					}
					a1 = CalculateLineOfSightAttenuationV2(d1, prop, propa);
					wq = false;

					if (d0 < d1)
					{
						a0 = CalculateLineOfSightAttenuationV2(d0, prop, propa);
						a2 = MinimumDouble(a2, CalculateLineOfSightAttenuationV2(d2, prop, propa));
						q = log(d2 / d0);
						propa.ak2 = MaximumDouble(0.0, ((d2 - d0)*(a1 - a0) - (d1 - d0)*(a2 - a0)) / ((d2 - d0)*log(d1 / d0) - (d1 - d0)*q));
						wq = propa.aed >= 0.0 || propa.ak2 > 0.0;

						if (wq)
						{
							propa.ak1 = (a2 - a0 - propa.ak2*q) / (d2 - d0);

							if (propa.ak1 < 0.0)
							{
								propa.ak1 = 0.0;
								propa.ak2 = PositiveDifferenceOrZero(a2, a0) / q;

								if (propa.ak2 == 0.0)
								{
									propa.ak1 = propa.emd;
								}
							}
						}
					}

					if (!wq)
					{
						propa.ak1 = PositiveDifferenceOrZero(a2, a1) / (d2 - d1);
						propa.ak2 = 0.0;

						if (propa.ak1 == 0.0)
						{
							propa.ak1 = propa.emd;
						}
					}
					propa.ael = a2 - propa.ak1*d2 - propa.ak2*log(d2);
					wlos = true;
				}
			}
		}
		else	//| for ITWOM point-to-point mode
		{
			if (!wlos)
			{
				q = CalculateLineOfSightAttenuationV2(0.0, prop, propa);	//| coefficient setup
				wlos = true;
			}

			if (prop.lineOfSight == 1)	//| if line of sight
			{
				prop.referenceAttenuation = CalculateLineOfSightAttenuationV2(pd1, prop, propa);
			}
			else
			{
				if (int(prop.distance - prop.horizonDistance[0]) == 0)	//| if at 1st horiz
				{
					prop.referenceAttenuation = 5.8 + CalculateLineOfSightAttenuationV2(pd1, prop, propa);
				}
				else if (int(prop.distance - prop.horizonDistance[0]) > 0.0)	//| if past 1st horiz
				{
					q = CalculateDiffractionAttenuationV2(0.0, prop, propa);
					prop.referenceAttenuation = CalculateDiffractionAttenuationV2(pd1, prop, propa);
				}
				else
				{
					prop.referenceAttenuation = 1.0;
				}
			}
		}
	}

	//| lineOfSight and diff. range coefficents done. Starting troposcatter
	if ((prop.distance <= 0.0) || (prop.distance >= propa.dlsa))
	{
		if (iw == 0.0)	//| area mode
		{
			if (!wscat)
			{
				q = CalculateScatterAttenuation(0.0, prop, propa);
				d5 = propa.dla + 200e3;
				d6 = d5 + 200e3;
				a6 = CalculateScatterAttenuation(d6, prop, propa);
				a5 = CalculateScatterAttenuation(d5, prop, propa);

				if (a5 < 1000.0)
				{
					propa.ems = (a6 - a5) / 200e3;
					propa.dx = MaximumDouble(propa.dlsa, MaximumDouble(propa.dla + 0.3*xae*log(47.7*prop.waveNumber), (a5 - propa.aed - propa.ems*d5) / (propa.emd - propa.ems)));
					propa.aes = (propa.emd - propa.ems)*propa.dx + propa.aed;
				}
				else
				{
					propa.ems = propa.emd;
					propa.aes = propa.aed;
					propa.dx = 10000000;
				}
				wscat = true;
			}

			if (prop.distance > propa.dx)
			{
				prop.referenceAttenuation = propa.aes + propa.ems*prop.distance;
			}
			else
			{
				prop.referenceAttenuation = propa.aed + propa.emd*prop.distance;
			}
		}
		else	//| ITWOM mode q used to preset coefficients with zero input
		{
			if (!wscat)
			{
				d5 = 0.0;
				d6 = 0.0;
				q = CalculateScatterAttenuation(0.0, prop, propa);
				a6 = CalculateScatterAttenuation(pd1, prop, propa);
				q = CalculateDiffractionAttenuationV2(0.0, prop, propa);
				a5 = CalculateDiffractionAttenuationV2(pd1, prop, propa);

				if (a5 <= a6)
				{
					propa.dx = 10000000;
					prop.referenceAttenuation = a5;
				}
				else
				{
					propa.dx = propa.dlsa;
					prop.referenceAttenuation = a6;
				}
				wscat = true;
			}
		}
	}
	prop.referenceAttenuation = MaximumDouble(prop.referenceAttenuation, 0.0);
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
Curve
   (double const &c1, 
	double const &c2, 
	double const &x1,
	double const &x2, 
	double const &x3, 
	double const &de)
{
	//return (c1+c2/(1.0+pow((de-x2)/x3,2.0)))*pow(de/x1,2.0)/(1.0+pow(de/x1,2.0));
	double temp1, temp2;

	temp1 = (de - x2) / x3;
	temp2 = de / x1;

	temp1 *= temp1;
	temp2 *= temp2;

	return (c1 + c2 / (1.0 + temp1))*temp2 / (1.0 + temp2);
}


//| ------------------------------
//| 
//| FUNCTION: CalculateLongleyRiceVariability
//| 
//| OLD NAME: avar
//| 
//| NOTES: 
//|   When in the area prediction mode, one needs a threefold quantile for attenuation which corresponds to the
//|   fraction qt of time, the fraction ql of locations, and the fractin qs of "situations". In the point-to-point
//|   mode, one needs only qt and qs. For efficiency, CalculateLongleyRiceVariability is written as a function of the "standard normal deviates"
//|   zt, zl, and zs corresponding to the requested fractions. Thus for example, qt = Q(zt) where Q(z) is the
//|   "complementary standard normal distribution". For the point-to-point mode one sets zl = 0 which
//|   corresponds to the medial ql = 0.50
//|
//|   The subprogram is written trying to reduce duplicate calculations. This is done through the switch recomputeParameters.
//|   On first entering set recomputeParameters = 5. Then all parametes will be initialized, and recomputeParameters will be changed to 0. If
//|   the program is to be used to find several quantiles with different values of zt, zl, or zs, then recomputeParameters should
//|   be recomputed. If antenna heights are changed, set recomputeParameters = 2; if the frequency, recomputeParameters =3; if the mode of
//|   variability variabilityMode, set recomputeParameters = 5; and finally if the climate is changed set recomputeParameters = 5. The higher the value of
//|   recomputeParameters, the more parameters will be recomputed.
//| 
//| ------------------------------
double 
CalculateLongleyRiceVariability
   (double zzt, 
	double zzl, 
	double zzc, 
	prop_type &prop, 
	propv_type &propv)
{
	static int kdv;
	static double dexa, de, vmd, vs0, sgl, sgtm, sgtp, sgtd, tgtd,
		gm, gp, cv1, cv2, yv1, yv2, yv3, csm1, csm2, ysm1, ysm2,
		ysm3, csp1, csp2, ysp1, ysp2, ysp3, csd1, zd, cfm1, cfm2,
		cfm3, cfp1, cfp2, cfp3;

	double bv1[7] = { -9.67,-0.62,1.26,-9.21,-0.62,-0.39,3.15 };
	double bv2[7] = { 12.7,9.19,15.5,9.05,9.19,2.86,857.9 };
	double xv1[7] = { 144.9e3,228.9e3,262.6e3,84.1e3,228.9e3,141.7e3,2222.e3 };
	double xv2[7] = { 190.3e3,205.2e3,185.2e3,101.1e3,205.2e3,315.9e3,164.8e3 };
	double xv3[7] = { 133.8e3,143.6e3,99.8e3,98.6e3,143.6e3,167.4e3,116.3e3 };
	double bsm1[7] = { 2.13,2.66,6.11,1.98,2.68,6.86,8.51 };
	double bsm2[7] = { 159.5,7.67,6.65,13.11,7.16,10.38,169.8 };
	double xsm1[7] = { 762.2e3,100.4e3,138.2e3,139.1e3,93.7e3,187.8e3,609.8e3 };
	double xsm2[7] = { 123.6e3,172.5e3,242.2e3,132.7e3,186.8e3,169.6e3,119.9e3 };
	double xsm3[7] = { 94.5e3,136.4e3,178.6e3,193.5e3,133.5e3,108.9e3,106.6e3 };
	double bsp1[7] = { 2.11,6.87,10.08,3.68,4.75,8.58,8.43 };
	double bsp2[7] = { 102.3,15.53,9.60,159.3,8.12,13.97,8.19 };
	double xsp1[7] = { 636.9e3,138.7e3,165.3e3,464.4e3,93.2e3,216.0e3,136.2e3 };
	double xsp2[7] = { 134.8e3,143.7e3,225.7e3,93.1e3,135.9e3,152.0e3,188.5e3 };
	double xsp3[7] = { 95.6e3,98.6e3,129.7e3,94.2e3,113.4e3,122.7e3,122.9e3 };
	double bsd1[7] = { 1.224,0.801,1.380,1.000,1.224,1.518,1.518 };
	double bzd1[7] = { 1.282,2.161,1.282,20.,1.282,1.282,1.282 };
	double bfm1[7] = { 1.0,1.0,1.0,1.0,0.92,1.0,1.0 };
	double bfm2[7] = { 0.0,0.0,0.0,0.0,0.25,0.0,0.0 };
	double bfm3[7] = { 0.0,0.0,0.0,0.0,1.77,0.0,0.0 };
	double bfp1[7] = { 1.0,0.93,1.0,0.93,0.93,1.0,1.0 };
	double bfp2[7] = { 0.0,0.31,0.0,0.19,0.31,0.0,0.0 };
	double bfp3[7] = { 0.0,2.00,0.0,1.79,2.00,0.0,0.0 };
	static bool ws, w1;
	double rt = 7.8, rl = 24.0, avarv, q, vs, zt, zl, zc;
	double sgt, yr, temp1, temp2;
	int temp_klim = propv.radioClimate - 1;

	if (propv.recomputeParameters > 0)
	{
		switch (propv.recomputeParameters)
		{
		default:
			if ((propv.radioClimate <= 0) || (propv.radioClimate > 7))
			{
				propv.radioClimate = 5;
				temp_klim = 4;
				prop.errorCode = MaximumInteger(prop.errorCode, 2);
			}

			cv1 = bv1[temp_klim];
			cv2 = bv2[temp_klim];
			yv1 = xv1[temp_klim];
			yv2 = xv2[temp_klim];
			yv3 = xv3[temp_klim];
			csm1 = bsm1[temp_klim];
			csm2 = bsm2[temp_klim];
			ysm1 = xsm1[temp_klim];
			ysm2 = xsm2[temp_klim];
			ysm3 = xsm3[temp_klim];
			csp1 = bsp1[temp_klim];
			csp2 = bsp2[temp_klim];
			ysp1 = xsp1[temp_klim];
			ysp2 = xsp2[temp_klim];
			ysp3 = xsp3[temp_klim];
			csd1 = bsd1[temp_klim];
			zd = bzd1[temp_klim];
			cfm1 = bfm1[temp_klim];
			cfm2 = bfm2[temp_klim];
			cfm3 = bfm3[temp_klim];
			cfp1 = bfp1[temp_klim];
			cfp2 = bfp2[temp_klim];
			cfp3 = bfp3[temp_klim];

		case 4:
			kdv = propv.variabilityMode;
			ws = kdv >= 20;

			if (ws)
			{
				kdv -= 20;
			}

			w1 = kdv >= 10;

			if (w1)
			{
				kdv -= 10;
			}

			if ((kdv < 0) || (kdv > 3))
			{
				kdv = 0;
				prop.errorCode = MaximumInteger(prop.errorCode, 2);
			}

		case 3:
			q = log(0.133*prop.waveNumber);

			//gm = cfm1 + cfm2/(pow(cfm3*q, 2.0)+1.0);
			//gp = cfp1 + cfp2/(pow(cfp3*q, 2.0)+1.0);

			gm = cfm1 + cfm2 / ((cfm3*q*cfm3*q) + 1.0);
			gp = cfp1 + cfp2 / ((cfp3*q*cfp3*q) + 1.0);

		case 2:
			dexa = sqrt(18e6 * prop.antennaEffectiveHeight[0]) + sqrt(18e6 * prop.antennaEffectiveHeight[1]) + pow((575.7e12 / prop.waveNumber), ONE_THIRD);

		case 1:
			if (prop.distance < dexa)
			{
				de = 130e3*prop.distance / dexa;
			}
			else
			{
				de = 130e3 + prop.distance - dexa;
			}
		}

		vmd = Curve(cv1, cv2, yv1, yv2, yv3, de);
		sgtm = Curve(csm1, csm2, ysm1, ysm2, ysm3, de)*gm;
		sgtp = Curve(csp1, csp2, ysp1, ysp2, ysp3, de)*gp;
		sgtd = sgtp * csd1;
		tgtd = (sgtp - sgtd)*zd;

		if (w1)
		{
			sgl = 0.0;
		}
		else
		{
			q = (1.0 - 0.8*exp(-prop.distance / 50e3)) * prop.interdecileElevationRangeBetweenPoints * prop.waveNumber;
			sgl = 10.0*q / (q + 13.0);
		}

		if (ws)
		{
			vs0 = 0.0;
		}
		else
		{
			//vs0 = pow(5.0+3.0*exp(-de/100e3), 2.0);
			temp1 = (5.0 + 3.0*exp(-de / 100e3));
			vs0 = temp1 * temp1;

		}

		propv.recomputeParameters = 0;
	}

	zt = zzt;
	zl = zzl;
	zc = zzc;

	switch (kdv)
	{
	case 0:
		zt = zc;
		zl = zc;
		break;

	case 1:
		zl = zc;
		break;

	case 2:
		zl = zt;
	}

	if ((fabs(zt) > 3.1) || (fabs(zl) > 3.1) || (fabs(zc) > 3.1))
	{
		prop.errorCode = MaximumInteger(prop.errorCode, 1);
	}

	if (zt < 0.0)
	{
		sgt = sgtm;
	}
	else if (zt <= zd)
	{
		sgt = sgtp;
	}
	else
	{
		sgt = sgtd + tgtd / zt;
	}

	//vs = vs0 + pow(sgt*zt,2.0)/(rt+zc*zc) + pow(sgl*zl,2.0)/(rl+zc*zc);

	temp1 = sgt * zt;
	temp2 = sgl * zl;

	vs = vs0 + (temp1*temp1) / (rt + zc * zc) + (temp2*temp2) / (rl + zc * zc);

	if (kdv == 0)
	{
		yr = 0.0;
		propv.sgc = sqrt(sgt*sgt + sgl * sgl + vs);
	}
	else if (kdv == 1)
	{
		yr = sgt * zt;
		propv.sgc = sqrt(sgl*sgl + vs);
	}
	else if (kdv == 2)
	{
		yr = sqrt(sgt*sgt + sgl * sgl)*zt;
		propv.sgc = sqrt(vs);
	}
	else
	{
		yr = sgt * zt + sgl * zl;
		propv.sgc = sqrt(vs);
	}

	avarv = prop.referenceAttenuation - vmd - yr - propv.sgc*zc;

	if (avarv < 0.0)
	{
		avarv = avarv * (29.0 - avarv) / (29.0 - 10.0*avarv);
	}

	return avarv;
}


//| ------------------------------
//| 
//| FUNCTION: CalculateHorizonDistances
//| 
//| OLD NAME: hzns
//| 
//| NOTES: 
//|   Here we use the terrain profile pfl to find the two horizons. Output consists of the horizon distances
//|   horizonDistance and the horizon take-off angles. If the path is line-of-sight, the routine sets both horizon
//|   distances equal to distance.
//| 
//| ------------------------------
void 
CalculateHorizonDistances
   (double pfl[], 
	prop_type &prop)
{
	//| Used only with ITM 1.2.2
	bool wq;
	int np;
	double xi, za, zb, qc, q, sb, sa;

	np = (int)pfl[0];
	xi = pfl[1];
	za = pfl[2] + prop.antennaHeightAboveGroundLevel[0];
	zb = pfl[np + 2] + prop.antennaHeightAboveGroundLevel[1];
	qc = 0.5 * prop.relativeCurvature;
	q = qc * prop.distance;
	prop.grazingAngle[1] = (zb - za) / prop.distance;
	prop.grazingAngle[0] = prop.grazingAngle[1] - q;
	prop.grazingAngle[1] = -prop.grazingAngle[1] - q;
	prop.horizonDistance[0] = prop.distance;
	prop.horizonDistance[1] = prop.distance;

	if (np >= 2)
	{
		sa = 0.0;
		sb = prop.distance;
		wq = true;

		for (int i = 1; i < np; i++)
		{
			sa += xi;
			sb -= xi;
			q = pfl[i + 2] - (qc*sa + prop.grazingAngle[0])*sa - za;

			if (q > 0.0)
			{
				prop.grazingAngle[0] += q / sa;
				prop.horizonDistance[0] = sa;
				wq = false;
			}

			if (!wq)
			{
				q = pfl[i + 2] - (qc*sb + prop.grazingAngle[1])*sb - zb;

				if (q > 0.0)
				{
					prop.grazingAngle[1] += q / sb;
					prop.horizonDistance[1] = sb;
				}
			}
		}
	}
}


//| ------------------------------
//| 
//| FUNCTION: CalculateHorizonDistancesV2
//| 
//| OLD NAME: hzns2
//| 
//| NOTES: 
//| 
//| ------------------------------
void 
CalculateHorizonDistancesV2
   (double pfl[], 
	prop_type &prop, 
	propa_type &propa)
{
	bool wq;
	int np, rp, i, j;
	double xi, za, zb, qc, q, sb, sa, dr, dshh;

	np = (int)pfl[0];
	xi = pfl[1];
	za = pfl[2] + prop.antennaHeightAboveGroundLevel[0];
	zb = pfl[np + 2] + prop.antennaHeightAboveGroundLevel[1];
	prop.transmitterIntervalWidth = xi;
	prop.transmitterTotalHeight = za;
	prop.receiverTotalHeight = zb;
	qc = 0.5 * prop.relativeCurvature;
	q = qc * prop.distance;
	prop.grazingAngle[1] = atan((zb - za) / prop.distance);
	prop.grazingAngle[0] = prop.grazingAngle[1] - q;
	prop.grazingAngle[1] = -prop.grazingAngle[1] - q;
	prop.horizonDistance[0] = prop.distance;
	prop.horizonDistance[1] = prop.distance;
	prop.hht = 0.0;
	prop.hhr = 0.0;
	prop.lineOfSight = 1;

	if (np >= 2)
	{
		sa = 0.0;
		sb = prop.distance;
		wq = true;

		for (j = 1; j < np; j++)
		{
			sa += xi;
			q = pfl[j + 2] - (qc*sa + prop.grazingAngle[0])*sa - za;

			if (q > 0.0)
			{
				prop.lineOfSight = 0;
				prop.grazingAngle[0] += q / sa;
				prop.horizonDistance[0] = sa;
				prop.grazingAngle[0] = MinimumDouble(prop.grazingAngle[0], 1.569);
				prop.hht = pfl[j + 2];
				wq = false;
			}
		}

		if (!wq)
		{
			for (i = 1; i < np; i++)
			{
				sb -= xi;
				q = pfl[np + 2 - i] - (qc*(prop.distance - sb) + prop.grazingAngle[1])*(prop.distance - sb) - zb;
				if (q > 0.0)
				{
					prop.grazingAngle[1] += q / (prop.distance - sb);
					prop.grazingAngle[1] = MinimumDouble(prop.grazingAngle[1], 1.57);
					prop.grazingAngle[1] = MaximumDouble(prop.grazingAngle[1], -1.568);
					prop.hhr = pfl[np + 2 - i];
					prop.horizonDistance[1] = MaximumDouble(0.0, prop.distance - sb);
				}
			}
			prop.grazingAngle[0] = atan((prop.hht - za) / prop.horizonDistance[0]) - 0.5 * prop.relativeCurvature * prop.horizonDistance[0];
			prop.grazingAngle[1] = atan((prop.hhr - zb) / prop.horizonDistance[1]) - 0.5 * prop.relativeCurvature * prop.horizonDistance[1];
		}
	}

	if (prop.horizonDistance[1] < prop.distance)
	{
		dshh = prop.distance - prop.horizonDistance[0] - prop.horizonDistance[1];

		if (int(dshh) == 0)	//| one obstacle
		{
			dr = prop.horizonDistance[1] / (1 + zb / prop.hht);
		}
		else	//| two obstacles
		{
			dr = prop.horizonDistance[1] / (1 + zb / prop.hhr);
		}
	}
	else	//| line of sight
	{
		dr = (prop.distance) / (1 + zb / za);
	}
	rp = 2 + (int)(floor(0.5 + dr / xi));
	prop.horizonHeightIndex = rp;
	prop.horizonHeight = pfl[rp];
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
CalculateLinearLeastSquaresFit
   (double z[], 
	const double &x1, 
	const double &x2, 
	double &z0, 
	double &zn)
{
	//| Used only with ITM 1.2.2
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
	x = -0.5*xa;
	xb += x;
	a = 0.5*(z[ja + 2] + z[jb + 2]);
	b = 0.5*(z[ja + 2] - z[jb + 2])*x;

	for (int i = 2; i <= n; ++i)
	{
		++ja;
		x += 1.0;
		a += z[ja + 2];
		b += z[ja + 2] * x;
	}

	a /= xa;
	b = b * 12.0 / ((xa*xa + 2.0)*xa);
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
CalculateLinearLeastSquaresFitV2
   (double z[], 
	const double &x1, 
	const double &x2, 
	double &z0, 
	double &zn)
{
	//| corrected for use with ITWOM
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
	x = -0.5*(xa + 1);
	xb += x;
	ja = jb - 1 - (int)xa;
	n = jb - ja;
	a = (z[ja + 2] + z[jb + 2]);
	b = (z[ja + 2] - z[jb + 2])*x;
	bn = 2 * (x*x);

	for (int i = 2; i <= n; ++i)
	{
		++ja;
		x += 1.0;
		bn += (x*x);
		a += z[ja + 2];
		b += z[ja + 2] * x;
	}

	a /= (xa + 2);
	b = b / bn;
	z0 = a - (b*xb);
	zn = a + (b*(xn - xb));
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
CreateQuantile
   (const int &nn, 
	double a[], 
	const int &ir)
{
	double q = 0.0, r;	//| q initialization -- KD2BD
	int m, n, i, j, j1 = 0, i0 = 0, k;	//| more initializations -- KD2BD
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
StandardNormalTailDistributionFunction
   (const double &z)
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
		qerfv = exp(-0.5*x*x)*rrt2pi*((((b5*t + b4)*t + b3)*t + b2)*t + b1)*t;
	}

	if (x < 0.0)
	{
		qerfv = 1.0 - qerfv;
	}

	return qerfv;
}


//| ------------------------------
//| 
//| FUNCTION: CalculateTerrainInterdecileRangeDelta
//| 
//| OLD NAME: d1thx
//| 
//| NOTES: 
//|   Using the terrain profile pfl we find delta h, the interdecile range of elevations between the two points
//|   x1 and x2
//|   Changes suggested by IEEE Broadcast Technology Society Newsletter, Vol. 17, Number 3, Fall 2009
//| 
//| ------------------------------
double 
CalculateTerrainInterdecileRangeDelta
   (double pfl[], 
	const double &x1, 
	const double &x2)
{
	int np, ka, kb, n, k, j;
	double d1thxv, sn, xa, xb;
	double *s;

	np = (int)pfl[0];
	xa = x1 / pfl[1];
	xb = x2 / pfl[1];
	d1thxv = 0.0;

	if ((xb - xa) < 2.0)
	{
		return d1thxv;
	}

	ka = (int)(0.1*(xb - xa + 8.0));
	ka = MinimumInteger(MaximumInteger(4, ka), 25);
	n = 10 * ka - 5;
	kb = n - ka + 1;
	sn = n - 1;
	assert((s = new double[n + 2]) != 0);
	s[0] = sn;
	s[1] = 1.0;
	xb = (xb - xa) / sn;
	k = (int)(xa + 1.0);
	xa -= (double)k;

	for (j = 0; j < n; j++)
	{
		while ((xa > 0.0) && (k < np))
		{
			xa -= 1.0;
			++k;
		}

		s[j + 2] = pfl[k + 2] + (pfl[k + 2] - pfl[k + 1])*xa;
		xa = xa + xb;
	}

	CalculateLinearLeastSquaresFit(s, 0.0, sn, xa, xb);
	xb = (xb - xa) / sn;

	for (j = 0; j < n; j++)
	{
		s[j + 2] -= xa;
		xa = xa + xb;
	}

	d1thxv = CreateQuantile(n - 1, s + 2, ka - 1) - CreateQuantile(n - 1, s + 2, kb - 1);
	d1thxv /= 1.0 - 0.8*exp(-(x2 - x1) / 50.0e3);

	delete[] s;
	s = NULL;

	return d1thxv;
}


//| ------------------------------
//| 
//| FUNCTION: CalculateTerrainInterdecileRangeDeltaV2
//| 
//| OLD NAME: d1thx2
//| 
//| NOTES: 
//| 
//| ------------------------------
double 
CalculateTerrainInterdecileRangeDeltaV2
   (double pfl[], 
	const double &x1, 
	const double &x2, 
	propa_type &propa)
{
	int np, ka, kb, n, k, kmx, j;
	double d1thx2v, sn, xa, xb, xc;
	double *s;

	np = (int)pfl[0];
	xa = x1 / pfl[1];
	xb = x2 / pfl[1];
	d1thx2v = 0.0;

	if ((xb - xa) < 2.0)
	{
		return d1thx2v;
	}

	ka = (int)(0.1*(xb - xa + 8.0));
	kmx = MaximumInteger(25, (int)(83350 / (pfl[1])));
	ka = MinimumInteger(MaximumInteger(4, ka), kmx);
	n = 10 * ka - 5;
	kb = n - ka + 1;
	sn = n - 1;
	assert((s = new double[n + 2]) != 0);
	s[0] = sn;
	s[1] = 1.0;
	xb = (xb - xa) / sn;
	k = (int(xa + 1.0));
	xc = xa - (double(k));

	for (j = 0; j<n; j++)
	{
		while ((xc > 0.0) && (k < np))
		{
			xc -= 1.0;
			++k;
		}

		s[j + 2] = pfl[k + 2] + (pfl[k + 2] - pfl[k + 1])*xc;
		xc = xc + xb;
	}

	CalculateLinearLeastSquaresFitV2(s, 0.0, sn, xa, xb);
	xb = (xb - xa) / sn;

	for (j = 0; j < n; j++)
	{
		s[j + 2] -= xa;
		xa = xa + xb;
	}

	d1thx2v = CreateQuantile(n - 1, s + 2, ka - 1) - CreateQuantile(n - 1, s + 2, kb - 1);
	d1thx2v /= 1.0 - 0.8*exp(-(x2 - x1) / 50.0e3);

	delete[] s;
	s = NULL;

	return d1thx2v;
}


//| ------------------------------
//| 
//| FUNCTION: PrepareForPointToPointModeAnalysis
//| 
//| OLD NAME: qlrpfl
//| 
//| NOTES: 
//|   This subroutine is used to prepare for the point-to-point mode. Since the path is fixed, it has
//|   only one value of referenceAttenuation and therefore at the end of the routine there is a call 
//|   to CalculateLongleyRicePropagation. To complete the
//|   process one needs to call CalculateLongleyRiceVariability for whatever quantiles are desired.
//|
//|   This mode requires the terrain profile lying between the termals. This should be a sequence of surface
//|   elevations at points along the great circle path joining the two points. It should start at the ground beneath
//|   the first terminal and end at the ground beneath the second. In the present routine it is assumed that the
//|   elevations are equispaced along the path. They are stored in the array pfl along with two defining parameters.
//|     pfl[0] = enp, the number of increments in the path
//|     pfl[1] = xi, the length of each increment
//|     pfl[2] = z[0], the beginning elevation
//|     pfl[np+3] = z[np] the last elevation
//| 
//| ------------------------------
void 
PrepareForPointToPointModeAnalysis
   (double pfl[], 
	int klimx, 
	int mdvarx, 
	prop_type &prop, 
	propa_type &propa, 
	propv_type &propv)
{
	int np, j;
	double xl[2], q, za, zb, temp;

	prop.distance = pfl[0] * pfl[1];
	np = (int)pfl[0];
	CalculateHorizonDistances(pfl, prop);

	for (j = 0; j < 2; j++)
	{
		xl[j] = MinimumDouble(15.0 * prop.antennaHeightAboveGroundLevel[j], 0.1 * prop.horizonDistance[j]);
	}

	xl[1] = prop.distance - xl[1];
	prop.interdecileElevationRangeBetweenPoints = CalculateTerrainInterdecileRangeDelta(pfl, xl[0], xl[1]);

	if ((prop.horizonDistance[0] + prop.horizonDistance[1]) > (1.5 * prop.distance))
	{
		CalculateLinearLeastSquaresFit(pfl, xl[0], xl[1], za, zb);
		prop.antennaEffectiveHeight[0] = prop.antennaHeightAboveGroundLevel[0] + PositiveDifferenceOrZero(pfl[2], za);
		prop.antennaEffectiveHeight[1] = prop.antennaHeightAboveGroundLevel[1] + PositiveDifferenceOrZero(pfl[np + 2], zb);

		for (j = 0; j < 2; j++)
		{
			prop.horizonDistance[j] = sqrt(2.0*prop.antennaEffectiveHeight[j] / prop.relativeCurvature)*exp(-0.07*sqrt(prop.interdecileElevationRangeBetweenPoints / MaximumDouble(prop.antennaEffectiveHeight[j], 5.0)));
		}

		q = prop.horizonDistance[0] + prop.horizonDistance[1];

		if (q <= prop.distance)	//| if there is a rounded horizon, or two obstructions, in the path
		{
			//q = pow(prop.distance/q, 2.0);
			temp = prop.distance / q;
			q = temp * temp;

			for (j = 0; j < 2; j++)
			{
				prop.antennaEffectiveHeight[j] *= q;	//| tx effective height set to be path distance between obstacles
				prop.horizonDistance[j] = sqrt(2.0*prop.antennaEffectiveHeight[j] / prop.relativeCurvature)*exp(-0.07*sqrt(prop.interdecileElevationRangeBetweenPoints / MaximumDouble(prop.antennaEffectiveHeight[j], 5.0)));
			}
		}

		for (j = 0; j < 2; j++)	//| original empirical adjustment? uses delta-h to adjust grazing angles
		{
			q = sqrt(2.0*prop.antennaEffectiveHeight[j] / prop.relativeCurvature);
			prop.grazingAngle[j] = (0.65 * prop.interdecileElevationRangeBetweenPoints * (q / prop.horizonDistance[j] - 1.0) - 2.0*prop.antennaEffectiveHeight[j]) / q;
		}
	}
	else
	{
		CalculateLinearLeastSquaresFit(pfl, xl[0], 0.9*prop.horizonDistance[0], za, q);
		CalculateLinearLeastSquaresFit(pfl, prop.distance - 0.9*prop.horizonDistance[1], xl[1], q, zb);
		prop.antennaEffectiveHeight[0] = prop.antennaHeightAboveGroundLevel[0] + PositiveDifferenceOrZero(pfl[2], za);
		prop.antennaEffectiveHeight[1] = prop.antennaHeightAboveGroundLevel[1] + PositiveDifferenceOrZero(pfl[np + 2], zb);
	}

	prop.analysisMode = -1;
	propv.recomputeParameters = MaximumInteger(propv.recomputeParameters, 3);

	if (mdvarx >= 0)
	{
		propv.variabilityMode = mdvarx;
		propv.recomputeParameters = MaximumInteger(propv.recomputeParameters, 4);
	}

	if (klimx > 0)
	{
		propv.radioClimate = klimx;
		propv.recomputeParameters = 5;
	}

	CalculateLongleyRicePropagation(0.0, prop, propa);
}


//| ------------------------------
//| 
//| FUNCTION: PrepareForPointToPointModeAnalysisV2
//| 
//| OLD NAME: qlrpfl2
//| 
//| NOTES: 
//| 
//| ------------------------------
void 
PrepareForPointToPointModeAnalysisV2
   (double pfl[], 
	int klimx, 
	int mdvarx, 
	prop_type &prop, 
	propa_type &propa, 
	propv_type &propv)
{
	int np, j;
	double xl[2], dlb, q, za, zb, temp, rad, rae1, rae2;

	prop.distance = pfl[0] * pfl[1];
	np = (int)pfl[0];
	CalculateHorizonDistancesV2(pfl, prop, propa);
	dlb = prop.horizonDistance[0] + prop.horizonDistance[1];
	prop.antennaHeightAndPathElevation[0] = prop.antennaHeightAboveGroundLevel[0] + pfl[2];
	prop.antennaHeightAndPathElevation[1] = prop.antennaHeightAboveGroundLevel[1] + pfl[np + 2];

	for (j = 0; j < 2; j++)
	{
		xl[j] = MinimumDouble(15.0*prop.antennaHeightAboveGroundLevel[j], 0.1*prop.horizonDistance[j]);
	}

	xl[1] = prop.distance - xl[1];
	prop.interdecileElevationRangeBetweenPoints = CalculateTerrainInterdecileRangeDeltaV2(pfl, xl[0], xl[1], propa);

	if ((np < 1) || (pfl[1] > 150.0))
	{
		//| for TRANSHORIZON; diffraction over a mutual horizon, or for one or more obstructions
		if (dlb < (1.5 * prop.distance))
		{
			CalculateLinearLeastSquaresFitV2(pfl, xl[0], 0.9*prop.horizonDistance[0], za, q);
			CalculateLinearLeastSquaresFitV2(pfl, prop.distance - 0.9*prop.horizonDistance[1], xl[1], q, zb);
			prop.antennaEffectiveHeight[0] = prop.antennaHeightAboveGroundLevel[0] + PositiveDifferenceOrZero(pfl[2], za);
			prop.antennaEffectiveHeight[1] = prop.antennaHeightAboveGroundLevel[1] + PositiveDifferenceOrZero(pfl[np + 2], zb);
		}
		//| for a Line-of-Sight path
		else
		{
			CalculateLinearLeastSquaresFitV2(pfl, xl[0], xl[1], za, zb);
			prop.antennaEffectiveHeight[0] = prop.antennaHeightAboveGroundLevel[0] + PositiveDifferenceOrZero(pfl[2], za);
			prop.antennaEffectiveHeight[1] = prop.antennaHeightAboveGroundLevel[1] + PositiveDifferenceOrZero(pfl[np + 2], zb);

			for (j = 0; j < 2; j++)
			{
				prop.horizonDistance[j] = sqrt(2.0*prop.antennaEffectiveHeight[j] / prop.relativeCurvature)*exp(-0.07*sqrt(prop.interdecileElevationRangeBetweenPoints / MaximumDouble(prop.antennaEffectiveHeight[j], 5.0)));
			}

			//| for one or more obstructions only NOTE buried as in ITM FORTRAN and DLL, not functional
			if ((prop.horizonDistance[0] + prop.horizonDistance[1]) <= prop.distance)
			{
				//q = pow(prop.distance/(dl[0]+dl[1])), 2.0);
				temp = prop.distance / (prop.horizonDistance[0] + prop.horizonDistance[1]);
				q = temp * temp;
			}

			for (j = 0; j < 2; j++)
			{
				prop.antennaEffectiveHeight[j] *= q;
				prop.horizonDistance[j] = sqrt(2.0*prop.antennaEffectiveHeight[j] / prop.relativeCurvature)*exp(-0.07*sqrt(prop.interdecileElevationRangeBetweenPoints / MaximumDouble(prop.antennaEffectiveHeight[j], 5.0)));
			}

			//| this sets (or resets) prop.grazingAngle, and is not in The Guide FORTRAN QLRPFL
			for (j = 0; j < 2; j++)
			{
				q = sqrt(2.0*prop.antennaEffectiveHeight[j] / prop.relativeCurvature);
				prop.grazingAngle[j] = (0.65 * prop.interdecileElevationRangeBetweenPoints * (q / prop.horizonDistance[j] - 1.0) - 2.0*prop.antennaEffectiveHeight[j]) / q;
			}
		}
	}
	else	//| for ITWOM, computes antennaEffectiveHeight for tx, rcvr, and the receiver approach angles for use in CalculateClutterLoss
	{
		prop.antennaEffectiveHeight[0] = prop.antennaHeightAboveGroundLevel[0] + (pfl[2]);
		prop.antennaEffectiveHeight[1] = prop.antennaHeightAboveGroundLevel[1] + (pfl[np + 2]);

		rad = (prop.distance - 500.0);

		if (prop.distance > 550.0)
		{
			CalculateLinearLeastSquaresFitV2(pfl, rad, prop.distance, rae1, rae2);
		}
		else
		{
			rae1 = 0.0;
			rae2 = 0.0;
		}

		prop.thera = atan(abs(rae2 - rae1) / prop.distance);

		if (rae2 < rae1)
		{
			prop.thera = -prop.thera;
		}

		prop.thenr = atan(MaximumDouble(0.0, (pfl[np + 2] - pfl[np + 1])) / pfl[1]);
	}

	prop.analysisMode = -1;
	propv.recomputeParameters = MaximumInteger(propv.recomputeParameters, 3);

	if (mdvarx >= 0)
	{
		propv.variabilityMode = mdvarx;
		propv.recomputeParameters = MaximumInteger(propv.recomputeParameters, 4);
	}

	if (klimx > 0)
	{
		propv.radioClimate = klimx;
		propv.recomputeParameters = 5;
	}

	CalculateLongleyRicePropagationV2(0.0, prop, propa);
}


//| ------------------------------------------------------------
//| Point-To-Point Mode Calculations 
//|
//| This mode requires the terrain profile lying between the terminals. This should be a sequence of surface
//| elevations at points along the great circle path joining the two points. It should  start at the ground beneath
//| the first terminal and end at the ground under the second. In the present routine it is assumed that the 
//| equispaced along the path. They are stored in the array pfl along with two defining parameters.
//|   elev[0] = enp, the number of increments in the path
//|   elev[1] = xi, the length of each increment
//|   elev[2] = z[0], the beginning elevation
//|   elev[np+3] = z[np] the last elevation
//| ------------------------------------------------------------

//| ------------------------------
//| 
//| FUNCTION: PointToPointCalculationLegacy
//| 
//| OLD NAME: point_to_point_ITM
//| 
//| NOTES: 
//|   antennaPolarization:
//|     0-Horizontal, 1-Vertical
//|   radioClimate:
//|     1-Equatorial, 2-Continental Subtropical,
//|     3-Maritime Tropical, 4-Desert, 5-Continental Temperate,
//|     6-Maritime Temperate, Over Land, 7-Maritime Temperate, Over Sea
//|   fractionOfSituations: .01 to .99
//|   elev[]: [num points - 1], [delta distance(meters)],
//|     [height(meters) point 1], ..., [height(meters) point n]
//|   errnum: 0- No Error.
//|     1 - Warning: Some parameters are nearly out of range.
//|         Results should be used with caution.
//|     2 - Note: Default parameters have been substituted for
//|         impossible ones.
//|     3 - Warning: A combination of parameters is out of range.
//|         Results are probably invalid.
//|     Other - Warning: Some parameters are out of range.
//|             Results are probably invalid.
//| 
//| ------------------------------
void 
PointToPointCalculationLegacy
   (double elev[], 
	double tht_m, 
	double rht_m, 
	double dielectricRelativePermittivityConstant,
	double conductivitySiemensPerMeter,
	double atmosphericBendingConstant,
	double referenceFrequency,
	int radioClimate,
	int antennaPolarization,
	double fractionOfSituations,
	double fractionOfTime,
	double &dbloss, 
	char *strmode, 
	int strmodeLen, 
	int &errnum)
{
	prop_type   prop;
	propv_type  propv;
	propa_type  propa;
	double zsys = 0;
	double zc, zr;
	double eno, enso, q;
	long ja, jb, i, np;
	//double dkm, xkm;
	double fs;
	errno_t err;

	prop.antennaHeightAboveGroundLevel[0] = tht_m;
	prop.antennaHeightAboveGroundLevel[1] = rht_m;
	propv.radioClimate = radioClimate;
	prop.errorCode = 0;
	propv.recomputeParameters = 5;
	prop.analysisMode = -1;
	zc = StandardNormalTailDistributionFunctionInverse(fractionOfSituations);
	zr = StandardNormalTailDistributionFunctionInverse(fractionOfTime);
	np = (long)elev[0];
	//dkm = (elev[1]*elev[0])/1000.0;
	//xkm = elev[1]/1000.0;
	eno = atmosphericBendingConstant;
	enso = 0.0;
	q = enso;

	if (q <= 0.0)
	{
		ja = (long)(3.0 + 0.1*elev[0]);	//| added (long) to correct
		jb = np - ja + 6;

		for (i = ja - 1; i < jb; ++i)
		{
			zsys += elev[i];
		}

		zsys /= (jb - ja + 1);
		q = eno;
	}

	propv.variabilityMode = 12;
	Qlrps(referenceFrequency, zsys, q, antennaPolarization, dielectricRelativePermittivityConstant, conductivitySiemensPerMeter, prop);
	PrepareForPointToPointModeAnalysis(elev, propv.radioClimate, propv.variabilityMode, prop, propa, propv);
	fs = 32.45 + 20.0*log10(referenceFrequency) + 20.0*log10(prop.distance / 1000.0);
	q = prop.distance - propa.dla;

	if (int(q) < 0.0)
	{
		err = strcpy_s(strmode, strmodeLen, "Line-Of-Sight Mode");
	}
	else
	{
		if (int(q) == 0.0)
		{
			err = strcpy_s(strmode, strmodeLen, "Single Horizon");
		}
		else if (int(q) > 0.0)
		{
			err = strcpy_s(strmode, strmodeLen, "Double Horizon");
		}

		if ((prop.distance <= propa.dlsa) || (prop.distance <= propa.dx))
		{
			err = strcat_s(strmode, strmodeLen, ", Diffraction Dominant");
		}
		else if (prop.distance > propa.dx)
		{
			err = strcat_s(strmode, strmodeLen, ", Troposcatter Dominant");
		}
	}

	dbloss = CalculateLongleyRiceVariability(zr, 0.0, zc, prop, propv) + fs;
	errnum = prop.errorCode;
}


//| ------------------------------
//| 
//| FUNCTION: PointToPointCalculation
//| 
//| OLD NAME: point_to_point
//| 
//| NOTES: 
//|   The new variable inputs:
//|     double enc_ncc_clcref,
//|     double clutter_height,
//|     double clutter_density,
//|     double delta_h_diff, and
//|     int mode_var
//|   have been given fixed values below.
//|     antennaPolarization:
//|       0-Horizontal, 1-Vertical, 2-Circular
//|     radioClimate:
//|       1-Equatorial, 2-Continental Subtropical,
//|       3-Maritime Tropical, 4-Desert, 5-Continental Temperate,
//|       6-Maritime Temperate, Over Land, 7-Maritime Temperate,
//|     Over Sea
//|       fractionOfSituations: .01 to .99
//|     elev[]: [num points - 1], [delta distance(meters)],
//|       [height(meters) point 1], ..., [height(meters) point n]
//|     clutter_height  	25.2 meters for compatibility with ITU-R P.1546-2.
//|     clutter_density 	1.0 for compatibility with ITU-R P.1546-2.
//|     delta_h_diff		optional delta h for beyond line of sight. 90 m. average.
//|                         setting to 0.0 will default to use of original internal
//|                         use of delta-h for beyond line-of-sight range.
//|     mode_var		    set to 12; or to 1 for FCC ILLR;  see documentation
//|     enc_ncc_clcref 		clutter refractivity; 1000 N-units to match ITU-R P.1546-2
//|     eno=atmosphericBendingConstant	atmospheric refractivity at sea level; 301 N-units nominal
//|       (ranges from 250 for dry, hot day to 450 on hot, humid day]
//|       (stabilizes near 301 in cold, clear weather)
//|     errnum: 0- No Error.
//|       1 - Warning: Some parameters are nearly out of range.
//|           Results should be used with caution.
//|       2 - Note: Default parameters have been substituted for
//|           impossible ones.
//|       3 - Warning: A combination of parameters is out of range.
//|           Results are probably invalid.
//|       Other - Warning: Some parameters are out of range.
//|               Results are probably invalid.
//| 
//| ------------------------------
void 
PointToPointCalculation
   (double elev[], 
	double tht_m, 
	double rht_m, 
	double dielectricRelativePermittivityConstant,
	double conductivitySiemensPerMeter,
	double atmosphericBendingConstant,
	double referenceFrequency,
	int radioClimate,
	int antennaPolarization,
	double fractionOfSituations,
	double fractionOfTime,
	double &dbloss, 
	char *strmode, 
	int strmodeLen, 
	int &errnum)
{
	prop_type   prop;
	propv_type  propv;
	propa_type  propa;

	double zsys = 0;
	double zc, zr;
	double eno, enso, q;
	long ja, jb, i, np;
	//double dkm, xkm;
	double tpd, fs;
	errno_t err;

	prop.antennaHeightAboveGroundLevel[0] = tht_m;
	prop.antennaHeightAboveGroundLevel[1] = rht_m;
	propv.radioClimate = radioClimate;
	prop.errorCode = 0;
	propv.recomputeParameters = 5;
	prop.analysisMode = -1;
	prop.transmitterPolarization = antennaPolarization;
	prop.thera = 0.0;
	prop.thenr = 0.0;
	zc = StandardNormalTailDistributionFunctionInverse(fractionOfSituations);
	zr = StandardNormalTailDistributionFunctionInverse(fractionOfTime);
	np = (long)elev[0];
	//dkm = (elev[1]*elev[0])/1000.0;
	//xkm = elev[1]/1000.0;
	eno = atmosphericBendingConstant;
	enso = 0.0;
	q = enso;

	//| PRESET VALUES for Basic Version w/o additional inputs active

	prop.encc = 1000.0;					//| double enc_ncc_clcref preset
	prop.canopyClutterHeight = 22.5;	//| double clutter_height preset to ILLR calibration.
										//| use 25.3 for ITU-P1546-2 calibration
	prop.clutterDensity = 1.0;			//| double clutter_density preset
	int mode_var = 1; 					//| int mode_var set to 1 for FCC compatibility;
										//| normally, SPLAT presets this to 12

	if (q <= 0.0)
	{
		ja = (long)(3.0 + 0.1*elev[0]);
		jb = np - ja + 6;

		for (i = ja - 1; i < jb; ++i)
		{
			zsys += elev[i];
		}

		zsys /= (jb - ja + 1);
		q = eno;
	}

	propv.variabilityMode = mode_var;
	Qlrps(referenceFrequency, zsys, q, antennaPolarization, dielectricRelativePermittivityConstant, conductivitySiemensPerMeter, prop);
	PrepareForPointToPointModeAnalysisV2(elev, propv.radioClimate, propv.variabilityMode, prop, propa, propv);
	tpd = sqrt((prop.antennaEffectiveHeight[0] - prop.antennaEffectiveHeight[1])*(prop.antennaEffectiveHeight[0] - prop.antennaEffectiveHeight[1]) + (prop.distance)*(prop.distance));
	fs = 32.45 + 20.0*log10(referenceFrequency) + 20.0*log10(tpd / 1000.0);
	q = prop.distance - propa.dla;

	if (int(q) < 0.0)
	{
		err = strcpy_s(strmode, strmodeLen, "L-o-S");
	}
	else
	{
		if (int(q) == 0.0)
		{
			err = strcpy_s(strmode, strmodeLen, "1_Hrzn");
		}
		else if (int(q) > 0.0)
		{
			err = strcpy_s(strmode, strmodeLen, "2_Hrzn");
		}

		if ((prop.distance <= propa.dlsa) || (prop.distance <= propa.dx))
		{
			if (int(prop.horizonDistance[1]) == 0.0)
			{
				err = strcat_s(strmode, strmodeLen, "_Peak");
			}
			else
			{
				err = strcat_s(strmode, strmodeLen, "_Diff");
			}
		}
		else if (prop.distance > propa.dx)
		{
			err = strcat_s(strmode, strmodeLen, "_Tropo");
		}
	}

	dbloss = CalculateLongleyRiceVariability(zr, 0.0, zc, prop, propv) + fs;
	errnum = prop.errorCode;
}


//| ------------------------------
//| 
//| FUNCTION: PointToPointCalculationMdhTwo
//| 
//| OLD NAME: point_to_pointMDH_two
//| 
//| NOTES: 
//|   antennaPolarization: 0-Horizontal, 1-Vertical
//|   radioClimate: 1-Equatorial, 2-Continental Subtropical, 3-Maritime Tropical,
//|     4-Desert, 5-Continental Temperate, 6-Maritime Temperate, Over Land,
//|     7-Maritime Temperate, Over Sea
//|   timepct, locpct, confpct: .01 to .99
//|   elev[]: [num points - 1], [delta distance(meters)], [height(meters) point 1], ..., [height(meters) point n]
//|   propmode:  Value - Mode
//|    -1 - mode is undefined
//|     0 - Line of Sight
//|     5 - Single Horizon, Diffraction
//|     6 - Single Horizon, Troposcatter
//|     9 - Double Horizon, Diffraction
//|    10 - Double Horizon, Troposcatter
//|   errnum: 0- No Error.
//|     1 - Warning: Some parameters are nearly out of range.
//|         Results should be used with caution.
//|     2 - Note: Default parameters have been substituted for impossible ones.
//|     3 - Warning: A combination of parameters is out of range.
//|         Results are probably invalid.
//|     Other - Warning: Some parameters are out of range.
//|             Results are probably invalid.
//| 
//| ------------------------------
void 
PointToPointCalculationMdhTwo
(double elev[], 
	double tht_m, 
	double rht_m,
	double dielectricRelativePermittivityConstant,
	double conductivitySiemensPerMeter,
	double atmosphericBendingConstant,
	double enc_ncc_clcref, 
	double clutter_height, 
	double clutter_density,
	double delta_h_diff, 
	double referenceFrequency,
	int radioClimate,
	int antennaPolarization,
	int mode_var,
	double timepct, 
	double locpct, 
	double confpct,
	double &dbloss, 
	int &propmode, 
	double &deltaH, 
	int &errnum)
{
	prop_type   prop;
	propv_type  propv;
	propa_type  propa;
	double zsys = 0;
	double ztime, zloc, zconf;
	double eno, enso, q;
	long ja, jb, i, np;
	//double dkm, xkm;
	double fs;

	propmode = -1;	//| mode is undefined
	prop.antennaHeightAboveGroundLevel[0] = tht_m;
	prop.antennaHeightAboveGroundLevel[1] = rht_m;
	propv.radioClimate = radioClimate;
	prop.encc = enc_ncc_clcref;
	prop.canopyClutterHeight = clutter_height;
	prop.clutterDensity = clutter_density;
	prop.errorCode = 0;
	propv.recomputeParameters = 5;
	prop.analysisMode = -1;
	prop.transmitterPolarization = antennaPolarization;
	prop.thera = 0.0;
	prop.thenr = 0.0;
	ztime = StandardNormalTailDistributionFunctionInverse(timepct);
	zloc = StandardNormalTailDistributionFunctionInverse(locpct);
	zconf = StandardNormalTailDistributionFunctionInverse(confpct);
	np = (long)elev[0];
	//dkm = (elev[1] * elev[0]) / 1000.0;
	//xkm = elev[1] / 1000.0;
	eno = atmosphericBendingConstant;
	enso = 0.0;
	q = enso;

	//| PRESET VALUES for Basic Version w/o additional inputs active

	prop.encc = 1000.0;					//| double enc_ncc_clcref
	prop.canopyClutterHeight = 22.5;	//| double clutter_height
	prop.clutterDensity = 1.0;			//| double clutter_density
	mode_var = 1;						//| int mode_var set for FCC ILLR

	if (q <= 0.0)
	{
		ja = (long)(3.0 + 0.1 * elev[0]); //| to match addition of (long)
		jb = np - ja + 6;
		for (i = ja - 1; i < jb; ++i)
		{
			zsys += elev[i];
		}
		zsys /= (jb - ja + 1);
		q = eno;
	}
	propv.variabilityMode = 12;
	Qlrps(referenceFrequency, zsys, q, antennaPolarization, dielectricRelativePermittivityConstant, conductivitySiemensPerMeter, prop);
	PrepareForPointToPointModeAnalysisV2(elev, propv.radioClimate, propv.variabilityMode, prop, propa, propv);
	fs = 32.45 + 20.0*log10(referenceFrequency) + 20.0*log10(prop.distance / 1000.0);

	deltaH = prop.interdecileElevationRangeBetweenPoints;
	q = prop.distance - propa.dla;
	if (int(q) < 0.0)
	{
		propmode = 0;	//| L-of-S 
	}
	else
	{
		if (int(q) == 0.0)
		{
			propmode = 4;	//| 1-Hrzn
		}
		else if (int(q) > 0.0)
		{
			propmode = 8;	//| 2-Hrzn
		}

		if ((prop.distance <= propa.dlsa) || (prop.distance <= propa.dx))
		{
			propmode += 1;	//| Diff
		}
		else if (prop.distance > propa.dx)
		{
			propmode += 2;	//| Tropo
		}
	}
	dbloss = CalculateLongleyRiceVariability(ztime, zloc, zconf, prop, propv) + fs;	
	//dbloss = CalculateLongleyRiceVariability(time, location, confidence);

	errnum = prop.errorCode;
}


//| ------------------------------
//| 
//| FUNCTION: PointToPointCalculationDh
//| 
//| OLD NAME: point_to_pointDH
//| 
//| NOTES: 
//|   antennaPolarization: 0-Horizontal, 1-Vertical
//|   radio_climate: 1-Equatorial, 2-Continental Subtropical, 3-Maritime Tropical,
//|     4-Desert, 5-Continental Temperate, 6-Maritime Temperate, Over Land,
//|     7-Maritime Temperate, Over Sea
//|   fractionOfSituations: .01 to .99
//|   elev[]: [num points - 1], [delta distance(meters)], [height(meters) point 1], ..., [height(meters) point n]
//|   errnum: 0 - No Error.
//|     1 - Warning: Some parameters are nearly out of range.
//|         Results should be used with caution.
//|     2 - Note: Default parameters have been substituted for impossible ones.
//|     3 - Warning: A combination of parameters is out of range.
//|         Results are probably invalid.
//|     Other - Warning: Some parameters are out of range.
//|             Results are probably invalid.
//| 
//| ------------------------------
void 
PointToPointCalculationDh
(double elev[], 
	double tht_m, 
	double rht_m,
	double dielectricRelativePermittivityConstant,
	double conductivitySiemensPerMeter,
	double atmosphericBendingConstant,
	double enc_ncc_clcref, 
	double clutter_height, 
	double clutter_density,
	double delta_h_diff, 
	double referenceFrequency,
	int radioClimate,
	int antennaPolarization,
	double fractionOfSituations,
	double fractionOfTime,
	double loc, 
	double &dbloss, 
	double &deltaH,
	int &errnum)
{
	char strmode[100];
	int strmodeLen = 100;
	prop_type   prop;
	propv_type  propv;
	propa_type  propa;
	double zsys = 0;
	double zc, zr;
	double eno, enso, q;
	long ja, jb, i, np;
	//double dkm, xkm;
	double fs;
	errno_t err;

	prop.antennaHeightAboveGroundLevel[0] = tht_m;
	prop.antennaHeightAboveGroundLevel[1] = rht_m;
	propv.radioClimate = radioClimate;
	prop.encc = enc_ncc_clcref;
	prop.canopyClutterHeight = clutter_height;
	prop.clutterDensity = clutter_density;
	prop.errorCode = 0;
	propv.recomputeParameters = 5;
	prop.analysisMode = -1;
	prop.transmitterPolarization = antennaPolarization;
	prop.thera = 0.0;
	prop.thenr = 0.0;
	zc = StandardNormalTailDistributionFunctionInverse(fractionOfSituations);
	zr = StandardNormalTailDistributionFunctionInverse(fractionOfTime);
	np = (long)elev[0];
	//dkm = (elev[1] * elev[0]) / 1000.0;
	//xkm = elev[1] / 1000.0;
	eno = atmosphericBendingConstant;
	enso = 0.0;
	q = enso;

	//| PRESET VALUES for Basic Version w/o additional inputs active

	prop.encc = 1000.00;				//| double enc_ncc_clcref
	prop.canopyClutterHeight = 22.5;   	//| double clutter_height
	prop.clutterDensity = 1.00;			//| double clutter_density

	if (q <= 0.0)
	{
		ja = (long)(3.0 + 0.1 * elev[0]);	//| to match KD2BD addition of (long)
		jb = np - ja + 6;
		for (i = ja - 1; i < jb; ++i)
		{
			zsys += elev[i];
		}
		zsys /= (jb - ja + 1);
		q = eno;
	}
	propv.variabilityMode = 12;
	Qlrps(referenceFrequency, zsys, q, antennaPolarization, dielectricRelativePermittivityConstant, conductivitySiemensPerMeter, prop);
	PrepareForPointToPointModeAnalysisV2(elev, propv.radioClimate, propv.variabilityMode, prop, propa, propv);
	fs = 32.45 + 20.0*log10(referenceFrequency) + 20.0*log10(prop.distance / 1000.0);

	deltaH = prop.interdecileElevationRangeBetweenPoints;
	q = prop.distance - propa.dla;
	if (int(q) < 0.0)
	{
		err = strcpy_s(strmode, strmodeLen, "Line-Of-Sight Mode");
	}
	else
	{
		if (int(q) == 0.0)
		{
			err = strcpy_s(strmode, strmodeLen, "Single Horizon");
		}
		else if (int(q) > 0.0)
		{
			err = strcpy_s(strmode, strmodeLen, "Double Horizon");
		}

		if ((prop.distance <= propa.dlsa) || (prop.distance <= propa.dx))
		{
			err = strcat_s(strmode, strmodeLen, ", Diffraction Dominant");
		}
		else if (prop.distance > propa.dx)
		{
			err = strcat_s(strmode, strmodeLen, ", Troposcatter Dominant");
		}
	}
	dbloss = CalculateLongleyRiceVariability(zr, 0.0, zc, prop, propv) + fs;
	//dbloss = CalculateLongleyRiceVariability(time, location, confidence);

	errnum = prop.errorCode;
}


//| ------------------------------------------------------------
//| Area Mode Calculations 
//| ------------------------------------------------------------

//| ------------------------------
//| 
//| FUNCTION: AreaCalculation
//| 
//| OLD NAME: area
//| 
//| NOTES: 
//|   antennaPolarization: 0-Horizontal, 1-Vertical
//|   TSiteCriteria, RSiteCriteria:
//|     0 - random, 1 - careful, 2 - very careful
//|   radioClimate: 1-Equatorial, 2-Continental Subtropical, 3-Maritime Tropical,
//|     4-Desert, 5-Continental Temperate, 6-Maritime Temperate, Over Land,
//|     7-Maritime Temperate, Over Sea
//|   ModVar: 0 - Single: pctConf is "Time/Situation/Location", pctTime, pctLoc not used
//|     1 - Individual: pctTime is "Situation/Location", pctConf is "Confidence", pctLoc not used
//|     2 - Mobile: pctTime is "Time/Locations (Reliability)", pctConf is "Confidence", pctLoc not used
//|     3 - Broadcast: pctTime is "Time", pctLoc is "Location", pctConf is "Confidence"
//|   pctTime, pctLoc, pctConf: .01 to .99
//|   errnum: 0- No Error.
//|     1 - Warning: Some parameters are nearly out of range.
//|         Results should be used with caution.
//|     2 - Note: Default parameters have been substituted for impossible ones.
//|     3 - Warning: A combination of parameters is out of range.
//|         Results are probably invalid.
//|     Other - Warning: Some parameters are out of range.
//|             Results are probably invalid.
//|   strmode is not used at this time.
//| 
//| ------------------------------
void 
AreaCalculation
   (long ModVar, 
	double deltaH, 
	double tht_m, 
	double rht_m, 
	double dist_km, 
	int TSiteCriteria, 
	int RSiteCriteria, 
	double dielectricRelativePermittivityConstant,
	double conductivitySiemensPerMeter,
	double atmosphericBendingConstant,
	double enc_ncc_clcref, 
	double clutter_height, 
	double clutter_density, 
	double delta_h_diff, 
	double referenceFrequency,
	int radioClimate,
	int antennaPolarization,
	int mode_var,
	double pctTime, 
	double pctLoc, 
	double pctConf, 
	double &dbloss, 
	char *strmode, 
	int &errnum)
{
	prop_type prop;
	propv_type propv;
	propa_type propa;
	double zt, zl, zc, xlb;
	double fs;
	long ivar;
	double eps, eno, sgm;
	long ipol;
	int kst[2];

	kst[0] = (int)TSiteCriteria;
	kst[1] = (int)RSiteCriteria;
	zt = StandardNormalTailDistributionFunctionInverse(pctTime / 100.0);
	zl = StandardNormalTailDistributionFunctionInverse(pctLoc / 100.0);
	zc = StandardNormalTailDistributionFunctionInverse(pctConf / 100.0);
	eps = dielectricRelativePermittivityConstant;
	sgm = conductivitySiemensPerMeter;
	eno = atmosphericBendingConstant;
	prop.interdecileElevationRangeBetweenPoints = deltaH;
	prop.antennaHeightAboveGroundLevel[0] = tht_m;
	prop.antennaHeightAboveGroundLevel[1] = rht_m;
	propv.radioClimate = (long)radioClimate;
	prop.encc = enc_ncc_clcref;
	prop.canopyClutterHeight = clutter_height;
	prop.clutterDensity = clutter_density;
	prop.surfaceRefractivity = eno;
	prop.errorCode = 0;
	ivar = (long)ModVar;
	ipol = (long)antennaPolarization;
	Qlrps(referenceFrequency, 0.0, eno, ipol, eps, sgm, prop);
	Qlra(kst, propv.radioClimate, ivar, prop, propv);

	if (propv.recomputeParameters < 1)
	{
		propv.recomputeParameters = 1;
	}

	CalculateLongleyRicePropagationV2(dist_km*1000.0, prop, propa);
	fs = 32.45 + 20.0*log10(referenceFrequency) + 20.0*log10(prop.distance / 1000.0);
	xlb = fs + CalculateLongleyRiceVariability(zt, zl, zc, prop, propv);
	dbloss = xlb;
	if (prop.errorCode == 0)
	{
		errnum = 0;
	}
	else
	{
		errnum = prop.errorCode;
	}
}


//| ------------------------------
//| 
//| FUNCTION: IrregularTerrainModelAreaCalculationDbLoss
//| 
//| OLD NAME: ITMAreadBLoss
//| 
//| NOTES: 
//| 
//| ------------------------------
double 
IrregularTerrainModelAreaCalculationDbLoss
   (long ModVar, 
	double deltaH, 
	double tht_m, 
	double rht_m,
	double dist_km, 
	int TSiteCriteria, 
	int RSiteCriteria,
	double dielectricRelativePermittivityConstant,
	double conductivitySiemensPerMeter,
	double atmosphericBendingConstant,
	double enc_ncc_clcref, 
	double clutter_height, 
	double clutter_density,
	double delta_h_diff, 
	double referenceFrequency,
	int radioClimate,
	int antennaPolarization,
	int mode_var, 
	double pctTime, 
	double pctLoc, 
	double pctConf)
{
	char strmode[200];
	int errnum;
	double dbloss;
	AreaCalculation(ModVar, deltaH, tht_m, rht_m, dist_km, TSiteCriteria, RSiteCriteria,
		dielectricRelativePermittivityConstant, conductivitySiemensPerMeter, atmosphericBendingConstant,
		enc_ncc_clcref, clutter_height, clutter_density,
		delta_h_diff, referenceFrequency, radioClimate, antennaPolarization, mode_var, pctTime,
		pctLoc, pctConf, dbloss, strmode, errnum);
	return dbloss;
}
