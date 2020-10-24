/****************************************************************************\
*						itwom.p2p - Point-to-Point							 *
*****************************************************************************/

#include "stdafx.h"
#include <math.h>
#include <complex>
#include <assert.h>
#include <string.h>
#include "itwom.h"

using namespace std;


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
PrepareForPointToPointModeAnalysis(
	double pfl[],
	int klimx,
	int mdvarx,
	prop_type& prop,
	propa_type& propa,
	propv_type& propv)
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
			prop.horizonDistance[j] = sqrt(2.0 * prop.antennaEffectiveHeight[j] / prop.relativeCurvature) * exp(-0.07 * sqrt(prop.interdecileElevationRangeBetweenPoints / MaximumDouble(prop.antennaEffectiveHeight[j], 5.0)));
		}

		q = prop.horizonDistance[0] + prop.horizonDistance[1];

		if (q <= prop.distance)	// if there is a rounded horizon, or two obstructions, in the path
		{
			//q = pow(prop.distance/q, 2.0);
			temp = prop.distance / q;
			q = temp * temp;

			for (j = 0; j < 2; j++)
			{
				prop.antennaEffectiveHeight[j] *= q;	// tx effective height set to be path distance between obstacles
				prop.horizonDistance[j] = sqrt(2.0 * prop.antennaEffectiveHeight[j] / prop.relativeCurvature) * exp(-0.07 * sqrt(prop.interdecileElevationRangeBetweenPoints / MaximumDouble(prop.antennaEffectiveHeight[j], 5.0)));
			}
		}

		for (j = 0; j < 2; j++)	// original empirical adjustment? uses delta-h to adjust grazing angles
		{
			q = sqrt(2.0 * prop.antennaEffectiveHeight[j] / prop.relativeCurvature);
			prop.grazingAngle[j] = (0.65 * prop.interdecileElevationRangeBetweenPoints * (q / prop.horizonDistance[j] - 1.0) - 2.0 * prop.antennaEffectiveHeight[j]) / q;
		}
	}
	else
	{
		CalculateLinearLeastSquaresFit(pfl, xl[0], 0.9 * prop.horizonDistance[0], za, q);
		CalculateLinearLeastSquaresFit(pfl, prop.distance - 0.9 * prop.horizonDistance[1], xl[1], q, zb);
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
PrepareForPointToPointModeAnalysisV2(
	double pfl[],
	int klimx,
	int mdvarx,
	prop_type& prop,
	propa_type& propa,
	propv_type& propv)
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
		xl[j] = MinimumDouble(15.0 * prop.antennaHeightAboveGroundLevel[j], 0.1 * prop.horizonDistance[j]);
	}

	xl[1] = prop.distance - xl[1];
	prop.interdecileElevationRangeBetweenPoints = CalculateTerrainInterdecileRangeDeltaV2(pfl, xl[0], xl[1], propa);

	if ((np < 1) || (pfl[1] > 150.0))
	{
		// for TRANSHORIZON; diffraction over a mutual horizon, or for one or more obstructions
		if (dlb < (1.5 * prop.distance))
		{
			CalculateLinearLeastSquaresFitV2(pfl, xl[0], 0.9 * prop.horizonDistance[0], za, q);
			CalculateLinearLeastSquaresFitV2(pfl, prop.distance - 0.9 * prop.horizonDistance[1], xl[1], q, zb);
			prop.antennaEffectiveHeight[0] = prop.antennaHeightAboveGroundLevel[0] + PositiveDifferenceOrZero(pfl[2], za);
			prop.antennaEffectiveHeight[1] = prop.antennaHeightAboveGroundLevel[1] + PositiveDifferenceOrZero(pfl[np + 2], zb);
		}
		// for a Line-of-Sight path
		else
		{
			CalculateLinearLeastSquaresFitV2(pfl, xl[0], xl[1], za, zb);
			prop.antennaEffectiveHeight[0] = prop.antennaHeightAboveGroundLevel[0] + PositiveDifferenceOrZero(pfl[2], za);
			prop.antennaEffectiveHeight[1] = prop.antennaHeightAboveGroundLevel[1] + PositiveDifferenceOrZero(pfl[np + 2], zb);

			for (j = 0; j < 2; j++)
			{
				prop.horizonDistance[j] = sqrt(2.0 * prop.antennaEffectiveHeight[j] / prop.relativeCurvature) * exp(-0.07 * sqrt(prop.interdecileElevationRangeBetweenPoints / MaximumDouble(prop.antennaEffectiveHeight[j], 5.0)));
			}

			// for one or more obstructions only NOTE buried as in ITM FORTRAN and DLL, not functional
			if ((prop.horizonDistance[0] + prop.horizonDistance[1]) <= prop.distance)
			{
				//q = pow(prop.distance/(dl[0]+dl[1])), 2.0);
				temp = prop.distance / (prop.horizonDistance[0] + prop.horizonDistance[1]);
				q = temp * temp;
			}

			for (j = 0; j < 2; j++)
			{
				prop.antennaEffectiveHeight[j] *= q;
				prop.horizonDistance[j] = sqrt(2.0 * prop.antennaEffectiveHeight[j] / prop.relativeCurvature) * exp(-0.07 * sqrt(prop.interdecileElevationRangeBetweenPoints / MaximumDouble(prop.antennaEffectiveHeight[j], 5.0)));
			}

			// this sets (or resets) prop.grazingAngle, and is not in The Guide FORTRAN QLRPFL
			for (j = 0; j < 2; j++)
			{
				q = sqrt(2.0 * prop.antennaEffectiveHeight[j] / prop.relativeCurvature);
				prop.grazingAngle[j] = (0.65 * prop.interdecileElevationRangeBetweenPoints * (q / prop.horizonDistance[j] - 1.0) - 2.0 * prop.antennaEffectiveHeight[j]) / q;
			}
		}
	}
	else	// for ITWOM, computes antennaEffectiveHeight for tx, rcvr, and the receiver approach angles for use in CalculateClutterLoss
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
PointToPointCalculationLegacy(
	double elev[],
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
	double& dbloss,
	char* strmode,
	int strmodeLen,
	int& errnum)
{
	prop_type prop;
	propv_type propv;
	propa_type propa;
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
		ja = (long)(3.0 + 0.1 * elev[0]);	// added (long) to correct
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
	fs = 32.45 + 20.0 * log10(referenceFrequency) + 20.0 * log10(prop.distance / 1000.0);
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
PointToPointCalculation(
	double elev[],
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
	double& dbloss,
	char* strmode,
	int strmodeLen,
	int& errnum)
{
	prop_type prop;
	propv_type propv;
	propa_type propa;

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

	// PRESET VALUES for Basic Version w/o additional inputs active

	prop.encc = 1000.0;					// double enc_ncc_clcref preset
	prop.canopyClutterHeight = 22.5;	// double clutter_height preset to ILLR calibration.
										// use 25.3 for ITU-P1546-2 calibration
	prop.clutterDensity = 1.0;			// double clutter_density preset
	int mode_var = 1; 					// int mode_var set to 1 for FCC compatibility;
										// normally, SPLAT presets this to 12

	if (q <= 0.0)
	{
		ja = (long)(3.0 + 0.1 * elev[0]);
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
	tpd = sqrt((prop.antennaEffectiveHeight[0] - prop.antennaEffectiveHeight[1]) * (prop.antennaEffectiveHeight[0] - prop.antennaEffectiveHeight[1]) + (prop.distance) * (prop.distance));
	fs = 32.45 + 20.0 * log10(referenceFrequency) + 20.0 * log10(tpd / 1000.0);
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
PointToPointCalculationMdhTwo(
	double elev[],
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
	double& dbloss,
	int& propmode,
	double& deltaH,
	int& errnum)
{
	prop_type prop;
	propv_type propv;
	propa_type propa;
	double zsys = 0;
	double ztime, zloc, zconf;
	double eno, enso, q;
	long ja, jb, i, np;
	//double dkm, xkm;
	double fs;

	propmode = -1;	// mode is undefined
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

	// PRESET VALUES for Basic Version w/o additional inputs active

	prop.encc = 1000.0;					// double enc_ncc_clcref
	prop.canopyClutterHeight = 22.5;	// double clutter_height
	prop.clutterDensity = 1.0;			// double clutter_density
	mode_var = 1;						// int mode_var set for FCC ILLR

	if (q <= 0.0)
	{
		ja = (long)(3.0 + 0.1 * elev[0]); // to match addition of (long)
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
	fs = 32.45 + 20.0 * log10(referenceFrequency) + 20.0 * log10(prop.distance / 1000.0);

	deltaH = prop.interdecileElevationRangeBetweenPoints;
	q = prop.distance - propa.dla;
	if (int(q) < 0.0)
	{
		propmode = 0;	// L-of-S 
	}
	else
	{
		if (int(q) == 0.0)
		{
			propmode = 4;	// 1-Hrzn
		}
		else if (int(q) > 0.0)
		{
			propmode = 8;	// 2-Hrzn
		}

		if ((prop.distance <= propa.dlsa) || (prop.distance <= propa.dx))
		{
			propmode += 1;	// Diff
		}
		else if (prop.distance > propa.dx)
		{
			propmode += 2;	// Tropo
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
PointToPointCalculationDh(
	double elev[],
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
	double& dbloss,
	double& deltaH,
	int& errnum)
{
	char strmode[100];
	int strmodeLen = 100;
	prop_type prop;
	propv_type propv;
	propa_type propa;
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

	// PRESET VALUES for Basic Version w/o additional inputs active

	prop.encc = 1000.00;				// double enc_ncc_clcref
	prop.canopyClutterHeight = 22.5;   	// double clutter_height
	prop.clutterDensity = 1.00;			// double clutter_density

	if (q <= 0.0)
	{
		ja = (long)(3.0 + 0.1 * elev[0]);	// to match KD2BD addition of (long)
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
	fs = 32.45 + 20.0 * log10(referenceFrequency) + 20.0 * log10(prop.distance / 1000.0);

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
