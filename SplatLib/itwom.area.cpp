/****************************************************************************\
*						itwom.area - Area									 *
*****************************************************************************/

#include "stdafx.h"
#include <math.h>
#include <complex>
#include <assert.h>
#include <string.h>
#include "itwom.h"

using namespace std;


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
AreaCalculation(
	long ModVar,
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
	double& dbloss,
	char* strmode,
	int& errnum)
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

	CalculateLongleyRicePropagationV2(dist_km * 1000.0, prop, propa);
	fs = 32.45 + 20.0 * log10(referenceFrequency) + 20.0 * log10(prop.distance / 1000.0);
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
IrregularTerrainModelAreaCalculationDbLoss(
	long ModVar,
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
