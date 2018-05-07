/********************************************************************************
*							ITWOM version 3.0a							        *
********************************************************************************/

#pragma once
#include "stdafx.h"
#include <complex>
#include "prop_type.h"
#include "propa_type.h"
#include "propv_type.h"
#include "..\Common\constants.h"


using namespace std;


//| Export functions

int _declspec(dllexport) MinimumInteger(const int &i, const int &j);

int _declspec(dllexport) MaximumInteger(const int &i, const int &j);

double _declspec(dllexport) MinimumDouble(const double &a, const double &b);

double _declspec(dllexport) MaximumDouble(const double &a, const double &b);

double _declspec(dllexport) PositiveDifferenceOrZero(const double &x, const double &y);

double _declspec(dllexport) AttenuationFromSingleKnifeEdge(const double &v2);

double _declspec(dllexport) HeightGainOverSmoothSphere(const double &x, const double &pk);

double _declspec(dllexport) H01ScatterFields(double r, double et);

double _declspec(dllexport) FThetaDScatterFields(double td);

double _declspec(dllexport) AbsoluteValueOfComplexNumber(complex<double> r);

double _declspec(dllexport) StandardNormalTailDistributionFunctionInverse(double q);

double _declspec(dllexport) Curve(double const &c1, double const &c2, double const &x1,
	double const &x2, double const &x3, double const &de);

void _declspec(dllexport) CalculateLinearLeastSquaresFit(double z[], const double &x1, const double &x2, double &z0, double &zn);

void _declspec(dllexport) CalculateLinearLeastSquaresFitV2(double z[], const double &x1, const double &x2, double &z0, double &zn);

double _declspec(dllexport) CreateQuantile(const int &nn, double a[], const int &ir);

double _declspec(dllexport) StandardNormalTailDistributionFunction(const double &z);

double _declspec(dllexport) CalculateTerrainInterdecileRangeDelta(double pfl[], const double &x1, const double &x2);

void _declspec(dllexport) PointToPointCalculationLegacy(double elev[], double tht_m, double rht_m, double eps_dielect, double sgm_conductivity,
	double eno_ns_surfref, double frq_mhz, int radio_climate, int pol, double conf, double rel, double &dbloss,
	char *strmode, int strmodeLen, int &errnum);

void _declspec(dllexport) PointToPointCalculation(double elev[], double tht_m, double rht_m, double eps_dielect, double sgm_conductivity,
	double eno_ns_surfref, double frq_mhz, int radio_climate, int pol, double conf, double rel, double &dbloss,
	char *strmode, int strmodeLen, int &errnum);

void _declspec(dllexport) PointToPointCalculationMdhTwo(double elev[], double tht_m, double rht_m, 
	double eps_dielect, double sgm_conductivity, double eno_ns_surfref, 
	double enc_ncc_clcref, double clutter_height, double clutter_density, 
	double delta_h_diff, double frq_mhz, int radio_climate, int pol, 
	int mode_var, double timepct, double locpct, double confpct, 
	double &dbloss, int &propmode, double &deltaH, int &errnum);

void _declspec(dllexport) PointToPointCalculationDh(double elev[], double tht_m, double rht_m,
	double eps_dielect, double sgm_conductivity, double eno_ns_surfref,
	double enc_ncc_clcref, double clutter_height, double clutter_density,
	double delta_h_diff, double frq_mhz, int radio_climate, int pol,
	double conf, double rel, double loc, double &dbloss, double &deltaH,
	int &errnum);

void _declspec(dllexport) AreaCalculation(long ModVar, double deltaH, double tht_m, double rht_m, 
	double dist_km, int TSiteCriteria, int RSiteCriteria, 
	double eps_dielect, double sgm_conductivity, double eno_ns_surfref, 
	double enc_ncc_clcref, double clutter_height, double clutter_density, 
	double delta_h_diff, double frq_mhz, int radio_climate, int pol, 
	int mode_var, double pctTime, double pctLoc, double pctConf, 
	double &dbloss, char *strmode, int &errnum);

double _declspec(dllexport) IrregularTerrainModelAreaCalculationDbLoss(long ModVar, double deltaH, double tht_m, double rht_m,
	double dist_km, int TSiteCriteria, int RSiteCriteria,
	double eps_dielect, double sgm_conductivity, double eno_ns_surfref,
	double enc_ncc_clcref, double clutter_height, double clutter_density,
	double delta_h_diff, double frq_mhz, int radio_climate, int pol,
	int mode_var, double pctTime, double pctLoc, double pctConf);


//| Functions

double CalculateClutterLoss(double d, prop_type &prop, propa_type &propa);

double CalculateDiffractionAttenuation(double d, prop_type &prop, propa_type &propa);

double CalculateDiffractionAttenuationV2(double d, prop_type &prop, propa_type &propa);

double CalculateScatterAttenuation(double d, prop_type &prop, propa_type &propa);

void Qlrps(double fmhz, double zsys, double en0, int ipol, double eps, double sgm, prop_type &prop);

double CalculateLineOfSightAttenuation(double d, prop_type &prop, propa_type &propa);

double CalculateLineOfSightAttenuationV2(double d, prop_type &prop, propa_type &propa);

void Qlra(int kst[], int klimx, int mdvarx, prop_type &prop, propv_type &propv);

void CalculateLongleyRicePropagation(double d, prop_type &prop, propa_type &propa);

void CalculateLongleyRicePropagationV2(double d, prop_type &prop, propa_type &propa);

double CalculateLongleyRiceVariability(double zzt, double zzl, double zzc, prop_type &prop, propv_type &propv);

void CalculateHorizonDistances(double pfl[], prop_type &prop);

void CalculateHorizonDistancesV2(double pfl[], prop_type &prop, propa_type &propa);

double CalculateTerrainInterdecileRangeDeltaV2(double pfl[], const double &x1, const double &x2, propa_type &propa);

void PrepareForPointToPointModeAnalysis(double pfl[], int klimx, int mdvarx, prop_type &prop, propa_type &propa, propv_type &propv);

void PrepareForPointToPointModeAnalysisV2(double pfl[], int klimx, int mdvarx, prop_type &prop, propa_type &propa, propv_type &propv);
