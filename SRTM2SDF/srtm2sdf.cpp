/**************************************************************\
**						Srtm2Sdf							  **
**     Created originally by Jonathan Naylor, G4KLX.          **
**     Later embellished by John Magliacane, KD2BD to         **
**     detect and handle voids found in the SRTM data,        **
**     SRTM-3 data in .BIL and .HGT format, and high          **
**     resolution SRTM-1 one arc-second topography data.      **
****************************************************************
**                    Compile like this:                      **
**      cc -Wall -O3 -s -lbz2 srtm2sdf.c -o srtm2sdf          **
**              Last modification: 08-Jan-2014                **
\**************************************************************/

#include "stdafx.h"
#include <stdlib.h>
#include <string.h>
#include "srtm2sdf.h"
#include "..\Common\constants.h"
#include "..\Common\Common.h"
#include "..\srtm2sdfLib\srtm2sdfLib.h"


//| ------------------------------
//| 
//| FUNCTION: main
//| 
//| OLD NAME: main
//| 
//| NOTES: 
//| 
//| ------------------------------
int
main(
	int argc,
	char *argv[])
{
	SetUnhandledExceptionFilter(CommonUnhandledExceptionFilter);

	int x, y, z = 0;
	char *env = NULL, SrtmString[255];
	errno_t err;

	char elevationProblem;			// OLD NAME: replacement_flag
	char splatDataFileName[30];		// OLD NAME: sdf_filename
	char splatDataFilePath[255];	// OLD NAME: sdf_path

	int integerPixelsPerDegree;		// OLD NAME: ippd
	int maximumPixelIndexPerDegree;	// OLD NAME: mpi
	int minimumElevation;			// OLD NAME: min_elevation
	int mergeData = 0;				// OLD NAME: merge

	int maximumLatitudeNorth;		// OLD NAME: max_north
	int maximumLongitudeWest;		// OLD NAME: max_west
	int minimumLatitudeNorth;		// OLD NAME: min_north
	int minimumLongitudeWest;		// OLD NAME: min_west

	int	**spaceShuttleRadarTopographyMissionData = new int*[PIXELS_PER_DEGREE_HIGH_DEF + 1];	// OLD NAME: srtm
	for (int i = 0; i < (PIXELS_PER_DEGREE_HIGH_DEF + 1); i++)
	{
		spaceShuttleRadarTopographyMissionData[i] = new int[PIXELS_PER_DEGREE_HIGH_DEF + 1];
	}

	int **unitedStatesGeologicalSurveyData = new int*[PIXELS_PER_DEGREE_STANDARD + 1];		// OLD NAME: usgs
	for (int i = 0; i < (PIXELS_PER_DEGREE_STANDARD + 1); i++)
	{
		unitedStatesGeologicalSurveyData[i] = new int[PIXELS_PER_DEGREE_STANDARD + 1];
	}

	if (strstr(argv[0], "srtm2sdf-hd") != NULL)
	{
		integerPixelsPerDegree = PIXELS_PER_DEGREE_HIGH_DEF;	// High Definition (1 arc-sec) Mode
		err = strncpy_s(SrtmString, _countof(SrtmString), "srtm2sdf-hd\0", 12);
	}
	else
	{
		integerPixelsPerDegree = PIXELS_PER_DEGREE_STANDARD;	// Standard Definition (3 arc-sec) Mode
		err = strncpy_s(SrtmString, _countof(SrtmString), "srtm2sdf\0", 9);
	}

	maximumPixelIndexPerDegree = integerPixelsPerDegree - 1;	// Maximum pixel index per degree

	if (argc == 1 || ((argc == 2) && (strncmp(argv[1], "-h", 2) == 0)))
	{
		if (integerPixelsPerDegree == PIXELS_PER_DEGREE_STANDARD)
		{
			fprintf(stderr, "\nsrtm2sdf: Generates SPLAT elevation data files from unzipped\nSRTM-3 elevation data files, and replaces SRTM data voids with\nelevation data from older usgs2sdf derived SDF files.\n\n");
		}

		if (integerPixelsPerDegree == PIXELS_PER_DEGREE_HIGH_DEF)
		{
			fprintf(stderr, "\nsrtm2sdf-hd: Generates SPLAT elevation data files from unzipped\nSRTM-1 elevation data files, and replaces SRTM data voids with\naverages, or elevation data from older usgs2sdf derived SDF files.\n\n");
		}

		fprintf(stderr, "\tAvailable Options...\n\n");
		fprintf(stderr, "\t-d directory path of usgs2sdf-derived SDF files\n\n");
		fprintf(stderr, "\t-n elevation limit (in meters) below which SRTM data is\n\t    replaced by USGS-derived .sdf data (default = 0 meters)\n\n");
		fprintf(stderr, "Examples: %s N40W074.hgt\n", SrtmString);
		fprintf(stderr, "          %s -d /cdrom/sdf N40W074.hgt\n", SrtmString);
		fprintf(stderr, "          %s -d /dev/null N40W074.hgt  (prevents data replacement)\n", SrtmString);
		fprintf(stderr, "          %s -n -5 N40W074.hgt\n\n", SrtmString);

		return 1;
	}

	y = argc - 1;

	minimumElevation = 0;
	splatDataFilePath[0] = 0;

	for (x = 1, z = 0; x <= y; x++)
	{
		// -d directory path of usgs2sdf-derived SDF files
		if (strcmp(argv[x], "-d") == 0)
		{
			z = x + 1;

			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				err = strncpy_s(splatDataFilePath, _countof(splatDataFilePath), argv[z], 253);
			}
		}

		// -n elevation limit (in meters) below which SRTM data is replaced by USGS-derived .sdf data (default = 0 meters)
		if (strcmp(argv[x], "-n") == 0)
		{
			z = x + 1;

			if ((z <= y) && argv[z][0])
			{
				sscanf_s(argv[z], "%d", &minimumElevation);

				if (minimumElevation < -32767)
				{
					minimumElevation = 0;
				}
			}
		}
	}

	// Ensure a trailing '\\' is present in splatDataFilePath

	if (splatDataFilePath[0])
	{
		x = (int)strlen(splatDataFilePath);

		if ((splatDataFilePath[x - 1] != '\\') && (x != 0))
		{
			splatDataFilePath[x] = '\\';
			splatDataFilePath[x + 1] = 0;
		}
	}

	// Process multiple SRTM files per call.

	while (argv[z + 1] != 0)
	{
		if (ReadSpaceShuttleRadarTopographyMissionDataFile(argv[z + 1], &elevationProblem, splatDataFileName, 
				integerPixelsPerDegree, minimumElevation, &maximumLatitudeNorth, &maximumLongitudeWest, &minimumLatitudeNorth,
				&minimumLongitudeWest, spaceShuttleRadarTopographyMissionData) == 0)
		{
			if (elevationProblem && splatDataFilePath[0])
			{
				mergeData = ReadUnitedStatesGeologicalSurveyDataFile(splatDataFilePath, maximumLatitudeNorth, maximumLongitudeWest, 
					minimumLatitudeNorth, minimumLongitudeWest, unitedStatesGeologicalSurveyData);
			}

			// Write SDFs to user-specified directory.
			char sdfPathAndName[285];
			sprintf_s(sdfPathAndName, _countof(sdfPathAndName), "%s%s", splatDataFilePath, splatDataFileName);
			WriteSplatDataFile(sdfPathAndName, integerPixelsPerDegree, maximumPixelIndexPerDegree, minimumElevation, mergeData,
				maximumLatitudeNorth, maximumLongitudeWest, minimumLatitudeNorth, minimumLongitudeWest,
				spaceShuttleRadarTopographyMissionData, unitedStatesGeologicalSurveyData);
		}
		z++;
	}

	for (int i = 0; i < (PIXELS_PER_DEGREE_HIGH_DEF + 1); i++)
	{
		delete[] spaceShuttleRadarTopographyMissionData[i];
		spaceShuttleRadarTopographyMissionData[i] = NULL;
	}
	delete[] spaceShuttleRadarTopographyMissionData;
	spaceShuttleRadarTopographyMissionData = NULL;

	for (int i = 0; i < (PIXELS_PER_DEGREE_STANDARD + 1); i++)
	{
		delete[] unitedStatesGeologicalSurveyData[i];
		unitedStatesGeologicalSurveyData[i] = NULL;
	}
	delete[] unitedStatesGeologicalSurveyData;
	unitedStatesGeologicalSurveyData = NULL;

	return 0;
}
