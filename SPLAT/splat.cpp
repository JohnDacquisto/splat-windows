/****************************************************************************\
*	   SPLAT: An RF Signal Path Loss And Terrain Analysis Tool				 *
******************************************************************************
*	     Project started in 1997 by John A. Magliacane, KD2BD 				 *
*			  Last update: 07-Feb-2014										 *
******************************************************************************
*         Please consult the documentation for a complete list of			 *
*	     individuals who have contributed to this project. 					 *
******************************************************************************
*                                                                            *
*  This program is free software; you can redistribute it and/or modify it   *
*  under the terms of the GNU General Public License as published by the     *
*  Free Software Foundation; either version 2 of the License or any later    *
*  version.																	 *
* 																			 *
*  This program is distributed in the hope that it will useful, but WITHOUT  *
*  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
*  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License      *
*  for more details.														 *
*																			 *
\****************************************************************************/

#include "stdafx.h"
#include <chrono>
#include "splat.h"
#include "..\Common\version.h"
#include "..\Common\constants.h"
#include "..\Common\Site.h"
#include "..\Common\Common.h"
#include "..\SplatLib\SplatLib.h"
#include "..\SplatLib\itwom.h"

using namespace std;
using namespace std::chrono;


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

	int x, y, z = 0, min_lat, min_lon, max_lat, max_lon,
		rxlat, rxlon, txlat, txlon, west_min, west_max,
		north_min, north_max, numCityFiles = 0, numBoundaryFiles = 0;

	bool coverage = false, LRmap = false, terrain_plot = false,
		elevation_plot = false, height_plot = false, map = false,
		longley_plot = false, topomap = false, pt2pt_mode = false,
		area_mode = false, nolospath = false, nositereports = false,
		command_line_log = false;

	unsigned char numTxSites = 0, norm = 0, geo = 0, kml = 0, 
		ngs = 0, fresnel_plot = 1;

	char mapfile[255], header[80], elevation_file[255], elevation_file_temp[255],
		height_file[255], height_file_temp[255], longley_file[255],
		longley_file_temp[255], terrain_file[255], terrain_file_temp[255],
		MainString[255], rxfile[255], *env = NULL,
		txfile[255], udt_file[255], rxsite = 0, ani_filename[255],
		ano_filename[255], time_filename[255], ext[20], logfile[255];

	char **cityFiles = NULL;		// OLD NAME: city_file[5][255]
	char **boundaryFiles = NULL;	// OLD NAME: boundary_file[5][255]

	double altitude = 0.0, altitudeLR = 0.0, tx_range = 0.0,
		rx_range = 0.0, deg_range = 0.0, deg_limit = 0.0,
		deg_range_lon, er_mult;

	Site *txSites = NULL;		// OLD NAME: tx_site[32]
	Site rx_site;

	FILE *fd, *timeTraceFile = NULL;

	errno_t err;

	char splatDataFilePath[255];		// OLD NAME: sdf_path
	char reportSavePath[255], reportSavePathForPlot[255];

	double maximumAnalysisDistance = 0.0;	// OLD NAME: max_range
	double fresnelZoneFrequency;			// OLD NAME: forced_freq
	double groundClutterHeight;				// OLD NAME: clutter
	double effectiveRadiatedPower = -1.0;	// OLD NAME: forced_erp
	double fresnelZoneClearanceRatio = 0.6;	// OLD NAME: fzone_clearance
	double sphereRadius;					// OLD NAME: earthradius

	int	minimumLatitudeNorth = 90;	// OLD NAME: min_north
	int maximumLatitudeNorth = -90;	// OLD NAME: max_north
	int minimumLongitudeWest = 360;	// OLD NAME: min_west
	int maximumLongitudeWest = -1;	// OLD NAME: max_west
	int maximumElevation = -32768;	// OLD NAME: max_elevation
	int minimumElevation = 32768;	// OLD NAME: min_elevation

	char useOldLongleyRiceModel;		// OLD NAME: olditm
	char saveGnuPlotWorkingFiles = 0;	// OLD NAME: gpsav
	int contourDisplayThreshold;		// OLD NAME: contour_threshold

	unsigned char plotSignalPowerLevelContours = 0;	// OLD NAME: dbm
	unsigned char displaySmoothContourLevels = 0;	// OLD NAME: smooth_contours
	unsigned char gotAntennaElevationAnglePattern;	// OLD NAME: got_elevation_pattern
	unsigned char gotAntennaAzimuthAnglePattern;	// OLD NAME: got_azimuth_pattern
	unsigned char useMetricUnits = 0;				// OLD NAME: metric

	int hdMode;				// OLD NAME: HDMODE
	int maxPages;			// OLD NAME: MAXPAGES
	long arraySize;			// OLD NAME: ARRAY_SIZE
	int intPixelsPerDegree;	// OLD NAME: IPPD

	if (argc == 1)
	{
		fprintf(stdout, "\n\n\t\t --==[ %s v%s Available Options... ]==--\n\n", APP_NAME, VERSION_HEADER);

		fprintf(stdout, "      -t : txsite(s).qth\n");
		fprintf(stdout, "      -r : rxsite.qth\n");
		fprintf(stdout, "      -c : plot LOS coverage of TX(s) with an RX antenna at X feet/meters AGL\n");
		fprintf(stdout, "      -L : plot path loss map of TX based on an RX at X feet/meters AGL\n");
		fprintf(stdout, "      -s : filename(s) of city/site file(s) to import\n");
		fprintf(stdout, "      -b : filename(s) of cartographic boundary file(s) to import\n");
		fprintf(stdout, "      -p : filename of terrain profile graph to plot\n");
		fprintf(stdout, "      -e : filename of terrain elevation graph to plot\n");
		fprintf(stdout, "      -h : filename of terrain height graph to plot\n");
		fprintf(stdout, "      -H : filename of normalized terrain height graph to plot\n");
		fprintf(stdout, "      -l : filename of path loss graph to plot\n");
		fprintf(stdout, "      -o : filename of topographic map to generate (.ppm)\n");
		fprintf(stdout, "      -d : sdf file directory path\n");
		fprintf(stdout, "      -m : earth radius multiplier\n");
		fprintf(stdout, "      -n : do not plot LOS paths in .ppm maps\n");
		fprintf(stdout, "      -N : do not produce unnecessary site or obstruction reports\n");
		fprintf(stdout, "      -f : frequency for Fresnel zone calculation (MHz)\n");
		fprintf(stdout, "      -R : modify default range for -c or -L (miles/kilometers)\n");
		fprintf(stdout, "     -sc : display smooth rather than quantized contour levels\n");
		fprintf(stdout, "     -db : threshold beyond which contours will not be displayed\n");
		fprintf(stdout, "     -nf : do not plot Fresnel zones in height plots\n");
		fprintf(stdout, "     -fz : Fresnel zone clearance percentage (default = 60)\n");
		fprintf(stdout, "     -gc : ground clutter height (feet/meters)\n");
		fprintf(stdout, "    -ngs : display greyscale topography as white in .ppm files\n");
		fprintf(stdout, "    -erp : override ERP in .lrp file (Watts)\n");
		fprintf(stdout, "    -ano : name of alphanumeric output file\n");
		fprintf(stdout, "    -ani : name of alphanumeric input file\n");
		fprintf(stdout, "    -udt : name of user defined terrain input file\n");
		fprintf(stdout, "    -kml : generate Google Earth (.kml) compatible output\n");
		fprintf(stdout, "    -geo : generate an Xastir .geo georeference file (with .ppm output)\n");
		fprintf(stdout, "    -dbm : plot signal power level contours rather than field strength\n");
		fprintf(stdout, "    -log : copy command line string to this output file\n");
		fprintf(stdout, "    -rsp : specify location to save output reports\n");
		fprintf(stdout, "   -time : name of file which tracks analysis duration\n");
		fprintf(stdout, "  -gpsav : preserve gnuplot temporary working files after SPLAT execution\n");
		fprintf(stdout, " -metric : employ metric rather than imperial units for all user I/O\n");
		fprintf(stdout, " -olditm : invoke Longley-Rice rather than the default ITWOM model\n");
		fprintf(stdout, "     -hd : run SPLAT in HD mode (0 for standard, 1 for high-definition)\n");
		fprintf(stdout, "   -mxpg : max SPLAT data files to include (1 (HD only), 4, 9, 16, 25, 36, 49, 64)\n\n");
		fflush(stdout);

		return 1;
	}

	y = argc - 1;

	useOldLongleyRiceModel = 0;
	kml = 0;
	geo = 0;
	plotSignalPowerLevelContours = 0;
	saveGnuPlotWorkingFiles = 0;
	useMetricUnits = 0;
	rxfile[0] = 0;
	txfile[0] = 0;
	MainString[0] = 0;
	mapfile[0] = 0;
	groundClutterHeight = 0.0;
	effectiveRadiatedPower = -1.0;
	fresnelZoneFrequency = 0.0;
	elevation_file[0] = 0;
	elevation_file_temp[0] = 0;
	terrain_file[0] = 0;
	terrain_file_temp[0] = 0;
	splatDataFilePath[0] = 0;
	reportSavePath[0] = 0;
	reportSavePathForPlot[0] = 0;
	udt_file[0] = 0;
	fresnelZoneClearanceRatio = 0.6;
	contourDisplayThreshold = 0;
	rx_site.latitude = 91.0;
	rx_site.longitude = 361.0;
	longley_file[0] = 0;
	longley_file_temp[0] = 0;
	time_filename[0] = 0;
	ano_filename[0] = 0;
	ani_filename[0] = 0;
	displaySmoothContourLevels = 0;
	sphereRadius = EARTH_RADIUS_FEET;

	// Default standard mode at 4 degrees.
	hdMode = 0;
	maxPages = 4;
	arraySize = 4950;
	intPixelsPerDegree = PIXELS_PER_DEGREE_STANDARD;

	sprintf_s(header, _countof(header), "\n\n\t\t\t--==[ %s v%s ]==--\n\n", APP_NAME, VERSION_HEADER);

	// Scan for HD mode and max pages to include first, to take care of initializations.
	for (x = 1; x <= y; x++)
	{
		// -hd run SPLAT in HD mode (0 == standard, 1 == HD)
		if (strcmp(argv[x], "-hd") == 0)
		{
			z = x + 1;
			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				sscanf_s(argv[z], "%ld", &hdMode);

				if (hdMode < 0)
				{
					hdMode = 0;
				}

				if (hdMode > 1)
				{
					hdMode = 1;
				}
			}
		}

		// -mxpg max SPLAT data files to include (1 (HD only), 4, 9, 16, 25, 36, 49, or 64)
		if (strcmp(argv[x], "-mxpg") == 0)
		{
			z = x + 1;
			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				sscanf_s(argv[z], "%ld", &maxPages);

				if (maxPages < 1)
				{
					maxPages = 1;
				}

				if (maxPages > 64)
				{
					maxPages = 64;
				}
			}
		}
	}

	if (hdMode == 0)
	{
		if (maxPages == 4)
		{
			arraySize = 4950;
		}
		else if (maxPages == 9)
		{
			arraySize = 10870;
		}
		else if (maxPages == 16)
		{
			arraySize = 19240;
		}
		else if (maxPages == 25)
		{
			arraySize = 30025;
		}
		else if (maxPages == 36)
		{
			arraySize = 43217;
		}
		else if (maxPages == 49)
		{
			arraySize = 58813;
		}
		else if (maxPages == 64)
		{
			arraySize = 76810;
		}
		intPixelsPerDegree = PIXELS_PER_DEGREE_STANDARD;
	}
	else if (hdMode == 1)
	{
		if (maxPages == 1)
		{
			arraySize = 5092;
		}
		else if (maxPages == 4)
		{
			arraySize = 14844;
		}
		else if (maxPages == 9)
		{
			arraySize = 32600;
		}
		else if (maxPages == 16)
		{
			arraySize = 57713;
		}
		else if (maxPages == 25)
		{
			arraySize = 90072;
		}
		else if (maxPages == 36)
		{
			arraySize = 129650;
		}
		else if (maxPages == 49)
		{
			arraySize = 176437;
		}
		else if (maxPages == 64)
		{
			arraySize = 230430;
		}
		intPixelsPerDegree = PIXELS_PER_DEGREE_HIGH_DEF;
	}

	DigitalElevationModelWrapper *digitalElevationModelWrapper = new DigitalElevationModelWrapper(maxPages, intPixelsPerDegree);

	double *pathElevation = new double[arraySize + 10];	// OLD NAME: elev

	Path *path = new Path(arraySize);
	path->pathLength = 0;

	IrregularTerrainModelParameters *itmParameters = new IrregularTerrainModelParameters();	// OLD NAME: LR

	ColorRegion *region = new ColorRegion();

	system_clock::time_point startTime, endTime;


	// Scan for other command line arguments

	for (x = 1; x <= y; x++)
	{
		// -R modify default range for -c or -L (miles/kilometers)
		if (strcmp(argv[x], "-R") == 0)
		{
			z = x + 1;

			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				sscanf_s(argv[z], "%lf", &maximumAnalysisDistance);

				if (maximumAnalysisDistance < 0.0)
				{
					maximumAnalysisDistance = 0.0;
				}

				if (maximumAnalysisDistance > 1000.0)
				{
					maximumAnalysisDistance = 1000.0;
				}
			}
		}

		// -m earth radius multiplier
		if (strcmp(argv[x], "-m") == 0)
		{
			z = x + 1;

			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				sscanf_s(argv[z], "%lf", &er_mult);

				if (er_mult < 0.1)
				{
					er_mult = 1.0;
				}

				if (er_mult > 1.0e6)
				{
					er_mult = 1.0e6;
				}

				sphereRadius *= er_mult;
			}
		}

		// -gc ground clutter height (feet/meters)
		if (strcmp(argv[x], "-gc") == 0)
		{
			z = x + 1;

			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				sscanf_s(argv[z], "%lf", &groundClutterHeight);

				if (groundClutterHeight < 0.0)
				{
					groundClutterHeight = 0.0;
				}
			}
		}

		// -fz Fresnel zone clearance percentage (default = 60)
		if (strcmp(argv[x], "-fz") == 0)
		{
			z = x + 1;

			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				sscanf_s(argv[z], "%lf", &fresnelZoneClearanceRatio);

				if ((fresnelZoneClearanceRatio < 0.0) || (fresnelZoneClearanceRatio > 100.0))
				{
					fresnelZoneClearanceRatio = 60.0;
				}

				fresnelZoneClearanceRatio /= 100.0;
			}
		}

		// -o filename of topographic map to generate (.ppm)
		if (strcmp(argv[x], "-o") == 0)
		{
			z = x + 1;

			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				err = strncpy_s(mapfile, _countof(mapfile), argv[z], 253);
			}

			map = true;
		}

		// -log copy command line string to this output file
		if (strcmp(argv[x], "-log") == 0)
		{
			z = x + 1;

			logfile[0] = 0;

			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				err = strncpy_s(logfile, _countof(logfile), argv[z], 253);
			}

			command_line_log = true;
		}

		// -udt name of user defined terrain input file
		// -u filename of user-defined terrain file to import
		if ((strcmp(argv[x], "-udt") == 0) || (strcmp(argv[x], "-u") == 0))
		{
			z = x + 1;

			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				err = strncpy_s(udt_file, _countof(udt_file), argv[z], 253);
			}
		}

		// -c plot LOS coverage of TX(s) with an RX antenna at X feet/meters AGL
		if (strcmp(argv[x], "-c") == 0)
		{
			z = x + 1;

			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				sscanf_s(argv[z], "%lf", &altitude);
				map = true;
				coverage = true;
				area_mode = true;
			}
		}

		// -db threshold beyond which contours will not be displayed
		if ((strcmp(argv[x], "-db") == 0) || (strcmp(argv[x], "-dB") == 0))
		{
			z = x + 1;

			if ((z <= y) && argv[z][0])	// A minus argument is legal here
			{
				sscanf_s(argv[z], "%d", &contourDisplayThreshold);
			}
		}

		// -p filename of terrain profile graph to plot
		if (strcmp(argv[x], "-p") == 0)
		{
			z = x + 1;

			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				err = strncpy_s(terrain_file_temp, _countof(terrain_file_temp), argv[z], 253);
				terrain_plot = true;
				pt2pt_mode = true;
			}
		}

		// -e filename of terrain elevation graph to plot
		if (strcmp(argv[x], "-e") == 0)
		{
			z = x + 1;

			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				err = strncpy_s(elevation_file_temp, _countof(elevation_file_temp), argv[z], 253);
				elevation_plot = true;
				pt2pt_mode = true;
			}
		}

		// -h filename of terrain height graph to plot
		// -H filename of normalized terrain height graph to plot
		if ((strcmp(argv[x], "-h") == 0) || (strcmp(argv[x], "-H") == 0))
		{
			z = x + 1;

			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				err = strncpy_s(height_file_temp, _countof(height_file_temp), argv[z], 253);
				height_plot = true;
				pt2pt_mode = true;
			}

			if (strcmp(argv[x], "-H") == 0)
			{
				norm = 1;
			}
			else
			{
				norm = 0;
			}
		}

		// -metric employ metric rather than imperial units for all user I/O
		if (strcmp(argv[x], "-metric") == 0)
		{
			useMetricUnits = 1;
		}

		// -gpsav preserve gnuplot temporary working files after SPLAT execution
		if (strcmp(argv[x], "-gpsav") == 0)
		{
			saveGnuPlotWorkingFiles = 1;
		}

		// -geo generate an Xastir .geo georeference file (with .ppm output)
		if (strcmp(argv[x], "-geo") == 0)
		{
			geo = 1;
		}

		// -kml generate Google Earth (.kml) compatible output
		if (strcmp(argv[x], "-kml") == 0)
		{
			kml = 1;
		}

		// -nf do not plot Fresnel zones in height plots
		if (strcmp(argv[x], "-nf") == 0)
		{
			fresnel_plot = 0;
		}

		// -ngs display greyscale topography as white in .ppm files
		if (strcmp(argv[x], "-ngs") == 0)
		{
			ngs = 1;
		}

		// -n do not plot LOS paths in .ppm maps
		if (strcmp(argv[x], "-n") == 0)
		{
			nolospath = true;
		}

		// -dbm plot signal power level contours rather than field strength
		if (strcmp(argv[x], "-dbm") == 0)
		{
			plotSignalPowerLevelContours = 1;
		}

		// -sc display smooth rather than quantized contour levels
		if (strcmp(argv[x], "-sc") == 0)
		{
			displaySmoothContourLevels = 1;
		}

		// -olditm invoke Longley-Rice rather than the default ITWOM model
		if (strcmp(argv[x], "-olditm") == 0)
		{
			useOldLongleyRiceModel = 1;
		}

		// -N do not produce unnecessary site or obstruction reports
		if (strcmp(argv[x], "-N") == 0)
		{
			nolospath = true;
			nositereports = true;
		}

		// -d sdf file directory path
		if (strcmp(argv[x], "-d") == 0)
		{
			z = x + 1;

			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				err = strncpy_s(splatDataFilePath, _countof(splatDataFilePath), argv[z], 253);
			}
		}

		// -rsp specify location to save various output reports
		if (strcmp(argv[x], "-rsp") == 0)
		{
			z = x + 1;

			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				err = strncpy_s(reportSavePath, _countof(reportSavePath), argv[z], 253);
			}
		}

		// -t txsite(s).qth
		if (strcmp(argv[x], "-t") == 0)
		{
			// Read Transmitter Location

			z = x + 1;
			int zHold = z;

			// Determine how many tx sites are present, and allocate accordingly, before loading them.

			while ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				numTxSites++;
				z++;
			}

			txSites = new Site[numTxSites];
			for (int i = 0; i < numTxSites; i++)
			{
				txSites[i].latitude = 91.0;
				txSites[i].longitude = 361.0;
			}

			numTxSites = 0;
			z = zHold;

			while ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				err = strncpy_s(txfile, _countof(txfile), argv[z], 253);
				LoadSplatSiteLocationFile(txfile, &txSites[numTxSites]);
				numTxSites++;
				z++;
			}

			z--;
		}

		// -L plot path loss map of TX based on an RX at X feet/meters AGL
		if (strcmp(argv[x], "-L") == 0)
		{
			z = x + 1;

			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				sscanf_s(argv[z], "%lf", &altitudeLR);
				map = true;
				LRmap = true;
				area_mode = true;

				if (coverage)
				{
					fprintf(stdout, "c and L are exclusive options, ignoring L.\n");
				}
			}
		}

		// -l filename of path loss graph to plot
		if (strcmp(argv[x], "-l") == 0)
		{
			z = x + 1;

			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				err = strncpy_s(longley_file_temp, _countof(longley_file_temp), argv[z], 253);
				longley_plot = true;
				pt2pt_mode = true;
			}
		}

		// -r rxsite.qth
		if (strcmp(argv[x], "-r") == 0)
		{
			// Read Receiver Location

			z = x + 1;

			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				err = strncpy_s(rxfile, _countof(rxfile), argv[z], 253);
				LoadSplatSiteLocationFile(rxfile, &rx_site);
				rxsite = 1;
				pt2pt_mode = true;
			}
		}

		// -s filename(s) of city/site file(s) to import
		if (strcmp(argv[x], "-s") == 0)
		{
			// Read city file(s)

			z = x + 1;
			int zHold = z;

			// Determine how many city/state files are present, and allocate accordingly.

			while ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				numCityFiles++;
				z++;
			}

			cityFiles = new char*[numCityFiles];
			for (int i = 0; i < numCityFiles; i++)
			{
				cityFiles[i] = new char[255];
			}

			numCityFiles = 0;
			z = zHold;

			while ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				err = strncpy_s(cityFiles[numCityFiles], 255, argv[z], 253);
				numCityFiles++;
				z++;
			}

			z--;
		}

		// -b filename(s) of cartographic boundary file(s) to import
		if (strcmp(argv[x], "-b") == 0)
		{
			// Read Boundary File(s)

			z = x + 1;
			int zHold = z;

			// Determine how many boundary files are present, and allocate accordingly.

			while ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				numBoundaryFiles++;
				z++;
			}

			boundaryFiles = new char*[numBoundaryFiles];
			for (int i = 0; i < numBoundaryFiles; i++) 
			{
				boundaryFiles[i] = new char[255];
			}

			numBoundaryFiles = 0;
			z = zHold;
			
			while ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				err = strncpy_s(boundaryFiles[numBoundaryFiles], 255, argv[z], 253);
				numBoundaryFiles++;
				z++;
			}

			z--;
		}

		// -f frequency for Fresnel zone calculation (MHz)
		if (strcmp(argv[x], "-f") == 0)
		{
			z = x + 1;

			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				sscanf_s(argv[z], "%lf", &fresnelZoneFrequency);

				if (fresnelZoneFrequency < MINIMUM_FREQUENCY)
				{
					fresnelZoneFrequency = MINIMUM_FREQUENCY;
				}

				if (fresnelZoneFrequency > MAXIMUM_FREQUENCY)
				{
					fresnelZoneFrequency = MAXIMUM_FREQUENCY;
				}
			}
		}

		// -erp override ERP in .lrp file (Watts)
		if (strcmp(argv[x], "-erp") == 0)
		{
			z = x + 1;

			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				sscanf_s(argv[z], "%lf", &effectiveRadiatedPower);

				if (effectiveRadiatedPower < 0.0)
				{
					effectiveRadiatedPower = -1.0;
				}
			}
		}

		// -time name of file which tracks analysis duration
		if (strcmp(argv[x], "-time") == 0)
		{
			z = x + 1;

			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				err = strncpy_s(time_filename, _countof(time_filename), argv[z], 253);

				// Attempt to create and open the time trace file.
				err = fopen_s(&timeTraceFile, time_filename, "wb");
			}
		}

		// -ano name of alphanumeric output file
		if (strcmp(argv[x], "-ano") == 0)
		{
			z = x + 1;

			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				err = strncpy_s(ano_filename, _countof(ano_filename), argv[z], 253);
			}
		}

		// -ani name of alphanumeric input file
		if (strcmp(argv[x], "-ani") == 0)
		{
			z = x + 1;

			if ((z <= y) && argv[z][0] && (argv[z][0] != '-'))
			{
				err = strncpy_s(ani_filename, _countof(ani_filename), argv[z], 253);
			}
		}
	}

	// Perform some error checking on the arguments
	// and switches parsed from the command-line.
	// If an error is encountered, print a message
	// and exit gracefully.

	if (numTxSites == 0)
	{
		fprintf(stderr, "\n%c*** ERROR: No transmitter site(s) specified\n\n", 7);
		exit(-1);
	}

	for (x = 0, y = 0; x < numTxSites; x++)
	{
		if ((txSites[x].latitude == 91.0) && (txSites[x].longitude == 361.0))
		{
			fprintf(stderr, "\n*** ERROR: Transmitter site #%d not found", x + 1);
			y++;
		}
	}

	if (y)
	{
		fprintf(stderr, "%c\n\n", 7);
		exit(-1);
	}

	if (!coverage && !LRmap && (ani_filename[0] == 0) && (rx_site.latitude == 91.0) && (rx_site.longitude == 361.0))
	{
		if ((maximumAnalysisDistance != 0.0) && (numTxSites != 0))
		{
			// Plot topographic map of radius "maximumAnalysisDistance"

			map = false;
			topomap = true;
		}
		else
		{
			fprintf(stderr, "\n%c*** ERROR: No receiver site found or specified\n\n", 7);
			exit(-1);
		}
	}

	// No major errors were detected

	// Capture start time of analysis

	if (timeTraceFile != NULL)
	{
		startTime = system_clock::now();
	}

	// Adjust input parameters if -metric option is used

	if (useMetricUnits)
	{
		altitudeLR /= METERS_PER_FOOT;
		maximumAnalysisDistance /= KM_PER_MILE;
		altitude /= METERS_PER_FOOT;
		groundClutterHeight /= METERS_PER_FOOT;
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

	// Ensure a trailing '\\' is present in reportSavePath
	if (reportSavePath[0])
	{
		x = (int)strlen(reportSavePath);

		if ((reportSavePath[x - 1] != '\\') && (x != 0))
		{
			reportSavePath[x] = '\\';
			reportSavePath[x + 1] = 0;
		}

		// Parse reportSavePath to replace '\\' with '\\\\'
		ReplaceBackslashWithDoubleBackslash(reportSavePath, reportSavePathForPlot);
	}

	fprintf(stdout, "%s", header);
	fflush(stdout);

	if (ani_filename[0])
	{
		ReadLongleyRiceParameterDataForSite(txSites[0], 0, itmParameters, &gotAntennaElevationAnglePattern, &gotAntennaAzimuthAnglePattern,
			fresnelZoneFrequency, effectiveRadiatedPower, reportSavePath); // Get ERP status

		y = LoadSplatAlphanumericOutputFile(ani_filename, digitalElevationModelWrapper, itmParameters, plotSignalPowerLevelContours, 
			contourDisplayThreshold, &maximumElevation, &minimumElevation, &minimumLatitudeNorth, &maximumLatitudeNorth,
			&minimumLongitudeWest, &maximumLongitudeWest, splatDataFilePath);

		for (x = 0; x < numTxSites; x++)
		{
			PlaceTextAndMarkerDataInMaskArrayForMaps(txSites[x], digitalElevationModelWrapper, minimumLatitudeNorth, maximumLatitudeNorth,
				minimumLongitudeWest, maximumLongitudeWest);
		}

		if (rxsite)
		{
			PlaceTextAndMarkerDataInMaskArrayForMaps(rx_site, digitalElevationModelWrapper, minimumLatitudeNorth, maximumLatitudeNorth,
				minimumLongitudeWest, maximumLongitudeWest);
		}

		if (numBoundaryFiles > 0)
		{
			for (x = 0; x < numBoundaryFiles; x++)
			{
				LoadCartographicBoundaryFiles(boundaryFiles[x], digitalElevationModelWrapper, path);
			}

			fprintf(stdout, "\n");
			fflush(stdout);
		}

		if (numCityFiles > 0)
		{
			for (x = 0; x < numCityFiles; x++)
			{
				LoadAndPlotCitiesAndSitesOnMaps(cityFiles[x], digitalElevationModelWrapper, minimumLatitudeNorth, maximumLatitudeNorth,
					minimumLongitudeWest, maximumLongitudeWest);
			}

			fprintf(stdout, "\n");
			fflush(stdout);
		}

		if (itmParameters->effectiveRadiatedPower == 0.0)
		{
			WritePortablePixMapAttenuationFile(mapfile, geo, kml, ngs, txSites, numTxSites, digitalElevationModelWrapper, region, displaySmoothContourLevels,
				contourDisplayThreshold, maximumElevation, minimumElevation, minimumLatitudeNorth, maximumLatitudeNorth, minimumLongitudeWest, maximumLongitudeWest);
		}
		else
		{
			if (plotSignalPowerLevelContours != 0)
			{
				WritePortablePixMapPowerLevelFile(mapfile, geo, kml, ngs, txSites, numTxSites, digitalElevationModelWrapper, region, displaySmoothContourLevels,
					contourDisplayThreshold, maximumElevation, minimumElevation, minimumLatitudeNorth, maximumLatitudeNorth, minimumLongitudeWest, maximumLongitudeWest);
			}
			else
			{
				WritePortablePixMapSignalStrengthFile(mapfile, geo, kml, ngs, txSites, numTxSites, digitalElevationModelWrapper, region, displaySmoothContourLevels,
					contourDisplayThreshold, maximumElevation, minimumElevation, minimumLatitudeNorth, maximumLatitudeNorth, minimumLongitudeWest, maximumLongitudeWest);
			}
		}

		if (timeTraceFile != NULL)
		{
			endTime = system_clock::now();
			auto secsDuration = duration_cast<seconds>(endTime - startTime).count();
			fprintf(timeTraceFile, "Duration in seconds : %lld\n", secsDuration);
			fclose(timeTraceFile);
			timeTraceFile = NULL;
		}

		exit(0);
	}

	x = 0;
	y = 0;

	min_lat = 90;
	max_lat = -90;

	min_lon = (int)floor(txSites[0].longitude);
	max_lon = (int)floor(txSites[0].longitude);

	for (y = 0, z = 0; z < numTxSites; z++)
	{
		txlat = (int)floor(txSites[z].latitude);
		txlon = (int)floor(txSites[z].longitude);

		if (txlat < min_lat)
		{
			min_lat = txlat;
		}

		if (txlat > max_lat)
		{
			max_lat = txlat;
		}

		if (LongitudeDifference(txlon, min_lon) < 0.0)
		{
			min_lon = txlon;
		}

		if (LongitudeDifference(txlon, max_lon) >= 0.0)
		{
			max_lon = txlon;
		}
	}

	if (rxsite)
	{
		rxlat = (int)floor(rx_site.latitude);
		rxlon = (int)floor(rx_site.longitude);

		if (rxlat < min_lat)
		{
			min_lat = rxlat;
		}

		if (rxlat > max_lat)
		{
			max_lat = rxlat;
		}

		if (LongitudeDifference(rxlon, min_lon) < 0.0)
		{
			min_lon = rxlon;
		}

		if (LongitudeDifference(rxlon, max_lon) >= 0.0)
		{
			max_lon = rxlon;
		}
	}

	// Load the required SDF files

	LoadSplatDataFilesForRegion(max_lon, min_lon, max_lat, min_lat, digitalElevationModelWrapper, &maximumElevation, &minimumElevation,
		&minimumLatitudeNorth, &maximumLatitudeNorth, &minimumLongitudeWest, &maximumLongitudeWest, splatDataFilePath);

	if (area_mode || topomap)
	{
		for (z = 0; z < numTxSites; z++)
		{
			// "Ball park" estimates used to load any additional
			// SDF files required to conduct this analysis.

			tx_range = sqrt(1.5 * (txSites[z].altitude + GetSiteLocationElevation(txSites[z], digitalElevationModelWrapper)));

			if (LRmap)
			{
				rx_range = sqrt(1.5 * altitudeLR);
			}
			else
			{
				rx_range = sqrt(1.5 * altitude);
			}

			// deg_range determines the maximum
			// amount of topo data we read

			deg_range = (tx_range + rx_range) / 57.0;

			// maximumAnalysisDistance regulates the size of the
			// analysis. A small, non-zero amount can
			// be used to shrink the size of the analysis
			// and limit the amount of topo data read by
			// SPLAT. A large number will increase the
			// width of the analysis and the size of
			// the map.

			if (maximumAnalysisDistance == 0.0)
			{
				maximumAnalysisDistance = tx_range + rx_range;
			}

			deg_range = maximumAnalysisDistance / 57.0;

			// Prevent the demand for a really wide coverage
			// from allocating more "pages" than are available
			// in memory.

			switch (maxPages)
			{
			case 1: 
				deg_limit = 0.125;
				break;

			case 2: 
				deg_limit = 0.25;
				break;

			case 4: 
				deg_limit = 0.5;
				break;

			case 9: 
				deg_limit = 1.0;
				break;

			case 16: 
				deg_limit = 1.5;	// WAS 2.0
				break;

			case 25: 
				deg_limit = 2.0;	// WAS 3.0
				break;

			case 36: 
				deg_limit = 2.5;	// New
				break;

			case 49: 
				deg_limit = 3.0;	// New
				break;

			case 64: 
				deg_limit = 3.5;	// New
				break;
			}

			if (fabs(txSites[z].latitude) < 70.0)
			{
				deg_range_lon = deg_range / cos(DEGREES_TO_RADIANS * txSites[z].latitude);
			}
			else
			{
				deg_range_lon = deg_range / cos(DEGREES_TO_RADIANS * 70.0);
			}

			// Correct for squares in degrees not being square in miles

			if (deg_range > deg_limit)
			{
				deg_range = deg_limit;
			}

			if (deg_range_lon > deg_limit)
			{
				deg_range_lon = deg_limit;
			}

			north_min = (int)floor(txSites[z].latitude - deg_range);
			north_max = (int)floor(txSites[z].latitude + deg_range);

			west_min = (int)floor(txSites[z].longitude - deg_range_lon);

			while (west_min < 0)
			{
				west_min += 360;
			}

			while (west_min >= 360)
			{
				west_min -= 360;
			}

			west_max = (int)floor(txSites[z].longitude + deg_range_lon);

			while (west_max < 0)
			{
				west_max += 360;
			}

			while (west_max >= 360)
			{
				west_max -= 360;
			}

			if (north_min < min_lat)
			{
				min_lat = north_min;
			}

			if (north_max > max_lat)
			{
				max_lat = north_max;
			}

			if (LongitudeDifference(west_min, min_lon) < 0.0)
			{
				min_lon = west_min;
			}

			if (LongitudeDifference(west_max, max_lon) >= 0.0)
			{
				max_lon = west_max;
			}
		}

		// Load any additional SDF files, if required

		LoadSplatDataFilesForRegion(max_lon, min_lon, max_lat, min_lat, digitalElevationModelWrapper, &maximumElevation, &minimumElevation,
			&minimumLatitudeNorth, &maximumLatitudeNorth, &minimumLongitudeWest, &maximumLongitudeWest, splatDataFilePath);
	}

	if (udt_file[0])
	{
		LoadUserDefinedTerrainFile(udt_file, digitalElevationModelWrapper);
	}

	// Begin SPLAT

	if (pt2pt_mode)
	{
		PlaceTextAndMarkerDataInMaskArrayForMaps(rx_site, digitalElevationModelWrapper, minimumLatitudeNorth, maximumLatitudeNorth,
			minimumLongitudeWest, maximumLongitudeWest);

		if (terrain_plot)
		{
			// Extract extension (if present)
			// from "terrain_file"

			y = (int)strlen(terrain_file_temp);

			for (x = y - 1; (x > 0) && (terrain_file_temp[x] != '.'); x--);

			if (x > 0)  // Extension found
			{
				for (z = x + 1; (z <= y) && ((z - (x + 1)) < 10); z++)
				{
					ext[z - (x + 1)] = tolower(terrain_file_temp[z]);
				}

				ext[z - (x + 1)] = 0;	// Ensure an ending 0
				terrain_file_temp[x] = 0;	// Chop off extension
			}
			else
			{
				err = strncpy_s(ext, _countof(ext), "png\0", 4);
			}

			// Parse terrain_file to replace '\\' with '\\\\'
			ReplaceBackslashWithDoubleBackslash(terrain_file_temp, terrain_file);
		}

		if (elevation_plot)
		{
			// Extract extension (if present)
			// from "elevation_file"

			y = (int)strlen(elevation_file_temp);

			for (x = y - 1; (x > 0) && (elevation_file_temp[x] != '.'); x--);

			if (x > 0)  // Extension found
			{
				for (z = x + 1; (z <= y) && ((z - (x + 1)) < 10); z++)
				{
					ext[z - (x + 1)] = tolower(elevation_file_temp[z]);
				}

				ext[z - (x + 1)] = 0;	// Ensure an ending 0
				elevation_file_temp[x] = 0;	// Chop off extension
			}
			else
			{
				err = strncpy_s(ext, _countof(ext), "png\0", 4);
			}

			// Parse elevation_file to replace '\\' with '\\\\'
			ReplaceBackslashWithDoubleBackslash(elevation_file_temp, elevation_file);
		}

		if (height_plot)
		{
			// Extract extension (if present)
			// from "height_file"			

			y = (int)strlen(height_file_temp);

			for (x = y - 1; (x > 0) && (height_file_temp[x] != '.'); x--);

			if (x > 0)  // Extension found
			{
				for (z = x + 1; (z <= y) && ((z - (x + 1)) < 10); z++)
				{
					ext[z - (x + 1)] = tolower(height_file_temp[z]);
				}

				ext[z - (x + 1)] = 0;	// Ensure an ending 0
				height_file_temp[x] = 0;		// Chop off extension
			}
			else
			{
				err = strncpy_s(ext, _countof(ext), "png\0", 4);
			}

			// Parse height_file to replace '\\' with '\\\\'
			ReplaceBackslashWithDoubleBackslash(height_file_temp, height_file);
		}

		if (longley_plot)
		{
			// Extract extension (if present)
			// from "longley_file"

			y = (int)strlen(longley_file_temp);

			for (x = y - 1; (x > 0) && (longley_file_temp[x] != '.'); x--);

			if (x > 0)  // Extension found
			{
				for (z = x + 1; (z <= y) && ((z - (x + 1)) < 10); z++)
				{
					ext[z - (x + 1)] = tolower(longley_file_temp[z]);
				}

				ext[z - (x + 1)] = 0;	// Ensure an ending 0
				longley_file_temp[x] = 0;	// Chop off extension
			}
			else
			{
				err = strncpy_s(ext, _countof(ext), "png\0", 4);
			}

			// Parse longley_file to replace '\\' with '\\\\'
			ReplaceBackslashWithDoubleBackslash(longley_file_temp, longley_file);
		}

		for (x = 0; x < numTxSites; x++)
		{
			PlaceTextAndMarkerDataInMaskArrayForMaps(txSites[x], digitalElevationModelWrapper, minimumLatitudeNorth, maximumLatitudeNorth,
				minimumLongitudeWest, maximumLongitudeWest);

			if (nolospath == false)
			{
				switch (x)
				{
				case 0:
					AnalyzeAndPlotLineOfSightCoverageBetweenSites(txSites[x], rx_site, 1, NULL, digitalElevationModelWrapper, path, groundClutterHeight, sphereRadius);
					break;

				case 1:
					AnalyzeAndPlotLineOfSightCoverageBetweenSites(txSites[x], rx_site, 8, NULL, digitalElevationModelWrapper, path, groundClutterHeight, sphereRadius);
					break;

				case 2:
					AnalyzeAndPlotLineOfSightCoverageBetweenSites(txSites[x], rx_site, 16, NULL, digitalElevationModelWrapper, path, groundClutterHeight, sphereRadius);
					break;

				case 3:
					AnalyzeAndPlotLineOfSightCoverageBetweenSites(txSites[x], rx_site, 32, NULL, digitalElevationModelWrapper, path, groundClutterHeight, sphereRadius);
				}
			}

			if (nositereports == false)
			{
				WriteSplatSiteReport(txSites[x], digitalElevationModelWrapper, path, useMetricUnits, groundClutterHeight, reportSavePath);
			}

			if (kml)
			{
				WriteKeyholeMarkupLanguageFile(txSites[x], rx_site, digitalElevationModelWrapper, path, useMetricUnits, sphereRadius);
			}

			if (numTxSites > 1)
			{
				snprintf(MainString, 250, "%s-%c.%s%c", longley_file, '1' + x, ext, 0);
			}
			else
			{
				snprintf(MainString, 250, "%s.%s%c", longley_file, ext, 0);
			}

			if (nositereports == false)
			{
				if (longley_file[0] == 0)
				{
					ReadLongleyRiceParameterDataForSite(txSites[x], 0, itmParameters, &gotAntennaElevationAnglePattern, &gotAntennaAzimuthAnglePattern,
						fresnelZoneFrequency, effectiveRadiatedPower, reportSavePath);

					WriteSplatPathReport(txSites[x], rx_site, MainString, 0, digitalElevationModelWrapper, path, itmParameters,
						&pathElevation[0], useOldLongleyRiceModel, saveGnuPlotWorkingFiles, 
						gotAntennaElevationAnglePattern, gotAntennaAzimuthAnglePattern, useMetricUnits, 
						groundClutterHeight, fresnelZoneClearanceRatio, sphereRadius, reportSavePathForPlot);
				}
				else
				{
					ReadLongleyRiceParameterDataForSite(txSites[x], 1, itmParameters, &gotAntennaElevationAnglePattern, &gotAntennaAzimuthAnglePattern,
						fresnelZoneFrequency, effectiveRadiatedPower, reportSavePath);

					WriteSplatPathReport(txSites[x], rx_site, MainString, longley_file[0], digitalElevationModelWrapper, path, itmParameters,
						&pathElevation[0], useOldLongleyRiceModel, saveGnuPlotWorkingFiles, 
						gotAntennaElevationAnglePattern, gotAntennaAzimuthAnglePattern, useMetricUnits, 
						groundClutterHeight, fresnelZoneClearanceRatio, sphereRadius, reportSavePathForPlot);
				}
			}

			if (terrain_plot)
			{
				if (numTxSites > 1)
				{
					snprintf(MainString, 250, "%s-%c.%s%c", terrain_file, '1' + x, ext, 0);
				}
				else
				{
					snprintf(MainString, 250, "%s.%s%c", terrain_file, ext, 0);
				}

				GenerateGnuPlotTerrainProfileBetweenSites(txSites[x], rx_site, MainString, digitalElevationModelWrapper, path, saveGnuPlotWorkingFiles,
					useMetricUnits, groundClutterHeight, reportSavePathForPlot);
			}

			if (elevation_plot)
			{
				if (numTxSites > 1)
				{
					snprintf(MainString, 250, "%s-%c.%s%c", elevation_file, '1' + x, ext, 0);
				}
				else
				{
					snprintf(MainString, 250, "%s.%s%c", elevation_file, ext, 0);
				}

				GenerateGnuPlotElevationProfileBetweenSites(txSites[x], rx_site, MainString, digitalElevationModelWrapper, path, saveGnuPlotWorkingFiles,
					useMetricUnits, groundClutterHeight, sphereRadius, reportSavePathForPlot);
			}

			if (height_plot)
			{
				if (numTxSites > 1)
				{
					snprintf(MainString, 250, "%s-%c.%s%c", height_file, '1' + x, ext, 0);
				}
				else
				{
					snprintf(MainString, 250, "%s.%s%c", height_file, ext, 0);
				}

				GenerateGnuPlotHeightProfileBetweenSites(txSites[x], rx_site, MainString, fresnel_plot, norm, digitalElevationModelWrapper,
					path, itmParameters, saveGnuPlotWorkingFiles, useMetricUnits, groundClutterHeight, fresnelZoneClearanceRatio, sphereRadius, reportSavePathForPlot);
			}
		}
	}

	if (area_mode && (topomap == false))
	{
		for (x = 0; x < numTxSites; x++)
		{
			if (coverage)
			{
				PlotLineOfSightCoverageFromSiteAtAltitude(txSites[x], altitude, ano_filename, digitalElevationModelWrapper, path, useMetricUnits,
					minimumLatitudeNorth, maximumLatitudeNorth, minimumLongitudeWest, maximumLongitudeWest, groundClutterHeight, sphereRadius);
			}
			else if (ReadLongleyRiceParameterDataForSite(txSites[x], 1, itmParameters, &gotAntennaElevationAnglePattern, &gotAntennaAzimuthAnglePattern, fresnelZoneFrequency, effectiveRadiatedPower, reportSavePath))
			{
				PlotAttenuationFromSiteAtAltitude(txSites[x], altitudeLR, ano_filename, digitalElevationModelWrapper, path, itmParameters,
					&pathElevation[0], useOldLongleyRiceModel, plotSignalPowerLevelContours, gotAntennaElevationAnglePattern, useMetricUnits,
					minimumLatitudeNorth, maximumLatitudeNorth, minimumLongitudeWest, maximumLongitudeWest,
					maximumAnalysisDistance, groundClutterHeight);
			}

			WriteSplatSiteReport(txSites[x], digitalElevationModelWrapper, path, useMetricUnits, groundClutterHeight, reportSavePath);
		}
	}

	if (map || topomap)
	{
		// Label the map

		if (kml == 0)
		{
			for (x = 0; x < numTxSites; x++)
			{
				PlaceTextAndMarkerDataInMaskArrayForMaps(txSites[x], digitalElevationModelWrapper, minimumLatitudeNorth, maximumLatitudeNorth,
					minimumLongitudeWest, maximumLongitudeWest);
			}
		}

		if (numCityFiles > 0)
		{
			for (y = 0; y < numCityFiles; y++)
			{
				LoadAndPlotCitiesAndSitesOnMaps(cityFiles[y], digitalElevationModelWrapper, minimumLatitudeNorth, maximumLatitudeNorth,
					minimumLongitudeWest, maximumLongitudeWest);
			}

			fprintf(stdout, "\n");
			fflush(stdout);
		}

		// Load city and county boundary data files

		if (numBoundaryFiles > 0)
		{
			for (y = 0; y < numBoundaryFiles; y++)
			{
				LoadCartographicBoundaryFiles(boundaryFiles[y], digitalElevationModelWrapper, path);
			}

			fprintf(stdout, "\n");
			fflush(stdout);
		}

		// Plot the map

		if (coverage || pt2pt_mode || topomap)
		{
			WritePortablePixMapLineOfSightCoverageFile(mapfile, geo, kml, ngs, txSites, numTxSites, digitalElevationModelWrapper, maximumElevation, minimumElevation,
				minimumLatitudeNorth, maximumLatitudeNorth, minimumLongitudeWest, maximumLongitudeWest);
		}
		else
		{
			if (itmParameters->effectiveRadiatedPower == 0.0)
			{
				WritePortablePixMapAttenuationFile(mapfile, geo, kml, ngs, txSites, numTxSites, digitalElevationModelWrapper, region, displaySmoothContourLevels,
					contourDisplayThreshold, maximumElevation, minimumElevation, minimumLatitudeNorth, maximumLatitudeNorth, minimumLongitudeWest, maximumLongitudeWest);
			}
			else
			{
				if (plotSignalPowerLevelContours != 0)
				{
					WritePortablePixMapPowerLevelFile(mapfile, geo, kml, ngs, txSites, numTxSites, digitalElevationModelWrapper, region, displaySmoothContourLevels,
						contourDisplayThreshold, maximumElevation, minimumElevation, minimumLatitudeNorth, maximumLatitudeNorth, minimumLongitudeWest, maximumLongitudeWest);
				}
				else
				{
					WritePortablePixMapSignalStrengthFile(mapfile, geo, kml, ngs, txSites, numTxSites, digitalElevationModelWrapper, region, displaySmoothContourLevels,
						contourDisplayThreshold, maximumElevation, minimumElevation, minimumLatitudeNorth, maximumLatitudeNorth, minimumLongitudeWest, maximumLongitudeWest);
				}
			}
		}
	}

	if (command_line_log && (strlen(logfile) > 0))
	{
		err = fopen_s(&fd, logfile, "w");

		if (fd != NULL)
		{
			for (x = 0; x < argc; x++)
			{
				fprintf(fd, "%s ", argv[x]);
			}

			fprintf(fd, "\n");

			fclose(fd);

			fprintf(stdout, "\nCommand-line parameter log written to: \"%s\"\n", logfile);
		}
	}

	printf("\n");

	delete[] txSites;
	txSites = NULL;

	for (int i = 0; i < numBoundaryFiles; i++)
	{
		delete[] boundaryFiles[i];
		boundaryFiles[i] = NULL;
	}
	delete[] boundaryFiles;
	boundaryFiles = NULL;

	for (int i = 0; i < numCityFiles; i++)
	{
		delete[] cityFiles[i];
		cityFiles[i] = NULL;
	}
	delete[] cityFiles;
	cityFiles = NULL;

	delete digitalElevationModelWrapper;
	digitalElevationModelWrapper = NULL;

	delete path;
	path = NULL;

	delete itmParameters;
	itmParameters = NULL;

	delete region;
	region = NULL;

	delete[] pathElevation;
	pathElevation = NULL;

	if (timeTraceFile != NULL)
	{
		endTime = system_clock::now();
		auto secsDuration = duration_cast<seconds>(endTime - startTime).count();
		fprintf(timeTraceFile, "Duration in seconds : %lld\n", secsDuration);
		fclose(timeTraceFile);
		timeTraceFile = NULL;
	}

	return 0;
}
