/***************************************************************************\
*     Bearing: Determines distance and azimuth bearing between locations    *
*                     specified in a pair of .qth files                     *
*						Last update: 08-Jan-2014							*
*****************************************************************************
*      Project started on December 7, 2007 by John A. Magliacane, KD2BD     *
*****************************************************************************
*                                                                           *
* This program is free software; you can redistribute it and/or modify it   *
* under the terms of the GNU General Public License as published by the     *
* Free Software Foundation; either version 2 of the License or any later    *
* version.																	*
*																			*
* This program is distributed in the hope that it will useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License      *
* for more details.															*
*																			*
*****************************************************************************
*      gcc -Wall -O3 -s -lm -fomit-frame-pointer bearing.c -o bearing       *
\***************************************************************************/

#include "stdafx.h"
#include <stdlib.h>
#include <string.h>
#include "bearing.h"
#include "..\Common\constants.h"
#include "..\Common\Common.h"
#include "..\BearingLib\BearingLib.h"


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

	int x, y, sitenum = 0;
	bool metric = false, error = false;
	double distance, azimuth;
	Site point[2];

	if (argc == 1)
	{
		fprintf(stdout, "\nProvides distance and azimuth bearing between two site locations.\n");
		fprintf(stdout, "\nUsage: bearing site1(.qth) site2(.qth)");
		fprintf(stdout, "\n       bearing site1(.qth) site2(.qth) -metric\n\n");
		fflush(stdout);
		return 1;
	}

	y = argc - 1;

	point[0].latitude = 91.0;
	point[0].longitude = 361.0;
	point[1].latitude = 91.0;
	point[1].longitude = 361.0;

	// Scan for command line arguments

	for (x = 1; x <= y; x++)
	{
		if (strcmp(argv[x], "-metric") == 0)
		{
			metric = true;
			x++;
		}

		// Read Receiver Location

		if ((x <= y) && argv[x][0] && (argv[x][0] != '-') && (sitenum < 2))
		{
			point[sitenum] = LoadSplatSiteLocationFileForBearing(argv[x]);

			if ((point[sitenum].latitude > 90.0) || (point[sitenum].longitude > 360.0))
			{
				fprintf(stderr, "\n%c*** \"%s\" is not a valid .qth file", 7, argv[x]);
			}
			else
			{
				sitenum++;
			}
		}
	}

	if (sitenum < 2)
	{
		fprintf(stderr, "\n%c*** ERROR: Not enough valid sites specified\n\n", 7);
		exit(-1);
	}

	for (x = 0; x < sitenum; x++)
	{
		if ((point[x].latitude > 90.0) && (point[x].longitude > 360.0))
		{
			fprintf(stderr, "\n*** ERROR: site #%d not found", x + 1);
			error = true;
		}
	}

	if (error)
	{
		return -1;
	}
	else
	{
		// Perform the calculations and display the results

		distance = GreatCircleDistanceBetweenSiteLocations(point[0], point[1]);
		azimuth = AzimuthAngleBetweenSites(point[0], point[1]);

		printf("\nThe distance between %s and %s is\n", point[0].name, point[1].name);

		if (metric)
		{
			printf("%.2f kilometers", distance*KM_PER_MILE);
		}
		else
		{
			printf("%.2f miles", distance);
		}
		printf(" at a bearing of %.2f%c azimuth.\n\n", azimuth, 176);
	}

	return 0;
}
