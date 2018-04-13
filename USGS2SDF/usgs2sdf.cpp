/****************************************************************************
*            USGS2SDF: USGS to SPLAT Data File Converter Utility            *
*               Copyright John A. Magliacane, KD2BD 1997-2014               *
*                         Last update: 08-Mar-2014                          *
*****************************************************************************
*                                                                           *
* This program reads files containing delimited US Geological Survey        *
* Digital Elevation Model Data files, and creates Splat Data Files          *
* containing ONLY the raw information needed by SPLAT, thereby saving       *
* disk space, as well as read/write time.                                   *
*                                                                           *
* The format of .sdf files created by this utility is as follows:           *
*                                                                           *
*	maximum west longitude (degrees West)									*
*	minimum north latitude (degrees North)									*
*	minimum west longitude (degrees West)									*
*	maximum north latitude (degrees North)									*
*       ...1440000 elevation points... (1200x1200)                          *
*                                                                           *
* All data is represented as integers. A single '\n' follows each value.    *
*                                                                           *
* SPLAT Data Files are named according to the geographical locations        *
* they represent (ie: min_north:max_north:min_west:max_west.sdf).           *
*                                                                           *
*****************************************************************************
*          To compile: gcc -Wall -O6 -s splat2sdf.c -o splat2sdf            *
*****************************************************************************
*                                                                           *
* This program is free software; you can redistribute it and/or modify it   *
* under the terms of the GNU General Public License as published by the     *
* Free Software Foundation; either version 2 of the License or any later    *
* version.                                                                  *
*                                                                           *
* This program is distributed in the hope that it will useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License      *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usgs2sdf.h"


//| ------------------------------
//| 
//| FUNCTION: ConvertExponentialNotation
//| 
//| OLD NAME: d2e
//| 
//| NOTES: 
//|   This function is used to replace 'D's with 'E's for proper
//|   exponential notation of numeric strings read from delimited
//|   USGS data files. It returns a pointer to a string.
//| 
//| ------------------------------
char 
*ConvertExponentialNotation
   (char *UsgsString)
{
	unsigned char x;

	for (x = 0; UsgsString[x] != 0; x++)
	{
		if (UsgsString[x] == 'D')
		{
			UsgsString[x] = 'E';
		}
	}

	return (UsgsString);
}


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
main
   (int argc, 
	char *argv[])
{
	unsigned char minimum[30], maximum[30], swlong[30],
		swlat[30], nelong[30], nelat[30];
	//unsigned char nwlat[30], nwlong[30], selong[30], selat[30];
	char MainString[40], UsgsString[255];
	double max_el, min_el, max_west, min_west, max_north, min_north;
	int x, y, z, c;
	int x1, x2, y1, z1;
	char *env = NULL;
	size_t envLen;
	char splatfile[25];
	FILE *fd;
	FILE *fd1;
	errno_t err;

	err = strncpy_s(UsgsString, _countof(UsgsString), "usgs2sdf\0", 9);

	if (argc == 1)
	{
		fprintf(stderr, "\nusgs2sdf: Generates SPLAT elevation data files from unzipped\nUSGS elevation data files.\n\n");
		fprintf(stderr, "\tAvailable Options...\n\n");
		fprintf(stderr, "\t-d output directory path\n\t    (overrides path in ~/.splat_path file)\n\n");
		fprintf(stderr, "Usage: usgs2sdf uncompressed_delimited_usgs_datafile (ie: wilmington-e)\n");
		exit(0);
	}

	//| Issue with stack overflow, allocate big array on heap instead.
	int** intArray = new int*[1202];	//| OLD NAME: array
	for (int i = 0; i < 1202; i++)
	{
		intArray[i] = new int[1202];
	}

	y1 = argc - 1;

	SplatDataFilePath[0] = 0;

	for (x1 = 1, z1 = 0; x1 <= y1; x1++)
	{
		//| -d output directory path
		if (strcmp(argv[x1], "-d") == 0)
		{
			z1 = x1 + 1;
			if ((z1 <= y1) && argv[z1][0] && (argv[z1][0] != '-'))
			{
				err = strncpy_s(SplatDataFilePath, _countof(SplatDataFilePath), argv[z1], 253);
			}
		}
	}

	//| If no SDF path was specified on the command line (-d), check
	//| for a path specified in the $HOME/.splat_path file. If the
	//| file is not found, then SplatDataFilePath[] remains NULL.

	if (SplatDataFilePath[0] == 0)
	{
		char* hpath;
		size_t hpathLen;
		err = _dupenv_s(&env, &envLen, "HOMEPATH");
		err = _dupenv_s(&hpath, &hpathLen, "HOMEDRIVE");
		sprintf_s(UsgsString, _countof(UsgsString), "%s%s\\.splat_path", hpath, env); // JAD TODO
		err = fopen_s(&fd1, UsgsString, "r");
		if (fd1 != NULL)
		{
			fgets(UsgsString, 253, fd1);

			//| Remove <CR> and/or <LF> from string

			for (x2 = 0; (UsgsString[x2] != 13) && (UsgsString[x2] != 10) && (UsgsString[x2] != 0) && (x2 < 253); x2++);
			UsgsString[x2] = 0;

			err = strncpy_s(SplatDataFilePath, _countof(SplatDataFilePath), UsgsString, 253);

			fclose(fd1);
		}
	}

	//| Ensure a trailing '\\' is present in SplatDataFilePath

	if (SplatDataFilePath[0])
	{
		x2 = (int)strlen(SplatDataFilePath);

		if ((SplatDataFilePath[x2 - 1] != '\\') && (x2 != 0))
		{
			SplatDataFilePath[x2] = '\\';
			SplatDataFilePath[x2 + 1] = 0;
		}
	}

	//| Process multiple USGS files per call.
	while (argv[z1 + 1] != 0)
	{
		err = fopen_s(&fd, argv[z1 + 1], "rb");

		if (fd != NULL)
		{
			fprintf(stdout, "Reading \"%s\"...", argv[z1 + 1]);
			fflush(stdout);

			//| Skip first 548 bytes

			for (x = 0; x < 548; x++)
			{
				getc(fd);
			}

			//| Read quadrangle corners

			//| Read southwest longitude

			for (x = 0; x < 22; x++)
			{
				swlong[x] = getc(fd);
			}
			swlong[x] = 0;

			//| Skip 2 bytes

			for (x = 0; x < 2; x++)
			{
				getc(fd);
			}

			//| Read southwest latitude

			for (x = 0; x < 22; x++)
			{
				swlat[x] = getc(fd);
			}
			swlat[x] = 0;

			//| Skip 2 bytes

			for (x = 0; x < 2; x++)
			{
				getc(fd);
			}

			//| Read northwest longitude

			//for (x = 0; x < 22; x++)
			//{
			//	nwlong[x] = getc(fd);
			//}
			//nwlong[x] = 0;

			for (x = 0; x < 22; x++)
			{
				getc(fd);
			}

			//| Skip 2 bytes

			for (x = 0; x < 2; x++)
			{
				getc(fd);
			}

			//| Read northwest latitude

			//for (x = 0; x < 22; x++)
			//{
			//	nwlat[x] = getc(fd);
			//}
			//nwlat[x] = 0;

			for (x = 0; x < 22; x++)
			{
				getc(fd);
			}

			//| Skip 2 bytes

			for (x = 0; x < 2; x++)
			{
				getc(fd);
			}

			//| Read northeast longitude

			for (x = 0; x < 22; x++)
			{
				nelong[x] = getc(fd);
			}
			nelong[x] = 0;

			//| Skip 2 bytes

			for (x = 0; x < 2; x++)
			{
				getc(fd);
			}

			//| Read northeast latitude

			for (x = 0; x < 22; x++)
			{
				nelat[x] = getc(fd);
			}
			nelat[x] = 0;

			//| Skip 2 bytes

			for (x = 0; x < 2; x++)
			{
				getc(fd);
			}

			//| Read southeast longitude

			//for (x = 0; x < 22; x++)
			//{
			//	selong[x] = getc(fd);
			//}
			//selong[x] = 0;

			for (x = 0; x < 22; x++)
			{
				getc(fd);
			}

			//| Skip 2 bytes

			for (x = 0; x < 2; x++)
			{
				getc(fd);
			}

			//| Read southeast latitude

			//for (x = 0; x < 22; x++)
			//{
			//	selat[x] = getc(fd);
			//}
			//selat[x] = 0;

			for (x = 0; x < 22; x++)
			{
				getc(fd);
			}

			//| Skip 2 bytes

			for (x = 0; x < 2; x++)
			{
				getc(fd);
			}

			//| Read minimum elevation

			for (x = 0; x < 22; x++)
			{
				minimum[x] = getc(fd);
			}
			minimum[x] = 0;

			//| Skip 2 bytes

			for (x = 0; x < 2; x++)
			{
				getc(fd);
			}

			//| Read maximum elevation

			for (x = 0; x < 22; x++)
			{
				maximum[x] = getc(fd);
			}

			maximum[x] = 0;

			sscanf_s(ConvertExponentialNotation((char*)minimum), "%lG", &min_el);
			sscanf_s(ConvertExponentialNotation((char*)maximum), "%lf", &max_el);

			sscanf_s(ConvertExponentialNotation((char*)swlong), "%lf", &max_west);
			sscanf_s(ConvertExponentialNotation((char*)swlat), "%lf", &min_north);

			sscanf_s(ConvertExponentialNotation((char*)nelong), "%lf", &min_west);
			sscanf_s(ConvertExponentialNotation((char*)nelat), "%lf", &max_north);

			max_west /= -3600.0;
			min_north /= 3600.0;
			max_north /= 3600.0;
			min_west /= -3600.0;

			//| Skip 84 Bytes

			for (x = 0; x < 84; x++)
			{
				getc(fd);
			}

			//| Read elevation data...

			for (x = 1200; x >= 0; x--)
			{
				if (x == 900)
				{
					printf(" 25%c...", 37);
					fflush(stdout);
				}

				if (x == 600)
				{
					printf(" 50%c...", 37);
					fflush(stdout);
				}

				if (x == 300)
				{
					printf(" 75%c... ", 37);
					fflush(stdout);
				}

				//| Skip over 9 strings of data

				for (y = 0; y<9; y++)
				{
					MainString[0] = 0;

					do
					{
						c = getc(fd);

					} 
					while ((c == ' ') || (c == '\n'));

					for (z = 0; (c != ' ') && (c != '\n') && (z < 28); z++)
					{
						MainString[z] = c;
						c = getc(fd);
					}

					MainString[z] = 0;
				}

				//| Store elevation data in intArray

				for (y = 0; y < 1201; y++)
				{
					MainString[0] = 0;

					do
					{
						c = getc(fd);

					} 
					while ((c == ' ') || (c == '\n'));

					for (z = 0; (c != ' ') && (c != '\n') && (z < 28); z++)
					{
						MainString[z] = c;
						c = getc(fd);
					}

					MainString[z] = 0;
					sscanf_s(MainString, "%d", &intArray[y][x]);
				}
			}

			fclose(fd);

			//| Write splat data file to disk
			sprintf_s(splatfile, _countof(splatfile), "%.0fx%.0fx%.0fx%.0f.sdf", min_north, max_north, min_west, max_west);

			//| Write SDFs to user-specified directory.
			char sdfPathAndName[285];
			sprintf_s(sdfPathAndName, _countof(sdfPathAndName), "%s%s", SplatDataFilePath, splatfile);

			fprintf(stdout, " Done\nWriting \"%s\"... ", sdfPathAndName);
			fflush(stdout);

			err = fopen_s(&fd, sdfPathAndName, "w");

			fprintf(fd, "%.0f\n%.0f\n%.0f\n%.0f\n", max_west, min_north, min_west, max_north);

			for (x = 0; x < 1200; x++)
			{
				for (y = 0; y < 1200; y++)
				{
					fprintf(fd, "%d\n", intArray[x][y]);
				}
			}

			fclose(fd);
			fprintf(stdout, "Done\n");
			fflush(stdout);
		}

		if (fd == NULL)
		{
			fprintf(stderr, "*** %c%s%c: File Not Found\n", 34, argv[z1 + 1], 34);
		}

		z1++;
	}

	for (int i = 0; i < 1202; i++)
	{
		delete[] intArray[i];
	}
	delete[] intArray;

	return 0;
}
