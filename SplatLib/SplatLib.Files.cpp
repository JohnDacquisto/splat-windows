/****************************************************************************\
*					SplatLib.Files: SPLAT Files								 *
*****************************************************************************/

#include "stdafx.h"
#include <math.h>
#include <io.h>
#include <stdlib.h>
#include <ctype.h>
#include <Windows.h>
#include "SplatLib.h"
#include "itwom.h"
#include "fontdata.h"
#include "..\Common\Common.h"
#include "..\Common\Site.h"
#include "..\Common\version.h"
#include "..\Common\constants.h"


//| ------------------------------
//| 
//| FUNCTION: LoadSplatSiteLocationFile
//| 
//| OLD NAME: LoadQTH
//| 
//| NOTES: 
//|   This function reads SPLAT .qth (site location) files.
//|   The latitude and longitude may be expressed either in
//|   decimal degrees, or in degree, minute, second format.
//|   Antenna height is assumed to be expressed in feet above
//|   ground level (AGL), unless followed by the letter 'M',
//|   or 'm', or by the word "meters" or "Meters", in which
//|   case meters is assumed, and is handled accordingly.
//| 
//| ------------------------------
void
LoadSplatSiteLocationFile(
	char* filename,
	Site* site)
{
	int	x;
	char QthString[50], qthfile[255];
	FILE* fd = NULL;
	errno_t err;

	x = (int)strlen(filename);
	err = strncpy_s(qthfile, _countof(qthfile), filename, 254);

	if ((qthfile[x - 3] != 'q') || (qthfile[x - 2] != 't') || (qthfile[x - 1] != 'h'))
	{
		if (x > 249)
		{
			qthfile[249] = 0;
		}
		strncat_s(qthfile, ".qth\0", 5);
	}

	site->latitude = 91.0;
	site->longitude = 361.0;
	site->altitude = 0.0;
	site->name[0] = 0;
	site->filename[0] = 0;

	err = fopen_s(&fd, qthfile, "r");

	if (fd != NULL)
	{
		// Site Name
		fgets(QthString, 49, fd);

		// Strip <CR> and/or <LF> from end of site name

		for (x = 0; (QthString[x] != 13) && (QthString[x] != 10) && (QthString[x] != 0); site->name[x] = QthString[x], x++);

		site->name[x] = 0;

		// Site Latitude
		fgets(QthString, 49, fd);
		site->latitude = BearingStringToDecimalDegrees(QthString);

		// Site Longitude
		fgets(QthString, 49, fd);
		site->longitude = BearingStringToDecimalDegrees(QthString);

		if (site->longitude < 0.0)
		{
			site->longitude += 360.0;
		}

		// Antenna Height
		fgets(QthString, 49, fd);
		fclose(fd);

		// Remove <CR> and/or <LF> from antenna height string
		for (x = 0; (QthString[x] != 13) && (QthString[x] != 10) && (QthString[x] != 0); x++);

		QthString[x] = 0;

		// Antenna height may either be in feet or meters.
		// If the letter 'M' or 'm' is discovered in
		// the string, then this is an indication that
		// the value given is expressed in meters, and
		// must be converted to feet before exiting.

		for (x = 0; (QthString[x] != 'M') && (QthString[x] != 'm') && (QthString[x] != 0) && (x < 48); x++);

		if ((QthString[x] == 'M') || (QthString[x] == 'm'))
		{
			QthString[x] = 0;
			sscanf_s(QthString, "%f", &site->altitude);
			site->altitude *= (float)FEET_PER_METER;
		}
		else
		{
			QthString[x] = 0;
			sscanf_s(QthString, "%f", &site->altitude);
		}

		for (x = 0; (x < 254) && (qthfile[x] != 0); x++)
		{
			site->filename[x] = qthfile[x];
		}

		site->filename[x] = 0;
	}
}


//| ------------------------------
//| 
//| FUNCTION: LoadUserDefinedTerrainFile
//| 
//| OLD NAME: LoadUDT
//| 
//| NOTES: 
//|   This function reads a file containing User-Defined Terrain
//|   features for their addition to the digital elevation model
//|   data used by SPLAT. Elevations in the UDT file are evaluated
//|   and then copied into a temporary file under /tmp. Then the
//|   contents of the temp file are scanned, and if found to be unique,
//|   are added to the ground elevations described by the digital
//|   elevation data already loaded into memory.
//| 
//| ------------------------------
void
LoadUserDefinedTerrainFile(
	char* filename,
	DigitalElevationModelWrapper* digitalElevationModelWrapper)
{
	int	i, x, y, z, ypix, xpix, tempxpix, tempypix;
	char input[80], str[3][80], tempname[15], * pointer = NULL;
	double latitude, longitude, height, tempheight;
	FILE* fd1 = NULL, * fd2 = NULL;
	errno_t err;

	err = strcpy_s(tempname, 15, "splatXXXXXX\0");
	int sizeInChars = (int)strnlen(tempname, 13) + 1;

	err = fopen_s(&fd1, filename, "r");

	if (fd1 != NULL)
	{
		err = _mktemp_s(tempname, sizeInChars);
		err = fopen_s(&fd2, tempname, "w");

		fgets(input, 78, fd1);

		pointer = strchr(input, ';');

		if (pointer != NULL)
		{
			*pointer = 0;
		}

		fprintf(stdout, "\nReading \"%s\"... ", filename);
		fflush(stdout);

		while (feof(fd1) == 0)
		{
			// Parse line for latitude, longitude, height

			for (x = 0, y = 0, z = 0; (x < 78) && (input[x] != 0) && (z < 3); x++)
			{
				if ((input[x] != ',') && (y < 78))
				{
					str[z][y] = input[x];
					y++;
				}
				else
				{
					str[z][y] = 0;
					z++;
					y = 0;
				}
			}

			latitude = BearingStringToDecimalDegrees(str[0]);
			longitude = BearingStringToDecimalDegrees(str[1]);

			if (longitude < 0.0)
			{
				longitude += 360.0;
			}

			// Remove <CR> and/or <LF> from antenna height string

			for (i = 0; (str[2][i] != 13) && (str[2][i] != 10) && (str[2][i] != 0); i++);

			str[2][i] = 0;

			// The terrain feature may be expressed in either
			// feet or meters. If the letter 'M' or 'm' is
			// discovered in the string, then this is an
			// indication that the value given is expressed
			// in meters. Otherwise the height is interpreted
			// as being expressed in feet.

			for (i = 0; (str[2][i] != 'M') && (str[2][i] != 'm') && (str[2][i] != 0) && (i < 48); i++);

			if ((str[2][i] == 'M') || (str[2][i] == 'm'))
			{
				str[2][i] = 0;
				height = rint(atof(str[2]));
			}
			else
			{
				str[2][i] = 0;
				height = rint(METERS_PER_FOOT * atof(str[2]));
			}

			if (height > 0.0)
			{
				fprintf(fd2, "%d, %d, %f\n", (int)rint(latitude / (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree))), (int)rint(longitude / (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree))), height);
			}

			fgets(input, 78, fd1);

			pointer = strchr(input, ';');

			if (pointer != NULL)
			{
				*pointer = 0;
			}
		}

		fclose(fd1);
		fclose(fd2);

		fprintf(stdout, "Done");
		fflush(stdout);

		err = fopen_s(&fd1, tempname, "r");
		err = fopen_s(&fd2, tempname, "r");

		y = 0;

		if (!err && fd1 != NULL && fd2 != NULL)
		{
			fscanf_s(fd1, "%ld, %ld, %lf", &xpix, &ypix, &height);

			do
			{
				x = 0;
				z = 0;

				fscanf_s(fd2, "%ld, %ld, %lf", &tempxpix, &tempypix, &tempheight);

				do
				{
					if ((x > y) && (xpix == tempxpix) && (ypix == tempypix))
					{
						z = 1;  // Dupe

						if (tempheight > height)
						{
							height = tempheight;
						}
					}
					else
					{
						fscanf_s(fd2, "%ld, %ld, %lf", &tempxpix, &tempypix, &tempheight);
						x++;
					}

				} while ((feof(fd2) == 0) && (z == 0));

				if (z == 0)  // No duplicate found
				{
					AddUserDefinedTerrainToDigitalElevationModelData(xpix * (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree)), ypix * (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree)), height, digitalElevationModelWrapper);
				}

				fscanf_s(fd1, "%ld, %ld, %lf", &xpix, &ypix, &height);
				y++;

				rewind(fd2);
			} while (feof(fd1) == 0);

			fclose(fd1);
			fclose(fd2);
		}
		_unlink(tempname);
	}
	else
	{
		fprintf(stderr, "\n*** ERROR: \"%s\": not found", filename);
	}

	fprintf(stdout, "\n");
}


//| ------------------------------
//| 
//| FUNCTION: LoadCartographicBoundaryFiles
//| 
//| OLD NAME: LoadBoundaries
//| 
//| NOTES: 
//|   This function reads Cartographic Boundary Files available from
//|   the U.S. Census Bureau, and plots the data contained in those
//|   files on the PPM Map generated by SPLAT. Such files contain
//|   the coordinates that describe the boundaries of cities,
//|   counties, and states.
//| 
//| ------------------------------
void
LoadCartographicBoundaryFiles(
	char* filename,
	DigitalElevationModelWrapper* digitalElevationModelWrapper,
	Path* path)
{
	int	x;
	double lat0, lon0, lat1, lon1;
	char BoundariesString[80];
	Site source, destination;
	FILE* fd = NULL;
	errno_t err;

	err = fopen_s(&fd, filename, "r");

	if (fd != NULL)
	{
		fgets(BoundariesString, 78, fd);

		fprintf(stdout, "\nReading \"%s\"... ", filename);
		fflush(stdout);

		do
		{
			fgets(BoundariesString, 78, fd);
			sscanf_s(BoundariesString, "%lf %lf", &lon0, &lat0);
			fgets(BoundariesString, 78, fd);

			do
			{
				sscanf_s(BoundariesString, "%lf %lf", &lon1, &lat1);

				source.latitude = lat0;
				source.longitude = (lon0 > 0.0 ? 360.0 - lon0 : -lon0);
				destination.latitude = lat1;
				destination.longitude = (lon1 > 0.0 ? 360.0 - lon1 : -lon1);

				GeneratePathBetweenSites(source, destination, digitalElevationModelWrapper, path);

				for (x = 0; x < path->pathLength; x++)
				{
					SetOrValueInDigitalElevationModelMask(path->latitudes[x], path->longitudes[x], 4, digitalElevationModelWrapper);
				}

				lat0 = lat1;
				lon0 = lon1;

				fgets(BoundariesString, 78, fd);

			} while ((strncmp(BoundariesString, "END", 3) != 0) && (feof(fd) == 0));

			fgets(BoundariesString, 78, fd);

		} while ((strncmp(BoundariesString, "END", 3) != 0) && (feof(fd) == 0));

		fclose(fd);

		fprintf(stdout, "Done");
		fflush(stdout);
	}
	else
	{
		fprintf(stderr, "\n*** ERROR: \"%s\": not found", filename);
	}
}


//| ------------------------------
//| 
//| FUNCTION: LoadSplatDataFilesForRegion
//| 
//| OLD NAME: LoadTopoData
//| 
//| NOTES: 
//|   This function loads the SDF files required
//|   to cover the limits of the region specified.
//| 
//| ------------------------------
void
LoadSplatDataFilesForRegion(
	int max_lon,
	int min_lon,
	int max_lat,
	int min_lat,
	DigitalElevationModelWrapper* digitalElevationModelWrapper,
	int* maximumElevation,
	int* minimumElevation,
	int* minimumLatitudeNorth,
	int* maximumLatitudeNorth,
	int* minimumLongitudeWest,
	int* maximumLongitudeWest,
	char* splatDataFilePath)
{
	int x, y, width, ymin, ymax;
	char nameString[255];	// OLD NAME: string

	width = ConvertToNormalizedAngle(ToDouble(max_lon - min_lon));

	if ((max_lon - min_lon) <= 180)
	{
		for (y = 0; y <= width; y++)
		{
			for (x = min_lat; x <= max_lat; x++)
			{
				ymin = (int)(min_lon + (double)y);

				while (ymin < 0)
				{
					ymin += 360;
				}

				while (ymin >= 360)
				{
					ymin -= 360;
				}

				ymax = ymin + 1;

				while (ymax < 0)
				{
					ymax += 360;
				}

				while (ymax >= 360)
				{
					ymax -= 360;
				}

				if (digitalElevationModelWrapper->demPixelsPerDegree == PIXELS_PER_DEGREE_HIGH_DEF)
				{
					snprintf(nameString, 19, "%dx%dx%dx%d-hd", x, x + 1, ymin, ymax);
				}
				else
				{
					snprintf(nameString, 16, "%dx%dx%dx%d", x, x + 1, ymin, ymax);
				}

				LoadSplatDataFile(nameString, digitalElevationModelWrapper, maximumElevation, minimumElevation, minimumLatitudeNorth, maximumLatitudeNorth,
					minimumLongitudeWest, maximumLongitudeWest, splatDataFilePath);
			}
		}
	}
	else
	{
		for (y = 0; y <= width; y++)
		{
			for (x = min_lat; x <= max_lat; x++)
			{
				ymin = max_lon + y;

				while (ymin < 0)
				{
					ymin += 360;
				}

				while (ymin >= 360)
				{
					ymin -= 360;
				}

				ymax = ymin + 1;

				while (ymax < 0)
				{
					ymax += 360;
				}

				while (ymax >= 360)
				{
					ymax -= 360;
				}

				if (digitalElevationModelWrapper->demPixelsPerDegree == PIXELS_PER_DEGREE_HIGH_DEF)
				{
					snprintf(nameString, 19, "%dx%dx%dx%d-hd", x, x + 1, ymin, ymax);
				}
				else
				{
					snprintf(nameString, 16, "%dx%dx%dx%d", x, x + 1, ymin, ymax);
				}

				LoadSplatDataFile(nameString, digitalElevationModelWrapper, maximumElevation, minimumElevation, minimumLatitudeNorth, maximumLatitudeNorth,
					minimumLongitudeWest, maximumLongitudeWest, splatDataFilePath);
			}
		}
	}
}


//| ------------------------------
//| 
//| FUNCTION: LoadUncompressedSplatDataFile
//| 
//| OLD NAME: LoadSDF_SDF
//| 
//| NOTES: 
//|   This function reads uncompressed SPLAT Data Files (.sdf)
//|   containing digital elevation model data into memory.
//|   Elevation data, maximum and minimum elevations, and
//|   quadrangle limits are stored in the first available
//|   digitalElevationModel[] structure.
//| 
//| ------------------------------
int
LoadUncompressedSplatDataFile(
	char* name,
	DigitalElevationModelWrapper* digitalElevationModelWrapper,
	int* maximumElevation,
	int* minimumElevation,
	int* minimumLatitudeNorth,
	int* maximumLatitudeNorth,
	int* minimumLongitudeWest,
	int* maximumLongitudeWest,
	char* splatDataFilePath)
{
	int	x, y, data, indx, minlat, minlon, maxlat, maxlon;
	bool found, free_page = false;
	char line[20], sdf_file[255], path_plus_name[512];
	FILE* fd;
	errno_t err;

	for (x = 0; (name[x] != '.') && (name[x] != 0) && (x < 250); x++)
	{
		sdf_file[x] = name[x];
	}

	sdf_file[x] = 0;

	// Parse filename for minimum latitude and longitude values

	sscanf_s(sdf_file, "%dx%dx%dx%d", &minlat, &maxlat, &minlon, &maxlon);

	sdf_file[x] = '.';
	sdf_file[x + 1] = 's';
	sdf_file[x + 2] = 'd';
	sdf_file[x + 3] = 'f';
	sdf_file[x + 4] = 0;

	// Is it already in memory?

	for (indx = 0, found = false; (indx < digitalElevationModelWrapper->maxPages) && (found == false); indx++)
	{
		if ((minlat == digitalElevationModelWrapper->digitalElevationModel[indx].minimumLatitudeNorth) &&
			(minlon == digitalElevationModelWrapper->digitalElevationModel[indx].minimumLongitudeWest) &&
			(maxlat == digitalElevationModelWrapper->digitalElevationModel[indx].maximumLatitudeNorth) &&
			(maxlon == digitalElevationModelWrapper->digitalElevationModel[indx].maximumLongitudeWest))
		{
			found = true;
		}
	}

	// Is room available to load it?

	if (found == false)
	{
		for (indx = 0, free_page = false; (indx < digitalElevationModelWrapper->maxPages) && (free_page == false); indx++)
		{
			if (digitalElevationModelWrapper->digitalElevationModel[indx].maximumLatitudeNorth == -90)
			{
				free_page = true;
			}
		}
	}

	indx--;

	if (free_page && (found == false) && (indx >= 0) && (indx < digitalElevationModelWrapper->maxPages))
	{
		// Search for SDF file in current working directory first

		err = strncpy_s(path_plus_name, _countof(path_plus_name), sdf_file, 255);

		err = fopen_s(&fd, path_plus_name, "rb");

		if (fd == NULL)
		{
			// Next, try loading SDF file from path specified
			// by -d argument

			err = strncpy_s(path_plus_name, _countof(path_plus_name), splatDataFilePath, 255);
			strncat_s(path_plus_name, sdf_file, 254);

			err = fopen_s(&fd, path_plus_name, "rb");
		}

		if (fd != NULL)
		{
			fprintf(stdout, "Loading \"%s\" into page %d...", path_plus_name, indx + 1);
			fflush(stdout);

			fgets(line, 19, fd);
			sscanf_s(line, "%d", &digitalElevationModelWrapper->digitalElevationModel[indx].maximumLongitudeWest);

			fgets(line, 19, fd);
			sscanf_s(line, "%d", &digitalElevationModelWrapper->digitalElevationModel[indx].minimumLatitudeNorth);

			fgets(line, 19, fd);
			sscanf_s(line, "%d", &digitalElevationModelWrapper->digitalElevationModel[indx].minimumLongitudeWest);

			fgets(line, 19, fd);
			sscanf_s(line, "%d", &digitalElevationModelWrapper->digitalElevationModel[indx].maximumLatitudeNorth);

			for (x = 0; x < digitalElevationModelWrapper->demPixelsPerDegree; x++)
			{
				for (y = 0; y < digitalElevationModelWrapper->demPixelsPerDegree; y++)
				{
					fgets(line, 19, fd);
					data = atoi(line);

					digitalElevationModelWrapper->digitalElevationModel[indx].data[x][y] = data;
					digitalElevationModelWrapper->digitalElevationModel[indx].signal[x][y] = 0;
					digitalElevationModelWrapper->digitalElevationModel[indx].mask[x][y] = 0;

					if (data > digitalElevationModelWrapper->digitalElevationModel[indx].maximumElevation)
					{
						digitalElevationModelWrapper->digitalElevationModel[indx].maximumElevation = data;
					}

					if (data < digitalElevationModelWrapper->digitalElevationModel[indx].minimumElevation)
					{
						digitalElevationModelWrapper->digitalElevationModel[indx].minimumElevation = data;
					}
				}
			}

			fclose(fd);

			if (digitalElevationModelWrapper->digitalElevationModel[indx].minimumElevation < *minimumElevation)
			{
				*minimumElevation = digitalElevationModelWrapper->digitalElevationModel[indx].minimumElevation;
			}

			if (digitalElevationModelWrapper->digitalElevationModel[indx].maximumElevation > * maximumElevation)
			{
				*maximumElevation = digitalElevationModelWrapper->digitalElevationModel[indx].maximumElevation;
			}

			if (*maximumLatitudeNorth == -90)
			{
				*maximumLatitudeNorth = digitalElevationModelWrapper->digitalElevationModel[indx].maximumLatitudeNorth;
			}
			else if (digitalElevationModelWrapper->digitalElevationModel[indx].maximumLatitudeNorth > * maximumLatitudeNorth)
			{
				*maximumLatitudeNorth = digitalElevationModelWrapper->digitalElevationModel[indx].maximumLatitudeNorth;
			}

			if (*minimumLatitudeNorth == 90)
			{
				*minimumLatitudeNorth = digitalElevationModelWrapper->digitalElevationModel[indx].minimumLatitudeNorth;
			}
			else if (digitalElevationModelWrapper->digitalElevationModel[indx].minimumLatitudeNorth < *minimumLatitudeNorth)
			{
				*minimumLatitudeNorth = digitalElevationModelWrapper->digitalElevationModel[indx].minimumLatitudeNorth;
			}

			if (*maximumLongitudeWest == -1)
			{
				*maximumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].maximumLongitudeWest;
			}
			else
			{
				if (abs(digitalElevationModelWrapper->digitalElevationModel[indx].maximumLongitudeWest - *maximumLongitudeWest) < 180)
				{
					if (digitalElevationModelWrapper->digitalElevationModel[indx].maximumLongitudeWest > * maximumLongitudeWest)
					{
						*maximumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].maximumLongitudeWest;
					}
				}
				else
				{
					if (digitalElevationModelWrapper->digitalElevationModel[indx].maximumLongitudeWest < *maximumLongitudeWest)
					{
						*maximumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].maximumLongitudeWest;
					}
				}
			}

			if (*minimumLongitudeWest == 360)
			{
				*minimumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].minimumLongitudeWest;
			}
			else
			{
				if (fabs(digitalElevationModelWrapper->digitalElevationModel[indx].minimumLongitudeWest - *minimumLongitudeWest) < 180.0)
				{
					if (digitalElevationModelWrapper->digitalElevationModel[indx].minimumLongitudeWest < *minimumLongitudeWest)
					{
						*minimumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].minimumLongitudeWest;
					}
				}
				else
				{
					if (digitalElevationModelWrapper->digitalElevationModel[indx].minimumLongitudeWest > * minimumLongitudeWest)
					{
						*minimumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].minimumLongitudeWest;
					}
				}
			}

			fprintf(stdout, " Done\n");
			fflush(stdout);

			return 1;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return 0;
	}
}


//| ------------------------------
//| 
//| FUNCTION: LoadSplatDataFile
//| 
//| OLD NAME: LoadSDF
//| 
//| NOTES: 
//|   This function loads the requested SDF file from the filesystem.
//|   It first tries to invoke the LoadUncompressedSplatDataFile() function to load an
//|   uncompressed SDF file (since uncompressed files load slightly faster).
//|   If that fails, then we can assume that no elevation data
//|   exists for the region requested, and that the region
//|   requested must be entirely over water.
//| 
//| ------------------------------
char
LoadSplatDataFile(
	char* name,
	DigitalElevationModelWrapper* digitalElevationModelWrapper,
	int* maximumElevation,
	int* minimumElevation,
	int* minimumLatitudeNorth,
	int* maximumLatitudeNorth,
	int* minimumLongitudeWest,
	int* maximumLongitudeWest,
	char* splatDataFilePath)
{
	int	x, y, indx, minlat, minlon, maxlat, maxlon;
	bool found, free_page = false;
	int	return_value = -1;

	// Try to load an uncompressed SDF first.

	return_value = LoadUncompressedSplatDataFile(name, digitalElevationModelWrapper, maximumElevation, minimumElevation,
		minimumLatitudeNorth, maximumLatitudeNorth, minimumLongitudeWest, maximumLongitudeWest, splatDataFilePath);

	// If neither format can be found, then assume the area is water.

	if ((return_value == 0) || (return_value == -1))
	{
		// Parse SDF name for minimum latitude and longitude values

		sscanf_s(name, "%dx%dx%dx%d", &minlat, &maxlat, &minlon, &maxlon);

		// Is it already in memory?

		for (indx = 0, found = false; (indx < digitalElevationModelWrapper->maxPages) && (found == false); indx++)
		{
			if ((minlat == digitalElevationModelWrapper->digitalElevationModel[indx].minimumLatitudeNorth) &&
				(minlon == digitalElevationModelWrapper->digitalElevationModel[indx].minimumLongitudeWest) &&
				(maxlat == digitalElevationModelWrapper->digitalElevationModel[indx].maximumLatitudeNorth) &&
				(maxlon == digitalElevationModelWrapper->digitalElevationModel[indx].maximumLongitudeWest))
			{
				found = true;
			}
		}

		// Is room available to load it?

		if (found == false)
		{
			for (indx = 0, free_page = false; (indx < digitalElevationModelWrapper->maxPages) && (free_page == false); indx++)
			{
				if (digitalElevationModelWrapper->digitalElevationModel[indx].maximumLatitudeNorth == -90)
				{
					free_page = true;
				}
			}
		}

		indx--;

		if (free_page && (found == false) && (indx >= 0) && (indx < digitalElevationModelWrapper->maxPages))
		{
			fprintf(stdout, "Region  \"%s\" assumed as sea-level into page %d...", name, indx + 1);
			fflush(stdout);

			digitalElevationModelWrapper->digitalElevationModel[indx].maximumLongitudeWest = maxlon;
			digitalElevationModelWrapper->digitalElevationModel[indx].minimumLatitudeNorth = minlat;
			digitalElevationModelWrapper->digitalElevationModel[indx].minimumLongitudeWest = minlon;
			digitalElevationModelWrapper->digitalElevationModel[indx].maximumLatitudeNorth = maxlat;

			// Fill DEM with sea-level topography

			for (x = 0; x < digitalElevationModelWrapper->demPixelsPerDegree; x++)
			{
				for (y = 0; y < digitalElevationModelWrapper->demPixelsPerDegree; y++)
				{
					digitalElevationModelWrapper->digitalElevationModel[indx].data[x][y] = 0;
					digitalElevationModelWrapper->digitalElevationModel[indx].signal[x][y] = 0;
					digitalElevationModelWrapper->digitalElevationModel[indx].mask[x][y] = 0;

					if (digitalElevationModelWrapper->digitalElevationModel[indx].minimumElevation > 0)
					{
						digitalElevationModelWrapper->digitalElevationModel[indx].minimumElevation = 0;
					}
				}
			}

			if (digitalElevationModelWrapper->digitalElevationModel[indx].minimumElevation < *minimumElevation)
			{
				*minimumElevation = digitalElevationModelWrapper->digitalElevationModel[indx].minimumElevation;
			}

			if (digitalElevationModelWrapper->digitalElevationModel[indx].maximumElevation > * maximumElevation)
			{
				*maximumElevation = digitalElevationModelWrapper->digitalElevationModel[indx].maximumElevation;
			}

			if (*maximumLatitudeNorth == -90)
			{
				*maximumLatitudeNorth = digitalElevationModelWrapper->digitalElevationModel[indx].maximumLatitudeNorth;
			}
			else if (digitalElevationModelWrapper->digitalElevationModel[indx].maximumLatitudeNorth > * maximumLatitudeNorth)
			{
				*maximumLatitudeNorth = digitalElevationModelWrapper->digitalElevationModel[indx].maximumLatitudeNorth;
			}

			if (*minimumLatitudeNorth == 90)
			{
				*minimumLatitudeNorth = digitalElevationModelWrapper->digitalElevationModel[indx].minimumLatitudeNorth;
			}
			else if (digitalElevationModelWrapper->digitalElevationModel[indx].minimumLatitudeNorth < *minimumLatitudeNorth)
			{
				*minimumLatitudeNorth = digitalElevationModelWrapper->digitalElevationModel[indx].minimumLatitudeNorth;
			}

			if (*maximumLongitudeWest == -1)
			{
				*maximumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].maximumLongitudeWest;
			}
			else
			{
				if (abs(digitalElevationModelWrapper->digitalElevationModel[indx].maximumLongitudeWest - *maximumLongitudeWest) < 180)
				{
					if (digitalElevationModelWrapper->digitalElevationModel[indx].maximumLongitudeWest > * maximumLongitudeWest)
					{
						*maximumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].maximumLongitudeWest;
					}
				}
				else
				{
					if (digitalElevationModelWrapper->digitalElevationModel[indx].maximumLongitudeWest < *maximumLongitudeWest)
					{
						*maximumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].maximumLongitudeWest;
					}
				}
			}

			if (*minimumLongitudeWest == 360)
			{
				*minimumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].minimumLongitudeWest;
			}
			else
			{
				if (abs(digitalElevationModelWrapper->digitalElevationModel[indx].minimumLongitudeWest - *minimumLongitudeWest) < 180)
				{
					if (digitalElevationModelWrapper->digitalElevationModel[indx].minimumLongitudeWest < *minimumLongitudeWest)
					{
						*minimumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].minimumLongitudeWest;
					}
				}
				else
				{
					if (digitalElevationModelWrapper->digitalElevationModel[indx].minimumLongitudeWest > * minimumLongitudeWest)
					{
						*minimumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].minimumLongitudeWest;
					}
				}
			}

			fprintf(stdout, " Done\n");
			fflush(stdout);

			return_value = 1;
		}
	}

	return return_value;
}


//| ------------------------------
//| 
//| FUNCTION: LoadSplatAlphanumericOutputFile
//| 
//| OLD NAME: LoadANO
//| 
//| NOTES: 
//|   This function reads a SPLAT alphanumeric output
//|   file (-ani option) for analysis and/or map generation.
//| 
//| ------------------------------
int
LoadSplatAlphanumericOutputFile(
	char* filename,
	DigitalElevationModelWrapper* digitalElevationModelWrapper,
	IrregularTerrainModelParameters* itmParameters,
	unsigned char plotSignalPowerLevelContours,
	int contourDisplayThreshold,
	int* maximumElevation,
	int* minimumElevation,
	int* minimumLatitudeNorth,
	int* maximumLatitudeNorth,
	int* minimumLongitudeWest,
	int* maximumLongitudeWest,
	char* splatDataFilePath)
{
	int	error = 0, max_west, min_west, max_north, min_north;
	char AnoString[80], * pointer = NULL;
	double latitude = 0.0, longitude = 0.0, azimuth = 0.0, elevation = 0.0,
		ano = 0.0;
	FILE* fd;
	errno_t err;

	err = fopen_s(&fd, filename, "r");

	if (fd != NULL)
	{
		fgets(AnoString, 78, fd);
		pointer = strchr(AnoString, ';');

		if (pointer != NULL)
		{
			*pointer = 0;
		}

		sscanf_s(AnoString, "%d, %d", &max_west, &min_west);

		fgets(AnoString, 78, fd);
		pointer = strchr(AnoString, ';');

		if (pointer != NULL)
		{
			*pointer = 0;
		}

		sscanf_s(AnoString, "%d, %d", &max_north, &min_north);

		fgets(AnoString, 78, fd);
		pointer = strchr(AnoString, ';');

		if (pointer != NULL)
		{
			*pointer = 0;
		}

		LoadSplatDataFilesForRegion(max_west - 1, min_west, max_north - 1, min_north, digitalElevationModelWrapper, maximumElevation, minimumElevation,
			minimumLatitudeNorth, maximumLatitudeNorth, minimumLongitudeWest, maximumLongitudeWest, splatDataFilePath);

		fprintf(stdout, "\nReading \"%s\"... ", filename);
		fflush(stdout);

		fgets(AnoString, 78, fd);
		sscanf_s(AnoString, "%lf, %lf, %lf, %lf, %lf", &latitude, &longitude, &azimuth, &elevation, &ano);

		while (feof(fd) == 0)
		{
			if (itmParameters->effectiveRadiatedPower == 0.0)
			{
				// Path loss

				if ((contourDisplayThreshold == 0) || (fabs(ano) <= (double)contourDisplayThreshold))
				{
					ano = fabs(ano);

					if (ano > 255.0)
					{
						ano = 255.0;
					}

					SetValueInDigitalElevationModelSignal(latitude, longitude, ((unsigned char)round(ano)), digitalElevationModelWrapper);
				}
			}

			if ((itmParameters->effectiveRadiatedPower != 0.0) && (plotSignalPowerLevelContours != 0))
			{
				// signal power level in dBm

				if ((contourDisplayThreshold == 0) || (ano >= (double)contourDisplayThreshold))
				{
					ano = 200.0 + rint(ano);

					if (ano < 0.0)
					{
						ano = 0.0;
					}

					if (ano > 255.0)
					{
						ano = 255.0;
					}

					SetValueInDigitalElevationModelSignal(latitude, longitude, ((unsigned char)round(ano)), digitalElevationModelWrapper);
				}
			}

			if ((itmParameters->effectiveRadiatedPower != 0.0) && (plotSignalPowerLevelContours == 0))
			{
				// field strength dBuV/m

				if ((contourDisplayThreshold == 0) || (ano >= (double)contourDisplayThreshold))
				{
					ano = 100.0 + rint(ano);

					if (ano < 0.0)
					{
						ano = 0.0;
					}

					if (ano > 255.0)
					{
						ano = 255.0;
					}

					SetValueInDigitalElevationModelSignal(latitude, longitude, ((unsigned char)round(ano)), digitalElevationModelWrapper);
				}
			}

			fgets(AnoString, 78, fd);
			sscanf_s(AnoString, "%lf, %lf, %lf, %lf, %lf", &latitude, &longitude, &azimuth, &elevation, &ano);
		}

		fclose(fd);

		fprintf(stdout, " Done\n");
		fflush(stdout);
	}
	else
	{
		error = 1;
	}

	return error;
}


//| ------------------------------
//| 
//| FUNCTION: LoadAntennaAzimuthElevationPatternFiles
//| 
//| OLD NAME: LoadPAT
//| 
//| NOTES: 
//|   This function reads and processes antenna pattern (.az
//|   and .el) files that correspond in name to previously
//|   loaded SPLAT .lrp files.
//| 
//| ------------------------------
void
LoadAntennaAzimuthElevationPatternFiles(
	char* filename,
	IrregularTerrainModelParameters* itmParameters,
	unsigned char* gotAntennaElevationAnglePattern,
	unsigned char* gotAntennaAzimuthAnglePattern)
{
	int	a, b, w, x, y, z, last_index, next_index, span;
	char PatString[255], azfile[255], elfile[255], * pointer = NULL;
	float az, xx, elevation, amplitude, rotation, valid1, valid2,
		delta, tilt, mechanical_tilt = 0.0, tilt_azimuth, tilt_increment, sum;

	// Issue with stack overflow, allocate big arrays on heap instead.
	float* azimuth = new float[361];
	float* azimuth_pattern = new float[361];
	float* el_pattern = new float[10001];
	float* slant_angle = new float[361];
	float** elevation_pattern = new float* [361];
	for (int i = 0; i < 361; i++)
	{
		elevation_pattern[i] = new float[1001];
	}

	FILE* fd = NULL;
	unsigned char read_count[10001];
	errno_t err;

	for (x = 0; (filename[x] != '.') && (filename[x] != 0) && (x < 250); x++)
	{
		azfile[x] = filename[x];
		elfile[x] = filename[x];
	}

	azfile[x] = '.';
	azfile[x + 1] = 'a';
	azfile[x + 2] = 'z';
	azfile[x + 3] = 0;

	elfile[x] = '.';
	elfile[x + 1] = 'e';
	elfile[x + 2] = 'l';
	elfile[x + 3] = 0;

	rotation = 0.0;

	*gotAntennaAzimuthAnglePattern = 0;
	*gotAntennaElevationAnglePattern = 0;

	// Load .az antenna pattern file

	err = fopen_s(&fd, azfile, "r");

	if (fd != NULL)
	{
		// Clear azimuth pattern array

		for (x = 0; x <= 360; x++)
		{
			azimuth[x] = 0.0;
			read_count[x] = 0;
		}

		// Read azimuth pattern rotation
		// in degrees measured clockwise
		// from true North.

		fgets(PatString, 254, fd);
		pointer = strchr(PatString, ';');

		if (pointer != NULL)
		{
			*pointer = 0;
		}

		sscanf_s(PatString, "%f", &rotation);

		// Read azimuth (degrees) and corresponding
		// normalized field radiation pattern amplitude
		// (0.0 to 1.0) until EOF is reached.

		fgets(PatString, 254, fd);
		pointer = strchr(PatString, ';');

		if (pointer != NULL)
		{
			*pointer = 0;
		}

		sscanf_s(PatString, "%f %f", &az, &amplitude);

		do
		{
			x = (int)rintf(az);

			if ((x >= 0) && (x <= 360) && (fd != NULL))
			{
				azimuth[x] += amplitude;
				read_count[x]++;
			}

			fgets(PatString, 254, fd);
			pointer = strchr(PatString, ';');

			if (pointer != NULL)
			{
				*pointer = 0;
			}

			sscanf_s(PatString, "%f %f", &az, &amplitude);

		} while (feof(fd) == 0);

		fclose(fd);

		// Handle 0 = 360 degree ambiguity

		if ((read_count[0] == 0) && (read_count[360] != 0))
		{
			read_count[0] = read_count[360];
			azimuth[0] = azimuth[360];
		}

		if ((read_count[0] != 0) && (read_count[360] == 0))
		{
			read_count[360] = read_count[0];
			azimuth[360] = azimuth[0];
		}

		// Average pattern values in case more than
		// one was read for each degree of azimuth.

		for (x = 0; x <= 360; x++)
		{
			if (read_count[x] > 1)
			{
				azimuth[x] /= (float)read_count[x];
			}
		}

		// Interpolate missing azimuths
		// to completely fill the array

		last_index = -1;
		next_index = -1;

		for (x = 0; x <= 360; x++)
		{
			if (read_count[x] != 0)
			{
				if (last_index == -1)
				{
					last_index = x;
				}
				else
				{
					next_index = x;
				}
			}

			if ((last_index != -1) && (next_index != -1))
			{
				valid1 = azimuth[last_index];
				valid2 = azimuth[next_index];

				span = next_index - last_index;
				delta = (valid2 - valid1) / (float)span;

				for (y = last_index + 1; y < next_index; y++)
				{
					azimuth[y] = azimuth[y - 1] + delta;
				}

				last_index = y;
				next_index = -1;
			}
		}

		// Perform azimuth pattern rotation
		// and load azimuth_pattern[361] with
		// azimuth pattern data in its final form.

		for (x = 0; x < 360; x++)
		{
			y = x + (int)rintf(rotation);

			if (y >= 360)
			{
				y -= 360;
			}

			azimuth_pattern[y] = azimuth[x];
		}

		azimuth_pattern[360] = azimuth_pattern[0];

		*gotAntennaAzimuthAnglePattern = 255;
	}

	// Read and process .el file

	err = fopen_s(&fd, elfile, "r");

	if (fd != NULL)
	{
		for (x = 0; x <= 10000; x++)
		{
			el_pattern[x] = 0.0;
			read_count[x] = 0;
		}

		// Read mechanical tilt (degrees) and
		// tilt azimuth in degrees measured
		// clockwise from true North.

		fgets(PatString, 254, fd);
		pointer = strchr(PatString, ';');

		if (pointer != NULL)
		{
			*pointer = 0;
		}

		sscanf_s(PatString, "%f %f", &mechanical_tilt, &tilt_azimuth);

		// Read elevation (degrees) and corresponding
		// normalized field radiation pattern amplitude
		// (0.0 to 1.0) until EOF is reached.

		fgets(PatString, 254, fd);
		pointer = strchr(PatString, ';');

		if (pointer != NULL)
		{
			*pointer = 0;
		}

		sscanf_s(PatString, "%f %f", &elevation, &amplitude);

		while (feof(fd) == 0)
		{
			// Read in normalized radiated field values
			// for every 0.01 degrees of elevation between
			// -10.0 and +90.0 degrees

			x = (int)rintf(100.0f * (elevation + 10.0f));

			if ((x >= 0) && (x <= 10000))
			{
				el_pattern[x] += amplitude;
				read_count[x]++;
			}

			fgets(PatString, 254, fd);
			pointer = strchr(PatString, ';');

			if (pointer != NULL)
			{
				*pointer = 0;
			}

			sscanf_s(PatString, "%f %f", &elevation, &amplitude);
		}

		fclose(fd);

		// Average the field values in case more than
		// one was read for each 0.01 degrees of elevation.

		for (x = 0; x <= 10000; x++)
		{
			if (read_count[x] > 1)
			{
				el_pattern[x] /= (float)read_count[x];
			}
		}

		// Interpolate between missing elevations (if
		// any) to completely fill the array and provide
		// radiated field values for every 0.01 degrees of
		// elevation.

		last_index = -1;
		next_index = -1;

		for (x = 0; x <= 10000; x++)
		{
			if (read_count[x] != 0)
			{
				if (last_index == -1)
				{
					last_index = x;
				}
				else
				{
					next_index = x;
				}
			}

			if ((last_index != -1) && (next_index != -1))
			{
				valid1 = el_pattern[last_index];
				valid2 = el_pattern[next_index];

				span = next_index - last_index;
				delta = (valid2 - valid1) / (float)span;

				for (y = last_index + 1; y < next_index; y++)
				{
					el_pattern[y] = el_pattern[y - 1] + delta;
				}

				last_index = y;
				next_index = -1;
			}
		}

		// Fill slant_angle[] array with offset angles based
		// on the antenna's mechanical beam tilt (if any)
		// and tilt direction (azimuth).

		if (mechanical_tilt == 0.0)
		{
			for (x = 0; x <= 360; x++)
			{
				slant_angle[x] = 0.0;
			}
		}
		else
		{
			tilt_increment = mechanical_tilt / 90.0f;

			for (x = 0; x <= 360; x++)
			{
				xx = (float)x;
				y = (int)rintf(tilt_azimuth + xx);

				while (y >= 360)
				{
					y -= 360;
				}

				while (y < 0)
				{
					y += 360;
				}

				if (x <= 180)
				{
					slant_angle[y] = -(tilt_increment * (90.0f - xx));
				}

				if (x > 180)
				{
					slant_angle[y] = -(tilt_increment * (xx - 270.0f));
				}
			}
		}

		slant_angle[360] = slant_angle[0];   // 360 degree wrap-around

		for (w = 0; w <= 360; w++)
		{
			tilt = slant_angle[w];

			// Convert tilt angle to
			// an array index offset

			y = (int)rintf(100.0f * tilt);

			// Copy shifted el_pattern[10001] field
			// values into elevation_pattern[361][1001]
			// at the corresponding azimuth, downsampling
			// (averaging) along the way in chunks of 10.

			for (x = y, z = 0; z <= 1000; x += 10, z++)
			{
				for (sum = 0.0, a = 0; a < 10; a++)
				{
					b = a + x;

					if ((b >= 0) && (b <= 10000))
					{
						sum += el_pattern[b];
					}
					if (b < 0)
					{
						sum += el_pattern[0];
					}
					if (b > 10000)
					{
						sum += el_pattern[10000];
					}
				}

				elevation_pattern[w][z] = sum / 10.0f;
			}
		}

		*gotAntennaElevationAnglePattern = 255;
	}

	for (x = 0; x <= 360; x++)
	{
		for (y = 0; y <= 1000; y++)
		{
			if (*gotAntennaElevationAnglePattern != 0)
			{
				elevation = elevation_pattern[x][y];
			}
			else
			{
				elevation = 1.0;
			}

			if (*gotAntennaAzimuthAnglePattern != 0)
			{
				az = azimuth_pattern[x];
			}
			else
			{
				az = 1.0;
			}

			itmParameters->antennaRadiationPattern[x][y] = az * elevation;
		}
	}

	delete[] azimuth;
	azimuth = NULL;

	delete[] azimuth_pattern;
	azimuth_pattern = NULL;

	delete[] el_pattern;
	el_pattern = NULL;

	delete[] slant_angle;
	slant_angle = NULL;

	for (int i = 0; i < 361; i++)
	{
		delete[] elevation_pattern[i];
		elevation_pattern[i] = NULL;
	}

	delete[] elevation_pattern;
	elevation_pattern = NULL;
}
