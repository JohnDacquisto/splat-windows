/****************************************************************************\
*						itwom.longleyrice - Longley-Rice					 *
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
CalculateLongleyRicePropagation(
	double d,
	prop_type& prop,
	propa_type& propa)
{
	// PaulM_lrprop used for ITM
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
			propa.dls[j] = sqrt(2.0 * prop.antennaEffectiveHeight[j] / prop.relativeCurvature);
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
		//xae = pow(prop.waveNumber * pow(prop.relativeCurvature, 2.), -ONE_THIRD);	// JDM made argument 2 a double
		xae = pow(prop.waveNumber * (prop.relativeCurvature * prop.relativeCurvature), -ONE_THIRD);		// No 2nd pow()
		d3 = MaximumDouble(propa.dlsa, 1.3787 * xae + propa.dla);
		d4 = d3 + 2.7574 * xae;
		a3 = CalculateDiffractionAttenuation(d3, prop, propa);
		a4 = CalculateDiffractionAttenuation(d4, prop, propa);
		propa.emd = (a4 - a3) / (d4 - d3);
		propa.aed = a3 - propa.emd * d3;
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
			d0 = 1.908 * prop.waveNumber * prop.antennaEffectiveHeight[0] * prop.antennaEffectiveHeight[1];

			if (propa.aed >= 0.0)
			{
				d0 = MinimumDouble(d0, 0.5 * propa.dla);
				d1 = d0 + 0.25 * (propa.dla - d0);
			}
			else
			{
				d1 = MaximumDouble(-propa.aed / propa.emd, 0.25 * propa.dla);
			}

			a1 = CalculateLineOfSightAttenuation(d1, prop, propa);
			wq = false;

			if (d0 < d1)
			{
				a0 = CalculateLineOfSightAttenuation(d0, prop, propa);
				q = log(d2 / d0);
				propa.ak2 = MaximumDouble(0.0, ((d2 - d0) * (a1 - a0) - (d1 - d0) * (a2 - a0)) / ((d2 - d0) * log(d1 / d0) - (d1 - d0) * q));
				wq = propa.aed >= 0.0 || propa.ak2 > 0.0;

				if (wq)
				{
					propa.ak1 = (a2 - a0 - propa.ak2 * q) / (d2 - d0);

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

			propa.ael = a2 - propa.ak1 * d2 - propa.ak2 * log(d2);
			wlos = true;
		}

		if (prop.distance > 0.0)
		{
			prop.referenceAttenuation = propa.ael + propa.ak1 * prop.distance + propa.ak2 * log(prop.distance);
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
				propa.dx = MaximumDouble(propa.dlsa, MaximumDouble(propa.dla + 0.3 * xae * log(47.7 * prop.waveNumber), (a5 - propa.aed - propa.ems * d5) / (propa.emd - propa.ems)));
				propa.aes = (propa.emd - propa.ems) * propa.dx + propa.aed;
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
			prop.referenceAttenuation = propa.aes + propa.ems * prop.distance;
		}
		else
		{
			prop.referenceAttenuation = propa.aed + propa.emd * prop.distance;
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
CalculateLongleyRicePropagationV2(
	double d,
	prop_type& prop,
	propa_type& propa)
{
	// ITWOM_lrprop2
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

	if (prop.analysisMode != 0)	// if oper. mode is not 0, i.e. not area mode ongoing
	{
		for (j = 0; j < 2; j++)
		{
			propa.dls[j] = sqrt(2.0 * prop.antennaEffectiveHeight[j] / prop.relativeCurvature);
		}

		propa.dlsa = propa.dls[0] + propa.dls[1];
		propa.dlsa = MinimumDouble(propa.dlsa, 1000000.0);
		propa.dla = prop.horizonDistance[0] + prop.horizonDistance[1];
		propa.tha = MaximumDouble(prop.grazingAngle[0] + prop.grazingAngle[1], -propa.dla * prop.relativeCurvature);
		wlos = false;
		wscat = false;

		// checking for parameters-in-range, error codes set if not

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
		d3 = MaximumDouble(propa.dlsa, 1.3787 * xae + propa.dla);
		d4 = d3 + 2.7574 * xae;
		a3 = CalculateDiffractionAttenuationV2(d3, prop, propa);
		a4 = CalculateDiffractionAttenuationV2(d4, prop, propa);
		propa.emd = (a4 - a3) / (d4 - d3);
		propa.aed = a3 - propa.emd * d3;
	}

	if (prop.analysisMode >= 0)	// if initializing the area mode
	{
		prop.analysisMode = 0;	// area mode is initialized
		prop.distance = d;
	}

	if (prop.distance > 0.0)
	{
		if (prop.distance > 1000e3)	// prop.dist being in meters, if greater than 1000 km, errorCode = 1
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
		if (iw <= 0.0)	// if interval width is zero or less, used for area mode
		{
			if (!wlos)
			{
				q = CalculateLineOfSightAttenuationV2(0.0, prop, propa);
				d2 = propa.dlsa;
				a2 = propa.aed + d2 * propa.emd;
				d0 = 1.908 * prop.waveNumber * prop.antennaEffectiveHeight[0] * prop.antennaEffectiveHeight[1];

				if (propa.aed > 0.0)
				{
					prop.referenceAttenuation = propa.aed + propa.emd * prop.distance;
				}
				else
				{
					if (propa.aed == 0.0)
					{
						d0 = MinimumDouble(d0, 0.5 * propa.dla);
						d1 = d0 + 0.25 * (propa.dla - d0);
					}
					else
					{
						d1 = MaximumDouble(-propa.aed / propa.emd, 0.25 * propa.dla);
					}
					a1 = CalculateLineOfSightAttenuationV2(d1, prop, propa);
					wq = false;

					if (d0 < d1)
					{
						a0 = CalculateLineOfSightAttenuationV2(d0, prop, propa);
						a2 = MinimumDouble(a2, CalculateLineOfSightAttenuationV2(d2, prop, propa));
						q = log(d2 / d0);
						propa.ak2 = MaximumDouble(0.0, ((d2 - d0) * (a1 - a0) - (d1 - d0) * (a2 - a0)) / ((d2 - d0) * log(d1 / d0) - (d1 - d0) * q));
						wq = propa.aed >= 0.0 || propa.ak2 > 0.0;

						if (wq)
						{
							propa.ak1 = (a2 - a0 - propa.ak2 * q) / (d2 - d0);

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
					propa.ael = a2 - propa.ak1 * d2 - propa.ak2 * log(d2);
					wlos = true;
				}
			}
		}
		else	// for ITWOM point-to-point mode
		{
			if (!wlos)
			{
				q = CalculateLineOfSightAttenuationV2(0.0, prop, propa);	// coefficient setup
				wlos = true;
			}

			if (prop.lineOfSight == 1)	// if line of sight
			{
				prop.referenceAttenuation = CalculateLineOfSightAttenuationV2(pd1, prop, propa);
			}
			else
			{
				if (int(prop.distance - prop.horizonDistance[0]) == 0)	// if at 1st horiz
				{
					prop.referenceAttenuation = 5.8 + CalculateLineOfSightAttenuationV2(pd1, prop, propa);
				}
				else if (int(prop.distance - prop.horizonDistance[0]) > 0.0)	// if past 1st horiz
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

	// lineOfSight and diff. range coefficents done. Starting troposcatter
	if ((prop.distance <= 0.0) || (prop.distance >= propa.dlsa))
	{
		if (iw == 0.0)	// area mode
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
					propa.dx = MaximumDouble(propa.dlsa, MaximumDouble(propa.dla + 0.3 * xae * log(47.7 * prop.waveNumber), (a5 - propa.aed - propa.ems * d5) / (propa.emd - propa.ems)));
					propa.aes = (propa.emd - propa.ems) * propa.dx + propa.aed;
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
				prop.referenceAttenuation = propa.aes + propa.ems * prop.distance;
			}
			else
			{
				prop.referenceAttenuation = propa.aed + propa.emd * prop.distance;
			}
		}
		else	// ITWOM mode q used to preset coefficients with zero input
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
CalculateLongleyRiceVariability(
	double zzt,
	double zzl,
	double zzc,
	prop_type& prop,
	propv_type& propv)
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
			q = log(0.133 * prop.waveNumber);

			//gm = cfm1 + cfm2/(pow(cfm3*q, 2.0)+1.0);
			//gp = cfp1 + cfp2/(pow(cfp3*q, 2.0)+1.0);

			gm = cfm1 + cfm2 / ((cfm3 * q * cfm3 * q) + 1.0);
			gp = cfp1 + cfp2 / ((cfp3 * q * cfp3 * q) + 1.0);

		case 2:
			dexa = sqrt(18e6 * prop.antennaEffectiveHeight[0]) + sqrt(18e6 * prop.antennaEffectiveHeight[1]) + pow((575.7e12 / prop.waveNumber), ONE_THIRD);

		case 1:
			if (prop.distance < dexa)
			{
				de = 130e3 * prop.distance / dexa;
			}
			else
			{
				de = 130e3 + prop.distance - dexa;
			}
		}

		vmd = Curve(cv1, cv2, yv1, yv2, yv3, de);
		sgtm = Curve(csm1, csm2, ysm1, ysm2, ysm3, de) * gm;
		sgtp = Curve(csp1, csp2, ysp1, ysp2, ysp3, de) * gp;
		sgtd = sgtp * csd1;
		tgtd = (sgtp - sgtd) * zd;

		if (w1)
		{
			sgl = 0.0;
		}
		else
		{
			q = (1.0 - 0.8 * exp(-prop.distance / 50e3)) * prop.interdecileElevationRangeBetweenPoints * prop.waveNumber;
			sgl = 10.0 * q / (q + 13.0);
		}

		if (ws)
		{
			vs0 = 0.0;
		}
		else
		{
			//vs0 = pow(5.0+3.0*exp(-de/100e3), 2.0);
			temp1 = (5.0 + 3.0 * exp(-de / 100e3));
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

	vs = vs0 + (temp1 * temp1) / (rt + zc * zc) + (temp2 * temp2) / (rl + zc * zc);

	if (kdv == 0)
	{
		yr = 0.0;
		propv.sgc = sqrt(sgt * sgt + sgl * sgl + vs);
	}
	else if (kdv == 1)
	{
		yr = sgt * zt;
		propv.sgc = sqrt(sgl * sgl + vs);
	}
	else if (kdv == 2)
	{
		yr = sqrt(sgt * sgt + sgl * sgl) * zt;
		propv.sgc = sqrt(vs);
	}
	else
	{
		yr = sgt * zt + sgl * zl;
		propv.sgc = sqrt(vs);
	}

	avarv = prop.referenceAttenuation - vmd - yr - propv.sgc * zc;

	if (avarv < 0.0)
	{
		avarv = avarv * (29.0 - avarv) / (29.0 - 10.0 * avarv);
	}

	return avarv;
}
