#pragma once
#include "stdafx.h"
#include <complex>

//| JAD FIX
#include "C:/DEV/SPLAT/Common/constants.h"


using namespace std;


class tcomplex
{
public:
	tcomplex();
	~tcomplex();

	double tcreal;
	double tcimag;
};


class prop_type
{
public:
	prop_type();
	~prop_type();

	double aref;
	double dist;
	double hg[2];
	double rch[2];
	double wn;
	double dh;
	double dhd;
	double ens;
	double encc;
	double cch;
	double cd;
	double gme;
	double zgndreal;
	double zgndimag;
	double he[2];
	double dl[2];
	double the[2];
	double tiw;
	double ght;
	double ghr;
	double rph;
	double hht;
	double hhr;
	double tgh;
	double tsgh;
	double thera;
	double thenr;
	int rpl;
	int kwx;
	int mdp;
	int ptx;
	int los;
};


class propv_type
{
public:
	propv_type();
	~propv_type();

	double sgc;
	int lvar;
	int mdvar;
	int klim;
};


class propa_type
{
public:
	propa_type();
	~propa_type();

	double dlsa;
	double dx;
	double ael;
	double ak1;
	double ak2;
	double aed;
	double emd;
	double aes;
	double ems;
	double dls[2];
	double dla;
	double tha;
};


//| Functions

int MinimumInteger(const int &i, const int &j);

int MaximumInteger(const int &i, const int &j);

double MinimumDouble(const double &a, const double &b);

double MaximumDouble(const double &a, const double &b);

double PositiveDifferenceOrZero(const double &x, const double &y);

double AttenuationFromSingleKnifeEdge(const double &v2);

double HeightGainOverSmoothSphere(const double &x, const double &pk);

double H01ScatterFields(double r, double et);

double FThetaDScatterFields(double td);

double AbsoluteValueOfComplexNumber(complex<double> r);

double CalculateClutterLoss(double d, prop_type &prop, propa_type &propa);

double CalculateDiffractionAttenuation(double d, prop_type &prop, propa_type &propa);

double CalculateDiffractionAttenuationV2(double d, prop_type &prop, propa_type &propa);

double CalculateScatterAttenuation(double d, prop_type &prop, propa_type &propa);

double StandardNormalTailDistributionFunctionInverse(double q);

void Qlrps(double fmhz, double zsys, double en0, int ipol, double eps, double sgm, prop_type &prop);

double CalculateLineOfSightAttenuation(double d, prop_type &prop, propa_type &propa);

double CalculateLineOfSightAttenuationV2(double d, prop_type &prop, propa_type &propa);

void Qlra(int kst[], int klimx, int mdvarx, prop_type &prop, propv_type &propv);

void CalculateLongleyRicePropagation(double d, prop_type &prop, propa_type &propa);

void CalculateLongleyRicePropagationV2(double d, prop_type &prop, propa_type &propa);

double Curve(double const &c1, double const &c2, double const &x1,
	double const &x2, double const &x3, double const &de);

double CalculateLongleyRiceVariability(double zzt, double zzl, double zzc, prop_type &prop, propv_type &propv);

void CalculateHorizonDistances(double pfl[], prop_type &prop);

void CalculateHorizonDistancesV2(double pfl[], prop_type &prop, propa_type &propa);

void CalculateLinearLeastSquaresFit(double z[], const double &x1, const double &x2, double &z0, double &zn);

void CalculateLinearLeastSquaresFitV2(double z[], const double &x1, const double &x2, double &z0, double &zn);

double CreateQuantile(const int &nn, double a[], const int &ir);

double StandardNormalTailDistributionFunction(const double &z);

double CalculateTerrainInterdecileRangeDelta(double pfl[], const double &x1, const double &x2);

double CalculateTerrainInterdecileRangeDeltaV2(double pfl[], const double &x1, const double &x2, propa_type &propa);

void PrepareForPointToPointModeAnalysis(double pfl[], int klimx, int mdvarx, prop_type &prop, propa_type &propa, propv_type &propv);

void PrepareForPointToPointModeAnalysisV2(double pfl[], int klimx, int mdvarx, prop_type &prop, propa_type &propa, propv_type &propv);

double DegreesToRadians(double d);

void PointToPointCalculationLegacy(double elev[], double tht_m, double rht_m, double eps_dielect, double sgm_conductivity,
	double eno_ns_surfref, double frq_mhz, int radio_climate, int pol, double conf, double rel, double &dbloss,
	char *strmode, int strmodeLen, int &errnum);

void PointToPointCalculation(double elev[], double tht_m, double rht_m, double eps_dielect, double sgm_conductivity,
	double eno_ns_surfref, double frq_mhz, int radio_climate, int pol, double conf, double rel, double &dbloss,
	char *strmode, int strmodeLen, int &errnum);

void PointToPointCalculationMdhTwo(double elev[], double tht_m, double rht_m,
	double eps_dielect, double sgm_conductivity, double eno_ns_surfref,
	double enc_ncc_clcref, double clutter_height, double clutter_density,
	double delta_h_diff, double frq_mhz, int radio_climate, int pol, int mode_var,
	double timepct, double locpct, double confpct,
	double &dbloss, int &propmode, double &deltaH, int &errnum);

void PointToPointCalculationDh(double elev[], double tht_m, double rht_m,
	double eps_dielect, double sgm_conductivity, double eno_ns_surfref,
	double enc_ncc_clcref, double clutter_height, double clutter_density,
	double delta_h_diff, double frq_mhz, int radio_climate, int pol,
	double conf, double rel, double loc, double &dbloss, double &deltaH,
	int &errnum);

void AreaCalculation(long ModVar, double deltaH, double tht_m, double rht_m, double dist_km, int TSiteCriteria, int RSiteCriteria,
	double eps_dielect, double sgm_conductivity, double eno_ns_surfref, double enc_ncc_clcref, double clutter_height,
	double clutter_density, double delta_h_diff, double frq_mhz, int radio_climate, int pol, int mode_var,
	double pctTime, double pctLoc, double pctConf, double &dbloss, char *strmode, int &errnum);

double IrregularTerrainModelAreaCalculationDbLoss(long ModVar, double deltaH, double tht_m, double rht_m,
	double dist_km, int TSiteCriteria, int RSiteCriteria,
	double eps_dielect, double sgm_conductivity, double eno_ns_surfref,
	double enc_ncc_clcref, double clutter_height, double clutter_density,
	double delta_h_diff, double frq_mhz, int radio_climate, int pol,
	int mode_var, double pctTime, double pctLoc, double pctConf);

double IrregularTerrainModelVersion();
