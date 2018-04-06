#pragma once
#include "stdafx.h"
#include <complex>


using namespace std;


//| Constants

#define ONE_THIRD (1.0/3.0)


//| Global variables

struct tcomplex
{
	double tcreal;
	double tcimag;
};

struct prop_type
{
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

struct propv_type
{
	double sgc;
	int lvar;
	int mdvar;
	int klim;
};

struct propa_type
{
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

int mymin(const int &i, const int &j);

int mymax(const int &i, const int &j);

double mymin(const double &a, const double &b);

double mymax(const double &a, const double &b);

double FORTRAN_DIM(const double &x, const double &y);

double aknfe(const double &v2);

double fht(const double &x, const double &pk);

double h0f(double r, double et);

double ahd(double td);

double abq_alos(complex<double> r);

double saalos(double d, prop_type &prop, propa_type &propa);

double adiff(double d, prop_type &prop, propa_type &propa);

double adiff2(double d, prop_type &prop, propa_type &propa);

double ascat(double d, prop_type &prop, propa_type &propa);

double qerfi(double q);

void qlrps(double fmhz, double zsys, double en0, int ipol, double eps, double sgm, prop_type &prop);

double alos(double d, prop_type &prop, propa_type &propa);

double alos2(double d, prop_type &prop, propa_type &propa);

void qlra(int kst[], int klimx, int mdvarx, prop_type &prop, propv_type &propv);

void lrprop(double d, prop_type &prop, propa_type &propa);

void lrprop2(double d, prop_type &prop, propa_type &propa);

double curve(double const &c1, double const &c2, double const &x1,
	double const &x2, double const &x3, double const &de);

double avar(double zzt, double zzl, double zzc, prop_type &prop, propv_type &propv);

void hzns(double pfl[], prop_type &prop);

void hzns2(double pfl[], prop_type &prop, propa_type &propa);

void z1sq1(double z[], const double &x1, const double &x2, double &z0, double &zn);

void z1sq2(double z[], const double &x1, const double &x2, double &z0, double &zn);

double qtile(const int &nn, double a[], const int &ir);

double qerf(const double &z);

double d1thx(double pfl[], const double &x1, const double &x2);

double d1thx2(double pfl[], const double &x1, const double &x2, propa_type &propa);

void qlrpfl(double pfl[], int klimx, int mdvarx, prop_type &prop, propa_type &propa, propv_type &propv);

void qlrpfl2(double pfl[], int klimx, int mdvarx, prop_type &prop, propa_type &propa, propv_type &propv);

double deg2rad(double d);

void point_to_point_ITM(double elev[], double tht_m, double rht_m, double eps_dielect, double sgm_conductivity,
	double eno_ns_surfref, double frq_mhz, int radio_climate, int pol, double conf, double rel, double &dbloss,
	char *strmode, int strmodeLen, int &errnum);

void point_to_point(double elev[], double tht_m, double rht_m, double eps_dielect, double sgm_conductivity,
	double eno_ns_surfref, double frq_mhz, int radio_climate, int pol, double conf, double rel, double &dbloss,
	char *strmode, int strmodeLen, int &errnum);

void point_to_pointMDH_two(double elev[], double tht_m, double rht_m,
	double eps_dielect, double sgm_conductivity, double eno_ns_surfref,
	double enc_ncc_clcref, double clutter_height, double clutter_density,
	double delta_h_diff, double frq_mhz, int radio_climate, int pol, int mode_var,
	double timepct, double locpct, double confpct,
	double &dbloss, int &propmode, double &deltaH, int &errnum);

void point_to_pointDH(double elev[], double tht_m, double rht_m,
	double eps_dielect, double sgm_conductivity, double eno_ns_surfref,
	double enc_ncc_clcref, double clutter_height, double clutter_density,
	double delta_h_diff, double frq_mhz, int radio_climate, int pol,
	double conf, double rel, double loc, double &dbloss, double &deltaH,
	int &errnum);

void area(long ModVar, double deltaH, double tht_m, double rht_m, double dist_km, int TSiteCriteria, int RSiteCriteria,
	double eps_dielect, double sgm_conductivity, double eno_ns_surfref, double enc_ncc_clcref, double clutter_height,
	double clutter_density, double delta_h_diff, double frq_mhz, int radio_climate, int pol, int mode_var,
	double pctTime, double pctLoc, double pctConf, double &dbloss, char *strmode, int &errnum);

double ITMAreadBLoss(long ModVar, double deltaH, double tht_m, double rht_m,
	double dist_km, int TSiteCriteria, int RSiteCriteria,
	double eps_dielect, double sgm_conductivity, double eno_ns_surfref,
	double enc_ncc_clcref, double clutter_height, double clutter_density,
	double delta_h_diff, double frq_mhz, int radio_climate, int pol,
	int mode_var, double pctTime, double pctLoc, double pctConf);

double ITWOMVersion();
