/****************************************************************************\
*					SplatLib: Functions used by Splat						 *
*****************************************************************************/

#pragma once
#include "stdafx.h"
#include <stdio.h>
#include "DigitalElevationModel.h"
#include "DigitalElevationModelWrapper.h"
#include "Path.h"
#include "IrregularTerrainModelParameters.h"
#include "ColorRegion.h"
#include "..\Common\Site.h"


//| Export functions

int _declspec(dllexport) LinearInterpolation(int y0, int y1, int x0, int x1, int n);

double _declspec(dllexport) LongitudeDifference(double lon1, double lon2);

int _declspec(dllexport) ConvertToNormalizedAngle(double angle);

void _declspec(dllexport) ConvertDecimalToDegreesMinutesSeconds(double decimal, char *dmsString);

Site _declspec(dllexport) LoadSplatSiteLocationFile(char *filename);

double _declspec(dllexport) GetSiteLocationElevation(Site location, DigitalElevationModelWrapper *digitalElevationModelWrapper);

void _declspec(dllexport) PlaceTextAndMarkerDataInMaskArrayForMaps(Site location, DigitalElevationModelWrapper *digitalElevationModelWrapper, int minimumLatitudeNorth, int maximumLatitudeNorth,
	int minimumLongitudeWest, int maximumLongitudeWest);

void _declspec(dllexport) LoadAndPlotCitiesAndSitesOnMaps(char *filename, DigitalElevationModelWrapper *digitalElevationModelWrapper, int minimumLatitudeNorth, int maximumLatitudeNorth,
	int minimumLongitudeWest, int maximumLongitudeWest);

void _declspec(dllexport) LoadUserDefinedTerrainFile(char *filename, DigitalElevationModelWrapper *digitalElevationModelWrapper);

void _declspec(dllexport) LoadCartographicBoundaryFiles(char *filename, DigitalElevationModelWrapper *digitalElevationModelWrapper, Path *path);

char _declspec(dllexport) ReadLongleyRiceParameterDataForSite(Site txsite, char forced_read, IrregularTerrainModelParameters *itmParameters, unsigned char *gotAntennaElevationAnglePattern,
	unsigned char *gotAntennaAzimuthAnglePattern, double fresnelZoneFrequency, double effectiveRadiatedPower, char *lrpFilePath);

void _declspec(dllexport) AnalyzeAndPlotLineOfSightCoverageBetweenSites(Site source, Site destination, char mask_value, FILE *fd, DigitalElevationModelWrapper *digitalElevationModelWrapper, Path *path,
	double groundClutterHeight, double sphereRadius);

void _declspec(dllexport) PlotLineOfSightCoverageFromSiteAtAltitude(Site source, double altitude, char *plo_filename, DigitalElevationModelWrapper *digitalElevationModelWrapper, Path *path,
	unsigned char useMetricUnits, int minimumLatitudeNorth, int maximumLatitudeNorth, int minimumLongitudeWest, int maximumLongitudeWest, double groundClutterHeight, double sphereRadius);

void _declspec(dllexport) PlotAttenuationFromSiteAtAltitude(Site source, double altitude, char *plo_filename, DigitalElevationModelWrapper *digitalElevationModelWrapper, Path *path,
	IrregularTerrainModelParameters *itmParameters, double *pathElevation, char useOldLongleyRiceModel, unsigned char plotSignalPowerLevelContours, unsigned char gotAntennaElevationAnglePattern,
	unsigned char useMetricUnits, int minimumLatitudeNorth, int maximumLatitudeNorth, int minimumLongitudeWest, int maximumLongitudeWest,
	double maximumAnalysisDistance, double groundClutterHeight);

void _declspec(dllexport) WritePortablePixMapLineOfSightCoverageFile(char *filename, unsigned char geo, unsigned char kml, unsigned char ngs, Site *xmtr, unsigned char txsites,
	DigitalElevationModelWrapper *digitalElevationModelWrapper, int maximumElevation, int minimumElevation, int minimumLatitudeNorth, int maximumLatitudeNorth,
	int minimumLongitudeWest, int maximumLongitudeWest);

void _declspec(dllexport) WritePortablePixMapAttenuationFile(char *filename, unsigned char geo, unsigned char kml, unsigned char ngs, Site *xmtr, unsigned char txsites,
	DigitalElevationModelWrapper *digitalElevationModelWrapper, ColorRegion *region, unsigned char displaySmoothContourLevels, int contourDisplayThreshold,
	int maximumElevation, int minimumElevation, int minimumLatitudeNorth, int maximumLatitudeNorth, int minimumLongitudeWest, int maximumLongitudeWest);

void _declspec(dllexport) WritePortablePixMapSignalStrengthFile(char *filename, unsigned char geo, unsigned char kml, unsigned char ngs, Site *xmtr, unsigned char txsites,
	DigitalElevationModelWrapper *digitalElevationModelWrapper, ColorRegion *region, unsigned char displaySmoothContourLevels, int contourDisplayThreshold,
	int maximumElevation, int minimumElevation, int minimumLatitudeNorth, int maximumLatitudeNorth, int minimumLongitudeWest, int maximumLongitudeWest);

void _declspec(dllexport) WritePortablePixMapPowerLevelFile(char *filename, unsigned char geo, unsigned char kml, unsigned char ngs, Site *xmtr, unsigned char txsites,
	DigitalElevationModelWrapper *digitalElevationModelWrapper, ColorRegion *region, unsigned char displaySmoothContourLevels, int contourDisplayThreshold,
	int maximumElevation, int minimumElevation, int minimumLatitudeNorth, int maximumLatitudeNorth, int minimumLongitudeWest, int maximumLongitudeWest);

void _declspec(dllexport) GenerateGnuPlotTerrainProfileBetweenSites(Site source, Site destination, char *name, DigitalElevationModelWrapper *digitalElevationModelWrapper, Path *path,
	char saveGnuPlotWorkingFiles, unsigned char useMetricUnits, double groundClutterHeight, char *reportSavePath);

void _declspec(dllexport) GenerateGnuPlotElevationProfileBetweenSites(Site source, Site destination, char *name, DigitalElevationModelWrapper *digitalElevationModelWrapper, Path *path,
	char saveGnuPlotWorkingFiles, unsigned char useMetricUnits, double groundClutterHeight, double sphereRadius, char *reportSavePath);

void _declspec(dllexport) GenerateGnuPlotHeightProfileBetweenSites(Site source, Site destination, char *name, unsigned char fresnel_plot, unsigned char normalized,
	DigitalElevationModelWrapper *digitalElevationModelWrapper, Path *path, IrregularTerrainModelParameters *itmParameters, char saveGnuPlotWorkingFiles,
	unsigned char useMetricUnits, double groundClutterHeight, double fresnelZoneClearanceRatio, double sphereRadius, char *reportSavePath);

void _declspec(dllexport) WriteSplatPathReport(Site source, Site destination, char *name, char graph_it, DigitalElevationModelWrapper *digitalElevationModelWrapper, Path *path,
	IrregularTerrainModelParameters *itmParameters, double *pathElevation, char useOldLongleyRiceModel, char saveGnuPlotWorkingFiles,
	unsigned char gotAntennaElevationAnglePattern, unsigned char gotAntennaAzimuthAnglePattern, unsigned char useMetricUnits,
	double groundClutterHeight, double fresnelZoneClearanceRatio, double sphereRadius, char *reportSavePath);

void _declspec(dllexport) WriteSplatSiteReport(Site xmtr, DigitalElevationModelWrapper *digitalElevationModelWrapper, Path *path, unsigned char useMetricUnits,
	double groundClutterHeight, char *reportSavePath);

void _declspec(dllexport) LoadSplatDataFilesForRegion(int max_lon, int min_lon, int max_lat, int min_lat, DigitalElevationModelWrapper *digitalElevationModelWrapper,
	int *maximumElevation, int *minimumElevation, int *minimumLatitudeNorth, int *maximumLatitudeNorth,
	int *minimumLongitudeWest, int *maximumLongitudeWest, char *splatDataFilePath);

int _declspec(dllexport) LoadSplatAlphanumericOutputFile(char *filename, DigitalElevationModelWrapper *digitalElevationModelWrapper, IrregularTerrainModelParameters *itmParameters,
	unsigned char plotSignalPowerLevelContours, int contourDisplayThreshold, int *maximumElevation, int *minimumElevation,
	int *minimumLatitudeNorth, int *maximumLatitudeNorth, int *minimumLongitudeWest, int *maximumLongitudeWest, char *splatDataFilePath);

void _declspec(dllexport) WriteKeyholeMarkupLanguageFile(Site source, Site destination, DigitalElevationModelWrapper *digitalElevationModelWrapper, Path *path,
	unsigned char useMetricUnits, double sphereRadius);


//| Functions

int SetValueInDigitalElevationModelMask(double latitude, double longitude, int value, DigitalElevationModelWrapper *digitalElevationModelWrapper);

int SetOrValueInDigitalElevationModelMask(double latitude, double longitude, int value, DigitalElevationModelWrapper *digitalElevationModelWrapper);

int GetValueInDigitalElevationModelMask(double latitude, double longitude, DigitalElevationModelWrapper *digitalElevationModelWrapper);

int SetValueInDigitalElevationModelSignal(double latitude, double longitude, unsigned char signal, DigitalElevationModelWrapper *digitalElevationModelWrapper);

unsigned char GetValueInDigitalElevationModelSignal(double latitude, double longitude, DigitalElevationModelWrapper *digitalElevationModelWrapper);

int AddUserDefinedTerrainToDigitalElevationModelData(double latitude, double longitude, double height, DigitalElevationModelWrapper *digitalElevationModelWrapper);

double ElevationAngleBetweenSites(Site source, Site destination, DigitalElevationModelWrapper *digitalElevationModelWrapper, double sphereRadius);

void GeneratePathBetweenSites(Site source, Site destination, DigitalElevationModelWrapper *digitalElevationModelWrapper, Path *path);

double ObstructedElevationAngleBetweenSites(Site source, Site destination, double er, DigitalElevationModelWrapper *digitalElevationModelWrapper, Path *path,
	double groundClutterHeight, double sphereRadius);

double AverageTerrainOverDistanceAtAzimuthFromSite(Site source, double azimuthx, double start_distance, double end_distance,
	DigitalElevationModelWrapper *digitalElevationModelWrapper, Path *path, double groundClutterHeight);

double AntennaHeightAboveAverageTerrain(Site antenna, DigitalElevationModelWrapper *digitalElevationModelWrapper, Path *path, double groundClutterHeight);

void LoadAntennaAzimuthElevationPatternFiles(char *filename, IrregularTerrainModelParameters *itmParameters, unsigned char *gotAntennaElevationAnglePattern,
	unsigned char *gotAntennaAzimuthAnglePattern);

int LoadUncompressedSplatDataFile(char *name, DigitalElevationModelWrapper *digitalElevationModelWrapper, int *maximumElevation, int *minimumElevation,
	int *minimumLatitudeNorth, int *maximumLatitudeNorth, int *minimumLongitudeWest, int *maximumLongitudeWest, char *splatDataFilePath);

char LoadSplatDataFile(char *name, DigitalElevationModelWrapper *digitalElevationModelWrapper, int *maximumElevation, int *minimumElevation,
	int *minimumLatitudeNorth, int *maximumLatitudeNorth, int *minimumLongitudeWest, int *maximumLongitudeWest, char *splatDataFilePath);

void AnalyzeAndPlotPathLossBetweenSites(Site source, Site destination, unsigned char mask_value, FILE *fd, DigitalElevationModelWrapper *digitalElevationModelWrapper,
	Path *path, IrregularTerrainModelParameters *itmParameters, double *pathElevation, char useOldLongleyRiceModel, unsigned char plotSignalPowerLevelContours, unsigned char gotAntennaElevationAnglePattern,
	double maximumAnalysisDistance, double groundClutterHeight);

void LoadSplatSignalColorsForSite(Site xmtr, ColorRegion *region);

void LoadSplatLossColorsForSite(Site xmtr, ColorRegion *region);

void LoadSplatPowerColorsForSite(Site xmtr, ColorRegion *region);

void PerformObstructionAnalysisBetweenSites(Site xmtr, Site rcvr, double f, FILE *outfile, DigitalElevationModelWrapper *digitalElevationModelWrapper, Path *path,
	unsigned char useMetricUnits, double groundClutterHeight, double fresnelZoneClearanceRatio, double sphereRadius);
