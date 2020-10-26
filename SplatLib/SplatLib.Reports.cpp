/****************************************************************************\
*					SplatLib.Reports: Splat Reports							 *
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
//| FUNCTION: WriteSplatPathReport
//| 
//| OLD NAME: PathReport
//| 
//| NOTES: 
//|   This function writes a SPLAT Path Report (name.txt) to
//|   the filesystem. If (graph_it == 1), then gnuplot is invoked
//|   to generate an appropriate output file indicating the ITM
//|   model loss between the source and destination locations.
//|   "filename" is the name assigned to the output file generated
//|   by gnuplot. The filename extension is used to set gnuplot's
//|   terminal setting and output file type. If no extension is
//|   found, .png is assumed.
//| 
//| ------------------------------
void
WriteSplatPathReport(
	Site source,
	Site destination,
	char* name,
	char graph_it,
	DigitalElevationModelWrapper* digitalElevationModelWrapper,
	Path* path,
	IrregularTerrainModelParameters* itmParameters,
	double* pathElevation,
	char useOldLongleyRiceModel,
	char saveGnuPlotWorkingFiles,
	unsigned char gotAntennaElevationAnglePattern,
	unsigned char gotAntennaAzimuthAnglePattern,
	unsigned char useMetricUnits,
	double groundClutterHeight,
	double fresnelZoneClearanceRatio,
	double sphereRadius,
	char* reportSavePath)
{
	int	x, y, z, errnum;
	char basename[255], term[30], ext[15], strmode[100],
		report_name[80], propstring[20];
	bool block = false;
	double maxloss = -100000.0, minloss = 100000.0, loss, haavt,
		angle1, angle2, azimuth, pattern = 1.0, patterndB = 0.0,
		total_loss = 0.0, cos_xmtr_angle, cos_test_angle = 0.0,
		source_alt, test_alt, dest_alt, source_alt2, dest_alt2,
		distance, elevation, four_thirds_earth, field_strength,
		free_space_loss = 0.0, eirp = 0.0, voltage, rxp, dBm,
		power_density;
	FILE* fd = NULL, * fd2 = NULL;
	errno_t err;

	char dmsString[255];

	char lineOfDashes[80];	// OLD NAME: dashes
	err = strncpy_s(lineOfDashes, _countof(lineOfDashes), "---------------------------------------------------------------------------\0", 76);

	sprintf_s(report_name, _countof(report_name), "%s-to-%s.txt", source.name, destination.name);

	char plotNameAndPath[335];
	sprintf_s(plotNameAndPath, _countof(plotNameAndPath), "%s%s", (reportSavePath[0] ? reportSavePath : ""), "splat.gp");

	char profileNameAndPath[335];
	sprintf_s(profileNameAndPath, _countof(profileNameAndPath), "%s%s", (reportSavePath[0] ? reportSavePath : ""), "profile.gp");

	char referenceNameAndPath[335];
	sprintf_s(referenceNameAndPath, _countof(referenceNameAndPath), "%s%s", (reportSavePath[0] ? reportSavePath : ""), "reference.gp");

	four_thirds_earth = FOUR_THIRDS * EARTH_RADIUS_FEET;

	for (x = 0; report_name[x] != 0; x++)
	{
		if ((report_name[x] == 32) || (report_name[x] == 17) || (report_name[x] == 92) || (report_name[x] == 42) || (report_name[x] == 47))
		{
			report_name[x] = '_';
		}
	}

	char reportNameAndPath[335];
	sprintf_s(reportNameAndPath, _countof(reportNameAndPath), "%s%s", (reportSavePath[0] ? reportSavePath : ""), report_name);

	err = fopen_s(&fd2, reportNameAndPath, "w");

	fprintf(fd2, "\n\t\t--==[ %s v%s Path Analysis ]==--\n\n", APP_NAME, VERSION_HEADER);
	fprintf(fd2, "%s\n\n", lineOfDashes);
	fprintf(fd2, "Transmitter site: %s\n", source.name);

	if (source.latitude >= 0.0)
	{
		ConvertDecimalToDegreesMinutesSeconds(source.latitude, dmsString);
		fprintf(fd2, "Site location: %.4f North / %.4f West", source.latitude, source.longitude);
		fprintf(fd2, " (%s N / ", dmsString);
	}
	else
	{
		ConvertDecimalToDegreesMinutesSeconds(source.latitude, dmsString);
		fprintf(fd2, "Site location: %.4f South / %.4f West", -source.latitude, source.longitude);
		fprintf(fd2, " (%s S / ", dmsString);
	}

	ConvertDecimalToDegreesMinutesSeconds(source.longitude, dmsString);
	fprintf(fd2, "%s W)\n", dmsString);

	if (useMetricUnits)
	{
		fprintf(fd2, "Ground elevation: %.2f meters AMSL\n", METERS_PER_FOOT * GetSiteLocationElevation(source, digitalElevationModelWrapper));
		fprintf(fd2, "Antenna height: %.2f meters AGL / %.2f meters AMSL\n", METERS_PER_FOOT * source.altitude, METERS_PER_FOOT * (source.altitude + GetSiteLocationElevation(source, digitalElevationModelWrapper)));
	}
	else
	{
		fprintf(fd2, "Ground elevation: %.2f feet AMSL\n", GetSiteLocationElevation(source, digitalElevationModelWrapper));
		fprintf(fd2, "Antenna height: %.2f feet AGL / %.2f feet AMSL\n", source.altitude, source.altitude + GetSiteLocationElevation(source, digitalElevationModelWrapper));
	}

	haavt = AntennaHeightAboveAverageTerrain(source, digitalElevationModelWrapper, path, groundClutterHeight);

	if (haavt > (LOCATION_NOT_IN_MEMORY + 1))
	{
		if (useMetricUnits)
		{
			fprintf(fd2, "Antenna height above average terrain: %.2f meters\n", METERS_PER_FOOT * haavt);
		}
		else
		{
			fprintf(fd2, "Antenna height above average terrain: %.2f feet\n", haavt);
		}
	}

	azimuth = AzimuthAngleBetweenSites(source, destination);
	angle1 = ElevationAngleBetweenSites(source, destination, digitalElevationModelWrapper, sphereRadius);
	angle2 = ObstructedElevationAngleBetweenSites(source, destination, sphereRadius, digitalElevationModelWrapper, path, groundClutterHeight, sphereRadius);

	if (gotAntennaAzimuthAnglePattern || gotAntennaElevationAnglePattern)
	{
		x = (int)rint(10.0 * (10.0 - angle2));

		if ((x >= 0) && (x <= 1000))
		{
			pattern = (double)itmParameters->antennaRadiationPattern[(int)rint(azimuth)][x];
		}

		patterndB = 20.0 * log10(pattern);
	}

	if (useMetricUnits)
	{
		fprintf(fd2, "Distance to %s: %.2f kilometers\n", destination.name, KM_PER_MILE * GreatCircleDistanceBetweenSiteLocations(source, destination));
	}
	else
	{
		fprintf(fd2, "Distance to %s: %.2f miles\n", destination.name, GreatCircleDistanceBetweenSiteLocations(source, destination));
	}

	fprintf(fd2, "Azimuth to %s: %.2f degrees\n", destination.name, azimuth);

	if (angle1 >= 0.0)
	{
		fprintf(fd2, "Elevation angle to %s: %+.4f degrees\n", destination.name, angle1);
	}
	else
	{
		fprintf(fd2, "Depression angle to %s: %+.4f degrees\n", destination.name, angle1);
	}

	if ((angle2 - angle1) > 0.0001)
	{
		if (angle2 < 0.0)
		{
			fprintf(fd2, "Depression");
		}
		else
		{
			fprintf(fd2, "Elevation");
		}
		fprintf(fd2, " angle to the first obstruction: %+.4f degrees\n", angle2);
	}

	fprintf(fd2, "\n%s\n\n", lineOfDashes);

	// Receiver

	fprintf(fd2, "Receiver site: %s\n", destination.name);

	if (destination.latitude >= 0.0)
	{
		ConvertDecimalToDegreesMinutesSeconds(destination.latitude, dmsString);
		fprintf(fd2, "Site location: %.4f North / %.4f West", destination.latitude, destination.longitude);
		fprintf(fd2, " (%s N / ", dmsString);
	}
	else
	{
		ConvertDecimalToDegreesMinutesSeconds(destination.latitude, dmsString);
		fprintf(fd2, "Site location: %.4f South / %.4f West", -destination.latitude, destination.longitude);
		fprintf(fd2, " (%s S / ", dmsString);
	}

	ConvertDecimalToDegreesMinutesSeconds(destination.longitude, dmsString);
	fprintf(fd2, "%s W)\n", dmsString);

	if (useMetricUnits)
	{
		fprintf(fd2, "Ground elevation: %.2f meters AMSL\n", METERS_PER_FOOT * GetSiteLocationElevation(destination, digitalElevationModelWrapper));
		fprintf(fd2, "Antenna height: %.2f meters AGL / %.2f meters AMSL\n", METERS_PER_FOOT * destination.altitude, METERS_PER_FOOT * (destination.altitude + GetSiteLocationElevation(destination, digitalElevationModelWrapper)));
	}
	else
	{
		fprintf(fd2, "Ground elevation: %.2f feet AMSL\n", GetSiteLocationElevation(destination, digitalElevationModelWrapper));
		fprintf(fd2, "Antenna height: %.2f feet AGL / %.2f feet AMSL\n", destination.altitude, destination.altitude + GetSiteLocationElevation(destination, digitalElevationModelWrapper));
	}

	haavt = AntennaHeightAboveAverageTerrain(destination, digitalElevationModelWrapper, path, groundClutterHeight);

	if (haavt > (LOCATION_NOT_IN_MEMORY + 1))
	{
		if (useMetricUnits)
		{
			fprintf(fd2, "Antenna height above average terrain: %.2f meters\n", METERS_PER_FOOT * haavt);
		}
		else
		{
			fprintf(fd2, "Antenna height above average terrain: %.2f feet\n", haavt);
		}
	}

	if (useMetricUnits)
	{
		fprintf(fd2, "Distance to %s: %.2f kilometers\n", source.name, KM_PER_MILE * GreatCircleDistanceBetweenSiteLocations(source, destination));
	}
	else
	{
		fprintf(fd2, "Distance to %s: %.2f miles\n", source.name, GreatCircleDistanceBetweenSiteLocations(source, destination));
	}

	azimuth = AzimuthAngleBetweenSites(destination, source);

	angle1 = ElevationAngleBetweenSites(destination, source, digitalElevationModelWrapper, sphereRadius);
	angle2 = ObstructedElevationAngleBetweenSites(destination, source, sphereRadius, digitalElevationModelWrapper, path, groundClutterHeight, sphereRadius);

	fprintf(fd2, "Azimuth to %s: %.2f degrees\n", source.name, azimuth);

	if (angle1 >= 0.0)
	{
		fprintf(fd2, "Elevation angle to %s: %+.4f degrees\n", source.name, angle1);
	}
	else
	{
		fprintf(fd2, "Depression angle to %s: %+.4f degrees\n", source.name, angle1);
	}

	if ((angle2 - angle1) > 0.0001)
	{
		if (angle2 < 0.0)
		{
			fprintf(fd2, "Depression");
		}
		else
		{
			fprintf(fd2, "Elevation");
		}
		fprintf(fd2, " angle to the first obstruction: %+.4f degrees\n", angle2);
	}

	fprintf(fd2, "\n%s\n\n", lineOfDashes);

	if (itmParameters->referenceFrequency > 0.0)
	{
		if (useOldLongleyRiceModel)
		{
			fprintf(fd2, "Longley-Rice Parameters Used In This Analysis:\n\n");
		}
		else
		{
			fprintf(fd2, "ITWOM Version %.1f Parameters Used In This Analysis:\n\n", ITWOM_VERSION);
		}

		fprintf(fd2, "Earth's Dielectric Constant: %.3lf\n", itmParameters->dielectricRelativePermittivityConstant);
		fprintf(fd2, "Earth's Conductivity: %.3lf Siemens/meter\n", itmParameters->conductivitySiemensPerMeter);
		fprintf(fd2, "Atmospheric Bending Constant (N-units): %.3lf ppm\n", itmParameters->atmosphericBendingConstant);
		fprintf(fd2, "Frequency: %.3lf MHz\n", itmParameters->referenceFrequency);
		fprintf(fd2, "Radio Climate: %d (", itmParameters->radioClimate);

		switch (itmParameters->radioClimate)
		{
		case 1:
			fprintf(fd2, "Equatorial");
			break;

		case 2:
			fprintf(fd2, "Continental Subtropical");
			break;

		case 3:
			fprintf(fd2, "Maritime Subtropical");
			break;

		case 4:
			fprintf(fd2, "Desert");
			break;

		case 5:
			fprintf(fd2, "Continental Temperate");
			break;

		case 6:
			fprintf(fd2, "Martitime Temperate, Over Land");
			break;

		case 7:
			fprintf(fd2, "Maritime Temperate, Over Sea");
			break;

		default:
			fprintf(fd2, "Unknown");
		}

		fprintf(fd2, ")\nPolarization: %d (", itmParameters->antennaPolarization);

		if (itmParameters->antennaPolarization == 0)
		{
			fprintf(fd2, "Horizontal");
		}

		if (itmParameters->antennaPolarization == 1)
		{
			fprintf(fd2, "Vertical");
		}

		fprintf(fd2, ")\nFraction of Situations: %.1lf%c\n", itmParameters->fractionOfSituations * 100.0, 37);
		fprintf(fd2, "Fraction of Time: %.1lf%c\n", itmParameters->fractionOfTime * 100.0, 37);

		if (itmParameters->effectiveRadiatedPower != 0.0)
		{
			fprintf(fd2, "Transmitter ERP: ");

			if (itmParameters->effectiveRadiatedPower < 1.0)
			{
				fprintf(fd2, "%.1lf milliwatts", 1000.0 * itmParameters->effectiveRadiatedPower);
			}

			if ((itmParameters->effectiveRadiatedPower >= 1.0) && (itmParameters->effectiveRadiatedPower < 10.0))
			{
				fprintf(fd2, "%.1lf Watts", itmParameters->effectiveRadiatedPower);
			}

			if ((itmParameters->effectiveRadiatedPower >= 10.0) && (itmParameters->effectiveRadiatedPower < 10.0e3))
			{
				fprintf(fd2, "%.0lf Watts", itmParameters->effectiveRadiatedPower);
			}

			if (itmParameters->effectiveRadiatedPower >= 10.0e3)
			{
				fprintf(fd2, "%.3lf kilowatts", itmParameters->effectiveRadiatedPower / 1.0e3);
			}

			dBm = 10.0 * log10(itmParameters->effectiveRadiatedPower * 1000.0);
			fprintf(fd2, " (%+.2f dBm)\n", dBm);

			// EIRP = ERP + DIPOLE_TO_ISOTROPIC_ANTENNA (dB)

			fprintf(fd2, "Transmitter EIRP: ");

			eirp = itmParameters->effectiveRadiatedPower * DIPOLE_TO_ISOTROPIC_ANTENNA_RATIO;

			if (eirp < 1.0)
			{
				fprintf(fd2, "%.1lf milliwatts", 1000.0 * eirp);
			}

			if ((eirp >= 1.0) && (eirp < 10.0))
			{
				fprintf(fd2, "%.1lf Watts", eirp);
			}

			if ((eirp >= 10.0) && (eirp < 10.0e3))
			{
				fprintf(fd2, "%.0lf Watts", eirp);
			}

			if (eirp >= 10.0e3)
			{
				fprintf(fd2, "%.3lf kilowatts", eirp / 1.0e3);
			}

			dBm = 10.0 * log10(eirp * 1000.0);
			fprintf(fd2, " (%+.2f dBm)\n", dBm);
		}

		fprintf(fd2, "\n%s\n\n", lineOfDashes);

		fprintf(fd2, "Summary For The Link Between %s and %s:\n\n", source.name, destination.name);

		if (patterndB != 0.0)
		{
			fprintf(fd2, "%s antenna pattern towards %s: %.3f (%.2f dB)\n", source.name, destination.name, pattern, patterndB);
		}

		GeneratePathBetweenSites(source, destination, digitalElevationModelWrapper, path);  // source=TX, destination=RX

		// Copy elevations plus clutter along
		// path into the pathElevation[] array.

		for (x = 1; x < path->pathLength - 1; x++)
		{
			pathElevation[x + 2] = METERS_PER_FOOT * (path->elevations[x] == 0.0 ? path->elevations[x] : (groundClutterHeight + path->elevations[x]));
		}

		// Copy ending points without clutter

		pathElevation[2] = path->elevations[0] * METERS_PER_FOOT;
		pathElevation[path->pathLength + 1] = path->elevations[path->pathLength - 1] * METERS_PER_FOOT;

		err = fopen_s(&fd, profileNameAndPath, "w");

		azimuth = rint(AzimuthAngleBetweenSites(source, destination));

		for (y = 2; y < (path->pathLength - 1); y++)  // path->length-1 avoids itmParameters error
		{
			distance = FEET_PER_MILE * path->distances[y];
			source_alt = four_thirds_earth + source.altitude + path->elevations[0];
			dest_alt = four_thirds_earth + destination.altitude + path->elevations[y];
			dest_alt2 = dest_alt * dest_alt;
			source_alt2 = source_alt * source_alt;

			// Calculate the cosine of the elevation of
			// the receiver as seen by the transmitter.

			cos_xmtr_angle = (source_alt2 + (distance * distance) - dest_alt2) / (2.0 * source_alt * distance);

			if (gotAntennaElevationAnglePattern)
			{
				// If an antenna elevation pattern is available, the
				// following code determines the elevation angle to
				// the first obstruction along the path.

				for (x = 2, block = false; (x < y) && (block == false); x++)
				{
					distance = FEET_PER_MILE * (path->distances[y] - path->distances[x]);
					test_alt = four_thirds_earth + path->elevations[x];

					// Calculate the cosine of the elevation
					// angle of the terrain (test point)
					// as seen by the transmitter.

					cos_test_angle = (source_alt2 + (distance * distance) - (test_alt * test_alt)) / (2.0 * source_alt * distance);

					// Compare these two angles to determine if
					// an obstruction exists. Since we're comparing
					// the cosines of these angles rather than
					// the angles themselves, the sense of the
					// following "if" statement is reversed from
					// what it would be if the angles themselves
					// were compared.

					if (cos_xmtr_angle >= cos_test_angle)
					{
						block = true;
					}
				}

				// At this point, we have the elevation angle
				// to the first obstruction (if it exists).
			}

			// Determine path loss for each point along
			// the path using ITWOM's PointToPointCalculation mode
			// starting at x=2 (number_of_points = 1), the
			// shortest distance terrain can play a role in
			// path loss.

			pathElevation[0] = ToDouble(y - 1);	// (number of points - 1)

			// Distance between elevation samples

			pathElevation[1] = METERS_PER_MILE * (path->distances[y] - path->distances[y - 1]);

			if (useOldLongleyRiceModel)
			{
				PointToPointCalculationLegacy(pathElevation, (source.altitude * METERS_PER_FOOT),
					(destination.altitude * METERS_PER_FOOT), itmParameters->dielectricRelativePermittivityConstant,
					itmParameters->conductivitySiemensPerMeter, itmParameters->atmosphericBendingConstant, itmParameters->referenceFrequency,
					itmParameters->radioClimate, itmParameters->antennaPolarization, itmParameters->fractionOfSituations, itmParameters->fractionOfTime, loss,
					strmode, 100, errnum);
			}
			else
			{
				PointToPointCalculation(pathElevation, (source.altitude * METERS_PER_FOOT),
					(destination.altitude * METERS_PER_FOOT), itmParameters->dielectricRelativePermittivityConstant,
					itmParameters->conductivitySiemensPerMeter, itmParameters->atmosphericBendingConstant, itmParameters->referenceFrequency,
					itmParameters->radioClimate, itmParameters->antennaPolarization, itmParameters->fractionOfSituations, itmParameters->fractionOfTime, loss,
					strmode, 100, errnum);
			}

			if (block)
			{
				elevation = ((acos(cos_test_angle)) / DEGREES_TO_RADIANS) - 90.0;
			}
			else
			{
				elevation = ((acos(cos_xmtr_angle)) / DEGREES_TO_RADIANS) - 90.0;
			}

			// Integrate the antenna's radiation
			// pattern into the overall path loss.

			x = (int)rint(10.0 * (10.0 - elevation));

			if ((x >= 0) && (x <= 1000))
			{
				pattern = (double)itmParameters->antennaRadiationPattern[(int)azimuth][x];

				if (pattern != 0.0)
				{
					patterndB = 20.0 * log10(pattern);
				}
			}
			else
			{
				patterndB = 0.0;
			}

			total_loss = loss - patterndB;

			if (useMetricUnits)
			{
				fprintf(fd, "%f\t%f\n", KM_PER_MILE * path->distances[y], total_loss);
			}
			else
			{
				fprintf(fd, "%f\t%f\n", path->distances[y], total_loss);
			}

			if (total_loss > maxloss)
			{
				maxloss = total_loss;
			}

			if (total_loss < minloss)
			{
				minloss = total_loss;
			}
		}

		fclose(fd);

		distance = GreatCircleDistanceBetweenSiteLocations(source, destination);

		if (distance != 0.0)
		{
			free_space_loss = (20.0 * log10(itmParameters->referenceFrequency)) + (20.0 * log10(distance)) + FREE_SPACE_PATH_LOSS_CONSTANT;

			fprintf(fd2, "Free space path loss: %.2f dB\n", free_space_loss);
		}

		if (useOldLongleyRiceModel)
		{
			fprintf(fd2, "Longley-Rice path loss: %.2f dB\n", loss);
		}
		else
		{
			fprintf(fd2, "ITWOM Version %.1f path loss: %.2f dB\n", ITWOM_VERSION, loss);
		}

		if (free_space_loss != 0.0)
		{
			fprintf(fd2, "Attenuation due to terrain shielding: %.2f dB\n", loss - free_space_loss);
		}

		if (patterndB != 0.0)
		{
			fprintf(fd2, "Total path loss including %s antenna pattern: %.2f dB\n", source.name, total_loss);
		}

		if (itmParameters->effectiveRadiatedPower != 0.0)
		{
			field_strength = (FIELD_STRENGTH_MAGIC_NUMBER + (20.0 * log10(itmParameters->referenceFrequency)) - total_loss) + (10.0 * log10(itmParameters->effectiveRadiatedPower / 1000.0));

			// dBm is referenced to EIRP

			rxp = eirp / (pow(10.0, (total_loss / 10.0)));
			dBm = 10.0 * log10(rxp * 1000.0);
			power_density = eirp / (pow(10.0, (total_loss - free_space_loss) / 10.0));
			// divide by 4*PI*distance_in_meters squared
			power_density /= (4.0 * PI * distance * distance * POWER_DENSITY_MAGIC_NUMBER);

			fprintf(fd2, "Field strength at %s: %.2f dBuV/meter\n", destination.name, field_strength);
			fprintf(fd2, "Signal power level at %s: %+.2f dBm\n", destination.name, dBm);
			fprintf(fd2, "Signal power density at %s: %+.2f dBW per square meter\n", destination.name, 10.0 * log10(power_density));
			voltage = 1.0e6 * sqrt(DIPOLE_50_OHM * (eirp / (pow(10.0, (total_loss - DIPOLE_TO_ISOTROPIC_ANTENNA) / 10.0))));
			fprintf(fd2, "Voltage across a 50 ohm dipole at %s: %.2f uV (%.2f dBuV)\n", destination.name, voltage, 20.0 * log10(voltage));

			voltage = 1.0e6 * sqrt(DIPOLE_75_OHM * (eirp / (pow(10.0, (total_loss - DIPOLE_TO_ISOTROPIC_ANTENNA) / 10.0))));
			fprintf(fd2, "Voltage across a 75 ohm dipole at %s: %.2f uV (%.2f dBuV)\n", destination.name, voltage, 20.0 * log10(voltage));
		}

		fprintf(fd2, "Mode of propagation: ");

		if (useOldLongleyRiceModel)
		{
			fprintf(fd2, "%s\n", strmode);
			fprintf(fd2, "Longley-Rice model error number: %d", errnum);
		}
		else
		{
			if (strcmp(strmode, "L-o-S") == 0)
			{
				fprintf(fd2, "Line of Sight\n");
			}

			if (strncmp(strmode, "1_Hrzn", 6) == 0)
			{
				fprintf(fd2, "Single Horizon ");
			}

			if (strncmp(strmode, "2_Hrzn", 6) == 0)
			{
				fprintf(fd2, "Double Horizon ");
			}

			y = (int)strlen(strmode);

			if (y > 19)
			{
				y = 19;
			}

			for (x = 6; x < y; x++)
			{
				propstring[x - 6] = strmode[x];
			}

			propstring[x] = 0;

			if (strncmp(propstring, "_Diff", 5) == 0)
			{
				fprintf(fd2, "Diffraction Dominant\n");
			}

			if (strncmp(propstring, "_Tropo", 6) == 0)
			{
				fprintf(fd2, "Troposcatter Dominant\n");
			}

			if (strncmp(propstring, "_Peak", 5) == 0)
			{
				fprintf(fd2, "RX at Peak Terrain Along Path\n");
			}

			fprintf(fd2, "ITWOM error number: %d", errnum);
		}

		switch (errnum)
		{
		case 0:
			fprintf(fd2, " (No error)\n");
			break;

		case 1:
			fprintf(fd2, "\n  Warning: Some parameters are nearly out of range.\n");
			fprintf(fd2, "  Results should be used with caution.\n");
			break;

		case 2:
			fprintf(fd2, "\n  Note: Default parameters have been substituted for impossible ones.\n");
			break;

		case 3:
			fprintf(fd2, "\n  Warning: A combination of parameters is out of range.\n");
			fprintf(fd2, "  Results are probably invalid.\n");
			break;

		default:
			fprintf(fd2, "\n  Warning: Some parameters are out of range.\n");
			fprintf(fd2, "  Results are probably invalid.\n");
		}

		fprintf(fd2, "\n%s\n\n", lineOfDashes);
	}

	fprintf(stdout, "\nPath Loss Report written to: \"%s\"\n", reportNameAndPath);
	fflush(stdout);

	PerformObstructionAnalysisBetweenSites(source, destination, itmParameters->referenceFrequency, fd2, digitalElevationModelWrapper, path,
		useMetricUnits, groundClutterHeight, fresnelZoneClearanceRatio, sphereRadius);

	fclose(fd2);

	// Skip plotting the graph if ONLY a path-loss report is needed.

	if (graph_it)
	{
		if (name[0] == '.')
		{
			// Default filename and output file type

			err = strncpy_s(basename, _countof(basename), "profile\0", 8);
			err = strncpy_s(term, _countof(term), "png\0", 4);
			err = strncpy_s(ext, _countof(ext), "png\0", 4);
		}
		else
		{
			// Extract extension and terminal type from "name"

			ext[0] = 0;
			y = (int)strlen(name);
			err = strncpy_s(basename, _countof(basename), name, 254);

			for (x = y - 1; (x > 0) && (name[x] != '.'); x--);

			if (x > 0)  // Extension found
			{
				for (z = x + 1; (z <= y) && ((z - (x + 1)) < 10); z++)
				{
					ext[z - (x + 1)] = tolower(name[z]);
					term[z - (x + 1)] = name[z];
				}

				ext[z - (x + 1)] = 0;  // Ensure an ending 0
				term[z - (x + 1)] = 0;
				basename[x] = 0;
			}
		}

		if (ext[0] == 0)	// No extension -- Default is png
		{
			err = strncpy_s(term, _countof(term), "png\0", 4);
			err = strncpy_s(ext, _countof(ext), "png\0", 4);
		}

		// Either .ps or .postscript may be used
		// as an extension for postscript output.

		if (strncmp(term, "postscript", 10) == 0)
		{
			err = strncpy_s(ext, _countof(ext), "ps\0", 3);
		}
		else if (strncmp(ext, "ps", 2) == 0)
		{
			err = strncpy_s(term, _countof(term), "postscript enhanced color\0", 26);
		}

		err = fopen_s(&fd, plotNameAndPath, "w");

		if (!err && fd != NULL)
		{
			fprintf(fd, "set grid\n");
			fprintf(fd, "set yrange [%2.3f to %2.3f]\n", minloss, maxloss);
			fprintf(fd, "set encoding iso_8859_1\n");
			fprintf(fd, "set term %s\n", term);
			fprintf(fd, "set title \"%s Loss Profile Along Path Between %s and %s (%.2f%c azimuth)\"\n", APP_NAME, destination.name, source.name, AzimuthAngleBetweenSites(destination, source), 176);

			if (useMetricUnits)
			{
				fprintf(fd, "set xlabel \"Distance Between %s and %s (%.2f kilometers)\"\n", destination.name, source.name, KM_PER_MILE * GreatCircleDistanceBetweenSiteLocations(destination, source));
			}
			else
			{
				fprintf(fd, "set xlabel \"Distance Between %s and %s (%.2f miles)\"\n", destination.name, source.name, GreatCircleDistanceBetweenSiteLocations(destination, source));
			}

			if (gotAntennaAzimuthAnglePattern || gotAntennaElevationAnglePattern)
			{
				fprintf(fd, "set ylabel \"Total Path Loss (including TX antenna pattern) (dB)");
			}
			else
			{
				if (useOldLongleyRiceModel)
				{
					fprintf(fd, "set ylabel \"Longley-Rice Path Loss (dB)");
				}
				else
				{
					fprintf(fd, "set ylabel \"ITWOM Version %.1f Path Loss (dB)", ITWOM_VERSION);
				}
			}

			fprintf(fd, "\"\nset output \"%s.%s\"\n", basename, ext);
			fprintf(fd, "plot \"%s\" title \"Path Loss\" with lines\n", profileNameAndPath);

			fclose(fd);
		}

		char gnuPlotAndSplat[335];
		sprintf_s(gnuPlotAndSplat, _countof(gnuPlotAndSplat), "gnuplot \"%s\"", plotNameAndPath);

		// Invokve gnuplot and run the splat.gp script.

		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		// Start the child process.

		bool gnuplotSuccess = CreateProcess(NULL, gnuPlotAndSplat, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

		if (gnuplotSuccess == true)
		{
			// Wait until child process exits.
			WaitForSingleObject(pi.hProcess, INFINITE);

			// Close process and thread handles.
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);

			if (saveGnuPlotWorkingFiles == 0)
			{
				_unlink(plotNameAndPath);
				_unlink(profileNameAndPath);
				_unlink(referenceNameAndPath);
			}

			fprintf(stdout, "Path loss plot written to: \"%s.%s\"\n", basename, ext);
			fflush(stdout);
		}
		else
		{
			fprintf(stderr, "\n*** ERROR: Error occurred invoking gnuplot\n");
		}
	}

	if ((x != -1) && (saveGnuPlotWorkingFiles == 0))
	{
		_unlink(profileNameAndPath);
	}
}


//| ------------------------------
//| 
//| FUNCTION: WriteSplatSiteReport
//| 
//| OLD NAME: SiteReport
//| 
//| NOTES: 
//| 
//| ------------------------------
void
WriteSplatSiteReport(
	Site xmtr,
	DigitalElevationModelWrapper* digitalElevationModelWrapper,
	Path* path,
	unsigned char useMetricUnits,
	double groundClutterHeight,
	char* reportSavePath)
{
	char report_name[80];
	double terrain;
	int	x, azi;
	FILE* fd;
	errno_t err;

	char dmsString[255];

	char lineOfDashes[80];	// OLD NAME: dashes
	err = strncpy_s(lineOfDashes, _countof(lineOfDashes), "---------------------------------------------------------------------------\0", 76);

	sprintf_s(report_name, _countof(report_name), "%s-site_report.txt", xmtr.name);

	for (x = 0; report_name[x] != 0; x++)
	{
		if ((report_name[x] == 32) || (report_name[x] == 17) || (report_name[x] == 92) || (report_name[x] == 42) || (report_name[x] == 47))
		{
			report_name[x] = '_';
		}
	}

	char reportNameAndPath[335];
	sprintf_s(reportNameAndPath, _countof(reportNameAndPath), "%s%s", (reportSavePath[0] ? reportSavePath : ""), report_name);

	err = fopen_s(&fd, reportNameAndPath, "w");

	if (!err && fd != NULL)
	{
		fprintf(fd, "\n\t--==[ %s v%s Site Analysis Report For: %s ]==--\n\n", APP_NAME, VERSION_HEADER, xmtr.name);

		fprintf(fd, "%s\n\n", lineOfDashes);

		if (xmtr.latitude >= 0.0)
		{
			ConvertDecimalToDegreesMinutesSeconds(xmtr.latitude, dmsString);
			fprintf(fd, "Site location: %.4f North / %.4f West", xmtr.latitude, xmtr.longitude);
			fprintf(fd, " (%s N / ", dmsString);
		}
		else
		{
			ConvertDecimalToDegreesMinutesSeconds(xmtr.latitude, dmsString);
			fprintf(fd, "Site location: %.4f South / %.4f West", -xmtr.latitude, xmtr.longitude);
			fprintf(fd, " (%s S / ", dmsString);
		}

		ConvertDecimalToDegreesMinutesSeconds(xmtr.longitude, dmsString);
		fprintf(fd, "%s W)\n", dmsString);

		if (useMetricUnits)
		{
			fprintf(fd, "Ground elevation: %.2f meters AMSL\n", METERS_PER_FOOT * GetSiteLocationElevation(xmtr, digitalElevationModelWrapper));
			fprintf(fd, "Antenna height: %.2f meters AGL / %.2f meters AMSL\n", METERS_PER_FOOT * xmtr.altitude, METERS_PER_FOOT * (xmtr.altitude + GetSiteLocationElevation(xmtr, digitalElevationModelWrapper)));
		}
		else
		{
			fprintf(fd, "Ground elevation: %.2f feet AMSL\n", GetSiteLocationElevation(xmtr, digitalElevationModelWrapper));
			fprintf(fd, "Antenna height: %.2f feet AGL / %.2f feet AMSL\n", xmtr.altitude, xmtr.altitude + GetSiteLocationElevation(xmtr, digitalElevationModelWrapper));
		}

		terrain = AntennaHeightAboveAverageTerrain(xmtr, digitalElevationModelWrapper, path, groundClutterHeight);

		if (terrain > (LOCATION_NOT_IN_MEMORY + 1))
		{
			if (useMetricUnits)
			{
				fprintf(fd, "Antenna height above average terrain: %.2f meters\n\n", METERS_PER_FOOT * terrain);
			}
			else
			{
				fprintf(fd, "Antenna height above average terrain: %.2f feet\n\n", terrain);
			}

			// Display the average terrain between AVERAGE_TERRAIN_MIN_DISTANCE and AVERAGE_TERRAIN_MAX_DISTANCE 
			// from the transmitter site at azimuths of 0, 45, 90,
			// 135, 180, 225, 270, and 315 degrees.

			for (azi = 0; azi <= 315; azi += 45)
			{
				fprintf(fd, "Average terrain at %3d degrees azimuth: ", azi);
				terrain = AverageTerrainOverDistanceAtAzimuthFromSite(xmtr, (double)azi, AVERAGE_TERRAIN_MIN_DISTANCE, AVERAGE_TERRAIN_MAX_DISTANCE, digitalElevationModelWrapper, path, groundClutterHeight);

				if (terrain > (LOCATION_NOT_IN_MEMORY + 1))
				{
					if (useMetricUnits)
					{
						fprintf(fd, "%.2f meters AMSL\n", METERS_PER_FOOT * terrain);
					}
					else
					{
						fprintf(fd, "%.2f feet AMSL\n", terrain);
					}
				}
				else
				{
					fprintf(fd, "No terrain\n");
				}
			}
		}

		fprintf(fd, "\n%s\n\n", lineOfDashes);
		fclose(fd);
	}

	fprintf(stdout, "\nSite analysis report written to: \"%s\"\n", reportNameAndPath);
}
