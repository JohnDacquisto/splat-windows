#pragma once
#include "stdafx.h"


//| ------------------------------
//| Pick these values for each build.
#define HD_MODE 0
#define MAXPAGES 4
//| ------------------------------


//| Constants

#define GAMMA 2.5

#if HD_MODE==0
	#if MAXPAGES==4
		#define ARRAY_SIZE 4950
	#endif
	#if MAXPAGES==9
		#define ARRAY_SIZE 10870
	#endif
	#if MAXPAGES==16
		#define ARRAY_SIZE 19240
	#endif
	#if MAXPAGES==25
		#define ARRAY_SIZE 30025
	#endif
	#if MAXPAGES==36
		#define ARRAY_SIZE 43217
	#endif
	#if MAXPAGES==49
		#define ARRAY_SIZE 58813
	#endif
	#if MAXPAGES==64
		#define ARRAY_SIZE 76810
	#endif
	#define IPPD 1200
#endif
#if HD_MODE==1
	#if MAXPAGES==1
		#define ARRAY_SIZE 5092 
	#endif
	#if MAXPAGES==4
		#define ARRAY_SIZE 14844 
	#endif
	#if MAXPAGES==9
		#define ARRAY_SIZE 32600
	#endif
	#if MAXPAGES==16
		#define ARRAY_SIZE 57713
	#endif
	#if MAXPAGES==25
		#define ARRAY_SIZE 90072
	#endif
	#if MAXPAGES==36
		#define ARRAY_SIZE 129650
	#endif
	#if MAXPAGES==49 
		#define ARRAY_SIZE 176437
	#endif
	#if MAXPAGES==64
		#define ARRAY_SIZE 230430
	#endif
	#define IPPD 3600
#endif

#ifndef PI
	#define PI 3.141592653589793
#endif

#ifndef TWOPI
	#define TWOPI 6.283185307179586
#endif

#ifndef HALFPI
	#define HALFPI 1.570796326794896
#endif

#define DEG2RAD 1.74532925199e-02
#define EARTHRADIUS 20902230.97
#define	METERS_PER_MILE 1609.344
#define METERS_PER_FOOT 0.3048
#define	KM_PER_MILE 1.609344
#define FOUR_THIRDS 1.3333333333333


//| Global variables

char SplatString[255];				//| OLD NAME: string
char SplatDataFilePath[255];		//| OLD NAME: sdf_path 
char opened = 0;					//| OLD NAME: 
char gpsav = 0;						//| OLD NAME: 
char SplatName[10];					//| OLD NAME: splat_name
char SplatVersion[6];				//| OLD NAME: splat_version
char dashes[80];					//| OLD NAME: 
char olditm;						//| OLD NAME: olditm
char SiteAnalysisReportPath[255];

double	earthradius, max_range = 0.0, forced_erp = -1.0, dpp, ppd,
	fzone_clearance = 0.6, forced_freq, clutter;

int	min_north = 90, max_north = -90, min_west = 360, max_west = -1, ippd, mpi,
	max_elevation = -32768, min_elevation = 32768, bzerror, contour_threshold;

unsigned char got_elevation_pattern, got_azimuth_pattern, metric = 0, dbm = 0, smooth_contours = 0;

double elev[ARRAY_SIZE + 10];

struct site {
	double lat;
	double lon;
	float alt;
	char name[50];
	char filename[255];
} 	site;

struct path {
	double lat[ARRAY_SIZE];
	double lon[ARRAY_SIZE];
	double elevation[ARRAY_SIZE];
	double distance[ARRAY_SIZE];
	int length;
}	path;

struct dem {
	int min_north;
	int max_north;
	int min_west;
	int max_west;
	int max_el;
	int min_el;
	short data[IPPD][IPPD];
	unsigned char mask[IPPD][IPPD];
	unsigned char signal[IPPD][IPPD];
}	dem[MAXPAGES];

struct LR {
	double eps_dielect;
	double sgm_conductivity;
	double eno_ns_surfref;
	double frq_mhz;
	double conf;
	double rel;
	double erp;
	int radio_climate;
	int pol;
	float antenna_pattern[361][1001];
}	LR;

struct region {
	unsigned char color[32][3];
	int level[32];
	int levels;
}	region;


//| Functions

int interpolate(int y0, int y1, int x0, int x1, int n);

double arccos(double x, double y);

int ReduceAngle(double angle);

double LonDiff(double lon1, double lon2);

char *dec2dms(double decimal);

int PutMask(double lat, double lon, int value);

int OrMask(double lat, double lon, int value);

int GetMask(double lat, double lon);

int PutSignal(double lat, double lon, unsigned char signal);

unsigned char GetSignal(double lat, double lon);

double GetElevation(struct site location);

int AddElevation(double lat, double lon, double height);

double Distance(struct site site1, struct site site2);

double Azimuth(struct site source, struct site destination);

double ElevationAngle(struct site source, struct site destination);

void ReadPath(struct site source, struct site destination);

double ElevationAngle2(struct site source, struct site destination, double er);

double AverageTerrain(struct site source, double azimuthx, double start_distance, double end_distance);

double haat(struct site antenna);

void PlaceMarker(struct site location);

double ReadBearing(char *input);

struct site LoadQTH(char *filename);

void LoadPAT(char *filename);

int LoadSDF_SDF(char *name);

char LoadSDF(char *name);

void LoadCities(char *filename);

void LoadUDT(char *filename);

void LoadBoundaries(char *filename);

char ReadLRParm(struct site txsite, char forced_read);

void PlotPath(struct site source, struct site destination, char mask_value);

void PlotLRPath(struct site source, struct site destination, unsigned char mask_value, FILE *fd);

void PlotLOSMap(struct site source, double altitude);

void PlotLRMap(struct site source, double altitude, char *plo_filename);

void LoadSignalColors(struct site xmtr);

void LoadLossColors(struct site xmtr);

void LoadDBMColors(struct site xmtr);

void WritePPM(char *filename, unsigned char geo, unsigned char kml, unsigned char ngs, struct site *xmtr, unsigned char txsites);

void WritePPMLR(char *filename, unsigned char geo, unsigned char kml, unsigned char ngs, struct site *xmtr, unsigned char txsites);

void WritePPMSS(char *filename, unsigned char geo, unsigned char kml, unsigned char ngs, struct site *xmtr, unsigned char txsites);

void WritePPMDBM(char *filename, unsigned char geo, unsigned char kml, unsigned char ngs, struct site *xmtr, unsigned char txsites);

void GraphTerrain(struct site source, struct site destination, char *name);

void GraphElevation(struct site source, struct site destination, char *name);

void GraphHeight(struct site source, struct site destination, char *name, unsigned char fresnel_plot, unsigned char normalized);

void ObstructionAnalysis(struct site xmtr, struct site rcvr, double f, FILE *outfile);

void PathReport(struct site source, struct site destination, char *name, char graph_it);

void SiteReport(struct site xmtr);

void LoadTopoData(int max_lon, int min_lon, int max_lat, int min_lat);

int LoadANO(char *filename);

void WriteKML(struct site source, struct site destination);

int main(int argc, char *argv[]);
