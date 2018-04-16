#pragma once
#include "stdafx.h"
#include "IrregularTerrainModelParameters.h"
#include "ColorRegion.h"
#include "Path.h"
#include "DigitalElevationModel.h"

//| JAD FIX
#include "C:/DEV/SPLAT/Common/constants.h"
#include "C:/DEV/SPLAT/Common/Site.h"


//| Global variables

char SplatString[255];				//| OLD NAME: string
char SplatDataFilePath[255];		//| OLD NAME: sdf_path 
char SaveGnuPlotWorkingFiles = 0;	//| OLD NAME: gpsav
char SplatName[10];					//| OLD NAME: splat_name
char SplatVersion[6];				//| OLD NAME: splat_version
char LineOfDashes[80];				//| OLD NAME: dashes
char UseOldLongleyRiceModel;		//| OLD NAME: olditm
char ReportSavePath[255];

double SphereRadius;					//| OLD NAME: earthradius
double MaximumAnalysisDistance = 0.0;	//| OLD NAME: max_range
double EffectiveRadiatedPower = -1.0;	//| OLD NAME: forced_erp
double DegreesPerPixel;					//| OLD NAME: dpp
double PixelsPerDegree;					//| OLD NAME: ppd
double FresnelZoneClearanceRatio = 0.6;	//| OLD NAME: fzone_clearance
double FresnelZoneFrequency;			//| OLD NAME: forced_freq
double GroundClutterHeight;				//| OLD NAME: clutter

int	MinimumLatitudeNorth = 90;	//| OLD NAME: min_north
int MaximumLatitudeNorth = -90;	//| OLD NAME: max_north
int MinimumLongitudeWest = 360;	//| OLD NAME: min_west
int MaximumLongitudeWest = -1;	//| OLD NAME: max_west
int IntegerPixelsPerDegree;		//| OLD NAME: ippd
int MaximumPixelIndexPerDegree;	//| OLD NAME: mpi
int MaximumElevation = -32768;	//| OLD NAME: max_elevation
int MinimumElevation = 32768;	//| OLD NAME: min_elevation
int ContourDisplayThreshold;	//| OLD NAME: contour_threshold

unsigned char GotAntennaElevationAnglePattern;	//| OLD NAME: got_elevation_pattern
unsigned char GotAntennaAzimuthAnglePattern;	//| OLD NAME: got_azimuth_pattern
unsigned char UseMetricUnits = 0;				//| OLD NAME: metric
unsigned char PlotSignalPowerLevelContours = 0;	//| OLD NAME: dbm
unsigned char DisplaySmoothContourLevels = 0;	//| OLD NAME: smooth_contours

double PathElevation[ARRAY_SIZE + 10];	//| OLD NAME: elev

Site site;

Path path;

DigitalElevationModel dem[MAXIMUM_PAGES];

IrregularTerrainModelParameters LR;

ColorRegion region;


//| Functions

int LinearInterpolation(int y0, int y1, int x0, int x1, int n);

double ArcCosine(double x, double y);

int ConvertToNormalizedAngle(double angle);

double LongitudeDifference(double lon1, double lon2);

char *ConvertDecimalToDegreesMinutesSeconds(double decimal);

int SetValueInDigitalElevationModelMask(double lat, double lon, int value);

int SetOrValueInDigitalElevationModelMask(double lat, double lon, int value);

int GetValueInDigitalElevationModelMask(double lat, double lon);

int SetValueInDigitalElevationModelSignal(double lat, double lon, unsigned char signal);

unsigned char GetValueInDigitalElevationModelSignal(double lat, double lon);

double GetSiteLocationElevation(Site location);

int AddUserDefinedTerrainToDigitalElevationModelData(double lat, double lon, double height);

double GreatCircleDistanceBetweenSiteLocations(Site site1, Site site2);

double AzimuthAngleBetweenSites(Site source, Site destination);

void GeneratePathBetweenSites(Site source, Site destination);

double ObstructedElevationAngleBetweenSites(Site source, Site destination, double er);

double AverageTerrainOverDistanceAtAzimuthFromSite(Site source, double azimuthx, double start_distance, double end_distance);

double AntennaHeightAboveAverageTerrain(Site antenna);

void PlaceTextAndMarkerDataInMaskArrayForMaps(Site location);

double BearingStringToDecimalDegrees(char *input);

Site LoadSplatSiteLocationFile(char *filename);

void LoadAntennaAzimuthElevationPatternFiles(char *filename);

int LoadUncompressedSplatDataFile(char *name);

char LoadSplatDataFile(char *name);

void LoadAndPlotCitiesAndSitesOnMaps(char *filename);

void LoadUserDefinedTerrainFile(char *filename);

void LoadCartographicBoundaryFiles(char *filename);

char ReadLongleyRiceParameterDataForSite(Site txsite, char forced_read);

void AnalyzeAndPlotLineOfSightCoverageBetweenSites(Site source, Site destination, char mask_value, FILE *fd);

void AnalyzeAndPlotPathLossBetweenSites(Site source, Site destination, unsigned char mask_value, FILE *fd);

void PlotLineOfSightCoverageFromSiteAtAltitude(Site source, double altitude, char *plo_filename);

void PlotAttenuationFromSiteAtAltitude(Site source, double altitude, char *plo_filename);

void LoadSplatSignalColorsForSite(Site xmtr);

void LoadSplatLossColorsForSite(Site xmtr);

void LoadSplatPowerColorsForSite(Site xmtr);

void WritePortablePixMapLineOfSightCoverageFile(char *filename, unsigned char geo, unsigned char kml, unsigned char ngs, Site *xmtr, unsigned char txsites);

void WritePortablePixMapAttenuationFile(char *filename, unsigned char geo, unsigned char kml, unsigned char ngs, Site *xmtr, unsigned char txsites);

void WritePortablePixMapSignalStrengthFile(char *filename, unsigned char geo, unsigned char kml, unsigned char ngs, Site *xmtr, unsigned char txsites);

void WritePortablePixMapPowerLevelFile(char *filename, unsigned char geo, unsigned char kml, unsigned char ngs, Site *xmtr, unsigned char txsites);

void GenerateGnuPlotTerrainProfileBetweenSites(Site source, Site destination, char *name);

void GenerateGnuPlotElevationProfileBetweenSites(Site source, Site destination, char *name);

void GenerateGnuPlotHeightProfileBetweenSites(Site source, Site destination, char *name, unsigned char fresnel_plot, unsigned char normalized);

void PerformObstructionAnalysisBetweenSites(Site xmtr, Site rcvr, double f, FILE *outfile);

void WriteSplatPathReport(Site source, Site destination, char *name, char graph_it);

void WriteSplatSiteReport(Site xmtr);

void LoadSplatDataFilesForRegion(int max_lon, int min_lon, int max_lat, int min_lat);

int LoadSplatAlphanumericOutputFile(char *filename);

void WriteKeyholeMarkupLanguageFile(Site source, Site destination);

int main(int argc, char *argv[]);
