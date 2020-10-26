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
AttenuationFromSingleKnifeEdge(
	const double &v2)
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
HeightGainOverSmoothSphere(
	const double &x,
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
H01ScatterFields(
	double r,
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
FThetaDScatterFields(
	double td)
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
//| FUNCTION: CalculateClutterLoss
//| 
//| OLD NAME: saalos
//| 
//| NOTES: 
//| 
//| ------------------------------
double 
CalculateClutterLoss(
	double d,
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
		// at first, hone is transmitter antenna height
		// relative to receive site ground level.
		hone = prop.transmitterGroundHeight + prop.tsgh - (prop.antennaHeightAndPathElevation[1] - prop.antennaHeightAboveGroundLevel[1]);

		if (prop.transmitterGroundHeight > prop.canopyClutterHeight)  // for TX ant above all clutter height
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

			// if the ucrpc path touches the canopy before reaching the
			// end of the ucrpc, the entry point moves toward the
			// transmitter, extending the crpc and d1a. Estimating the d1a:

			if (ssnps <= 0.0)
			{
				d1a = MinimumDouble(0.1*pd, 600.0);
				crpc = d1a;
				// hone must be redefined as being barely above
				// the canopy height with respect to the receiver
				// canopy height, which despite the earth curvature
				// is at or above the transmitter antenna height.
				hone = prop.canopyClutterHeight + 1;
				rsp = .997;
				tsp = 1 - rsp;
			}
			else
			{
				if (prop.transmitterPolarization >= 1)  // vertical or circular
				{
					q = ((ensa*cttc - encca * ctic) / (ensa*cttc + encca * ctic));
					rsp = q * q;
					tsp = 1 - rsp;

					if (prop.transmitterPolarization == 2)  // circular - new
					{
						q = ((ensa*ctic - encca * cttc) / (ensa*ctic + encca * cttc));
						rsp = ((ensa*cttc - encca * ctic) / (ensa*cttc + encca * ctic));
						rsp = (q*q + rsp * rsp) / 2;
						tsp = 1 - rsp;
					}
				}
				else	// horizontal, or undefined
				{
					q = ((ensa*ctic - encca * cttc) / (ensa*ctic + encca * cttc));
					rsp = q * q;
					tsp = 1 - rsp;
				}
			}
			// tvsr is defined as tx ant height above receiver ant height
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
		else  // for TX at or below clutter height
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
CalculateDiffractionAttenuation(
	double d,
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
CalculateDiffractionAttenuationV2(
	double d,
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

	//sf1 = 1.0;	// average empirical hilltop foliage scatter factor for 1 obstruction
	sf2 = 1.0;		// average empirical hilltop foliage scatter factor for 2 obstructions

	//dfdh = prop.interdecileElevationRangeBetweenPoints;
	//ec = 0.5*prop.relativeCurvature;

	// CalculateDiffractionAttenuationV2 must first be run with d == 0.0 to set up coefficients
	if (d == 0)
	{
		q = prop.antennaHeightAboveGroundLevel[0] * prop.antennaHeightAboveGroundLevel[1];
		qk = prop.antennaEffectiveHeight[0] * prop.antennaEffectiveHeight[1] - q;
		//dhec = 2.73;

		if (prop.analysisMode < 0)
		{
			q += 10.0;
		}

		// coefficients for a standard four radii, rounded earth computation are prepared
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

		// for 1 obst tree base path
		dtof = sqrt(prop.horizonDistance[0] * prop.horizonDistance[0] + (toh - prop.canopyClutterHeight)*(toh - prop.canopyClutterHeight));
		dtof += prop.relativeCurvature * prop.horizonDistance[0];
		dto1f = sqrt(prop.horizonDistance[0] * prop.horizonDistance[0] + (toho - prop.canopyClutterHeight)*(toho - prop.canopyClutterHeight));
		dto1f += prop.relativeCurvature * prop.horizonDistance[0];
		drof = sqrt(prop.horizonDistance[1] * prop.horizonDistance[1] + (roh - prop.canopyClutterHeight)*(roh - prop.canopyClutterHeight));
		drof += prop.relativeCurvature * prop.horizonDistance[1];
		dro2f = sqrt(prop.horizonDistance[1] * prop.horizonDistance[1] + (roho - prop.canopyClutterHeight)*(roho - prop.canopyClutterHeight));
		dro2f += prop.relativeCurvature * prop.horizonDistance[1];

		// CalculateClutterLoss coefficients preset for post-obstacle receive path
		prop.transmitterGroundHeight = prop.canopyClutterHeight + 1.0;
		prop.tsgh = prop.hhr;
		rd = prop.horizonDistance[1];

		// two obstacle diffraction calculation
		if (int(ds) > 0)	// there are 2 obstacles
		{
			if (int(prop.horizonDistance[1]) > 0.0)	// receive site past 2nd peak
			{
				// rounding attenuation
				q = (1.607 - pk)*151.0*wa*th + xht;
				//ar = 0.05751*q - 10*log10(q) - aht;

				// knife edge vs round weighting
				q = (1.0 - 0.8*exp(-d / 50e3))*prop.interdecileElevationRangeBetweenPoints;
				q = (wd1 + xd1 / d)*MinimumDouble((q * prop.waveNumber), 6283.2);
				//wd = 25.1/(25.1+sqrt(q));

				q = 0.6365 * prop.waveNumber;

				if (prop.grazingAngle[1] < 0.2)  // receive grazing angle below 0.2 rad
				{
					// knife edge attenuation for two obstructions

					if (prop.hht < 3400)	// if below tree line, foliage top loss
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

					// finally, add clutter loss
					closs = CalculateClutterLoss(rd, prop, propa);
					adiffv2 += MinimumDouble(22.0, closs);
				}
				else	// rcvr site too close to 2nd obs
				{
					// knife edge attenuation for 1st obs

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

					// weighted calc. of knife vs rounded edge
					//adiffv2 = ar*wd + (1.0 - wd)*adiffv2;

					// clutter path loss past 2nd peak
					if (prop.grazingAngle[1] < 1.22)
					{
						rd = prop.horizonDistance[1];

						if (prop.grazingAngle[1] > 0.6)	// through foliage downhill
						{
							prop.transmitterGroundHeight = prop.canopyClutterHeight;
						}
						else	// close to foliage, rcvr in foliage downslope
						{
							vv = 0.6365 * prop.waveNumber * abs(dro2 + dhh2 - drto);
						}
						adiffv2 += AttenuationFromSingleKnifeEdge(vv);
						closs = CalculateClutterLoss(rd, prop, propa);
						adiffv2 += MinimumDouble(closs, 22.0);
					}
					else	// rcvr very close to bare cliff or skyscraper
					{
						adiffv2 = 5.8 + 25.0;
					}
				}
			}
			else	// receive site is atop a 2nd peak
			{
				vv = 0.6365 * prop.waveNumber * abs(dto + dro - dtr);
				adiffv2 = 5.8 + AttenuationFromSingleKnifeEdge(vv);
			}
		}
		else	// for single obstacle
		{
			if (int(prop.horizonDistance[1]) > 0.0)	// receive site past 1st peak
			{
				if (prop.grazingAngle[1] < 0.2)	// receive grazing angle less than 0.2 radians
				{
					vv = 0.6365 * prop.waveNumber * abs(dto + dro - dtr);

					if (prop.hht < 3400)
					{
						sdl = 18.0;
						sdl = pow(10, (-sdl / 20));
						// ke phase difference with respect to direct t-r line
						kedr = 0.159155 * prop.waveNumber * abs(dto + dro - dtr);
						arp = abs(kedr - (int(kedr)));
						kem = AttenuationFromSingleKnifeEdge(vv);
						kem = pow(10, (-kem / 20));
						// scatter path phase with respect to direct t-r line
						sdr = 0.5 + 0.159155*prop.waveNumber*abs(dtof + drof - dtr);
						srp = abs(sdr - (int(sdr)));
						// difference between scatter and ke phase in radians
						pd = 6.283185307*abs(srp - arp);
						// report pd prior to restriction
						// keep pd between 0 and pi radians and adjust for 3&4 quadrant
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
					// finally, add clutter loss
					closs = CalculateClutterLoss(rd, prop, propa);
					adiffv2 += MinimumDouble(closs, 22.0);
				}
				else	// receive grazing angle too high
				{
					if (prop.grazingAngle[1] < 1.22)
					{
						rd = prop.horizonDistance[1];

						if (prop.grazingAngle[1] > 0.6)	// through foliage downhill
						{
							prop.transmitterGroundHeight = prop.canopyClutterHeight;
						}
						else	// downhill slope just above foliage
						{
							vv = 0.6365 * prop.waveNumber * abs(dto + dro - dtr);
							adiffv2 = AttenuationFromSingleKnifeEdge(vv);
						}
						closs = CalculateClutterLoss(rd, prop, propa);
						adiffv2 += MinimumDouble(22.0, closs);
					}
					else	// receiver very close to bare cliff or skyscraper
					{
						adiffv2 = 5.8 + 25.0;
					}
				}
			}
			else	// if occurs, receive site atop first peak
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
CalculateScatterAttenuation(
	double d,
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
Qlrps(
	double fmhz,
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
CalculateLineOfSightAttenuation(
	double d,
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
CalculateLineOfSightAttenuationV2(
	double d,
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

			if ((sps < 0.05) && (prop.canopyClutterHeight > hrg) && (prop.distance < prop.horizonDistance[0]))	// if far from transmitter and receiver below canopy
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

		// no longer valid complex conjugate removed
		// by removing minus sign from in front of sin function
		re = AbsoluteValueOfComplexNumber(complex<double>(cos(q), sin(q)) + r);
		alosv = -10 * log10(re);
		prop.transmitterGroundHeight = prop.antennaHeightAboveGroundLevel[0];						// tx above gnd hgt set to antenna height AGL
		prop.tsgh = prop.antennaHeightAndPathElevation[0] - prop.antennaHeightAboveGroundLevel[0];	// tsgh set to tx site gl AMSL

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
Qlra(
	int kst[],
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
CalculateHorizonDistances(
	double pfl[],
	prop_type &prop)
{
	// Used only with ITM 1.2.2
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
CalculateHorizonDistancesV2(
	double pfl[],
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

		if (int(dshh) == 0)	// one obstacle
		{
			dr = prop.horizonDistance[1] / (1 + zb / prop.hht);
		}
		else	// two obstacles
		{
			dr = prop.horizonDistance[1] / (1 + zb / prop.hhr);
		}
	}
	else	// line of sight
	{
		dr = (prop.distance) / (1 + zb / za);
	}
	rp = 2 + (int)(floor(0.5 + dr / xi));
	prop.horizonHeightIndex = rp;
	prop.horizonHeight = pfl[rp];
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
CalculateTerrainInterdecileRangeDelta(
	double pfl[],
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
	sn = ToDouble(n - 1);
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
CalculateTerrainInterdecileRangeDeltaV2(
	double pfl[],
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
