/*****************************************************************************
*				Srtm2SdfLib: Functions used by Srtm2Sdf						 *
*****************************************************************************/

#pragma once

#include "stdafx.h"


// Export functions

int _declspec(dllexport)
ReadSpaceShuttleRadarTopographyMissionDataFile(
	char *filename,
	char *elevationProblem,
	char *splatDataFileName,
	int integerPixelsPerDegree,
	int minimumElevation,
	int *maximumLatitudeNorth,
	int *maximumLongitudeWest,
	int *minimumLatitudeNorth,
	int *minimumLongitudeWest,
	int **spaceShuttleRadarTopographyMissionData);

int _declspec(dllexport)
ReadUnitedStatesGeologicalSurveyDataFile(
	char *splatDataFilePath,
	int maximumLatitudeNorth,
	int maximumLongitudeWest,
	int minimumLatitudeNorth,
	int minimumLongitudeWest,
	int **unitedStatesGeologicalSurveyData);

void _declspec(dllexport)
WriteSplatDataFile(
	char *filename,
	int integerPixelsPerDegree,
	int maximumPixelIndexPerDegree,
	int minimumElevation,
	int mergeData,
	int maximumLatitudeNorth,
	int maximumLongitudeWest,
	int minimumLatitudeNorth,
	int minimumLongitudeWest,
	int **spaceShuttleRadarTopographyMissionData,
	int **unitedStatesGeologicalSurveyData);


// Functions

int
LoadUncompressedSplatDataFileForSrtm(
	char *name,
	char *splatDataFilePath,
	int **unitedStatesGeologicalSurveyData);

char
LoadSplatDataFileForSrtm(
	char *name,
	char *splatDataFilePath,
	int **unitedStatesGeologicalSurveyData);

void
AverageTerrain(
	int y,
	int x,
	int z,
	int maximumPixelIndexPerDegree,
	int minimumElevation,
	int **spaceShuttleRadarTopographyMissionData);
