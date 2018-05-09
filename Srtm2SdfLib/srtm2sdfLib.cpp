/*****************************************************************************
*				Srtm2SdfLib: Functions used by Srtm2Sdf						 *
*****************************************************************************/

#include "stdafx.h"
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include "srtm2sdfLib.h"
#include "..\Common\constants.h"


//| ------------------------------
//| 
//| FUNCTION: ReadSpaceShuttleRadarTopographyMissionDataFile
//| 
//| OLD NAME: ReadSRTM
//| 
//| NOTES: 
//| 
//| ------------------------------
int
ReadSpaceShuttleRadarTopographyMissionDataFile
   (char *filename,
	char *elevationProblem,
	char *splatDataFileName,
	int integerPixelsPerDegree,
	int minimumElevation,
	int *maximumLatitudeNorth,
	int *maximumLongitudeWest,
	int *minimumLatitudeNorth,
	int *minimumLongitudeWest,
	int **spaceShuttleRadarTopographyMissionData)
{
	int x, y, infile, byte = 0, bytes_read;
	unsigned char buffer[2];
	bool error;
	char north[3], west[4], *base = NULL, blw_filename[255];
	double cell_size, deg_north, deg_west;
	FILE *fd = NULL;
	errno_t err;

	bool fileExtentionHgt = false;		//| OLD NAME: hgt
	bool fileExtentionBil = false;		//| OLD NAME: bil

	if (strstr(filename, ".zip") != NULL)
	{
		fprintf(stderr, "*** Error: \"%s\" must be uncompressed\n", filename);
		return -1;
	}

	if (strstr(filename, ".tgz") != NULL)
	{
		fprintf(stderr, "*** Error: \"%s\" must be uncompressed\n", filename);
		return -1;
	}

	if ((strstr(filename, ".hgt") == NULL) && (strstr(filename, ".bil") == NULL))
	{
		fprintf(stderr, "*** Error: \"%s\" does not have the correct extension (.hgt or .bil)\n", filename);
		return -1;
	}

	if (strstr(filename, ".hgt") != NULL)
	{
		fileExtentionHgt = true;
	}

	if (strstr(filename, ".bil") != NULL)
	{
		fileExtentionBil = true;
	}

	base = strrchr(filename, '\\');

	if (base == NULL)
	{
		base = filename;
	}
	else
	{
		base += 1;
	}

	if (fileExtentionHgt)
	{
		//| We obtain coordinates from the base of the .HGT filename

		north[0] = base[1];
		north[1] = base[2];
		north[2] = 0;

		west[0] = base[4];
		west[1] = base[5];
		west[2] = base[6];
		west[3] = 0;

		if (((base[0] != 'N') && (base[0] != 'S')) || ((base[3] != 'W') && (base[3] != 'E')))
		{
			fprintf(stderr, "*** Error: \"%s\" doesn't look like a valid .hgt SRTM filename.\n", filename);
			return -1;
		}

		*maximumLongitudeWest = atoi(west);

		if (base[3] == 'E')
		{
			*maximumLongitudeWest = 360 - *maximumLongitudeWest;
		}

		*minimumLongitudeWest = *maximumLongitudeWest - 1;

		if (*maximumLongitudeWest == 360)
		{
			*maximumLongitudeWest = 0;
		}

		if (base[0] == 'N')
		{
			*minimumLatitudeNorth = atoi(north);
		}
		else
		{
			*minimumLatitudeNorth = -atoi(north);
		}

		*maximumLatitudeNorth = *minimumLatitudeNorth + 1;
	}

	if (fileExtentionBil)
	{
		//| We obtain .BIL file coordinates
		//| from the corresponding .BLW file

		err = strncpy_s(blw_filename, _countof(blw_filename), filename, 250);
		x = (int)strlen(filename);

		if (x > 3)
		{
			blw_filename[x - 2] = 'l';
			blw_filename[x - 1] = 'w';
			blw_filename[x] = 0;

			err = fopen_s(&fd, blw_filename, "rb");

			if (fd != NULL)
			{
				fscanf_s(fd, "%lf", &cell_size);

				if ((cell_size < 0.0008) || (cell_size > 0.0009))
				{
					printf("\n*** .BIL file's cell size is incompatible with SPLAT\n");
					exit(1);
				}

				fscanf_s(fd, "%lf", &deg_west);
				fscanf_s(fd, "%lf", &deg_west);
				fscanf_s(fd, "%lf", &deg_west);
				fscanf_s(fd, "%lf", &deg_west);
				fscanf_s(fd, "%lf", &deg_north);
				fclose(fd);
			}

			*minimumLatitudeNorth = (int)(deg_north);
			*maximumLatitudeNorth = *maximumLatitudeNorth + 1;

			if (deg_west < 0.0)
			{
				deg_west = -deg_west;
			}
			else
			{
				deg_west = 360.0 - deg_west;
			}

			*minimumLongitudeWest = (int)deg_west;

			if (*minimumLongitudeWest == 360)
			{
				*minimumLongitudeWest = 0;
			}

			*maximumLongitudeWest = *minimumLongitudeWest + 1;
		}
	}

	err = _sopen_s(&infile, filename, (_O_RDONLY | _O_BINARY), _SH_DENYNO, 0);

	if (infile == 0)
	{
		fprintf(stderr, "*** Error: Cannot open \"%s\"\n", filename);
		return -1;
	}

	int n = _read(infile, &buffer, 2);

	if ((buffer[0] == 'P') && (buffer[1] == 'K'))
	{
		fprintf(stderr, "*** Error: \"%s\" still appears to be compressed\n", filename);
		_close(infile);
		return -1;
	}

	_lseek(infile, 0L, SEEK_SET);

	if (integerPixelsPerDegree == PIXELS_PER_DEGREE_HIGH_DEF)
	{
		sprintf_s(splatDataFileName, 30, "%ldx%ldx%ldx%ld-hd.sdf", *minimumLatitudeNorth, *maximumLatitudeNorth, *minimumLongitudeWest, *maximumLongitudeWest);
	}
	else
	{
		sprintf_s(splatDataFileName, 30, "%ldx%ldx%ldx%ld.sdf", *minimumLatitudeNorth, *maximumLatitudeNorth, *minimumLongitudeWest, *maximumLongitudeWest);
	}

	error = false;
	*elevationProblem = 0;

	printf("Reading %s... ", filename);
	fflush(stdout);

	for (x = 0; (x <= integerPixelsPerDegree) && (error == false); x++)
	{
		for (y = 0; (y <= integerPixelsPerDegree) && (error == false); y++)
		{
			bytes_read = _read(infile, &buffer, 2);

			if (bytes_read == 2)
			{
				if (fileExtentionBil)
				{
					//| "little-endian" structure

					byte = buffer[0] + (buffer[1] << 8);

					if (buffer[1] & 128)
					{
						byte -= 0x10000;
					}
				}

				if (fileExtentionHgt)
				{
					//| "big-endian" structure

					byte = buffer[1] + (buffer[0] << 8);

					if (buffer[0] & 128)
					{
						byte -= 0x10000;
					}
				}

				//| Flag problem elevations here

				if (byte < -32768)
				{
					byte = -32768;
				}

				if (byte > 32767)
				{
					byte = 32767;
				}

				if (byte <= minimumElevation)
				{
					*elevationProblem = 1;
				}

				spaceShuttleRadarTopographyMissionData[x][y] = byte;
			}
			else
			{
				error = true;
			}
		}
	}

	if (error)
	{
		fprintf(stderr, "\n*** Error: Premature EOF detected while reading \"%s\"\n", filename);
	}

	_close(infile);

	return 0;
}


//| ------------------------------
//| 
//| FUNCTION: LoadUncompressedSplatDataFileForSrtm
//| 
//| OLD NAME: LoadSDF_SDF
//| 
//| NOTES: 
//|   This function reads uncompressed
//|   SPLAT Data Files (.sdf) into memory.
//| 
//| ------------------------------
int
LoadUncompressedSplatDataFileForSrtm
   (char *name,
	char *splatDataFilePath,
	int **unitedStatesGeologicalSurveyData)
{
	int x, y, dummy;
	char sdf_file[255], path_plus_name[512];
	FILE *infile;
	errno_t err;

	for (x = 0; (name[x] != '.') && (name[x] != 0) && (x < 250); x++)
	{
		sdf_file[x] = name[x];
	}

	sdf_file[x] = '.';
	sdf_file[x + 1] = 's';
	sdf_file[x + 2] = 'd';
	sdf_file[x + 3] = 'f';
	sdf_file[x + 4] = 0;

	err = strncpy_s(path_plus_name, _countof(path_plus_name), splatDataFilePath, 255);
	err = strncat_s(path_plus_name, _countof(path_plus_name), sdf_file, 254);

	err = fopen_s(&infile, path_plus_name, "rb");

	if (infile == NULL)
	{
		return 0;
	}

	fscanf_s(infile, "%ld", &dummy);
	fscanf_s(infile, "%ld", &dummy);
	fscanf_s(infile, "%ld", &dummy);
	fscanf_s(infile, "%ld", &dummy);

	printf("\nReading %s... ", path_plus_name);
	fflush(stdout);

	for (x = 0; x < PIXELS_PER_DEGREE_STANDARD; x++)
	{
		for (y = 0; y < PIXELS_PER_DEGREE_STANDARD; y++)
		{
			fscanf_s(infile, "%ld", &unitedStatesGeologicalSurveyData[x][y]);
		}
	}

	fclose(infile);

	return 1;
}


//| ------------------------------
//| 
//| FUNCTION: LoadSplatDataFileForSrtm
//| 
//| OLD NAME: LoadSDF
//| 
//| NOTES: 
//|   This function loads the requested SDF file from the filesystem.
//|   First, it tries to invoke the LoadUncompressedSplatDataFileForSrtm() function to load an
//|   uncompressed SDF file (since uncompressed files load slightly faster).
//| 
//| ------------------------------
char
LoadSplatDataFileForSrtm
   (char *name,
	char *splatDataFilePath,
	int **unitedStatesGeologicalSurveyData)
{
	int return_value = -1;

	//| Try to load an uncompressed SDF first.

	return_value = LoadUncompressedSplatDataFileForSrtm(name, splatDataFilePath, unitedStatesGeologicalSurveyData);

	return return_value;
}


//| ------------------------------
//| 
//| FUNCTION: ReadUnitedStatesGeologicalSurveyDataFile
//| 
//| OLD NAME: ReadUSGS
//| 
//| NOTES: 
//|   usgs_filename is a minimal filename ("40:41:74:75").
//|   Full path and extentions are added later though
//|   subsequent function calls.
//| 
//| ------------------------------
int
ReadUnitedStatesGeologicalSurveyDataFile
   (char *splatDataFilePath,
	int maximumLatitudeNorth,
	int maximumLongitudeWest,
	int minimumLatitudeNorth,
	int minimumLongitudeWest,
	int **unitedStatesGeologicalSurveyData)
{
	char usgs_filename[15];

	sprintf_s(usgs_filename, _countof(usgs_filename), "%ldx%ldx%ldx%ld", minimumLatitudeNorth, maximumLatitudeNorth, minimumLongitudeWest, maximumLongitudeWest);

	return (LoadSplatDataFileForSrtm(usgs_filename, splatDataFilePath, unitedStatesGeologicalSurveyData));
}


//| ------------------------------
//| 
//| FUNCTION: AverageTerrain
//| 
//| OLD NAME: average_terrain
//| 
//| NOTES: 
//| 
//| ------------------------------
void
AverageTerrain
   (int y,
	int x,
	int z,
	int maximumPixelIndexPerDegree,
	int minimumElevation,
	int **spaceShuttleRadarTopographyMissionData)
{
	long accum;
	int temp = 0, count, bad_value;
	double average;

	bad_value = spaceShuttleRadarTopographyMissionData[y][x];

	accum = 0L;
	count = 0;

	if (y >= 2)
	{
		temp = spaceShuttleRadarTopographyMissionData[y - 1][x];

		if (temp > bad_value)
		{
			accum += temp;
			count++;
		}
	}

	if (y <= maximumPixelIndexPerDegree)
	{
		temp = spaceShuttleRadarTopographyMissionData[y + 1][x];

		if (temp > bad_value)
		{
			accum += temp;
			count++;
		}
	}

	if ((y >= 2) && (x <= (maximumPixelIndexPerDegree - 1)))
	{
		temp = spaceShuttleRadarTopographyMissionData[y - 1][x + 1];

		if (temp > bad_value)
		{
			accum += temp;
			count++;
		}
	}

	if (x <= (maximumPixelIndexPerDegree - 1))
	{
		temp = spaceShuttleRadarTopographyMissionData[y][x + 1];

		if (temp > bad_value)
		{
			accum += temp;
			count++;
		}
	}

	if ((x <= (maximumPixelIndexPerDegree - 1)) && (y <= maximumPixelIndexPerDegree))
	{
		temp = spaceShuttleRadarTopographyMissionData[y + 1][x + 1];

		if (temp > bad_value)
		{
			accum += temp;
			count++;
		}
	}

	if ((x >= 1) && (y >= 2))
	{
		temp = spaceShuttleRadarTopographyMissionData[y - 1][x - 1];

		if (temp > bad_value)
		{
			accum += temp;
			count++;
		}
	}

	if (x >= 1)
	{
		temp = spaceShuttleRadarTopographyMissionData[y][x - 1];

		if (temp > bad_value)
		{
			accum += temp;
			count++;
		}
	}

	if ((y <= maximumPixelIndexPerDegree) && (x >= 1))
	{
		temp = spaceShuttleRadarTopographyMissionData[y + 1][x - 1];

		if (temp > bad_value)
		{
			accum += temp;
			count++;
		}
	}

	if (count != 0)
	{
		average = (((double)accum) / ((double)count));
		temp = (int)(average + 0.5);
	}

	if (temp > minimumElevation)
	{
		spaceShuttleRadarTopographyMissionData[y][x] = temp;
	}
	else
	{
		spaceShuttleRadarTopographyMissionData[y][x] = minimumElevation;
	}
}


//| ------------------------------
//| 
//| FUNCTION: WriteSplatDataFile
//| 
//| OLD NAME: WriteSDF
//| 
//| NOTES: 
//|   Like the HGT files, the extreme southwest corner
//|   provides the point of reference for the SDF file.
//|   The SDF file extends from MinimumLatitudeNorth degrees to
//|   the south to MinimumLatitudeNorth+(maximumPixelIndexPerDegree/integerPixelsPerDegree) degrees to
//|   the north, and maximumLongitudeWest degrees to the west to
//|   maximumLongitudeWest-(maximumPixelIndexPerDegree/integerPixelsPerDegree) degrees to the east. The
//|   overlapping edge redundancy present in the HGT
//|   and earlier USGS files is not necessary, nor
//|   is it present in SDF files.
//| 
//| ------------------------------
void
WriteSplatDataFile
   (char *filename,
	int integerPixelsPerDegree,
	int maximumPixelIndexPerDegree,
	int minimumElevation,
	int mergeData,
	int maximumLatitudeNorth,
	int maximumLongitudeWest,
	int minimumLatitudeNorth,
	int minimumLongitudeWest,
	int **spaceShuttleRadarTopographyMissionData,
	int **unitedStatesGeologicalSurveyData)
{
	int x, y, byte, last_good_byte = 0;
	FILE *outfile;
	errno_t err;

	printf("\nWriting %s... ", filename);
	fflush(stdout);

	err = fopen_s(&outfile, filename, "wb");

	fprintf(outfile, "%d\n%d\n%d\n%d\n", maximumLongitudeWest, minimumLatitudeNorth, minimumLongitudeWest, maximumLatitudeNorth);

	for (y = integerPixelsPerDegree; y >= 1; y--)			//| Omit the northern most edge
	{
		for (x = maximumPixelIndexPerDegree; x >= 0; x--)	//| Omit the eastern most edge 
		{
			byte = spaceShuttleRadarTopographyMissionData[y][x];

			if (byte > minimumElevation)
			{
				last_good_byte = byte;
			}

			if (byte < minimumElevation)
			{
				if (mergeData)
				{
					if (integerPixelsPerDegree == PIXELS_PER_DEGREE_HIGH_DEF)
					{
						fprintf(outfile, "%d\n", unitedStatesGeologicalSurveyData[PIXELS_PER_DEGREE_STANDARD - (y / 3)][(PIXELS_PER_DEGREE_STANDARD - 1) - (x / 3)]);
					}
					else
					{
						fprintf(outfile, "%d\n", unitedStatesGeologicalSurveyData[PIXELS_PER_DEGREE_STANDARD - y][(PIXELS_PER_DEGREE_STANDARD - 1) - x]);
					}
				}
				else
				{
					AverageTerrain(y, x, last_good_byte, maximumPixelIndexPerDegree, minimumElevation, spaceShuttleRadarTopographyMissionData);
					fprintf(outfile, "%d\n", spaceShuttleRadarTopographyMissionData[y][x]);
				}
			}
			else
			{
				fprintf(outfile, "%d\n", byte);
			}
		}
	}

	printf("Done\n");

	fclose(outfile);
}
