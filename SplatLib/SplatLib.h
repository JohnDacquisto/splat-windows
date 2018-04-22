/****************************************************************************\
*					SplatLib: Functions used by Splat						 *
*****************************************************************************/

#pragma once
#include "stdafx.h"
#include <stdio.h>
#include "DigitalElevationModel.h"
#include "Path.h"
#include "IrregularTerrainModelParameters.h"
#include "ColorRegion.h"
#include "..\Common\Site.h"


//| Export functions

double _declspec(dllexport) LongitudeDifference(double lon1, double lon2);

Site _declspec(dllexport) LoadSplatSiteLocationFile(char *filename);

double _declspec(dllexport) GetSiteLocationElevation(Site location, DigitalElevationModel *digitalElevationModel);

void _declspec(dllexport) PlaceTextAndMarkerDataInMaskArrayForMaps(Site location, DigitalElevationModel *digitalElevationModel, int minimumLatitudeNorth, int maximumLatitudeNorth,
	int minimumLongitudeWest, int maximumLongitudeWest);

void _declspec(dllexport) LoadAndPlotCitiesAndSitesOnMaps(char *filename, DigitalElevationModel *digitalElevationModel, int minimumLatitudeNorth, int maximumLatitudeNorth,
	int minimumLongitudeWest, int maximumLongitudeWest);

void _declspec(dllexport) LoadUserDefinedTerrainFile(char *filename, DigitalElevationModel *digitalElevationModel);

void _declspec(dllexport) LoadCartographicBoundaryFiles(char *filename, DigitalElevationModel *digitalElevationModel, Path *path);

char _declspec(dllexport) ReadLongleyRiceParameterDataForSite(Site txsite, char forced_read, IrregularTerrainModelParameters *itmParameters, unsigned char *gotAntennaElevationAnglePattern,
	unsigned char *gotAntennaAzimuthAnglePattern, double fresnelZoneFrequency, double effectiveRadiatedPower);

void _declspec(dllexport) AnalyzeAndPlotLineOfSightCoverageBetweenSites(Site source, Site destination, char mask_value, FILE *fd, DigitalElevationModel *digitalElevationModel, Path *path,
	double groundClutterHeight, double sphereRadius);

void _declspec(dllexport) PlotLineOfSightCoverageFromSiteAtAltitude(Site source, double altitude, char *plo_filename, DigitalElevationModel *digitalElevationModel, Path *path,
	unsigned char useMetricUnits, int minimumLatitudeNorth, int maximumLatitudeNorth, int minimumLongitudeWest, int maximumLongitudeWest, double groundClutterHeight, double sphereRadius);

void _declspec(dllexport) PlotAttenuationFromSiteAtAltitude(Site source, double altitude, char *plo_filename, DigitalElevationModel *digitalElevationModel, Path *path,
	IrregularTerrainModelParameters *itmParameters, double *pathElevation, char useOldLongleyRiceModel, unsigned char plotSignalPowerLevelContours, unsigned char gotAntennaElevationAnglePattern,
	unsigned char useMetricUnits, int minimumLatitudeNorth, int maximumLatitudeNorth, int minimumLongitudeWest, int maximumLongitudeWest,
	double maximumAnalysisDistance, double groundClutterHeight);

void _declspec(dllexport) WritePortablePixMapLineOfSightCoverageFile(char *filename, unsigned char geo, unsigned char kml, unsigned char ngs, Site *xmtr, unsigned char txsites,
	DigitalElevationModel *digitalElevationModel, int maximumElevation, int minimumElevation, int minimumLatitudeNorth, int maximumLatitudeNorth,
	int minimumLongitudeWest, int maximumLongitudeWest);

void _declspec(dllexport) WritePortablePixMapAttenuationFile(char *filename, unsigned char geo, unsigned char kml, unsigned char ngs, Site *xmtr, unsigned char txsites,
	DigitalElevationModel *digitalElevationModel, ColorRegion *region, unsigned char displaySmoothContourLevels, int contourDisplayThreshold,
	int maximumElevation, int minimumElevation, int minimumLatitudeNorth, int maximumLatitudeNorth, int minimumLongitudeWest, int maximumLongitudeWest);

void _declspec(dllexport) WritePortablePixMapSignalStrengthFile(char *filename, unsigned char geo, unsigned char kml, unsigned char ngs, Site *xmtr, unsigned char txsites,
	DigitalElevationModel *digitalElevationModel, ColorRegion *region, unsigned char displaySmoothContourLevels, int contourDisplayThreshold,
	int maximumElevation, int minimumElevation, int minimumLatitudeNorth, int maximumLatitudeNorth, int minimumLongitudeWest, int maximumLongitudeWest);

void _declspec(dllexport) WritePortablePixMapPowerLevelFile(char *filename, unsigned char geo, unsigned char kml, unsigned char ngs, Site *xmtr, unsigned char txsites,
	DigitalElevationModel *digitalElevationModel, ColorRegion *region, unsigned char displaySmoothContourLevels, int contourDisplayThreshold,
	int maximumElevation, int minimumElevation, int minimumLatitudeNorth, int maximumLatitudeNorth, int minimumLongitudeWest, int maximumLongitudeWest);

void _declspec(dllexport) GenerateGnuPlotTerrainProfileBetweenSites(Site source, Site destination, char *name, DigitalElevationModel *digitalElevationModel, Path *path,
	char saveGnuPlotWorkingFiles, unsigned char useMetricUnits, double groundClutterHeight, char *reportSavePath);

void _declspec(dllexport) GenerateGnuPlotElevationProfileBetweenSites(Site source, Site destination, char *name, DigitalElevationModel *digitalElevationModel, Path *path,
	char saveGnuPlotWorkingFiles, unsigned char useMetricUnits, double groundClutterHeight, double sphereRadius, char *reportSavePath);

void _declspec(dllexport) GenerateGnuPlotHeightProfileBetweenSites(Site source, Site destination, char *name, unsigned char fresnel_plot, unsigned char normalized,
	DigitalElevationModel *digitalElevationModel, Path *path, IrregularTerrainModelParameters *itmParameters, char saveGnuPlotWorkingFiles,
	unsigned char useMetricUnits, double groundClutterHeight, double fresnelZoneClearanceRatio, double sphereRadius, char *reportSavePath);

void _declspec(dllexport) WriteSplatPathReport(Site source, Site destination, char *name, char graph_it, DigitalElevationModel *digitalElevationModel, Path *path,
	IrregularTerrainModelParameters *itmParameters, double *pathElevation, char useOldLongleyRiceModel, char saveGnuPlotWorkingFiles,
	unsigned char gotAntennaElevationAnglePattern, unsigned char gotAntennaAzimuthAnglePattern, unsigned char useMetricUnits,
	double groundClutterHeight, double fresnelZoneClearanceRatio, double sphereRadius, char *reportSavePath);

void _declspec(dllexport) WriteSplatSiteReport(Site xmtr, DigitalElevationModel *digitalElevationModel, Path *path, unsigned char useMetricUnits,
	double groundClutterHeight, char *reportSavePath);

void _declspec(dllexport) LoadSplatDataFilesForRegion(int max_lon, int min_lon, int max_lat, int min_lat, DigitalElevationModel *digitalElevationModel,
	int *maximumElevation, int *minimumElevation, int *minimumLatitudeNorth, int *maximumLatitudeNorth,
	int *minimumLongitudeWest, int *maximumLongitudeWest, char *splatDataFilePath);

int _declspec(dllexport) LoadSplatAlphanumericOutputFile(char *filename, DigitalElevationModel *digitalElevationModel, IrregularTerrainModelParameters *itmParameters,
	unsigned char plotSignalPowerLevelContours, int contourDisplayThreshold, int *maximumElevation, int *minimumElevation,
	int *minimumLatitudeNorth, int *maximumLatitudeNorth, int *minimumLongitudeWest, int *maximumLongitudeWest, char *splatDataFilePath);

void _declspec(dllexport) WriteKeyholeMarkupLanguageFile(Site source, Site destination, DigitalElevationModel *digitalElevationModel, Path *path,
	unsigned char useMetricUnits, double sphereRadius);


//| Functions

int LinearInterpolation(int y0, int y1, int x0, int x1, int n);

double ArcCosine(double x, double y);

int ConvertToNormalizedAngle(double angle);

void ConvertDecimalToDegreesMinutesSeconds(double decimal, char *dmsString);

double GreatCircleDistanceBetweenSiteLocations(Site site1, Site site2);

double AzimuthAngleBetweenSites(Site source, Site destination);

double BearingStringToDecimalDegrees(char *input);

int SetValueInDigitalElevationModelMask(double lat, double lon, int value, DigitalElevationModel *digitalElevationModel);

int SetOrValueInDigitalElevationModelMask(double lat, double lon, int value, DigitalElevationModel *digitalElevationModel);

int GetValueInDigitalElevationModelMask(double lat, double lon, DigitalElevationModel *digitalElevationModel);

int SetValueInDigitalElevationModelSignal(double lat, double lon, unsigned char signal, DigitalElevationModel *digitalElevationModel);

unsigned char GetValueInDigitalElevationModelSignal(double lat, double lon, DigitalElevationModel* digitalElevationModel);

int AddUserDefinedTerrainToDigitalElevationModelData(double lat, double lon, double height, DigitalElevationModel *digitalElevationModel);

double ElevationAngleBetweenSites(Site source, Site destination, DigitalElevationModel *digitalElevationModel, double sphereRadius);

void GeneratePathBetweenSites(Site source, Site destination, DigitalElevationModel *digitalElevationModel, Path *path);

double ObstructedElevationAngleBetweenSites(Site source, Site destination, double er, DigitalElevationModel *digitalElevationModel, Path *path,
	double groundClutterHeight, double sphereRadius);

double AverageTerrainOverDistanceAtAzimuthFromSite(Site source, double azimuthx, double start_distance, double end_distance,
	DigitalElevationModel *digitalElevationModel, Path *path, double groundClutterHeight);

double AntennaHeightAboveAverageTerrain(Site antenna, DigitalElevationModel *digitalElevationModel, Path *path, double groundClutterHeight);

void LoadAntennaAzimuthElevationPatternFiles(char *filename, IrregularTerrainModelParameters *itmParameters, unsigned char *gotAntennaElevationAnglePattern,
	unsigned char *gotAntennaAzimuthAnglePattern);

int LoadUncompressedSplatDataFile(char *name, DigitalElevationModel *digitalElevationModel, int *maximumElevation, int *minimumElevation,
	int *minimumLatitudeNorth, int *maximumLatitudeNorth, int *minimumLongitudeWest, int *maximumLongitudeWest, char *splatDataFilePath);

char LoadSplatDataFile(char *name, DigitalElevationModel *digitalElevationModel, int *maximumElevation, int *minimumElevation,
	int *minimumLatitudeNorth, int *maximumLatitudeNorth, int *minimumLongitudeWest, int *maximumLongitudeWest, char *splatDataFilePath);

void AnalyzeAndPlotPathLossBetweenSites(Site source, Site destination, unsigned char mask_value, FILE *fd, DigitalElevationModel *digitalElevationModel,
	Path *path, IrregularTerrainModelParameters *itmParameters, double *pathElevation, char useOldLongleyRiceModel, unsigned char plotSignalPowerLevelContours, unsigned char gotAntennaElevationAnglePattern,
	double maximumAnalysisDistance, double groundClutterHeight);

void LoadSplatSignalColorsForSite(Site xmtr, ColorRegion *region);

void LoadSplatLossColorsForSite(Site xmtr, ColorRegion *region);

void LoadSplatPowerColorsForSite(Site xmtr, ColorRegion *region);

void PerformObstructionAnalysisBetweenSites(Site xmtr, Site rcvr, double f, FILE *outfile, DigitalElevationModel *digitalElevationModel, Path *path,
	unsigned char useMetricUnits, double groundClutterHeight, double fresnelZoneClearanceRatio, double sphereRadius);
