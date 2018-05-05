/****************************************************************************\
*					SplatLib: Functions used by Splat						 *
*****************************************************************************/

#include "stdafx.h"
#include <math.h>
#include <io.h>
#include <stdlib.h>
#include <ctype.h>
#include "SplatLib.h"
#include "itwom.h"
#include "fontdata.h"
#include "..\Common\Site.h"
#include "..\Common\constants.h"


//| ------------------------------
//| 
//| FUNCTION: LinearInterpolation
//| 
//| OLD NAME: interpolate
//| 
//| NOTES: 
//|   Perform linear interpolation between quantized contour
//|	  levels displayed in field strength and path loss maps.
//|   If signal level x0 corresponds to color level y0, signal
//|   level x1 corresponds to color level y1, and signal level
//|   n falls somewhere between x0 and x1, determine what
//|   color value n corresponds to between y0 and y1.
//| 
//| ------------------------------
int
LinearInterpolation
   (int y0,
	int y1,
	int x0,
	int x1,
	int n)
{
	int result = 0;
	double delta_x, delta_y;

	if (n <= x0)
	{
		return y0;
	}

	if (n >= x1)
	{
		return y1;
	}

	if (y0 == y1)
	{
		return y0;
	}

	if (x0 == x1)
	{
		return y0;
	}

	delta_y = (double)(y0 - y1);
	delta_x = (double)(x0 - x1);

	result = y0 + (int)ceil((delta_y / delta_x)*(n - x0));

	return result;
}


//| ------------------------------
//| 
//| FUNCTION: ArcCosine
//| 
//| OLD NAME: arccos
//| 
//| NOTES: 
//|   This function implements the arc cosine function,
//|   returning a value between 0 and TWO_PI.
//| 
//| ------------------------------
double
ArcCosine
   (double x,
	double y)
{
	double result = 0.0;

	if (y > 0.0)
	{
		result = acos(x / y);
	}

	if (y < 0.0)
	{
		result = PI + acos(x / y);
	}

	return result;
}


//| ------------------------------
//| 
//| FUNCTION: ConvertToNormalizedAngle
//| 
//| OLD NAME: ReduceAngle
//| 
//| NOTES: 
//|   This function normalizes the argument to
//|   an integer angle between 0 and 180 degrees.
//| 
//| ------------------------------
int
ConvertToNormalizedAngle
   (double angle)
{
	double temp;

	temp = acos(cos(angle*DEGREES_TO_RADIANS));

	return (int)rint(temp / DEGREES_TO_RADIANS);
}


//| ------------------------------
//| 
//| FUNCTION: LongitudeDifference
//| 
//| OLD NAME: LonDiff
//| 
//| NOTES: 
//|   This function returns the short path longitudinal
//|   difference between longitude1 and longitude2
//|   as an angle between -180.0 and +180.0 degrees.
//|   If lon1 is west of lon2, the result is positive.
//|   If lon1 is east of lon2, the result is negative.
//| 
//| ------------------------------
double
LongitudeDifference
   (double lon1,
	double lon2)
{
	double diff;

	diff = lon1 - lon2;

	if (diff <= -180.0)
	{
		diff += 360.0;
	}

	if (diff >= 180.0)
	{
		diff -= 360.0;
	}

	return diff;
}


//| ------------------------------
//| 
//| FUNCTION: ConvertDecimalToDegreesMinutesSeconds
//| 
//| OLD NAME: dec2dms
//| 
//| NOTES: 
//|   Converts decimal degrees to degrees, minutes, seconds,
//|   (DMS) and returns the result as a character string.
//| 
//| ------------------------------
void
ConvertDecimalToDegreesMinutesSeconds
   (double decimal,
	char *dmsString)
{
	char sign;
	int	degrees, minutes, seconds;
	double a, b, c, d;

	if (decimal < 0.0)
	{
		decimal = -decimal;
		sign = -1;
	}
	else
	{
		sign = 1;
	}

	a = floor(decimal);
	b = 60.0*(decimal - a);
	c = floor(b);
	d = 60.0*(b - c);

	degrees = (int)a;
	minutes = (int)c;
	seconds = (int)d;

	if (seconds < 0)
	{
		seconds = 0;
	}

	if (seconds > 59)
	{
		seconds = 59;
	}

	dmsString[0] = 0;
	snprintf(dmsString, 250, "%d%c %d\' %d\"", degrees*sign, 176, minutes, seconds);
}


//| ------------------------------
//| 
//| FUNCTION: GreatCircleDistanceBetweenSiteLocations
//| 
//| OLD NAME: Distance
//| 
//| NOTES: 
//|   This function returns the great circle distance
//|   in miles between any two site locations.
//| 
//| ------------------------------
double
GreatCircleDistanceBetweenSiteLocations
   (Site site1,
	Site site2)
{
	double lat1, lon1, lat2, lon2, distance;

	lat1 = site1.lat*DEGREES_TO_RADIANS;
	lon1 = site1.lon*DEGREES_TO_RADIANS;
	lat2 = site2.lat*DEGREES_TO_RADIANS;
	lon2 = site2.lon*DEGREES_TO_RADIANS;

	distance = 3959.0*acos(sin(lat1)*sin(lat2) + cos(lat1)*cos(lat2)*cos((lon1)-(lon2)));

	return distance;
}


//| ------------------------------
//| 
//| FUNCTION: AzimuthAngleBetweenSites
//| 
//| OLD NAME: Azimuth
//| 
//| NOTES: 
//|   This function returns the azimuth (in degrees) to the
//|   destination as seen from the location of the source.
//| 
//| ------------------------------
double
AzimuthAngleBetweenSites
   (Site source,
	Site destination)
{
	double dest_lat, dest_lon, src_lat, src_lon,
		beta, azimuth, diff, num, den, fraction;

	dest_lat = destination.lat*DEGREES_TO_RADIANS;
	dest_lon = destination.lon*DEGREES_TO_RADIANS;

	src_lat = source.lat*DEGREES_TO_RADIANS;
	src_lon = source.lon*DEGREES_TO_RADIANS;

	//| Calculate Surface Distance

	beta = acos(sin(src_lat)*sin(dest_lat) + cos(src_lat)*cos(dest_lat)*cos(src_lon - dest_lon));

	//| Calculate Azimuth

	num = sin(dest_lat) - (sin(src_lat)*cos(beta));
	den = cos(src_lat)*sin(beta);
	fraction = num / den;

	//| Trap potential problems in acos() due to rounding

	if (fraction >= 1.0)
	{
		fraction = 1.0;
	}

	if (fraction <= -1.0)
	{
		fraction = -1.0;
	}

	//| Calculate azimuth

	azimuth = acos(fraction);

	//| Reference it to True North

	diff = dest_lon - src_lon;

	if (diff <= -PI)
	{
		diff += TWO_PI;
	}

	if (diff >= PI)
	{
		diff -= TWO_PI;
	}

	if (diff > 0.0)
	{
		azimuth = TWO_PI - azimuth;
	}

	return (azimuth / DEGREES_TO_RADIANS);
}


//| ------------------------------
//| 
//| FUNCTION: BearingStringToDecimalDegrees
//| 
//| OLD NAME: ReadBearing
//| 
//| NOTES: 
//|   This function takes numeric input in the form of a character
//|   string, and returns an equivalent bearing in degrees as a
//|   decimal number (double). The input may either be expressed
//|   in decimal format (40.139722) or degree, minute, second
//|   format (40 08 23). This function also safely handles
//|   extra spaces found either leading, trailing, or
//|   embedded within the numbers expressed in the
//|   input string. Decimal seconds are permitted.
//| 
//| ------------------------------
double
BearingStringToDecimalDegrees
   (char *input)
{
	double seconds, bearing = 0.0;
	char BearingString[20];
	int	a, b, length, degrees, minutes;

	//| Copy "input" to "string", and ignore any extra
	//| spaces that might be present in the process.

	BearingString[0] = 0;
	length = (int)strlen(input);

	for (a = 0, b = 0; (a < length) && (a < 18); a++)
	{
		if (((input[a] != 32) && (input[a] != '\n')) || ((input[a] == 32) && (input[a + 1] != 32) && (input[a + 1] != '\n') && (b != 0)))
		{
			BearingString[b] = input[a];
			b++;
		}
	}

	BearingString[b] = 0;

	//| Count number of spaces in the clean string.

	length = (int)strlen(BearingString);

	for (a = 0, b = 0; a < length; a++)
	{
		if (BearingString[a] == 32)
		{
			b++;
		}
	}

	if (b == 0)  //| Decimal Format (40.139722)
	{
		sscanf_s(BearingString, "%lf", &bearing);
	}

	if (b == 2)  //| Degree, Minute, Second Format (40 08 23.xx)
	{
		sscanf_s(BearingString, "%d %d %lf", &degrees, &minutes, &seconds);

		bearing = fabs((double)degrees);
		bearing += fabs(((double)minutes) / 60.0);
		bearing += fabs(seconds / 3600.0);

		if ((degrees < 0) || (minutes < 0) || (seconds < 0.0))
		{
			bearing = -bearing;
		}
	}

	//| Anything else returns a 0.0

	if ((bearing > 360.0) || (bearing < -360.0))
	{
		bearing = 0.0;
	}

	return bearing;
}


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
Site
LoadSplatSiteLocationFile
   (char *filename)
{
	int	x;
	char QthString[50], qthfile[255];
	Site tempsite;
	FILE *fd = NULL;
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

	tempsite.lat = 91.0;
	tempsite.lon = 361.0;
	tempsite.alt = 0.0;
	tempsite.name[0] = 0;
	tempsite.filename[0] = 0;

	err = fopen_s(&fd, qthfile, "r");

	if (fd != NULL)
	{
		//| Site Name
		fgets(QthString, 49, fd);

		//| Strip <CR> and/or <LF> from end of site name

		for (x = 0; (QthString[x] != 13) && (QthString[x] != 10) && (QthString[x] != 0); tempsite.name[x] = QthString[x], x++);

		tempsite.name[x] = 0;

		//| Site Latitude
		fgets(QthString, 49, fd);
		tempsite.lat = BearingStringToDecimalDegrees(QthString);

		//| Site Longitude
		fgets(QthString, 49, fd);
		tempsite.lon = BearingStringToDecimalDegrees(QthString);

		if (tempsite.lon < 0.0)
		{
			tempsite.lon += 360.0;
		}

		//| Antenna Height
		fgets(QthString, 49, fd);
		fclose(fd);

		//| Remove <CR> and/or <LF> from antenna height string
		for (x = 0; (QthString[x] != 13) && (QthString[x] != 10) && (QthString[x] != 0); x++);

		QthString[x] = 0;

		//| Antenna height may either be in feet or meters.
		//| If the letter 'M' or 'm' is discovered in
		//| the string, then this is an indication that
		//| the value given is expressed in meters, and
		//| must be converted to feet before exiting.

		for (x = 0; (QthString[x] != 'M') && (QthString[x] != 'm') && (QthString[x] != 0) && (x < 48); x++);

		if ((QthString[x] == 'M') || (QthString[x] == 'm'))
		{
			QthString[x] = 0;
			sscanf_s(QthString, "%f", &tempsite.alt);
			tempsite.alt *= 3.28084f;
		}
		else
		{
			QthString[x] = 0;
			sscanf_s(QthString, "%f", &tempsite.alt);
		}

		for (x = 0; (x < 254) && (qthfile[x] != 0); x++)
		{
			tempsite.filename[x] = qthfile[x];
		}

		tempsite.filename[x] = 0;
	}

	return tempsite;
}


//| ------------------------------
//| 
//| FUNCTION: SetValueInDigitalElevationModelMask
//| 
//| OLD NAME: PutMask
//| 
//| NOTES: 
//|   Lines, text, markings, and coverage areas are stored in a
//|   mask that is combined with topology data when topographic
//|   maps are generated by SPLAT. This function sets and resets
//|   bits in the mask based on the latitude and longitude of the
//|   area pointed to.
//| 
//| ------------------------------
int
SetValueInDigitalElevationModelMask
   (double lat,
	double lon,
	int value,
	DigitalElevationModelWrapper *digitalElevationModelWrapper)
{
	int	x, y, indx;
	char found;

	for (indx = 0, found = 0; (indx < digitalElevationModelWrapper->maxPages) && (found == 0);)
	{
		x = (int)rint(((double)digitalElevationModelWrapper->demPixelsPerDegree)*(lat - digitalElevationModelWrapper->digitalElevationModel[indx].min_north));
		y = (digitalElevationModelWrapper->demPixelsPerDegree - 1) - (int)rint(((double)digitalElevationModelWrapper->demPixelsPerDegree) * LongitudeDifference(digitalElevationModelWrapper->digitalElevationModel[indx].max_west, lon));

		if ((x >= 0) && (x <= (digitalElevationModelWrapper->demPixelsPerDegree - 1)) && (y >= 0) && (y <= (digitalElevationModelWrapper->demPixelsPerDegree - 1)))
		{
			found = 1;
		}
		else
		{
			indx++;
		}
	}

	if (found)
	{
		digitalElevationModelWrapper->digitalElevationModel[indx].mask[x][y] = value;
		return ((int)digitalElevationModelWrapper->digitalElevationModel[indx].mask[x][y]);
	}
	else
	{
		return -1;
	}
}


//| ------------------------------
//| 
//| FUNCTION: SetOrValueInDigitalElevationModelMask
//| 
//| OLD NAME: OrMask
//| 
//| NOTES: 
//|   Lines, text, markings, and coverage areas are stored in a
//|   mask that is combined with topology data when topographic
//|   maps are generated by SPLAT. This function sets bits in
//|   the mask based on the latitude and longitude of the area
//|   pointed to.
//| 
//| ------------------------------
int
SetOrValueInDigitalElevationModelMask
   (double lat,
	double lon,
	int value,
	DigitalElevationModelWrapper *digitalElevationModelWrapper)
{
	int	x, y, indx;
	char found;

	for (indx = 0, found = 0; (indx < digitalElevationModelWrapper->maxPages) && (found == 0);)
	{
		x = (int)rint(((double)digitalElevationModelWrapper->demPixelsPerDegree)*(lat - digitalElevationModelWrapper->digitalElevationModel[indx].min_north));
		y = (digitalElevationModelWrapper->demPixelsPerDegree - 1) - (int)rint(((double)digitalElevationModelWrapper->demPixelsPerDegree) * LongitudeDifference(digitalElevationModelWrapper->digitalElevationModel[indx].max_west, lon));

		if ((x >= 0) && (x <= (digitalElevationModelWrapper->demPixelsPerDegree - 1)) && (y >= 0) && (y <= (digitalElevationModelWrapper->demPixelsPerDegree - 1)))
		{
			found = 1;
		}
		else
		{
			indx++;
		}
	}

	if (found)
	{
		digitalElevationModelWrapper->digitalElevationModel[indx].mask[x][y] |= value;
		return ((int)digitalElevationModelWrapper->digitalElevationModel[indx].mask[x][y]);
	}
	else
	{
		return -1;
	}
}


//| ------------------------------
//| 
//| FUNCTION: GetValueInDigitalElevationModelMask
//| 
//| OLD NAME: GetMask
//| 
//| NOTES: 
//|   This function returns the mask bits based on the latitude
//|   and longitude given.
//| 
//| ------------------------------
int
GetValueInDigitalElevationModelMask
   (double lat,
	double lon,
	DigitalElevationModelWrapper *digitalElevationModelWrapper)
{
	return (SetOrValueInDigitalElevationModelMask(lat, lon, 0, digitalElevationModelWrapper));
}


//| ------------------------------
//| 
//| FUNCTION: SetValueInDigitalElevationModelSignal
//| 
//| OLD NAME: PutSignal
//| 
//| NOTES: 
//|   This function writes a signal level (0-255)
//|   at the specified location for later recall.
//| 
//| ------------------------------
int
SetValueInDigitalElevationModelSignal
   (double lat,
	double lon,
	unsigned char signal,
	DigitalElevationModelWrapper *digitalElevationModelWrapper)
{
	int	x, y, indx;
	char found;

	for (indx = 0, found = 0; (indx < digitalElevationModelWrapper->maxPages) && (found == 0);)
	{
		x = (int)rint(((double)digitalElevationModelWrapper->demPixelsPerDegree)*(lat - digitalElevationModelWrapper->digitalElevationModel[indx].min_north));
		y = (digitalElevationModelWrapper->demPixelsPerDegree - 1) - (int)rint(((double)digitalElevationModelWrapper->demPixelsPerDegree) * LongitudeDifference(digitalElevationModelWrapper->digitalElevationModel[indx].max_west, lon));

		if ((x >= 0) && (x <= (digitalElevationModelWrapper->demPixelsPerDegree - 1)) && (y >= 0) && (y <= (digitalElevationModelWrapper->demPixelsPerDegree - 1)))
		{
			found = 1;
		}
		else
		{
			indx++;
		}
	}

	if (found)
	{
		digitalElevationModelWrapper->digitalElevationModel[indx].signal[x][y] = signal;
		return (digitalElevationModelWrapper->digitalElevationModel[indx].signal[x][y]);
	}
	else
	{
		return 0;
	}
}


//| ------------------------------
//| 
//| FUNCTION: GetValueInDigitalElevationModelSignal
//| 
//| OLD NAME: GetSignal
//| 
//| NOTES: 
//|   This function reads the signal level (0-255) at the
//|   specified location that was previously written by the
//|   complimentary SetValueInDigitalElevationModelSignal() function.
//| 
//| ------------------------------
unsigned char
GetValueInDigitalElevationModelSignal
   (double lat,
	double lon,
	DigitalElevationModelWrapper *digitalElevationModelWrapper)
{
	int	x, y, indx;
	char found;

	for (indx = 0, found = 0; (indx < digitalElevationModelWrapper->maxPages) && (found == 0);)
	{
		x = (int)rint(((double)digitalElevationModelWrapper->demPixelsPerDegree)*(lat - digitalElevationModelWrapper->digitalElevationModel[indx].min_north));
		y = (digitalElevationModelWrapper->demPixelsPerDegree - 1) - (int)rint(((double)digitalElevationModelWrapper->demPixelsPerDegree) * LongitudeDifference(digitalElevationModelWrapper->digitalElevationModel[indx].max_west, lon));

		if ((x >= 0) && (x <= (digitalElevationModelWrapper->demPixelsPerDegree - 1)) && (y >= 0) && (y <= (digitalElevationModelWrapper->demPixelsPerDegree - 1)))
		{
			found = 1;
		}
		else
		{
			indx++;
		}
	}

	if (found)
	{
		return (digitalElevationModelWrapper->digitalElevationModel[indx].signal[x][y]);
	}
	else
	{
		return 0;
	}
}


//| ------------------------------
//| 
//| FUNCTION: GetSiteLocationElevation
//| 
//| OLD NAME: GetElevation
//| 
//| NOTES: 
//|   This function returns the elevation (in feet) of any location
//|   represented by the digital elevation model data in memory.
//|   Function returns -5000.0 for locations not found in memory.
//| 
//| ------------------------------
double
GetSiteLocationElevation
   (Site location,
	DigitalElevationModelWrapper *digitalElevationModelWrapper)
{
	char found;
	int	x, y, indx;
	double elevation;

	for (indx = 0, found = 0; (indx < digitalElevationModelWrapper->maxPages) && (found == 0);)
	{
		x = (int)rint(((double)digitalElevationModelWrapper->demPixelsPerDegree)*(location.lat - digitalElevationModelWrapper->digitalElevationModel[indx].min_north));
		y = (digitalElevationModelWrapper->demPixelsPerDegree - 1) - (int)rint(((double)digitalElevationModelWrapper->demPixelsPerDegree) * LongitudeDifference(digitalElevationModelWrapper->digitalElevationModel[indx].max_west, location.lon));

		if ((x >= 0) && (x <= (digitalElevationModelWrapper->demPixelsPerDegree - 1)) && (y >= 0) && (y <= (digitalElevationModelWrapper->demPixelsPerDegree - 1)))
		{
			found = 1;
		}
		else
		{
			indx++;
		}
	}

	if (found)
	{
		elevation = 3.28084*digitalElevationModelWrapper->digitalElevationModel[indx].data[x][y];
	}
	else
	{
		elevation = -5000.0;
	}

	return elevation;
}


//| ------------------------------
//| 
//| FUNCTION: AddUserDefinedTerrainToDigitalElevationModelData
//| 
//| OLD NAME: AddElevation
//| 
//| NOTES: 
//|   This function adds a user-defined terrain feature
//|   (in meters AGL) to the digital elevation model data
//|   in memory. Does nothing and returns 0 for locations
//|   not found in memory.
//| 
//| ------------------------------
int
AddUserDefinedTerrainToDigitalElevationModelData
   (double lat,
	double lon,
	double height,
	DigitalElevationModelWrapper *digitalElevationModelWrapper)
{
	char found;
	int	x, y, indx;

	for (indx = 0, found = 0; (indx < digitalElevationModelWrapper->maxPages) && (found == 0);)
	{
		x = (int)rint(((double)digitalElevationModelWrapper->demPixelsPerDegree)*(lat - digitalElevationModelWrapper->digitalElevationModel[indx].min_north));
		y = (digitalElevationModelWrapper->demPixelsPerDegree - 1) - (int)rint(((double)digitalElevationModelWrapper->demPixelsPerDegree) * LongitudeDifference(digitalElevationModelWrapper->digitalElevationModel[indx].max_west, lon));

		if ((x >= 0) && (x <= (digitalElevationModelWrapper->demPixelsPerDegree - 1)) && (y >= 0) && (y <= (digitalElevationModelWrapper->demPixelsPerDegree - 1)))
		{
			found = 1;
		}
		else
		{
			indx++;
		}
	}

	if (found)
	{
		digitalElevationModelWrapper->digitalElevationModel[indx].data[x][y] += (short)rint(height);
	}

	return found;
}


//| ------------------------------
//| 
//| FUNCTION: ElevationAngleBetweenSites
//| 
//| OLD NAME: ElevationAngle
//| 
//| NOTES: 
//|   This function returns the angle of elevation (in degrees)
//|   of the destination as seen from the source location.
//|   A positive result represents an angle of elevation (uptilt),
//|   while a negative result represents an angle of depression
//|   (downtilt), as referenced to a normal to the center of
//|   the earth.
//| 
//| ------------------------------
double
ElevationAngleBetweenSites
   (Site source,
	Site destination,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	double sphereRadius)
{
	register double a, b, dx;

	a = GetSiteLocationElevation(destination, digitalElevationModelWrapper) + destination.alt + sphereRadius;
	b = GetSiteLocationElevation(source, digitalElevationModelWrapper) + source.alt + sphereRadius;

	dx = 5280.0 * GreatCircleDistanceBetweenSiteLocations(source, destination);

	//| Apply the Law of Cosines

	return ((180.0*(acos(((b*b) + (dx*dx) - (a*a)) / (2.0*b*dx))) / PI) - 90.0);
}


//| ------------------------------
//| 
//| FUNCTION: GeneratePathBetweenSites
//| 
//| OLD NAME: ReadPath
//| 
//| NOTES: 
//|   This function generates a sequence of latitude and
//|   longitude positions between source and destination
//|   locations along a great circle path, and stores
//|   elevation and distance information for points
//|   along that path in the "path" structure.
//| 
//| ------------------------------
void
GeneratePathBetweenSites
   (Site source,
	Site destination,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	Path *path)
{
	int	c;
	double azimuth, distance, lat1, lon1, beta, den, num,
		lat2, lon2, total_distance, dx, dy, path_length,
		miles_per_sample, samples_per_radian = 68755.0;
	Site tempsite;

	lat1 = source.lat * DEGREES_TO_RADIANS;
	lon1 = source.lon * DEGREES_TO_RADIANS;

	lat2 = destination.lat * DEGREES_TO_RADIANS;
	lon2 = destination.lon * DEGREES_TO_RADIANS;

	if (((double)digitalElevationModelWrapper->demPixelsPerDegree) == 1200.0)
	{
		samples_per_radian = 68755.0;
	}

	if (((double)digitalElevationModelWrapper->demPixelsPerDegree) == 3600.0)
	{
		samples_per_radian = 206265.0;
	}

	azimuth = AzimuthAngleBetweenSites(source, destination) * DEGREES_TO_RADIANS;

	total_distance = GreatCircleDistanceBetweenSiteLocations(source, destination);

	if (total_distance > (30.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree)))		//| > 0.5 pixel distance
	{
		dx = samples_per_radian * acos(cos(lon1 - lon2));
		dy = samples_per_radian * acos(cos(lat1 - lat2));

		path_length = sqrt((dx*dx) + (dy*dy));		//| Total number of samples

		miles_per_sample = total_distance / path_length;	//| Miles per sample
	}
	else
	{
		c = 0;
		dx = 0.0;
		dy = 0.0;
		path_length = 0.0;
		miles_per_sample = 0.0;
		total_distance = 0.0;

		lat1 = lat1 / DEGREES_TO_RADIANS;
		lon1 = lon1 / DEGREES_TO_RADIANS;

		path->lat[c] = lat1;
		path->lon[c] = lon1;
		path->elevation[c] = GetSiteLocationElevation(source, digitalElevationModelWrapper);
		path->distance[c] = 0.0;
	}

	for (distance = 0.0, c = 0; (total_distance != 0.0) && (distance <= total_distance) && (c < path->pathArraySize); c++, distance = miles_per_sample * (double)c)
	{
		beta = distance / 3959.0;
		lat2 = asin(sin(lat1)*cos(beta) + cos(azimuth)*sin(beta)*cos(lat1));
		num = cos(beta) - (sin(lat1)*sin(lat2));
		den = cos(lat1)*cos(lat2);

		if ((azimuth == 0.0) && (beta >(HALF_PI - lat1)))
		{
			lon2 = lon1 + PI;
		}
		else if ((azimuth == HALF_PI) && (beta > (HALF_PI + lat1)))
		{
			lon2 = lon1 + PI;
		}
		else if (fabs(num / den) > 1.0)
		{
			lon2 = lon1;
		}
		else
		{
			if ((PI - azimuth) >= 0.0)
			{
				lon2 = lon1 - ArcCosine(num, den);
			}
			else
			{
				lon2 = lon1 + ArcCosine(num, den);
			}
		}

		while (lon2 < 0.0)
		{
			lon2 += TWO_PI;
		}

		while (lon2 > TWO_PI)
		{
			lon2 -= TWO_PI;
		}

		lat2 = lat2 / DEGREES_TO_RADIANS;
		lon2 = lon2 / DEGREES_TO_RADIANS;

		path->lat[c] = lat2;
		path->lon[c] = lon2;
		tempsite.lat = lat2;
		tempsite.lon = lon2;
		path->elevation[c] = GetSiteLocationElevation(tempsite, digitalElevationModelWrapper);
		path->distance[c] = distance;
	}

	//| Make sure exact destination point is recorded at path->length - 1

	if (c < path->pathArraySize)
	{
		path->lat[c] = destination.lat;
		path->lon[c] = destination.lon;
		path->elevation[c] = GetSiteLocationElevation(destination, digitalElevationModelWrapper);
		path->distance[c] = total_distance;
		c++;
	}

	if (c < path->pathArraySize)
	{
		path->length = c;
	}
	else
	{
		path->length = path->pathArraySize - 1;
	}
}


//| ------------------------------
//| 
//| FUNCTION: ObstructedElevationAngleBetweenSites
//| 
//| OLD NAME: ElevationAngle2
//| 
//| NOTES: 
//|   This function returns the angle of elevation (in degrees)
//|   of the destination as seen from the source location, UNLESS
//|   the path between the sites is obstructed, in which case, the
//|   elevation angle to the first obstruction is returned instead.
//|   "er" represents the earth radius.
//| 
//| ------------------------------
double
ObstructedElevationAngleBetweenSites
   (Site source,
	Site destination,
	double er,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	Path *path,
	double groundClutterHeight,
	double sphereRadius)
{
	int	x;
	char block = 0;
	double source_alt, destination_alt, cos_xmtr_angle,
		cos_test_angle, test_alt, elevation, distance,
		source_alt2, first_obstruction_angle = 0.0;

	Path *tempPath = new Path(path->pathArraySize);

	GeneratePathBetweenSites(source, destination, digitalElevationModelWrapper, tempPath);

	distance = 5280.0 * GreatCircleDistanceBetweenSiteLocations(source, destination);
	source_alt = er + source.alt + GetSiteLocationElevation(source, digitalElevationModelWrapper);
	destination_alt = er + destination.alt + GetSiteLocationElevation(destination, digitalElevationModelWrapper);
	source_alt2 = source_alt * source_alt;

	//| Calculate the cosine of the elevation angle of the
	//| destination (receiver) as seen by the source (transmitter).

	cos_xmtr_angle = ((source_alt2)+(distance*distance) - (destination_alt*destination_alt)) / (2.0*source_alt*distance);

	//| Test all points in between source and destination locations to
	//| see if the angle to a topographic feature generates a higher
	//| elevation angle than that produced by the destination. Begin
	//| at the source since we're interested in identifying the FIRST
	//| obstruction along the path between source and destination.

	for (x = 2, block = 0; (x < tempPath->length) && (block == 0); x++)
	{
		distance = 5280.0*tempPath->distance[x];

		test_alt = sphereRadius + (tempPath->elevation[x] == 0.0 ? tempPath->elevation[x] : tempPath->elevation[x] + groundClutterHeight);

		cos_test_angle = ((source_alt2)+(distance*distance) - (test_alt*test_alt)) / (2.0*source_alt*distance);

		//| Compare these two angles to determine if
		//| an obstruction exists. Since we're comparing
		//| the cosines of these angles rather than
		//| the angles themselves, the sense of the
		//| following "if" statement is reversed from
		//| what it would be if the angles themselves
		//| were compared.

		if (cos_xmtr_angle >= cos_test_angle)
		{
			block = 1;
			first_obstruction_angle = ((acos(cos_test_angle)) / DEGREES_TO_RADIANS) - 90.0;
		}
	}

	if (block)
	{
		elevation = first_obstruction_angle;
	}
	else
	{
		elevation = ((acos(cos_xmtr_angle)) / DEGREES_TO_RADIANS) - 90.0;
	}

	return elevation;
}


//| ------------------------------
//| 
//| FUNCTION: AverageTerrainOverDistanceAtAzimuthFromSite
//| 
//| OLD NAME: AverageTerrain
//| 
//| NOTES: 
//|   This function returns the average terrain calculated in
//|   the direction of "azimuth" (degrees) between "start_distance"
//|   and "end_distance" (miles) from the source location. If
//|   the terrain is all water (non-critical error), -5000.0 is
//|   returned. If not enough SDF data has been loaded into
//|   memory to complete the survey (critical error), then
//|   -9999.0 is returned.
//| 
//| ------------------------------
double
AverageTerrainOverDistanceAtAzimuthFromSite
   (Site source,
	double azimuthx,
	double start_distance,
	double end_distance,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	Path *path,
	double groundClutterHeight)
{
	int	c, samples, endpoint;
	double beta, lat1, lon1, lat2, lon2, num, den, azimuth, terrain = 0.0;
	Site destination;

	lat1 = source.lat * DEGREES_TO_RADIANS;
	lon1 = source.lon * DEGREES_TO_RADIANS;

	//| Generate a path of elevations between the source
	//| location and the remote location provided.

	beta = end_distance / 3959.0;

	azimuth = DEGREES_TO_RADIANS * azimuthx;

	lat2 = asin(sin(lat1)*cos(beta) + cos(azimuth)*sin(beta)*cos(lat1));
	num = cos(beta) - (sin(lat1)*sin(lat2));
	den = cos(lat1)*cos(lat2);

	if ((azimuth == 0.0) && (beta > (HALF_PI - lat1)))
	{
		lon2 = lon1 + PI;
	}
	else if ((azimuth == HALF_PI) && (beta > (HALF_PI + lat1)))
	{
		lon2 = lon1 + PI;
	}
	else if (fabs(num / den) > 1.0)
	{
		lon2 = lon1;
	}
	else
	{
		if ((PI - azimuth) >= 0.0)
		{
			lon2 = lon1 - ArcCosine(num, den);
		}
		else
		{
			lon2 = lon1 + ArcCosine(num, den);
		}
	}

	while (lon2 < 0.0)
	{
		lon2 += TWO_PI;
	}

	while (lon2 > TWO_PI)
	{
		lon2 -= TWO_PI;
	}

	lat2 = lat2 / DEGREES_TO_RADIANS;
	lon2 = lon2 / DEGREES_TO_RADIANS;

	destination.lat = lat2;
	destination.lon = lon2;

	//| If SDF data is missing for the endpoint of
	//| the radial, then the average terrain cannot
	//| be accurately calculated. Return -9999.0

	if (GetSiteLocationElevation(destination, digitalElevationModelWrapper) < -4999.0)
	{
		return (-9999.0);
	}
	else
	{
		GeneratePathBetweenSites(source, destination, digitalElevationModelWrapper, path);

		endpoint = path->length;

		//| Shrink the length of the radial if the
		//| outermost portion is not over U.S. land.

		for (c = endpoint - 1; (c >= 0) && (path->elevation[c] == 0.0); c--);

		endpoint = c + 1;

		for (c = 0, samples = 0; c < endpoint; c++)
		{
			if (path->distance[c] >= start_distance)
			{
				terrain += (path->elevation[c] == 0.0 ? path->elevation[c] : path->elevation[c] + groundClutterHeight);
				samples++;
			}
		}

		if (samples == 0)
		{
			terrain = -5000.0;  //| No land
		}
		else
		{
			terrain = (terrain / (double)samples);
		}

		return terrain;
	}
}


//| ------------------------------
//| 
//| FUNCTION: AntennaHeightAboveAverageTerrain
//| 
//| OLD NAME: haat
//| 
//| NOTES: 
//|   This function returns the antenna's Height Above Average
//|   Terrain (HAAT) based on FCC Part 73.313(d). If a critical
//|   error occurs, such as a lack of SDF data to complete the
//|   survey, -5000.0 is returned.
//| 
//| ------------------------------
double
AntennaHeightAboveAverageTerrain
   (Site antenna,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	Path *path,
	double groundClutterHeight)
{
	int	azi, c;
	char error = 0;
	double terrain, avg_terrain, haat, sum = 0.0;

	//| Calculate the average terrain between 2 and 10 miles
	//| from the antenna site at azimuths of 0, 45, 90, 135,
	//| 180, 225, 270, and 315 degrees.

	for (c = 0, azi = 0; (azi <= 315) && (error == 0); azi += 45)
	{
		terrain = AverageTerrainOverDistanceAtAzimuthFromSite(antenna, (double)azi, 2.0, 10.0, digitalElevationModelWrapper, path, groundClutterHeight);

		if (terrain < -9998.0)  //| SDF data is missing
		{
			error = 1;
		}

		if (terrain > -4999.0)  //| It's land, not water
		{
			sum += terrain;  //| Sum of averages
			c++;
		}
	}

	if (error)
	{
		return -5000.0;
	}
	else
	{
		avg_terrain = (sum / (double)c);
		haat = (antenna.alt + GetSiteLocationElevation(antenna, digitalElevationModelWrapper)) - avg_terrain;
		return haat;
	}
}


//| ------------------------------
//| 
//| FUNCTION: PlaceTextAndMarkerDataInMaskArrayForMaps
//| 
//| OLD NAME: PlaceMarker
//| 
//| NOTES: 
//|   This function places text and marker data in the mask array
//|   for illustration on topographic maps generated by SPLAT.
//|   By default, SPLAT centers text information BELOW the marker,
//|   but may move it above, to the left, or to the right of the
//|   marker depending on how much room is available on the map,
//|   or depending on whether the area is already occupied by
//|   another marker or label. If no room or clear space is
//|   available on the map to place the marker and its associated
//|   text, then the marker and text are not written to the map.
//| 
//| ------------------------------
void
PlaceTextAndMarkerDataInMaskArrayForMaps
   (Site location,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	int minimumLatitudeNorth,
	int maximumLatitudeNorth,
	int minimumLongitudeWest,
	int maximumLongitudeWest)
{
	int	a, b, c, byte;
	char ok2print, occupied;
	double x, y, lat, lon, textx = 0.0, texty = 0.0, xmin, xmax,
		ymin, ymax, p1, p3, p6, p8, p12, p16, p24, label_length;

	xmin = (double)minimumLatitudeNorth;
	xmax = (double)maximumLatitudeNorth;
	ymin = (double)minimumLongitudeWest;
	ymax = (double)maximumLongitudeWest;
	lat = location.lat;
	lon = location.lon;

	if ((lat < xmax) && (lat >= xmin) && (LongitudeDifference(lon, ymax) <= 0.0) && (LongitudeDifference(lon, ymin) >= (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree))))
	{
		p1 = 1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree);
		p3 = 3.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree);
		p6 = 6.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree);
		p8 = 8.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree);
		p12 = 12.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree);
		p16 = 16.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree);
		p24 = 24.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree);

		ok2print = 0;
		occupied = 0;

		//| Is Marker Position Clear Of Text Or Other Markers?

		for (a = 0, x = lat - p3; (x <= xmax) && (x >= xmin) && (a < 7); x += p1, a++)
		{
			for (b = 0, y = lon - p3; (LongitudeDifference(y, ymax) <= 0.0) && (LongitudeDifference(y, ymin) >= (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree))) && (b < 7); y += p1, b++)
			{
				occupied |= (GetValueInDigitalElevationModelMask(x, y, digitalElevationModelWrapper) & 2);
			}
		}

		if (occupied == 0)
		{
			//| Determine Where Text Can Be Positioned

			//| label_length=length in pixels.
			//| Each character is 8 pixels wide.

			label_length = p1 * (double)(strlen(location.name) << 3);

			if ((LongitudeDifference(lon + label_length, ymax) <= 0.0) && (LongitudeDifference(lon - label_length, ymin) >= (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree))))
			{
				//| Default: Centered Text

				texty = lon + label_length / 2.0;

				if ((lat - p8) >= p16)
				{
					//| Position Text Below The Marker

					textx = lat - p8;

					x = textx;
					y = texty;

					//| Is This Position Clear Of
					//| Text Or Other Markers?

					for (a = 0, occupied = 0; a < 16; a++)
					{
						for (b = 0; b < (int)strlen(location.name); b++)
						{
							for (c = 0; c < 8; c++, y -= p1)
							{
								occupied |= (GetValueInDigitalElevationModelMask(x, y, digitalElevationModelWrapper) & 2);
							}
						}

						x -= p1;
						y = texty;
					}

					x = textx;
					y = texty;

					if (occupied == 0)
					{
						ok2print = 1;
					}
				}
				else
				{
					//| Position Text Above The Marker

					textx = lat + p24;

					x = textx;
					y = texty;

					//| Is This Position Clear Of
					//| Text Or Other Markers?

					for (a = 0, occupied = 0; a < 16; a++)
					{
						for (b = 0; b < (int)strlen(location.name); b++)
						{
							for (c = 0; c < 8; c++, y -= p1)
							{
								occupied |= (GetValueInDigitalElevationModelMask(x, y, digitalElevationModelWrapper) & 2);
							}
						}

						x -= p1;
						y = texty;
					}

					x = textx;
					y = texty;

					if (occupied == 0)
					{
						ok2print = 1;
					}
				}
			}

			if (ok2print == 0)
			{
				if (LongitudeDifference(lon - label_length, ymin) >= (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree)))
				{
					//| Position Text To The
					//| Right Of The Marker

					textx = lat + p6;
					texty = lon - p12;

					x = textx;
					y = texty;

					//| Is This Position Clear Of
					//| Text Or Other Markers?

					for (a = 0, occupied = 0; a < 16; a++)
					{
						for (b = 0; b < (int)strlen(location.name); b++)
						{
							for (c = 0; c < 8; c++, y -= p1)
							{
								occupied |= (GetValueInDigitalElevationModelMask(x, y, digitalElevationModelWrapper) & 2);
							}
						}

						x -= p1;
						y = texty;
					}

					x = textx;
					y = texty;

					if (occupied == 0)
					{
						ok2print = 1;
					}
				}
				else
				{
					//| Position Text To The
					//| Left Of The Marker

					textx = lat + p6;
					texty = lon + p8 + (label_length);

					x = textx;
					y = texty;

					//| Is This Position Clear Of
					//| Text Or Other Markers?

					for (a = 0, occupied = 0; a < 16; a++)
					{
						for (b = 0; b < (int)strlen(location.name); b++)
						{
							for (c = 0; c < 8; c++, y -= p1)
							{
								occupied |= (GetValueInDigitalElevationModelMask(x, y, digitalElevationModelWrapper) & 2);
							}
						}

						x -= p1;
						y = texty;
					}

					x = textx;
					y = texty;

					if (occupied == 0)
					{
						ok2print = 1;
					}
				}
			}

			//| textx and texty contain the latitude and longitude
			//| coordinates that describe the placement of the text
			//| on the map.

			if (ok2print)
			{
				//| Draw Text

				x = textx;
				y = texty;

				for (a = 0; a < 16; a++)
				{
					for (b = 0; b<(int)strlen(location.name); b++)
					{
						byte = fontdata[16 * (location.name[b]) + a];

						for (c = 128; c > 0; c = c >> 1, y -= p1)
						{
							if (byte & c)
							{
								SetOrValueInDigitalElevationModelMask(x, y, 2, digitalElevationModelWrapper);
							}
						}
					}

					x -= p1;
					y = texty;
				}

				//| Draw Square Marker Centered
				//| On Location Specified

				for (a = 0, x = lat - p3; (x <= xmax) && (x >= xmin) && (a < 7); x += p1, a++)
				{
					for (b = 0, y = lon - p3; (LongitudeDifference(y, ymax) <= 0.0) && (LongitudeDifference(y, ymin) >= (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree))) && (b < 7); y += p1, b++)
					{
						SetOrValueInDigitalElevationModelMask(x, y, 2, digitalElevationModelWrapper);
					}
				}
			}
		}
	}
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
LoadAntennaAzimuthElevationPatternFiles
   (char *filename,
	IrregularTerrainModelParameters *itmParameters,
	unsigned char *gotAntennaElevationAnglePattern,
	unsigned char *gotAntennaAzimuthAnglePattern)
{
	int	a, b, w, x, y, z, last_index, next_index, span;
	char PatString[255], azfile[255], elfile[255], *pointer = NULL;
	float az, xx, elevation, amplitude, rotation, valid1, valid2,
		delta, tilt, mechanical_tilt = 0.0, tilt_azimuth, tilt_increment, sum;

	//| Issue with stack overflow, allocate big arrays on heap instead.
	float *azimuth = new float[361];
	float *azimuth_pattern = new float[361];
	float *el_pattern = new float[10001];
	float *slant_angle = new float[361];
	float **elevation_pattern = new float*[361];
	for (int i = 0; i < 361; i++)
	{
		elevation_pattern[i] = new float[1001];
	}

	FILE *fd = NULL;
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

	//| Load .az antenna pattern file

	err = fopen_s(&fd, azfile, "r");

	if (fd != NULL)
	{
		//| Clear azimuth pattern array

		for (x = 0; x <= 360; x++)
		{
			azimuth[x] = 0.0;
			read_count[x] = 0;
		}

		//| Read azimuth pattern rotation
		//| in degrees measured clockwise
		//| from true North.

		fgets(PatString, 254, fd);
		pointer = strchr(PatString, ';');

		if (pointer != NULL)
		{
			*pointer = 0;
		}

		sscanf_s(PatString, "%f", &rotation);

		//| Read azimuth (degrees) and corresponding
		//| normalized field radiation pattern amplitude
		//| (0.0 to 1.0) until EOF is reached.

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

		//| Handle 0=360 degree ambiguity

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

		//| Average pattern values in case more than
		//| one was read for each degree of azimuth.

		for (x = 0; x <= 360; x++)
		{
			if (read_count[x] > 1)
			{
				azimuth[x] /= (float)read_count[x];
			}
		}

		//| Interpolate missing azimuths
		//| to completely fill the array

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

		//| Perform azimuth pattern rotation
		//| and load azimuth_pattern[361] with
		//| azimuth pattern data in its final form.

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

	//| Read and process .el file

	err = fopen_s(&fd, elfile, "r");

	if (fd != NULL)
	{
		for (x = 0; x <= 10000; x++)
		{
			el_pattern[x] = 0.0;
			read_count[x] = 0;
		}

		//| Read mechanical tilt (degrees) and
		//| tilt azimuth in degrees measured
		//| clockwise from true North.

		fgets(PatString, 254, fd);
		pointer = strchr(PatString, ';');

		if (pointer != NULL)
		{
			*pointer = 0;
		}

		sscanf_s(PatString, "%f %f", &mechanical_tilt, &tilt_azimuth);

		//| Read elevation (degrees) and corresponding
		//| normalized field radiation pattern amplitude
		//| (0.0 to 1.0) until EOF is reached.

		fgets(PatString, 254, fd);
		pointer = strchr(PatString, ';');

		if (pointer != NULL)
		{
			*pointer = 0;
		}

		sscanf_s(PatString, "%f %f", &elevation, &amplitude);

		while (feof(fd) == 0)
		{
			//| Read in normalized radiated field values
			//| for every 0.01 degrees of elevation between
			//| -10.0 and +90.0 degrees

			x = (int)rintf(100.0f*(elevation + 10.0f));

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

		//| Average the field values in case more than
		//| one was read for each 0.01 degrees of elevation.

		for (x = 0; x <= 10000; x++)
		{
			if (read_count[x] > 1)
			{
				el_pattern[x] /= (float)read_count[x];
			}
		}

		//| Interpolate between missing elevations (if
		//| any) to completely fill the array and provide
		//| radiated field values for every 0.01 degrees of
		//| elevation.

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

		//| Fill slant_angle[] array with offset angles based
		//| on the antenna's mechanical beam tilt (if any)
		//| and tilt direction (azimuth).

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
					slant_angle[y] = -(tilt_increment*(90.0f - xx));
				}

				if (x > 180)
				{
					slant_angle[y] = -(tilt_increment*(xx - 270.0f));
				}
			}
		}

		slant_angle[360] = slant_angle[0];   //| 360 degree wrap-around

		for (w = 0; w <= 360; w++)
		{
			tilt = slant_angle[w];

			//| Convert tilt angle to
			//| an array index offset

			y = (int)rintf(100.0f*tilt);

			//| Copy shifted el_pattern[10001] field
			//| values into elevation_pattern[361][1001]
			//| at the corresponding azimuth, downsampling
			//| (averaging) along the way in chunks of 10.

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

			itmParameters->antenna_pattern[x][y] = az * elevation;
		}
	}

	delete[] azimuth;
	delete[] azimuth_pattern;
	delete[] el_pattern;
	delete[] slant_angle;
	for (int i = 0; i < 361; i++)
	{
		delete[] elevation_pattern[i];
	}
	delete[] elevation_pattern;
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
LoadUncompressedSplatDataFile
   (char *name,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	int *maximumElevation,
	int *minimumElevation,
	int *minimumLatitudeNorth,
	int *maximumLatitudeNorth,
	int *minimumLongitudeWest,
	int *maximumLongitudeWest,
	char *splatDataFilePath)
{
	int	x, y, data, indx, minlat, minlon, maxlat, maxlon;
	char found, free_page = 0, line[20], sdf_file[255],
		path_plus_name[512];
	FILE *fd;
	errno_t err;

	for (x = 0; (name[x] != '.') && (name[x] != 0) && (x < 250); x++)
	{
		sdf_file[x] = name[x];
	}

	sdf_file[x] = 0;

	//| Parse filename for minimum latitude and longitude values

	sscanf_s(sdf_file, "%dx%dx%dx%d", &minlat, &maxlat, &minlon, &maxlon);

	sdf_file[x] = '.';
	sdf_file[x + 1] = 's';
	sdf_file[x + 2] = 'd';
	sdf_file[x + 3] = 'f';
	sdf_file[x + 4] = 0;

	//| Is it already in memory?

	for (indx = 0, found = 0; (indx < digitalElevationModelWrapper->maxPages) && (found == 0); indx++)
	{
		if ((minlat == digitalElevationModelWrapper->digitalElevationModel[indx].min_north) && 
			(minlon == digitalElevationModelWrapper->digitalElevationModel[indx].min_west) && 
			(maxlat == digitalElevationModelWrapper->digitalElevationModel[indx].max_north) &&
			(maxlon == digitalElevationModelWrapper->digitalElevationModel[indx].max_west))
		{
			found = 1;
		}
	}

	//| Is room available to load it?

	if (found == 0)
	{
		for (indx = 0, free_page = 0; (indx < digitalElevationModelWrapper->maxPages) && (free_page == 0); indx++)
		{
			if (digitalElevationModelWrapper->digitalElevationModel[indx].max_north == -90)
			{
				free_page = 1;
			}
		}
	}

	indx--;

	if (free_page && (found == 0) && (indx >= 0) && (indx < digitalElevationModelWrapper->maxPages))
	{
		//| Search for SDF file in current working directory first

		err = strncpy_s(path_plus_name, _countof(path_plus_name), sdf_file, 255);

		err = fopen_s(&fd, path_plus_name, "rb");

		if (fd == NULL)
		{
			//| Next, try loading SDF file from path specified
			//| in $HOME/.splat_path file or by -d argument

			err = strncpy_s(path_plus_name, _countof(path_plus_name), splatDataFilePath, 255);
			strncat_s(path_plus_name, sdf_file, 254);

			err = fopen_s(&fd, path_plus_name, "rb");
		}

		if (fd != NULL)
		{
			fprintf(stdout, "Loading \"%s\" into page %d...", path_plus_name, indx + 1);
			fflush(stdout);

			fgets(line, 19, fd);
			sscanf_s(line, "%d", &digitalElevationModelWrapper->digitalElevationModel[indx].max_west);

			fgets(line, 19, fd);
			sscanf_s(line, "%d", &digitalElevationModelWrapper->digitalElevationModel[indx].min_north);

			fgets(line, 19, fd);
			sscanf_s(line, "%d", &digitalElevationModelWrapper->digitalElevationModel[indx].min_west);

			fgets(line, 19, fd);
			sscanf_s(line, "%d", &digitalElevationModelWrapper->digitalElevationModel[indx].max_north);

			for (x = 0; x < digitalElevationModelWrapper->demPixelsPerDegree; x++)
			{
				for (y = 0; y < digitalElevationModelWrapper->demPixelsPerDegree; y++)
				{
					fgets(line, 19, fd);
					data = atoi(line);

					digitalElevationModelWrapper->digitalElevationModel[indx].data[x][y] = data;
					digitalElevationModelWrapper->digitalElevationModel[indx].signal[x][y] = 0;
					digitalElevationModelWrapper->digitalElevationModel[indx].mask[x][y] = 0;

					if (data > digitalElevationModelWrapper->digitalElevationModel[indx].max_el)
					{
						digitalElevationModelWrapper->digitalElevationModel[indx].max_el = data;
					}

					if (data < digitalElevationModelWrapper->digitalElevationModel[indx].min_el)
					{
						digitalElevationModelWrapper->digitalElevationModel[indx].min_el = data;
					}
				}
			}

			fclose(fd);

			if (digitalElevationModelWrapper->digitalElevationModel[indx].min_el < *minimumElevation)
			{
				*minimumElevation = digitalElevationModelWrapper->digitalElevationModel[indx].min_el;
			}

			if (digitalElevationModelWrapper->digitalElevationModel[indx].max_el > *maximumElevation)
			{
				*maximumElevation = digitalElevationModelWrapper->digitalElevationModel[indx].max_el;
			}

			if (*maximumLatitudeNorth == -90)
			{
				*maximumLatitudeNorth = digitalElevationModelWrapper->digitalElevationModel[indx].max_north;
			}
			else if (digitalElevationModelWrapper->digitalElevationModel[indx].max_north > *maximumLatitudeNorth)
			{
				*maximumLatitudeNorth = digitalElevationModelWrapper->digitalElevationModel[indx].max_north;
			}

			if (*minimumLatitudeNorth == 90)
			{
				*minimumLatitudeNorth = digitalElevationModelWrapper->digitalElevationModel[indx].min_north;
			}
			else if (digitalElevationModelWrapper->digitalElevationModel[indx].min_north < *minimumLatitudeNorth)
			{
				*minimumLatitudeNorth = digitalElevationModelWrapper->digitalElevationModel[indx].min_north;
			}

			if (*maximumLongitudeWest == -1)
			{
				*maximumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].max_west;
			}
			else
			{
				if (abs(digitalElevationModelWrapper->digitalElevationModel[indx].max_west - *maximumLongitudeWest) < 180)
				{
					if (digitalElevationModelWrapper->digitalElevationModel[indx].max_west > *maximumLongitudeWest)
					{
						*maximumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].max_west;
					}
				}
				else
				{
					if (digitalElevationModelWrapper->digitalElevationModel[indx].max_west < *maximumLongitudeWest)
					{
						*maximumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].max_west;
					}
				}
			}

			if (*minimumLongitudeWest == 360)
			{
				*minimumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].min_west;
			}
			else
			{
				if (fabs(digitalElevationModelWrapper->digitalElevationModel[indx].min_west - *minimumLongitudeWest) < 180.0)
				{
					if (digitalElevationModelWrapper->digitalElevationModel[indx].min_west < *minimumLongitudeWest)
					{
						*minimumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].min_west;
					}
				}
				else
				{
					if (digitalElevationModelWrapper->digitalElevationModel[indx].min_west > *minimumLongitudeWest)
					{
						*minimumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].min_west;
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
LoadSplatDataFile
   (char *name,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	int *maximumElevation,
	int *minimumElevation,
	int *minimumLatitudeNorth,
	int *maximumLatitudeNorth,
	int *minimumLongitudeWest,
	int *maximumLongitudeWest,
	char *splatDataFilePath)
{
	int	x, y, indx, minlat, minlon, maxlat, maxlon;
	char found, free_page = 0;
	int	return_value = -1;

	//| Try to load an uncompressed SDF first.

	return_value = LoadUncompressedSplatDataFile(name, digitalElevationModelWrapper, maximumElevation, minimumElevation,
		minimumLatitudeNorth, maximumLatitudeNorth, minimumLongitudeWest, maximumLongitudeWest, splatDataFilePath);

	//| If neither format can be found, then assume the area is water.

	if ((return_value == 0) || (return_value == -1))
	{
		//| Parse SDF name for minimum latitude and longitude values

		sscanf_s(name, "%dx%dx%dx%d", &minlat, &maxlat, &minlon, &maxlon);

		//| Is it already in memory?

		for (indx = 0, found = 0; (indx < digitalElevationModelWrapper->maxPages) && (found == 0); indx++)
		{
			if ((minlat == digitalElevationModelWrapper->digitalElevationModel[indx].min_north) && 
				(minlon == digitalElevationModelWrapper->digitalElevationModel[indx].min_west) && 
				(maxlat == digitalElevationModelWrapper->digitalElevationModel[indx].max_north) &&
				(maxlon == digitalElevationModelWrapper->digitalElevationModel[indx].max_west))
			{
				found = 1;
			}
		}

		//| Is room available to load it?

		if (found == 0)
		{
			for (indx = 0, free_page = 0; (indx < digitalElevationModelWrapper->maxPages) && (free_page == 0); indx++)
			{
				if (digitalElevationModelWrapper->digitalElevationModel[indx].max_north == -90)
				{
					free_page = 1;
				}
			}
		}

		indx--;

		if (free_page && (found == 0) && (indx >= 0) && (indx < digitalElevationModelWrapper->maxPages))
		{
			fprintf(stdout, "Region  \"%s\" assumed as sea-level into page %d...", name, indx + 1);
			fflush(stdout);

			digitalElevationModelWrapper->digitalElevationModel[indx].max_west = maxlon;
			digitalElevationModelWrapper->digitalElevationModel[indx].min_north = minlat;
			digitalElevationModelWrapper->digitalElevationModel[indx].min_west = minlon;
			digitalElevationModelWrapper->digitalElevationModel[indx].max_north = maxlat;

			//| Fill DEM with sea-level topography

			for (x = 0; x < digitalElevationModelWrapper->demPixelsPerDegree; x++)
			{
				for (y = 0; y < digitalElevationModelWrapper->demPixelsPerDegree; y++)
				{
					digitalElevationModelWrapper->digitalElevationModel[indx].data[x][y] = 0;
					digitalElevationModelWrapper->digitalElevationModel[indx].signal[x][y] = 0;
					digitalElevationModelWrapper->digitalElevationModel[indx].mask[x][y] = 0;

					if (digitalElevationModelWrapper->digitalElevationModel[indx].min_el > 0)
					{
						digitalElevationModelWrapper->digitalElevationModel[indx].min_el = 0;
					}
				}
			}

			if (digitalElevationModelWrapper->digitalElevationModel[indx].min_el < *minimumElevation)
			{
				*minimumElevation = digitalElevationModelWrapper->digitalElevationModel[indx].min_el;
			}

			if (digitalElevationModelWrapper->digitalElevationModel[indx].max_el > *maximumElevation)
			{
				*maximumElevation = digitalElevationModelWrapper->digitalElevationModel[indx].max_el;
			}

			if (*maximumLatitudeNorth == -90)
			{
				*maximumLatitudeNorth = digitalElevationModelWrapper->digitalElevationModel[indx].max_north;
			}
			else if (digitalElevationModelWrapper->digitalElevationModel[indx].max_north > *maximumLatitudeNorth)
			{
				*maximumLatitudeNorth = digitalElevationModelWrapper->digitalElevationModel[indx].max_north;
			}

			if (*minimumLatitudeNorth == 90)
			{
				*minimumLatitudeNorth = digitalElevationModelWrapper->digitalElevationModel[indx].min_north;
			}
			else if (digitalElevationModelWrapper->digitalElevationModel[indx].min_north < *minimumLatitudeNorth)
			{
				*minimumLatitudeNorth = digitalElevationModelWrapper->digitalElevationModel[indx].min_north;
			}

			if (*maximumLongitudeWest == -1)
			{
				*maximumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].max_west;
			}
			else
			{
				if (abs(digitalElevationModelWrapper->digitalElevationModel[indx].max_west - *maximumLongitudeWest) < 180)
				{
					if (digitalElevationModelWrapper->digitalElevationModel[indx].max_west > *maximumLongitudeWest)
					{
						*maximumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].max_west;
					}
				}
				else
				{
					if (digitalElevationModelWrapper->digitalElevationModel[indx].max_west < *maximumLongitudeWest)
					{
						*maximumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].max_west;
					}
				}
			}

			if (*minimumLongitudeWest == 360)
			{
				*minimumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].min_west;
			}
			else
			{
				if (abs(digitalElevationModelWrapper->digitalElevationModel[indx].min_west - *minimumLongitudeWest) < 180)
				{
					if (digitalElevationModelWrapper->digitalElevationModel[indx].min_west < *minimumLongitudeWest)
					{
						*minimumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].min_west;
					}
				}
				else
				{
					if (digitalElevationModelWrapper->digitalElevationModel[indx].min_west > *minimumLongitudeWest)
					{
						*minimumLongitudeWest = digitalElevationModelWrapper->digitalElevationModel[indx].min_west;
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
//| FUNCTION: LoadAndPlotCitiesAndSitesOnMaps
//| 
//| OLD NAME: LoadCities
//| 
//| NOTES: 
//|   This function reads SPLAT city/site files, and plots
//|   the locations and names of the cities and site locations
//|   read on topographic maps generated by SPLAT.
//| 
//| ------------------------------
void
LoadAndPlotCitiesAndSitesOnMaps
   (char *filename,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	int minimumLatitudeNorth,
	int maximumLatitudeNorth,
	int minimumLongitudeWest,
	int maximumLongitudeWest)
{
	int	x, y, z;
	char input[80], str[3][80];
	Site city_site;
	FILE *fd = NULL;
	errno_t err;

	err = fopen_s(&fd, filename, "r");

	if (fd != NULL)
	{
		fgets(input, 78, fd);

		fprintf(stdout, "\nReading \"%s\"... ", filename);
		fflush(stdout);

		while ((fd != NULL) && (feof(fd) == 0))
		{
			//| Parse line for name, latitude, and longitude

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

			err = strncpy_s(city_site.name, _countof(city_site.name), str[0], 49);
			city_site.lat = BearingStringToDecimalDegrees(str[1]);
			city_site.lon = BearingStringToDecimalDegrees(str[2]);
			city_site.alt = 0.0;

			if (city_site.lon < 0.0)
			{
				city_site.lon += 360.0;
			}

			PlaceTextAndMarkerDataInMaskArrayForMaps(city_site, digitalElevationModelWrapper, minimumLatitudeNorth, maximumLatitudeNorth,
				minimumLongitudeWest, maximumLongitudeWest);

			fgets(input, 78, fd);
		}

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
LoadUserDefinedTerrainFile
   (char *filename,
	DigitalElevationModelWrapper *digitalElevationModelWrapper)
{
	int	i, x, y, z, ypix, xpix, tempxpix, tempypix, fd = 0;
	char input[80], str[3][80], tempname[15], *pointer = NULL;
	double latitude, longitude, height, tempheight;
	FILE *fd1 = NULL, *fd2 = NULL;
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
			//| Parse line for latitude, longitude, height

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

			//| Remove <CR> and/or <LF> from antenna height string

			for (i = 0; (str[2][i] != 13) && (str[2][i] != 10) && (str[2][i] != 0); i++);

			str[2][i] = 0;

			//| The terrain feature may be expressed in either
			//| feet or meters. If the letter 'M' or 'm' is
			//| discovered in the string, then this is an
			//| indication that the value given is expressed
			//| in meters. Otherwise the height is interpreted
			//| as being expressed in feet.

			for (i = 0; (str[2][i] != 'M') && (str[2][i] != 'm') && (str[2][i] != 0) && (i < 48); i++);

			if ((str[2][i] == 'M') || (str[2][i] == 'm'))
			{
				str[2][i] = 0;
				height = rint(atof(str[2]));
			}
			else
			{
				str[2][i] = 0;
				height = rint(METERS_PER_FOOT*atof(str[2]));
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
		_close(fd);

		fprintf(stdout, "Done");
		fflush(stdout);

		err = fopen_s(&fd1, tempname, "r");
		err = fopen_s(&fd2, tempname, "r");

		y = 0;

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
					z = 1;  //| Dupe

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

			if (z == 0)  //| No duplicate found
			{
				AddUserDefinedTerrainToDigitalElevationModelData(xpix * (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree)), ypix * (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree)), height, digitalElevationModelWrapper);
			}

			fscanf_s(fd1, "%ld, %ld, %lf", &xpix, &ypix, &height);
			y++;

			rewind(fd2);
		} while (feof(fd1) == 0);

		fclose(fd1);
		fclose(fd2);
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
LoadCartographicBoundaryFiles
   (char *filename,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	Path *path)
{
	int	x;
	double lat0, lon0, lat1, lon1;
	char BoundariesString[80];
	Site source, destination;
	FILE *fd = NULL;
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

				source.lat = lat0;
				source.lon = (lon0>0.0 ? 360.0 - lon0 : -lon0);
				destination.lat = lat1;
				destination.lon = (lon1>0.0 ? 360.0 - lon1 : -lon1);

				GeneratePathBetweenSites(source, destination, digitalElevationModelWrapper, path);

				for (x = 0; x < path->length; x++)
				{
					SetOrValueInDigitalElevationModelMask(path->lat[x], path->lon[x], 4, digitalElevationModelWrapper);
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
//| FUNCTION: ReadLongleyRiceParameterDataForSite
//| 
//| OLD NAME: ReadLRParm
//| 
//| NOTES: 
//|   This function reads ITM parameter data for the transmitter
//|   site. The file name is the same as the txsite, except the
//|   filename extension is .lrp. If the needed file is not found,
//|   then the .lrp file is read from the current working
//|   directory. Failure to load this file under a forced_read
//|   condition will result in the default parameters hard coded
//|   into this function to be used and written to a .lrp file for the txsite.
//| 
//| ------------------------------
char
ReadLongleyRiceParameterDataForSite
   (Site txsite,
	char forced_read,
	IrregularTerrainModelParameters *itmParameters,
	unsigned char *gotAntennaElevationAnglePattern,
	unsigned char *gotAntennaAzimuthAnglePattern,
	double fresnelZoneFrequency,
	double effectiveRadiatedPower,
	char *lrpFilePath)
{
	double din;
	char filename[255], LrString[80], *pointer = NULL, return_value = 0;
	int	iin, ok = 0, x;
	FILE *fd = NULL, *outfile = NULL;
	errno_t err;

	//| Default parameters

	itmParameters->eps_dielect = 0.0;
	itmParameters->sgm_conductivity = 0.0;
	itmParameters->eno_ns_surfref = 0.0;
	itmParameters->frq_mhz = 0.0;
	itmParameters->radio_climate = 0;
	itmParameters->pol = 0;
	itmParameters->conf = 0.0;
	itmParameters->rel = 0.0;
	itmParameters->erp = 0.0;

	//| Generate .lrp filename from txsite filename.

	for (x = 0; (txsite.filename[x] != '.') && (txsite.filename[x] != 0) && (x < 250); x++)
	{
		filename[x] = txsite.filename[x];
	}

	filename[x] = '.';
	filename[x + 1] = 'l';
	filename[x + 2] = 'r';
	filename[x + 3] = 'p';
	filename[x + 4] = 0;

	err = fopen_s(&fd, filename, "r");

	if (fd == NULL)
	{
		//| Load default "splat.lrp" file
		if (lrpFilePath != NULL)
		{
			ok = snprintf(filename, 253, "%s%s", lrpFilePath, "splat.lrp\0");
		}
		else
		{
			err = strncpy_s(filename, _countof(filename), "splat.lrp\0", 10);
		}
		err = fopen_s(&fd, filename, "r");
	}

	if (fd != NULL)
	{
		fgets(LrString, 80, fd);

		pointer = strchr(LrString, ';');

		if (pointer != NULL)
		{
			*pointer = 0;
		}

		ok = sscanf_s(LrString, "%lf", &din);

		if (ok)
		{
			itmParameters->eps_dielect = din;

			fgets(LrString, 80, fd);

			pointer = strchr(LrString, ';');

			if (pointer != NULL)
			{
				*pointer = 0;
			}

			ok = sscanf_s(LrString, "%lf", &din);
		}

		if (ok)
		{
			itmParameters->sgm_conductivity = din;

			fgets(LrString, 80, fd);

			pointer = strchr(LrString, ';');

			if (pointer != NULL)
			{
				*pointer = 0;
			}

			ok = sscanf_s(LrString, "%lf", &din);
		}

		if (ok)
		{
			itmParameters->eno_ns_surfref = din;

			fgets(LrString, 80, fd);

			pointer = strchr(LrString, ';');

			if (pointer != NULL)
			{
				*pointer = 0;
			}

			ok = sscanf_s(LrString, "%lf", &din);
		}

		if (ok)
		{
			itmParameters->frq_mhz = din;

			fgets(LrString, 80, fd);

			pointer = strchr(LrString, ';');

			if (pointer != NULL)
			{
				*pointer = 0;
			}

			ok = sscanf_s(LrString, "%d", &iin);
		}

		if (ok)
		{
			itmParameters->radio_climate = iin;

			fgets(LrString, 80, fd);

			pointer = strchr(LrString, ';');

			if (pointer != NULL)
			{
				*pointer = 0;
			}

			ok = sscanf_s(LrString, "%d", &iin);
		}

		if (ok)
		{
			itmParameters->pol = iin;

			fgets(LrString, 80, fd);

			pointer = strchr(LrString, ';');

			if (pointer != NULL)
			{
				*pointer = 0;
			}

			ok = sscanf_s(LrString, "%lf", &din);
		}

		if (ok)
		{
			itmParameters->conf = din;

			fgets(LrString, 80, fd);

			pointer = strchr(LrString, ';');

			if (pointer != NULL)
			{
				*pointer = 0;
			}

			ok = sscanf_s(LrString, "%lf", &din);
		}

		if (ok)
		{
			itmParameters->rel = din;
			din = 0.0;
			return_value = 1;

			if (fgets(LrString, 80, fd) != NULL)
			{
				pointer = strchr(LrString, ';');

				if (pointer != NULL)
				{
					*pointer = 0;
				}

				if (sscanf_s(LrString, "%lf", &din))
				{
					itmParameters->erp = din;
				}

				//| ERP in SPLAT is referenced to 1 Watt
				//| into a dipole (0 dBd). If ERP is
				//| expressed in dBm (referenced to a
				//| 0 dBi radiator), convert dBm in EIRP
				//| to ERP.

				if ((strstr(LrString, "dBm") != NULL) || (strstr(LrString, "dbm") != NULL))
				{
					itmParameters->erp = (pow(10.0, (itmParameters->erp - 32.14) / 10.0));
				}
			}
		}

		fclose(fd);

		if (effectiveRadiatedPower != -1.0)
		{
			itmParameters->erp = effectiveRadiatedPower;
		}

		if ((fresnelZoneFrequency >= 20.0) && (fresnelZoneFrequency <= 20000.0))
		{
			itmParameters->frq_mhz = fresnelZoneFrequency;
		}

		if (ok)
		{
			LoadAntennaAzimuthElevationPatternFiles(filename, itmParameters, gotAntennaElevationAnglePattern, gotAntennaAzimuthAnglePattern);
		}
	}

	if ((fd == NULL) && forced_read)
	{
		//| Assign some default parameters
		//| for use in this run.

		itmParameters->eps_dielect = 15.0;
		itmParameters->sgm_conductivity = 0.005;
		itmParameters->eno_ns_surfref = 301.0;
		itmParameters->frq_mhz = 300.0;
		itmParameters->radio_climate = 5;
		itmParameters->pol = 0;
		itmParameters->conf = 0.50;
		itmParameters->rel = 0.50;
		itmParameters->erp = 0.0;

		//| Write them to a .lrp file.

		err = fopen_s(&outfile, filename, "w");

		fprintf(outfile, "%.3f\t; Earth Dielectric Constant (Relative permittivity)\n", itmParameters->eps_dielect);
		fprintf(outfile, "%.3f\t; Earth Conductivity (Siemens per meter)\n", itmParameters->sgm_conductivity);
		fprintf(outfile, "%.3f\t; Atmospheric Bending Constant (N-Units)\n", itmParameters->eno_ns_surfref);
		fprintf(outfile, "%.3f\t; Frequency in MHz (20 MHz to 20 GHz)\n", itmParameters->frq_mhz);
		fprintf(outfile, "%d\t; Radio Climate\n", itmParameters->radio_climate);
		fprintf(outfile, "%d\t; Polarization (0 = Horizontal, 1 = Vertical)\n", itmParameters->pol);
		fprintf(outfile, "%.2f\t; Fraction of Situations\n", itmParameters->conf);
		fprintf(outfile, "%.2f\t; Fraction of Time\n", itmParameters->rel);
		fprintf(outfile, "%.2f\t; Transmitter Effective Radiated Power in Watts or dBm (optional)\n", itmParameters->erp);
		fprintf(outfile, "\nPlease consult SPLAT documentation for the meaning and use of this data.\n");

		fclose(outfile);

		return_value = 1;

		fprintf(stderr, "\n\n%c*** There were problems reading your \"%s\" file ***\nSPLAT will create and use an .lrp file with default data.\n", 7, filename);
	}
	else if (forced_read == 0)
	{
		return_value = 0;
	}

	if (forced_read && ((fd == NULL) || (ok == 0)))
	{
		itmParameters->eps_dielect = 15.0;
		itmParameters->sgm_conductivity = 0.005;
		itmParameters->eno_ns_surfref = 301.0;
		itmParameters->frq_mhz = 300.0;
		itmParameters->radio_climate = 5;
		itmParameters->pol = 0;
		itmParameters->conf = 0.50;
		itmParameters->rel = 0.50;
		itmParameters->erp = 0.0;

		fprintf(stderr, "Default parameters have been assumed for this analysis.\n");

		return_value = 1;
	}

	return (return_value);
}


//| ------------------------------
//| 
//| FUNCTION: AnalyzeAndPlotLineOfSightCoverageBetweenSites
//| 
//| OLD NAME: PlotPath
//| 
//| NOTES: 
//|   This function analyzes the path between the source and
//|   destination locations. It determines which points along
//|   the path have line-of-sight visibility to the source.
//|   Points along with path having line-of-sight visibility
//|   to the source at an AGL altitude equal to that of the
//|   destination location are stored by setting bit 1 in the
//|   mask[][] array, which are displayed in green when PPM
//|   maps are later generated by SPLAT.
//| 
//| ------------------------------
void
AnalyzeAndPlotLineOfSightCoverageBetweenSites
   (Site source,
	Site destination,
	char mask_value,
	FILE *fd,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	Path *path,
	double groundClutterHeight,
	double sphereRadius)
{
	char block;
	int x, y;
	register double cos_xmtr_angle, cos_test_angle, test_alt;
	double distance, rx_alt, tx_alt;

	GeneratePathBetweenSites(source, destination, digitalElevationModelWrapper, path);

	for (y = 0; y < path->length; y++)
	{
		//| Test this point only if it hasn't been already
		//| tested and found to be free of obstructions.

		if ((GetValueInDigitalElevationModelMask(path->lat[y], path->lon[y], digitalElevationModelWrapper) & mask_value) == 0)
		{
			distance = 5280.0*path->distance[y];
			tx_alt = sphereRadius + source.alt + path->elevation[0];
			rx_alt = sphereRadius + destination.alt + path->elevation[y];

			//| Calculate the cosine of the elevation of the
			//| transmitter as seen at the temp rx point.

			cos_xmtr_angle = ((rx_alt*rx_alt) + (distance*distance) - (tx_alt*tx_alt)) / (2.0*rx_alt*distance);

			for (x = y, block = 0; (x >= 0) && (block == 0); x--)
			{
				distance = 5280.0 * (path->distance[y] - path->distance[x]);
				test_alt = sphereRadius + (path->elevation[x] == 0.0 ? path->elevation[x] : path->elevation[x] + groundClutterHeight);

				cos_test_angle = ((rx_alt*rx_alt) + (distance*distance) - (test_alt*test_alt)) / (2.0*rx_alt*distance);

				//| Compare these two angles to determine if
				//| an obstruction exists. Since we're comparing
				//| the cosines of these angles rather than
				//| the angles themselves, the following "if"
				//| statement is reversed from what it would
				//| be if the actual angles were compared.

				if (cos_xmtr_angle >= cos_test_angle)
				{
					block = 1;
				}
			}

			if (block == 0)
			{
				SetOrValueInDigitalElevationModelMask(path->lat[y], path->lon[y], mask_value, digitalElevationModelWrapper);
			}

			if (fd != NULL)
			{
				fprintf(fd, "%.7f, %.7f", path->lat[y], path->lon[y]);
				if (block)
				{
					fprintf(fd, " (Obstruction)");
				}
				fprintf(fd, "\n");
			}
		}
	}
}


//| ------------------------------
//| 
//| FUNCTION: AnalyzeAndPlotPathLossBetweenSites
//| 
//| OLD NAME: PlotLRPath
//| 
//| NOTES: 
//|   This function plots the RF path loss between source and
//|   destination points based on the ITWOM propagation model,
//|   taking into account antenna pattern data, if available.
//| 
//| ------------------------------
void
AnalyzeAndPlotPathLossBetweenSites
   (Site source,
	Site destination,
	unsigned char mask_value,
	FILE *fd,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	Path *path,
	IrregularTerrainModelParameters *itmParameters,
	double* pathElevation,
	char useOldLongleyRiceModel,
	unsigned char plotSignalPowerLevelContours,
	unsigned char gotAntennaElevationAnglePattern,
	double maximumAnalysisDistance,
	double groundClutterHeight)
{
	int	x, y, ifs, ofs, errnum;
	char block = 0, strmode[100];
	double loss, azimuth, pattern = 0.0, xmtr_alt,
		dest_alt, xmtr_alt2, dest_alt2, cos_rcvr_angle,
		cos_test_angle = 0.0, test_alt, elevation = 0.0,
		distance = 0.0, four_thirds_earth, rxp, dBm,
		field_strength = 0.0;
	Site temp;

	GeneratePathBetweenSites(source, destination, digitalElevationModelWrapper, path);

	four_thirds_earth = FOUR_THIRDS * EARTH_RADIUS;

	//| Copy elevations plus clutter along path into the pathElevation[] array.

	for (x = 1; x < path->length - 1; x++)
	{
		pathElevation[x + 2] = (path->elevation[x] == 0.0 ? path->elevation[x] * METERS_PER_FOOT : (groundClutterHeight + path->elevation[x]) * METERS_PER_FOOT);
	}

	//| Copy ending points without clutter

	pathElevation[2] = path->elevation[0] * METERS_PER_FOOT;
	pathElevation[path->length + 1] = path->elevation[path->length - 1] * METERS_PER_FOOT;

	//| Since the only energy the propagation model considers
	//| reaching the destination is based on what is scattered
	//| or deflected from the first obstruction along the path,
	//| we first need to find the location and elevation angle
	//| of that first obstruction (if it exists). This is done
	//| using a 4/3rds Earth radius to match the radius used by
	//| the irregular terrain propagation model. This information
	//| is required for properly integrating the antenna's elevation
	//| pattern into the calculation for overall path loss.

	for (y = 2; (y < (path->length - 1)) && (path->distance[y] <= maximumAnalysisDistance); y++)
	{
		//| Process this point only if it
		//| has not already been processed.

		if ((GetValueInDigitalElevationModelMask(path->lat[y], path->lon[y], digitalElevationModelWrapper) & 248) != (mask_value << 3))
		{
			distance = 5280.0 * path->distance[y];
			xmtr_alt = four_thirds_earth + source.alt + path->elevation[0];
			dest_alt = four_thirds_earth + destination.alt + path->elevation[y];
			dest_alt2 = dest_alt * dest_alt;
			xmtr_alt2 = xmtr_alt * xmtr_alt;

			//| Calculate the cosine of the elevation of
			//| the receiver as seen by the transmitter.

			cos_rcvr_angle = (xmtr_alt2 + (distance*distance) - dest_alt2) / (2.0 * xmtr_alt * distance);

			if (cos_rcvr_angle > 1.0)
			{
				cos_rcvr_angle = 1.0;
			}

			if (cos_rcvr_angle < -1.0)
			{
				cos_rcvr_angle = -1.0;
			}

			if (gotAntennaElevationAnglePattern || (fd != NULL))
			{
				//| Determine the elevation angle to the first obstruction
				//| along the path IF elevation pattern data is available
				//| or an output (.ano) file has been designated.

				for (x = 2, block = 0; (x < y) && (block == 0); x++)
				{
					distance = 5280.0 * path->distance[x];

					test_alt = four_thirds_earth + (path->elevation[x] == 0.0 ? path->elevation[x] : path->elevation[x] + groundClutterHeight);

					//| Calculate the cosine of the elevation
					//| angle of the terrain (test point)
					//| as seen by the transmitter.

					cos_test_angle = (xmtr_alt2 + (distance*distance) - (test_alt*test_alt)) / (2.0 * xmtr_alt * distance);

					if (cos_test_angle > 1.0)
					{
						cos_test_angle = 1.0;
					}

					if (cos_test_angle < -1.0)
					{
						cos_test_angle = -1.0;
					}

					//| Compare these two angles to determine if
					//| an obstruction exists. Since we're comparing
					//| the cosines of these angles rather than
					//| the angles themselves, the sense of the
					//| following "if" statement is reversed from
					//| what it would be if the angles themselves
					//| were compared.

					if (cos_rcvr_angle >= cos_test_angle)
					{
						block = 1;
					}
				}

				if (block)
				{
					elevation = ((acos(cos_test_angle)) / DEGREES_TO_RADIANS) - 90.0;
				}
				else
				{
					elevation = ((acos(cos_rcvr_angle)) / DEGREES_TO_RADIANS) - 90.0;
				}
			}

			//| Determine attenuation for each point along
			//| the path using ITWOM's PointToPointCalculation mode
			//| starting at y=2 (number_of_points = 1), the
			//| shortest distance terrain can play a role in
			//| path loss.

			pathElevation[0] = y - 1;  //| (number of points - 1)

									   //| Distance between elevation samples

			pathElevation[1] = METERS_PER_MILE * (path->distance[y] - path->distance[y - 1]);

			if (useOldLongleyRiceModel)
			{
				PointToPointCalculationLegacy(pathElevation, (source.alt * METERS_PER_FOOT),
					(destination.alt * METERS_PER_FOOT), itmParameters->eps_dielect,
					itmParameters->sgm_conductivity, itmParameters->eno_ns_surfref, itmParameters->frq_mhz,
					itmParameters->radio_climate, itmParameters->pol, itmParameters->conf, itmParameters->rel, loss,
					strmode, 100, errnum);
			}
			else
			{
				PointToPointCalculation(pathElevation, (source.alt * METERS_PER_FOOT),
					(destination.alt * METERS_PER_FOOT), itmParameters->eps_dielect,
					itmParameters->sgm_conductivity, itmParameters->eno_ns_surfref, itmParameters->frq_mhz,
					itmParameters->radio_climate, itmParameters->pol, itmParameters->conf, itmParameters->rel, loss,
					strmode, 100, errnum);
			}

			temp.lat = path->lat[y];
			temp.lon = path->lon[y];

			azimuth = AzimuthAngleBetweenSites(source, temp);

			if (fd != NULL)
			{
				fprintf(fd, "%.7f, %.7f, %.3f, %.3f, ", path->lat[y], path->lon[y], azimuth, elevation);
			}

			//| If ERP==0, write path loss to alphanumeric
			//| output file. Otherwise, write field strength
			//| or received power level (below), as appropriate.

			if ((fd != NULL) && (itmParameters->erp == 0.0))
			{
				fprintf(fd, "%.2f", loss);
			}

			//| Integrate the antenna's radiation
			//| pattern into the overall path loss.

			x = (int)rint(10.0 * (10.0 - elevation));

			if ((x >= 0) && (x <= 1000))
			{
				azimuth = rint(azimuth);

				pattern = (double)itmParameters->antenna_pattern[(int)azimuth][x];

				if (pattern != 0.0)
				{
					pattern = 20.0 * log10(pattern);
					loss -= pattern;
				}
			}

			if (itmParameters->erp != 0.0)
			{
				if (plotSignalPowerLevelContours != 0)
				{
					//| dBm is based on EIRP (ERP + 2.14)

					rxp = itmParameters->erp / pow(10.0, (loss - 2.14) / 10.0);

					dBm = 10.0 * log10(rxp*1000.0);

					if (fd != NULL)
					{
						fprintf(fd, "%.3f", dBm);
					}

					//| Scale roughly between 0 and 255

					ifs = 200 + (int)rint(dBm);

					if (ifs < 0)
					{
						ifs = 0;
					}

					if (ifs > 255)
					{
						ifs = 255;
					}

					ofs = GetValueInDigitalElevationModelSignal(path->lat[y], path->lon[y], digitalElevationModelWrapper);

					if (ofs > ifs)
					{
						ifs = ofs;
					}

					SetValueInDigitalElevationModelSignal(path->lat[y], path->lon[y], (unsigned char)ifs, digitalElevationModelWrapper);
				}
				else
				{
					field_strength = (139.4 + (20.0*log10(itmParameters->frq_mhz)) - loss) + (10.0*log10(itmParameters->erp / 1000.0));

					ifs = 100 + (int)rint(field_strength);

					if (ifs < 0)
					{
						ifs = 0;
					}

					if (ifs > 255)
					{
						ifs = 255;
					}

					ofs = GetValueInDigitalElevationModelSignal(path->lat[y], path->lon[y], digitalElevationModelWrapper);

					if (ofs > ifs)
					{
						ifs = ofs;
					}

					SetValueInDigitalElevationModelSignal(path->lat[y], path->lon[y], (unsigned char)ifs, digitalElevationModelWrapper);

					if (fd != NULL)
					{
						fprintf(fd, "%.3f", field_strength);
					}
				}
			}
			else
			{
				if (loss > 255)
				{
					ifs = 255;
				}
				else
				{
					ifs = (int)rint(loss);
				}

				ofs = GetValueInDigitalElevationModelSignal(path->lat[y], path->lon[y], digitalElevationModelWrapper);

				if ((ofs < ifs) && (ofs != 0))
				{
					ifs = ofs;
				}

				SetValueInDigitalElevationModelSignal(path->lat[y], path->lon[y], (unsigned char)ifs, digitalElevationModelWrapper);
			}

			if (fd != NULL)
			{
				if (block)
				{
					fprintf(fd, " (Obstruction)");
				}
				fprintf(fd, "\n");
			}

			//| Mark this point as having been analyzed

			SetValueInDigitalElevationModelMask(path->lat[y], path->lon[y], (GetValueInDigitalElevationModelMask(path->lat[y], path->lon[y], digitalElevationModelWrapper) & 7) + (mask_value << 3), digitalElevationModelWrapper);
		}
	}
}


//| ------------------------------
//| 
//| FUNCTION: PlotLineOfSightCoverageFromSiteAtAltitude
//| 
//| OLD NAME: PlotLOSMap
//| 
//| NOTES: 
//|   This function performs a 360 degree sweep around the
//|   transmitter site (source location), and plots the
//|   line-of-sight coverage of the transmitter on the SPLAT
//|   generated topographic map based on a receiver located
//|   at the specified altitude (in feet AGL). Results
//|   are stored in memory, and written out in the form
//|   of a topographic map when the WritePortablePixMapLineOfSightCoverageFile() function
//|   is later invoked.
//| 
//| ------------------------------
void
PlotLineOfSightCoverageFromSiteAtAltitude
   (Site source,
	double altitude,
	char *plo_filename,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	Path *path,
	unsigned char useMetricUnits,
	int minimumLatitudeNorth,
	int maximumLatitudeNorth,
	int minimumLongitudeWest,
	int maximumLongitudeWest,
	double groundClutterHeight,
	double sphereRadius)
{
	int y, z, count;
	Site edge;
	unsigned char symbol[4], x;
	double lat, lon, minwest, maxnorth, th;
	static unsigned char mask_value = 1;
	FILE *fd = NULL;
	errno_t err;

	symbol[0] = '.';
	symbol[1] = 'o';
	symbol[2] = 'O';
	symbol[3] = 'o';

	count = 0;

	fprintf(stdout, "\nComputing line-of-sight coverage of \"%s\" with an RX antenna\nat %.2f %s AGL", source.name, (useMetricUnits ? altitude * METERS_PER_FOOT : altitude), (useMetricUnits ? "meters" : "feet"));

	if (groundClutterHeight > 0.0)
	{
		fprintf(stdout, " and %.2f %s of ground clutter", (useMetricUnits ? groundClutterHeight * METERS_PER_FOOT : groundClutterHeight), (useMetricUnits ? "meters" : "feet"));
	}

	fprintf(stdout, "...\n\n 0%c to  25%c ", 37, 37);
	fflush(stdout);

	if (plo_filename[0] != 0)
	{
		err = fopen_s(&fd, plo_filename, "wb");
	}

	if (fd != NULL)
	{
		//| Write header information to output file
		fprintf(fd, "%d, %d\t; max_west, min_west\n%d, %d\t; max_north, min_north\n\n", maximumLongitudeWest, minimumLongitudeWest, maximumLatitudeNorth, minimumLatitudeNorth);
		fprintf(fd, "Latitude, Longitude\n");
	}

	//| th=pixels/degree divided by 64 loops per
	//| progress indicator symbol (.oOo) printed.

	th = ((double)digitalElevationModelWrapper->demPixelsPerDegree) / 64.0;

	z = (int)(th*ConvertToNormalizedAngle(maximumLongitudeWest - minimumLongitudeWest));

	minwest = (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree)) + (double)minimumLongitudeWest;
	maxnorth = (double)maximumLatitudeNorth - (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree));

	for (lon = minwest, x = 0, y = 0; LongitudeDifference(lon, (double)maximumLongitudeWest) <= 0.0; y++, lon = (minwest + ((1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree))*(double)y)))
	{
		if (lon >= 360.0)
		{
			lon -= 360.0;
		}

		edge.lat = maximumLatitudeNorth;
		edge.lon = lon;
		edge.alt = (float)altitude;

		AnalyzeAndPlotLineOfSightCoverageBetweenSites(source, edge, mask_value, fd, digitalElevationModelWrapper, path, groundClutterHeight, sphereRadius);
		count++;

		if (count == z)
		{
			fprintf(stdout, "%c", symbol[x]);
			fflush(stdout);
			count = 0;

			if (x == 3)
			{
				x = 0;
			}
			else
			{
				x++;
			}
		}
	}

	count = 0;
	fprintf(stdout, "\n25%c to  50%c ", 37, 37);
	fflush(stdout);

	z = (int)(th*(double)(maximumLatitudeNorth - minimumLatitudeNorth));

	for (lat = maxnorth, x = 0, y = 0; lat >= (double)minimumLatitudeNorth; y++, lat = maxnorth - ((1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree))*(double)y))
	{
		edge.lat = lat;
		edge.lon = minimumLongitudeWest;
		edge.alt = (float)altitude;

		AnalyzeAndPlotLineOfSightCoverageBetweenSites(source, edge, mask_value, fd, digitalElevationModelWrapper, path, groundClutterHeight, sphereRadius);
		count++;

		if (count == z)
		{
			fprintf(stdout, "%c", symbol[x]);
			fflush(stdout);
			count = 0;

			if (x == 3)
			{
				x = 0;
			}
			else
			{
				x++;
			}
		}
	}

	count = 0;
	fprintf(stdout, "\n50%c to  75%c ", 37, 37);
	fflush(stdout);

	z = (int)(th*ConvertToNormalizedAngle(maximumLongitudeWest - minimumLongitudeWest));

	for (lon = minwest, x = 0, y = 0; LongitudeDifference(lon, (double)maximumLongitudeWest) <= 0.0; y++, lon = (minwest + ((1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree))*(double)y)))
	{
		if (lon >= 360.0)
		{
			lon -= 360.0;
		}

		edge.lat = minimumLatitudeNorth;
		edge.lon = lon;
		edge.alt = (float)altitude;

		AnalyzeAndPlotLineOfSightCoverageBetweenSites(source, edge, mask_value, fd, digitalElevationModelWrapper, path, groundClutterHeight, sphereRadius);
		count++;

		if (count == z)
		{
			fprintf(stdout, "%c", symbol[x]);
			fflush(stdout);
			count = 0;

			if (x == 3)
			{
				x = 0;
			}
			else
			{
				x++;
			}
		}
	}

	count = 0;
	fprintf(stdout, "\n75%c to 100%c ", 37, 37);
	fflush(stdout);

	z = (int)(th*(double)(maximumLatitudeNorth - minimumLatitudeNorth));

	for (lat = (double)minimumLatitudeNorth, x = 0, y = 0; lat < (double)maximumLatitudeNorth; y++, lat = ((double)minimumLatitudeNorth + ((1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree))*(double)y)))
	{
		edge.lat = lat;
		edge.lon = maximumLongitudeWest;
		edge.alt = (float)altitude;

		AnalyzeAndPlotLineOfSightCoverageBetweenSites(source, edge, mask_value, fd, digitalElevationModelWrapper, path, groundClutterHeight, sphereRadius);
		count++;

		if (count == z)
		{
			fprintf(stdout, "%c", symbol[x]);
			fflush(stdout);
			count = 0;

			if (x == 3)
			{
				x = 0;
			}
			else
			{
				x++;
			}
		}
	}

	if (fd != NULL)
	{
		fclose(fd);
	}

	fprintf(stdout, "\nDone\n");
	fflush(stdout);

	//| Assign next mask value

	switch (mask_value)
	{
	case 1:
		mask_value = 8;
		break;

	case 8:
		mask_value = 16;
		break;

	case 16:
		mask_value = 32;
	}
}


//| ------------------------------
//| 
//| FUNCTION: PlotAttenuationFromSiteAtAltitude
//| 
//| OLD NAME: PlotLRMap
//| 
//| NOTES: 
//|   This function performs a 360 degree sweep around the
//|   transmitter site (source location), and plots the
//|   Irregular Terrain Model attenuation on the SPLAT
//|   generated topographic map based on a receiver located
//|   at the specified altitude (in feet AGL). Results
//|   are stored in memory, and written out in the form
//|   of a topographic map when the WritePortablePixMapAttenuationFile() or
//|   WritePortablePixMapSignalStrengthFile() functions are later invoked.
//| 
//| ------------------------------
void
PlotAttenuationFromSiteAtAltitude
   (Site source,
	double altitude,
	char *plo_filename,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	Path *path,
	IrregularTerrainModelParameters *itmParameters,
	double *pathElevation,
	char useOldLongleyRiceModel,
	unsigned char plotSignalPowerLevelContours,
	unsigned char gotAntennaElevationAnglePattern,
	unsigned char useMetricUnits,
	int minimumLatitudeNorth,
	int maximumLatitudeNorth,
	int minimumLongitudeWest,
	int maximumLongitudeWest,
	double maximumAnalysisDistance,
	double groundClutterHeight)
{
	int y, z, count;
	Site edge;
	double lat, lon, minwest, maxnorth, th;
	unsigned char x, symbol[4];
	static unsigned char mask_value = 1;
	FILE *fd = NULL;
	errno_t err;

	minwest = (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree)) + (double)minimumLongitudeWest;
	maxnorth = (double)maximumLatitudeNorth - (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree));

	symbol[0] = '.';
	symbol[1] = 'o';
	symbol[2] = 'O';
	symbol[3] = 'o';

	count = 0;

	if (useOldLongleyRiceModel)
	{
		fprintf(stdout, "\nComputing ITM ");
	}
	else
	{
		fprintf(stdout, "\nComputing ITWOM ");
	}

	if (itmParameters->erp == 0.0)
	{
		fprintf(stdout, "path loss");
	}
	else
	{
		if (plotSignalPowerLevelContours != 0)
		{
			fprintf(stdout, "signal power level");
		}
		else
		{
			fprintf(stdout, "field strength");
		}
	}

	fprintf(stdout, " contours of \"%s\"\nout to a radius of %.2f %s with an RX antenna at %.2f %s AGL", source.name, (useMetricUnits ? maximumAnalysisDistance * KM_PER_MILE : maximumAnalysisDistance), (useMetricUnits ? "kilometers" : "miles"), (useMetricUnits ? altitude * METERS_PER_FOOT : altitude), (useMetricUnits ? "meters" : "feet"));

	if (groundClutterHeight > 0.0)
	{
		fprintf(stdout, "\nand %.2f %s of ground clutter", (useMetricUnits ? groundClutterHeight * METERS_PER_FOOT : groundClutterHeight), (useMetricUnits ? "meters" : "feet"));
	}

	fprintf(stdout, "...\n\n 0%c to  25%c ", 37, 37);
	fflush(stdout);

	if (plo_filename[0] != 0)
	{
		err = fopen_s(&fd, plo_filename, "wb");
	}

	if (fd != NULL)
	{
		//| Write header information to output file
		fprintf(fd, "%d, %d\t; max_west, min_west\n%d, %d\t; max_north, min_north\n\n", maximumLongitudeWest, minimumLongitudeWest, maximumLatitudeNorth, minimumLatitudeNorth);
		fprintf(fd, "Latitude, Longitude, AzimuthAngle, ElevationAngle, ");
		if (itmParameters->erp == 0.0)
		{
			fprintf(fd, "PathLoss\n");
		}
		else
		{
			if (plotSignalPowerLevelContours != 0)
			{
				fprintf(fd, "dBm\n");
			}
			else
			{
				fprintf(fd, "FieldStrength\n");
			}
		}
	}

	//| th=pixels/degree divided by 64 loops per
	//| progress indicator symbol (.oOo) printed.

	th = ((double)digitalElevationModelWrapper->demPixelsPerDegree) / 64.0;

	z = (int)(th*ConvertToNormalizedAngle(maximumLongitudeWest - minimumLongitudeWest));

	for (lon = minwest, x = 0, y = 0; LongitudeDifference(lon, (double)maximumLongitudeWest) <= 0.0; y++, lon = (minwest + ((1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree))*(double)y)))
	{
		if (lon >= 360.0)
		{
			lon -= 360.0;
		}

		edge.lat = maximumLatitudeNorth;
		edge.lon = lon;
		edge.alt = (float)altitude;

		AnalyzeAndPlotPathLossBetweenSites(source, edge, mask_value, fd, digitalElevationModelWrapper, path, itmParameters, pathElevation, useOldLongleyRiceModel,
			plotSignalPowerLevelContours, gotAntennaElevationAnglePattern, maximumAnalysisDistance, groundClutterHeight);
		count++;

		if (count == z)
		{
			fprintf(stdout, "%c", symbol[x]);
			fflush(stdout);
			count = 0;

			if (x == 3)
			{
				x = 0;
			}
			else
			{
				x++;
			}
		}
	}

	count = 0;
	fprintf(stdout, "\n25%c to  50%c ", 37, 37);
	fflush(stdout);

	z = (int)(th*(double)(maximumLatitudeNorth - minimumLatitudeNorth));

	for (lat = maxnorth, x = 0, y = 0; lat >= (double)minimumLatitudeNorth; y++, lat = (maxnorth - ((1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree))*(double)y)))
	{
		edge.lat = lat;
		edge.lon = minimumLongitudeWest;
		edge.alt = (float)altitude;

		AnalyzeAndPlotPathLossBetweenSites(source, edge, mask_value, fd, digitalElevationModelWrapper, path, itmParameters, pathElevation, useOldLongleyRiceModel,
			plotSignalPowerLevelContours, gotAntennaElevationAnglePattern, maximumAnalysisDistance, groundClutterHeight);
		count++;

		if (count == z)
		{
			fprintf(stdout, "%c", symbol[x]);
			fflush(stdout);
			count = 0;

			if (x == 3)
			{
				x = 0;
			}
			else
			{
				x++;
			}
		}
	}

	count = 0;
	fprintf(stdout, "\n50%c to  75%c ", 37, 37);
	fflush(stdout);

	z = (int)(th*ConvertToNormalizedAngle(maximumLongitudeWest - minimumLongitudeWest));

	for (lon = minwest, x = 0, y = 0; LongitudeDifference(lon, (double)maximumLongitudeWest) <= 0.0; y++, lon = (minwest + ((1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree))*(double)y)))
	{
		if (lon >= 360.0)
		{
			lon -= 360.0;
		}
		edge.lat = minimumLatitudeNorth;
		edge.lon = lon;
		edge.alt = (float)altitude;

		AnalyzeAndPlotPathLossBetweenSites(source, edge, mask_value, fd, digitalElevationModelWrapper, path, itmParameters, pathElevation, useOldLongleyRiceModel,
			plotSignalPowerLevelContours, gotAntennaElevationAnglePattern, maximumAnalysisDistance, groundClutterHeight);
		count++;

		if (count == z)
		{
			fprintf(stdout, "%c", symbol[x]);
			fflush(stdout);
			count = 0;

			if (x == 3)
			{
				x = 0;
			}
			else
			{
				x++;
			}
		}
	}

	count = 0;
	fprintf(stdout, "\n75%c to 100%c ", 37, 37);
	fflush(stdout);

	z = (int)(th*(double)(maximumLatitudeNorth - minimumLatitudeNorth));

	for (lat = (double)minimumLatitudeNorth, x = 0, y = 0; lat < (double)maximumLatitudeNorth; y++, lat = ((double)minimumLatitudeNorth + ((1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree))*(double)y)))
	{
		edge.lat = lat;
		edge.lon = maximumLongitudeWest;
		edge.alt = (float)altitude;

		AnalyzeAndPlotPathLossBetweenSites(source, edge, mask_value, fd, digitalElevationModelWrapper, path, itmParameters, pathElevation, useOldLongleyRiceModel,
			plotSignalPowerLevelContours, gotAntennaElevationAnglePattern, maximumAnalysisDistance, groundClutterHeight);
		count++;

		if (count == z)
		{
			fprintf(stdout, "%c", symbol[x]);
			fflush(stdout);
			count = 0;

			if (x == 3)
			{
				x = 0;
			}
			else
			{
				x++;
			}
		}
	}

	if (fd != NULL)
	{
		fclose(fd);
	}

	fprintf(stdout, "\nDone\n");
	fflush(stdout);

	if (mask_value < 30)
	{
		mask_value++;
	}
}


//| ------------------------------
//| 
//| FUNCTION: LoadSplatSignalColorsForSite
//| 
//| OLD NAME: LoadSignalColors
//| 
//| NOTES: 
//| 
//| ------------------------------
void
LoadSplatSignalColorsForSite
   (Site xmtr,
	ColorRegion *region)
{
	int x, y, ok, val[4];
	char filename[255], ColorString[80], *pointer = NULL;
	FILE *fd = NULL;
	errno_t err;

	for (x = 0; (xmtr.filename[x] != '.') && (xmtr.filename[x] != 0) && (x < 250); x++)
	{
		filename[x] = xmtr.filename[x];
	}

	filename[x] = '.';
	filename[x + 1] = 's';
	filename[x + 2] = 'c';
	filename[x + 3] = 'm'; //| Can't use .scf files in Windows.
	filename[x + 4] = 0;

	//| Default values

	region->level[0] = 128;
	region->color[0][0] = 255;
	region->color[0][1] = 0;
	region->color[0][2] = 0;

	region->level[1] = 118;
	region->color[1][0] = 255;
	region->color[1][1] = 165;
	region->color[1][2] = 0;

	region->level[2] = 108;
	region->color[2][0] = 255;
	region->color[2][1] = 206;
	region->color[2][2] = 0;

	region->level[3] = 98;
	region->color[3][0] = 255;
	region->color[3][1] = 255;
	region->color[3][2] = 0;

	region->level[4] = 88;
	region->color[4][0] = 184;
	region->color[4][1] = 255;
	region->color[4][2] = 0;

	region->level[5] = 78;
	region->color[5][0] = 0;
	region->color[5][1] = 255;
	region->color[5][2] = 0;

	region->level[6] = 68;
	region->color[6][0] = 0;
	region->color[6][1] = 208;
	region->color[6][2] = 0;

	region->level[7] = 58;
	region->color[7][0] = 0;
	region->color[7][1] = 196;
	region->color[7][2] = 196;

	region->level[8] = 48;
	region->color[8][0] = 0;
	region->color[8][1] = 148;
	region->color[8][2] = 255;

	region->level[9] = 38;
	region->color[9][0] = 80;
	region->color[9][1] = 80;
	region->color[9][2] = 255;

	region->level[10] = 28;
	region->color[10][0] = 0;
	region->color[10][1] = 38;
	region->color[10][2] = 255;

	region->level[11] = 18;
	region->color[11][0] = 142;
	region->color[11][1] = 63;
	region->color[11][2] = 255;

	region->level[12] = 8;
	region->color[12][0] = 140;
	region->color[12][1] = 0;
	region->color[12][2] = 128;

	region->levels = 13;

	err = fopen_s(&fd, "splat.scm", "r");	//| Can't use .scf files in Windows.

	if (fd == NULL)
	{
		err = fopen_s(&fd, filename, "r");
	}

	if (fd == NULL)
	{
		err = fopen_s(&fd, filename, "w");

		fprintf(fd, "; SPLAT Auto-generated Signal Color Definition (\"%s\") File\n", filename);
		fprintf(fd, ";\n; Format for the parameters held in this file is as follows:\n;\n");
		fprintf(fd, ";    dBuV/m: red, green, blue\n;\n");
		fprintf(fd, "; ...where \"dBuV/m\" is the signal strength (in dBuV/m) and\n");
		fprintf(fd, "; \"red\", \"green\", and \"blue\" are the corresponding RGB color\n");
		fprintf(fd, "; definitions ranging from 0 to 255 for the region specified.\n");
		fprintf(fd, ";\n; The following parameters may be edited and/or expanded\n");
		fprintf(fd, "; for future runs of SPLAT.  A total of 32 contour regions\n");
		fprintf(fd, "; may be defined in this file.\n;\n;\n");

		for (x = 0; x < region->levels; x++)
		{
			fprintf(fd, "%3d: %3d, %3d, %3d\n", region->level[x], region->color[x][0], region->color[x][1], region->color[x][2]);
		}

		fclose(fd);
	}
	else
	{
		x = 0;
		fgets(ColorString, 80, fd);

		while ((x < 32) && (feof(fd) == 0))
		{
			pointer = strchr(ColorString, ';');

			if (pointer != NULL)
			{
				*pointer = 0;
			}

			ok = sscanf_s(ColorString, "%d: %d, %d, %d", &val[0], &val[1], &val[2], &val[3]);

			if (ok == 4)
			{
				for (y = 0; y < 4; y++)
				{
					if (val[y] > 255)
					{
						val[y] = 255;
					}

					if (val[y] < 0)
					{
						val[y] = 0;
					}
				}

				region->level[x] = val[0];
				region->color[x][0] = val[1];
				region->color[x][1] = val[2];
				region->color[x][2] = val[3];
				x++;
			}

			fgets(ColorString, 80, fd);
		}

		fclose(fd);
		region->levels = x;
	}
}


//| ------------------------------
//| 
//| FUNCTION: LoadSplatLossColorsForSite
//| 
//| OLD NAME: LoadLossColors
//| 
//| NOTES: 
//| 
//| ------------------------------
void
LoadSplatLossColorsForSite
   (Site xmtr,
	ColorRegion *region)
{
	int x, y, ok, val[4];
	char filename[255], LossString[80], *pointer = NULL;
	FILE *fd = NULL;
	errno_t err;

	for (x = 0; (xmtr.filename[x] != '.') && (xmtr.filename[x] != 0) && (x < 250); x++)
	{
		filename[x] = xmtr.filename[x];
	}

	filename[x] = '.';
	filename[x + 1] = 'l';
	filename[x + 2] = 'c';
	filename[x + 3] = 'f';
	filename[x + 4] = 0;

	//| Default values

	region->level[0] = 80;
	region->color[0][0] = 255;
	region->color[0][1] = 0;
	region->color[0][2] = 0;

	region->level[1] = 90;
	region->color[1][0] = 255;
	region->color[1][1] = 128;
	region->color[1][2] = 0;

	region->level[2] = 100;
	region->color[2][0] = 255;
	region->color[2][1] = 165;
	region->color[2][2] = 0;

	region->level[3] = 110;
	region->color[3][0] = 255;
	region->color[3][1] = 206;
	region->color[3][2] = 0;

	region->level[4] = 120;
	region->color[4][0] = 255;
	region->color[4][1] = 255;
	region->color[4][2] = 0;

	region->level[5] = 130;
	region->color[5][0] = 184;
	region->color[5][1] = 255;
	region->color[5][2] = 0;

	region->level[6] = 140;
	region->color[6][0] = 0;
	region->color[6][1] = 255;
	region->color[6][2] = 0;

	region->level[7] = 150;
	region->color[7][0] = 0;
	region->color[7][1] = 208;
	region->color[7][2] = 0;

	region->level[8] = 160;
	region->color[8][0] = 0;
	region->color[8][1] = 196;
	region->color[8][2] = 196;

	region->level[9] = 170;
	region->color[9][0] = 0;
	region->color[9][1] = 148;
	region->color[9][2] = 255;

	region->level[10] = 180;
	region->color[10][0] = 80;
	region->color[10][1] = 80;
	region->color[10][2] = 255;

	region->level[11] = 190;
	region->color[11][0] = 0;
	region->color[11][1] = 38;
	region->color[11][2] = 255;

	region->level[12] = 200;
	region->color[12][0] = 142;
	region->color[12][1] = 63;
	region->color[12][2] = 255;

	region->level[13] = 210;
	region->color[13][0] = 196;
	region->color[13][1] = 54;
	region->color[13][2] = 255;

	region->level[14] = 220;
	region->color[14][0] = 255;
	region->color[14][1] = 0;
	region->color[14][2] = 255;

	region->level[15] = 230;
	region->color[15][0] = 255;
	region->color[15][1] = 194;
	region->color[15][2] = 204;

	region->levels = 16;

	err = fopen_s(&fd, "splat.lcf", "r");

	if (fd == NULL)
	{
		err = fopen_s(&fd, filename, "r");
	}

	if (fd == NULL)
	{
		err = fopen_s(&fd, filename, "w");

		fprintf(fd, "; SPLAT Auto-generated Path-Loss Color Definition (\"%s\") File\n", filename);
		fprintf(fd, ";\n; Format for the parameters held in this file is as follows:\n;\n");
		fprintf(fd, ";    dB: red, green, blue\n;\n");
		fprintf(fd, "; ...where \"dB\" is the path loss (in dB) and\n");
		fprintf(fd, "; \"red\", \"green\", and \"blue\" are the corresponding RGB color\n");
		fprintf(fd, "; definitions ranging from 0 to 255 for the region specified.\n");
		fprintf(fd, ";\n; The following parameters may be edited and/or expanded\n");
		fprintf(fd, "; for future runs of SPLAT.  A total of 32 contour regions\n");
		fprintf(fd, "; may be defined in this file.\n;\n;\n");

		for (x = 0; x < region->levels; x++)
		{
			fprintf(fd, "%3d: %3d, %3d, %3d\n", region->level[x], region->color[x][0], region->color[x][1], region->color[x][2]);
		}

		fclose(fd);
	}
	else
	{
		x = 0;
		fgets(LossString, 80, fd);

		while ((x < 32) && (feof(fd) == 0))
		{
			pointer = strchr(LossString, ';');

			if (pointer != NULL)
			{
				*pointer = 0;
			}

			ok = sscanf_s(LossString, "%d: %d, %d, %d", &val[0], &val[1], &val[2], &val[3]);

			if (ok == 4)
			{
				for (y = 0; y < 4; y++)
				{
					if (val[y] > 255)
					{
						val[y] = 255;
					}

					if (val[y] < 0)
					{
						val[y] = 0;
					}
				}

				region->level[x] = val[0];
				region->color[x][0] = val[1];
				region->color[x][1] = val[2];
				region->color[x][2] = val[3];
				x++;
			}

			fgets(LossString, 80, fd);
		}

		fclose(fd);
		region->levels = x;
	}
}


//| ------------------------------
//| 
//| FUNCTION: LoadSplatPowerColorsForSite
//| 
//| OLD NAME: LoadDBMColors
//| 
//| NOTES: 
//| 
//| ------------------------------
void
LoadSplatPowerColorsForSite
   (Site xmtr,
	ColorRegion *region)
{
	int x, y, ok, val[4];
	char filename[255], DbmString[80], *pointer = NULL;
	FILE *fd = NULL;
	errno_t err;

	for (x = 0; (xmtr.filename[x] != '.') && (xmtr.filename[x] != 0) && (x < 250); x++)
	{
		filename[x] = xmtr.filename[x];
	}

	filename[x] = '.';
	filename[x + 1] = 'd';
	filename[x + 2] = 'c';
	filename[x + 3] = 'f';
	filename[x + 4] = 0;

	//| Default values

	region->level[0] = 0;
	region->color[0][0] = 255;
	region->color[0][1] = 0;
	region->color[0][2] = 0;

	region->level[1] = -10;
	region->color[1][0] = 255;
	region->color[1][1] = 128;
	region->color[1][2] = 0;

	region->level[2] = -20;
	region->color[2][0] = 255;
	region->color[2][1] = 165;
	region->color[2][2] = 0;

	region->level[3] = -30;
	region->color[3][0] = 255;
	region->color[3][1] = 206;
	region->color[3][2] = 0;

	region->level[4] = -40;
	region->color[4][0] = 255;
	region->color[4][1] = 255;
	region->color[4][2] = 0;

	region->level[5] = -50;
	region->color[5][0] = 184;
	region->color[5][1] = 255;
	region->color[5][2] = 0;

	region->level[6] = -60;
	region->color[6][0] = 0;
	region->color[6][1] = 255;
	region->color[6][2] = 0;

	region->level[7] = -70;
	region->color[7][0] = 0;
	region->color[7][1] = 208;
	region->color[7][2] = 0;

	region->level[8] = -80;
	region->color[8][0] = 0;
	region->color[8][1] = 196;
	region->color[8][2] = 196;

	region->level[9] = -90;
	region->color[9][0] = 0;
	region->color[9][1] = 148;
	region->color[9][2] = 255;

	region->level[10] = -100;
	region->color[10][0] = 80;
	region->color[10][1] = 80;
	region->color[10][2] = 255;

	region->level[11] = -110;
	region->color[11][0] = 0;
	region->color[11][1] = 38;
	region->color[11][2] = 255;

	region->level[12] = -120;
	region->color[12][0] = 142;
	region->color[12][1] = 63;
	region->color[12][2] = 255;

	region->level[13] = -130;
	region->color[13][0] = 196;
	region->color[13][1] = 54;
	region->color[13][2] = 255;

	region->level[14] = -140;
	region->color[14][0] = 255;
	region->color[14][1] = 0;
	region->color[14][2] = 255;

	region->level[15] = -150;
	region->color[15][0] = 255;
	region->color[15][1] = 194;
	region->color[15][2] = 204;

	region->levels = 16;

	err = fopen_s(&fd, "splat.dcf", "r");

	if (fd == NULL)
	{
		err = fopen_s(&fd, filename, "r");
	}

	if (fd == NULL)
	{
		err = fopen_s(&fd, filename, "w");

		fprintf(fd, "; SPLAT Auto-generated DBM Signal Level Color Definition (\"%s\") File\n", filename);
		fprintf(fd, ";\n; Format for the parameters held in this file is as follows:\n;\n");
		fprintf(fd, ";    dBm: red, green, blue\n;\n");
		fprintf(fd, "; ...where \"dBm\" is the received signal power level between +40 dBm\n");
		fprintf(fd, "; and -200 dBm, and \"red\", \"green\", and \"blue\" are the corresponding\n");
		fprintf(fd, "; RGB color definitions ranging from 0 to 255 for the region specified.\n");
		fprintf(fd, ";\n; The following parameters may be edited and/or expanded\n");
		fprintf(fd, "; for future runs of SPLAT.  A total of 32 contour regions\n");
		fprintf(fd, "; may be defined in this file.\n;\n;\n");

		for (x = 0; x < region->levels; x++)
		{
			fprintf(fd, "%+4d: %3d, %3d, %3d\n", region->level[x], region->color[x][0], region->color[x][1], region->color[x][2]);
		}

		fclose(fd);
	}
	else
	{
		x = 0;
		fgets(DbmString, 80, fd);

		while ((x < 32) && (feof(fd) == 0))
		{
			pointer = strchr(DbmString, ';');

			if (pointer != NULL)
			{
				*pointer = 0;
			}

			ok = sscanf_s(DbmString, "%d: %d, %d, %d", &val[0], &val[1], &val[2], &val[3]);

			if (ok == 4)
			{
				if (val[0] < -200)
				{
					val[0] = -200;
				}

				if (val[0] > +40)
				{
					val[0] = +40;
				}

				region->level[x] = val[0];

				for (y = 1; y < 4; y++)
				{
					if (val[y] > 255)
					{
						val[y] = 255;
					}

					if (val[y] < 0)
					{
						val[y] = 0;
					}
				}

				region->color[x][0] = val[1];
				region->color[x][1] = val[2];
				region->color[x][2] = val[3];
				x++;
			}

			fgets(DbmString, 80, fd);
		}

		fclose(fd);
		region->levels = x;
	}
}


//| ------------------------------
//| 
//| FUNCTION: WritePortablePixMapLineOfSightCoverageFile
//| 
//| OLD NAME: WritePPM
//| 
//| NOTES: 
//|   This function generates a topographic map in Portable Pix Map
//|   (PPM) format based on logarithmically scaled topology data,
//|   as well as the content of flags held in the mask[][] array.
//|   The image created is rotated counter-clockwise 90 degrees
//|   from its representation in digitalElevationModel[][] so that north points
//|   up and east points right in the image generated.
//| 
//| ------------------------------
void
WritePortablePixMapLineOfSightCoverageFile
   (char *filename,
	unsigned char geo,
	unsigned char kml,
	unsigned char ngs,
	Site *xmtr,
	unsigned char txsites,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	int maximumElevation,
	int minimumElevation,
	int minimumLatitudeNorth,
	int maximumLatitudeNorth,
	int minimumLongitudeWest,
	int maximumLongitudeWest)
{
	char mapfile[255], geofile[255], kmlfile[255];
	unsigned char found, mask;
	unsigned width, height, terrain;
	int indx, x, y, x0 = 0, y0 = 0;
	double lat, lon, conversion, one_over_gamma,
		north, south, east, west, minwest;
	FILE *fd;
	errno_t err;

	one_over_gamma = 1.0 / GAMMA;
	conversion = 255.0 / pow((double)(maximumElevation - minimumElevation), one_over_gamma);

	width = (unsigned)(digitalElevationModelWrapper->demPixelsPerDegree * ConvertToNormalizedAngle(maximumLongitudeWest - minimumLongitudeWest));
	height = (unsigned)(digitalElevationModelWrapper->demPixelsPerDegree * ConvertToNormalizedAngle(maximumLatitudeNorth - minimumLatitudeNorth));

	if (filename[0] == 0)
	{
		err = strncpy_s(filename, 255, xmtr[0].filename, 254);
		filename[strlen(filename) - 4] = 0;  //| Remove .qth
	}

	y = (int)strlen(filename);

	if (y > 4)
	{
		if ((filename[y - 1] == 'm') && (filename[y - 2] == 'p') && (filename[y - 3] == 'p') && (filename[y - 4] == '.'))
		{
			y -= 4;
		}
	}

	for (x = 0; x < y; x++)
	{
		mapfile[x] = filename[x];
		geofile[x] = filename[x];
		kmlfile[x] = filename[x];
	}

	mapfile[x] = '.';
	geofile[x] = '.';
	kmlfile[x] = '.';
	mapfile[x + 1] = 'p';
	geofile[x + 1] = 'g';
	kmlfile[x + 1] = 'k';
	mapfile[x + 2] = 'p';
	geofile[x + 2] = 'e';
	kmlfile[x + 2] = 'm';
	mapfile[x + 3] = 'm';
	geofile[x + 3] = 'o';
	kmlfile[x + 3] = 'l';
	mapfile[x + 4] = 0;
	geofile[x + 4] = 0;
	kmlfile[x + 4] = 0;

	minwest = ((double)minimumLongitudeWest) + (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree));

	if (minwest > 360.0)
	{
		minwest -= 360.0;
	}

	north = (double)maximumLatitudeNorth - (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree));
	south = (double)minimumLatitudeNorth;
	east = (minwest < 180.0 ? -minwest : 360.0 - minimumLongitudeWest);
	west = (double)(maximumLongitudeWest < 180 ? -maximumLongitudeWest : 360 - maximumLongitudeWest);

	if ((kml == 0) && geo)
	{
		err = fopen_s(&fd, geofile, "wb");

		fprintf(fd, "FILENAME\t%s\n", mapfile);
		fprintf(fd, "#\t\tX\tY\tLong\t\tLat\n");
		fprintf(fd, "TIEPOINT\t0\t0\t%.3f\t\t%.3f\n", west, north);
		fprintf(fd, "TIEPOINT\t%u\t%u\t%.3f\t\t%.3f\n", width - 1, height - 1, east, south);
		fprintf(fd, "IMAGESIZE\t%u\t%u\n", width, height);
		fprintf(fd, "#\n# Auto Generated by %s v%s\n#\n", SPLAT_NAME, SPLAT_VERSION);

		fclose(fd);
	}

	if (kml && (geo == 0))
	{
		err = fopen_s(&fd, kmlfile, "wb");

		fprintf(fd, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		fprintf(fd, "<kml xmlns=\"http://earth.google.com/kml/2.1\">\n");
		fprintf(fd, "  <Folder>\n");
		fprintf(fd, "   <name>%s</name>\n", SPLAT_NAME);
		fprintf(fd, "     <description>Line-of-Sight Contour</description>\n");
		fprintf(fd, "       <GroundOverlay>\n");
		fprintf(fd, "         <name>%s Line-of-Sight Contour</name>\n", SPLAT_NAME);
		fprintf(fd, "           <description>SPLAT Coverage</description>\n");
		fprintf(fd, "		<Icon>\n");
		fprintf(fd, "              <href>%s</href>\n", mapfile);
		fprintf(fd, "		</Icon>\n");
		//fprintf(fd,"            <opacity>128</opacity>\n");
		fprintf(fd, "            <LatLonBox>\n");
		fprintf(fd, "               <north>%.5f</north>\n", north);
		fprintf(fd, "               <south>%.5f</south>\n", south);
		fprintf(fd, "               <east>%.5f</east>\n", east);
		fprintf(fd, "               <west>%.5f</west>\n", west);
		fprintf(fd, "               <rotation>0.0</rotation>\n");
		fprintf(fd, "            </LatLonBox>\n");
		fprintf(fd, "       </GroundOverlay>\n");

		for (x = 0; x < txsites; x++)
		{
			fprintf(fd, "     <Placemark>\n");
			fprintf(fd, "       <name>%s</name>\n", xmtr[x].name);
			fprintf(fd, "       <visibility>1</visibility>\n");
			fprintf(fd, "       <Style>\n");
			fprintf(fd, "       <IconStyle>\n");
			fprintf(fd, "        <Icon>\n");
			fprintf(fd, "          <href>root://icons/palette-5.png</href>\n");
			fprintf(fd, "          <x>224</x>\n");
			fprintf(fd, "          <y>224</y>\n");
			fprintf(fd, "          <w>32</w>\n");
			fprintf(fd, "          <h>32</h>\n");
			fprintf(fd, "        </Icon>\n");
			fprintf(fd, "       </IconStyle>\n");
			fprintf(fd, "       </Style>\n");
			fprintf(fd, "      <Point>\n");
			fprintf(fd, "        <extrude>1</extrude>\n");
			fprintf(fd, "        <altitudeMode>relativeToGround</altitudeMode>\n");
			fprintf(fd, "        <coordinates>%f,%f,%f</coordinates>\n", (xmtr[x].lon < 180.0 ? -xmtr[x].lon : 360.0 - xmtr[x].lon), xmtr[x].lat, xmtr[x].alt);
			fprintf(fd, "      </Point>\n");
			fprintf(fd, "     </Placemark>\n");
		}

		fprintf(fd, "  </Folder>\n");
		fprintf(fd, "</kml>\n");

		fclose(fd);
	}

	err = fopen_s(&fd, mapfile, "wb");

	fprintf(fd, "P6\n%u %u\n255\n", width, height);
	fprintf(stdout, "\nWriting \"%s\" (%ux%u pixmap image)... ", mapfile, width, height);
	fflush(stdout);

	for (y = 0, lat = north; y < (int)height; y++, lat = (north - ((1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree))*(double)y)))
	{
		for (x = 0, lon = maximumLongitudeWest; x < (int)width; x++, lon = ((double)maximumLongitudeWest - ((1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree))*(double)x)))
		{
			if (lon < 0.0)
			{
				lon += 360.0;
			}

			for (indx = 0, found = 0; (indx < digitalElevationModelWrapper->maxPages) && (found == 0);)
			{
				x0 = (int)rint(((double)digitalElevationModelWrapper->demPixelsPerDegree)*(lat - (double)digitalElevationModelWrapper->digitalElevationModel[indx].min_north));
				y0 = (digitalElevationModelWrapper->demPixelsPerDegree - 1) - (int)rint(((double)digitalElevationModelWrapper->demPixelsPerDegree) * LongitudeDifference((double)digitalElevationModelWrapper->digitalElevationModel[indx].max_west, lon));

				if ((x0 >= 0) && (x0 <= (digitalElevationModelWrapper->demPixelsPerDegree - 1)) && (y0 >= 0) && (y0 <= (digitalElevationModelWrapper->demPixelsPerDegree - 1)))
				{
					found = 1;
				}
				else
				{
					indx++;
				}
			}

			if (found)
			{
				mask = digitalElevationModelWrapper->digitalElevationModel[indx].mask[x0][y0];

				if (mask & 2)
				{
					//| Text Labels: Red
					fprintf(fd, "%c%c%c", 255, 0, 0);
				}
				else if (mask & 4)
				{
					//| County Boundaries: Light Cyan
					fprintf(fd, "%c%c%c", 128, 128, 255);
				}
				else
				{
					switch (mask & 57)
					{
					case 1:
						//| TX1: Green
						fprintf(fd, "%c%c%c", 0, 255, 0);
						break;

					case 8:
						//| TX2: Cyan
						fprintf(fd, "%c%c%c", 0, 255, 255);
						break;

					case 9:
						//| TX1 + TX2: Yellow
						fprintf(fd, "%c%c%c", 255, 255, 0);
						break;

					case 16:
						//| TX3: Medium Violet
						fprintf(fd, "%c%c%c", 147, 112, 219);
						break;

					case 17:
						//| TX1 + TX3: Pink
						fprintf(fd, "%c%c%c", 255, 192, 203);
						break;

					case 24:
						//| TX2 + TX3: Orange
						fprintf(fd, "%c%c%c", 255, 165, 0);
						break;

					case 25:
						//| TX1 + TX2 + TX3: Dark Green
						fprintf(fd, "%c%c%c", 0, 100, 0);
						break;

					case 32:
						//| TX4: Sienna 1
						fprintf(fd, "%c%c%c", 255, 130, 71);
						break;

					case 33:
						//| TX1 + TX4: Green Yellow
						fprintf(fd, "%c%c%c", 173, 255, 47);
						break;

					case 40:
						//| TX2 + TX4: Dark Sea Green 1
						fprintf(fd, "%c%c%c", 193, 255, 193);
						break;

					case 41:
						//| TX1 + TX2 + TX4: Blanched Almond
						fprintf(fd, "%c%c%c", 255, 235, 205);
						break;

					case 48:
						//| TX3 + TX4: Dark Turquoise
						fprintf(fd, "%c%c%c", 0, 206, 209);
						break;

					case 49:
						//| TX1 + TX3 + TX4: Medium Spring Green
						fprintf(fd, "%c%c%c", 0, 250, 154);
						break;

					case 56:
						//| TX2 + TX3 + TX4: Tan
						fprintf(fd, "%c%c%c", 210, 180, 140);
						break;

					case 57:
						//| TX1 + TX2 + TX3 + TX4: Gold2
						fprintf(fd, "%c%c%c", 238, 201, 0);
						break;

					default:
						if (ngs)  //| No terrain
						{
							fprintf(fd, "%c%c%c", 255, 255, 255);
						}
						else
						{
							//| Sea-level: Medium Blue
							if (digitalElevationModelWrapper->digitalElevationModel[indx].data[x0][y0] == 0)
							{
								fprintf(fd, "%c%c%c", 0, 0, 170);
							}
							else
							{
								//| Elevation: Greyscale
								terrain = (unsigned)(0.5 + pow((double)(digitalElevationModelWrapper->digitalElevationModel[indx].data[x0][y0] - minimumElevation), one_over_gamma)*conversion);
								fprintf(fd, "%c%c%c", terrain, terrain, terrain);
							}
						}
					}
				}
			}
			else
			{
				//| We should never get here, but if
				//| we do, display the region as black

				fprintf(fd, "%c%c%c", 0, 0, 0);
			}
		}
	}

	fclose(fd);
	fprintf(stdout, "Done\n");
	fflush(stdout);
}


//| ------------------------------
//| 
//| FUNCTION: WritePortablePixMapAttenuationFile
//| 
//| OLD NAME: WritePPMLR
//| 
//| NOTES: 
//|   This function generates a topographic map in Portable Pix Map
//|   (PPM) format based on the content of flags held in the mask[][]
//|   array (only). The image created is rotated counter-clockwise
//|   90 degrees from its representation in digitalElevationModel[][] so that north
//|   points up and east points right in the image generated.
//| 
//| ------------------------------
void
WritePortablePixMapAttenuationFile
   (char *filename,
	unsigned char geo,
	unsigned char kml,
	unsigned char ngs,
	Site *xmtr,
	unsigned char txsites,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	ColorRegion *region,
	unsigned char displaySmoothContourLevels,
	int contourDisplayThreshold,
	int maximumElevation,
	int minimumElevation,
	int minimumLatitudeNorth,
	int maximumLatitudeNorth,
	int minimumLongitudeWest,
	int maximumLongitudeWest)
{
	char mapfile[255], geofile[255], kmlfile[255], ckfile[255];
	unsigned width, height, red, green, blue, terrain = 0;
	unsigned char found, mask, cityorcounty;
	int indx, x, y, z, colorwidth, x0, y0, loss, level,
		hundreds, tens, units, match;
	double lat, lon, conversion, one_over_gamma,
		north, south, east, west, minwest;
	FILE *fd;
	errno_t err;

	one_over_gamma = 1.0 / GAMMA;
	conversion = 255.0 / pow((double)(maximumElevation - minimumElevation), one_over_gamma);

	width = (unsigned)(digitalElevationModelWrapper->demPixelsPerDegree * ConvertToNormalizedAngle(maximumLongitudeWest - minimumLongitudeWest));
	height = (unsigned)(digitalElevationModelWrapper->demPixelsPerDegree * ConvertToNormalizedAngle(maximumLatitudeNorth - minimumLatitudeNorth));

	LoadSplatLossColorsForSite(xmtr[0], region);

	if (filename[0] == 0)
	{
		err = strncpy_s(filename, 255, xmtr[0].filename, 254);
		filename[strlen(filename) - 4] = 0;  //| Remove .qth
	}

	y = (int)strlen(filename);

	if (y > 240)
	{
		y = 240;
	}

	if (y > 4)
	{
		if ((filename[y - 1] == 'm') && (filename[y - 2] == 'p') && (filename[y - 3] == 'p') && (filename[y - 4] == '.'))
		{
			y -= 4;
		}
	}

	for (x = 0; x < y; x++)
	{
		mapfile[x] = filename[x];
		geofile[x] = filename[x];
		kmlfile[x] = filename[x];
		ckfile[x] = filename[x];
	}

	mapfile[x] = '.';
	geofile[x] = '.';
	kmlfile[x] = '.';
	mapfile[x + 1] = 'p';
	geofile[x + 1] = 'g';
	kmlfile[x + 1] = 'k';
	mapfile[x + 2] = 'p';
	geofile[x + 2] = 'e';
	kmlfile[x + 2] = 'm';
	mapfile[x + 3] = 'm';
	geofile[x + 3] = 'o';
	kmlfile[x + 3] = 'l';
	mapfile[x + 4] = 0;
	geofile[x + 4] = 0;
	kmlfile[x + 4] = 0;

	ckfile[x] = '-';
	ckfile[x + 1] = 'c';
	ckfile[x + 2] = 'k';
	ckfile[x + 3] = '.';
	ckfile[x + 4] = 'p';
	ckfile[x + 5] = 'p';
	ckfile[x + 6] = 'm';
	ckfile[x + 7] = 0;

	minwest = ((double)minimumLongitudeWest) + (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree));

	if (minwest > 360.0)
	{
		minwest -= 360.0;
	}

	north = (double)maximumLatitudeNorth - (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree));

	if (kml || geo)
	{
		south = (double)minimumLatitudeNorth;	//| No bottom legend
	}
	else
	{
		south = (double)minimumLatitudeNorth - (30.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree)); //| 30 pixels for bottom legend
	}

	east = (minwest < 180.0 ? -minwest : 360.0 - minimumLongitudeWest);
	west = (double)(maximumLongitudeWest < 180 ? -maximumLongitudeWest : 360 - maximumLongitudeWest);

	if ((kml == 0) && geo)
	{
		err = fopen_s(&fd, geofile, "wb");

		fprintf(fd, "FILENAME\t%s\n", mapfile);
		fprintf(fd, "#\t\tX\tY\tLong\t\tLat\n");
		fprintf(fd, "TIEPOINT\t0\t0\t%.3f\t\t%.3f\n", west, north);

		fprintf(fd, "TIEPOINT\t%u\t%u\t%.3f\t\t%.3f\n", width - 1, height - 1, east, south);
		fprintf(fd, "IMAGESIZE\t%u\t%u\n", width, height);

		fprintf(fd, "#\n# Auto Generated by %s v%s\n#\n", SPLAT_NAME, SPLAT_VERSION);

		fclose(fd);
	}

	if (kml && (geo == 0))
	{
		err = fopen_s(&fd, kmlfile, "wb");

		fprintf(fd, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		fprintf(fd, "<kml xmlns=\"http://earth.google.com/kml/2.1\">\n");
		fprintf(fd, "<!-- Generated by %s Version %s -->\n", SPLAT_NAME, SPLAT_VERSION);
		fprintf(fd, "  <Folder>\n");
		fprintf(fd, "   <name>%s</name>\n", SPLAT_NAME);
		fprintf(fd, "     <description>%s Transmitter Path Loss Overlay</description>\n", xmtr[0].name);
		fprintf(fd, "       <GroundOverlay>\n");
		fprintf(fd, "         <name>SPLAT Path Loss Overlay</name>\n");
		fprintf(fd, "           <description>SPLAT Coverage</description>\n");
		fprintf(fd, "		<Icon>\n");
		fprintf(fd, "              <href>%s</href>\n", mapfile);
		fprintf(fd, "		</Icon>\n");
		//fprintf(fd,"            <opacity>128</opacity>\n");
		fprintf(fd, "            <LatLonBox>\n");
		fprintf(fd, "               <north>%.5f</north>\n", north);
		fprintf(fd, "               <south>%.5f</south>\n", south);
		fprintf(fd, "               <east>%.5f</east>\n", east);
		fprintf(fd, "               <west>%.5f</west>\n", west);
		fprintf(fd, "               <rotation>0.0</rotation>\n");
		fprintf(fd, "            </LatLonBox>\n");
		fprintf(fd, "       </GroundOverlay>\n");
		fprintf(fd, "       <ScreenOverlay>\n");
		fprintf(fd, "          <name>Color Key</name>\n");
		fprintf(fd, "		<description>Contour Color Key</description>\n");
		fprintf(fd, "          <Icon>\n");
		fprintf(fd, "            <href>%s</href>\n", ckfile);
		fprintf(fd, "          </Icon>\n");
		fprintf(fd, "          <overlayXY x=\"0\" y=\"1\" xunits=\"fraction\" yunits=\"fraction\"/>\n");
		fprintf(fd, "          <screenXY x=\"0\" y=\"1\" xunits=\"fraction\" yunits=\"fraction\"/>\n");
		fprintf(fd, "          <rotationXY x=\"0\" y=\"0\" xunits=\"fraction\" yunits=\"fraction\"/>\n");
		fprintf(fd, "          <size x=\"0\" y=\"0\" xunits=\"fraction\" yunits=\"fraction\"/>\n");
		fprintf(fd, "       </ScreenOverlay>\n");

		for (x = 0; x < txsites; x++)
		{
			fprintf(fd, "     <Placemark>\n");
			fprintf(fd, "       <name>%s</name>\n", xmtr[x].name);
			fprintf(fd, "       <visibility>1</visibility>\n");
			fprintf(fd, "       <Style>\n");
			fprintf(fd, "       <IconStyle>\n");
			fprintf(fd, "        <Icon>\n");
			fprintf(fd, "          <href>root://icons/palette-5.png</href>\n");
			fprintf(fd, "          <x>224</x>\n");
			fprintf(fd, "          <y>224</y>\n");
			fprintf(fd, "          <w>32</w>\n");
			fprintf(fd, "          <h>32</h>\n");
			fprintf(fd, "        </Icon>\n");
			fprintf(fd, "       </IconStyle>\n");
			fprintf(fd, "       </Style>\n");
			fprintf(fd, "      <Point>\n");
			fprintf(fd, "        <extrude>1</extrude>\n");
			fprintf(fd, "        <altitudeMode>relativeToGround</altitudeMode>\n");
			fprintf(fd, "        <coordinates>%f,%f,%f</coordinates>\n", (xmtr[x].lon < 180.0 ? -xmtr[x].lon : 360.0 - xmtr[x].lon), xmtr[x].lat, xmtr[x].alt);
			fprintf(fd, "      </Point>\n");
			fprintf(fd, "     </Placemark>\n");
		}

		fprintf(fd, "  </Folder>\n");
		fprintf(fd, "</kml>\n");

		fclose(fd);
	}

	err = fopen_s(&fd, mapfile, "wb");

	if (kml || geo)
	{
		//| No bottom legend

		fprintf(fd, "P6\n%u %u\n255\n", width, height);
		fprintf(stdout, "\nWriting \"%s\" (%ux%u pixmap image)... ", mapfile, width, height);
	}
	else
	{
		//| Allow space for bottom legend

		fprintf(fd, "P6\n%u %u\n255\n", width, height + 30);
		fprintf(stdout, "\nWriting \"%s\" (%ux%u pixmap image)... ", mapfile, width, height + 30);
	}

	fflush(stdout);

	for (y = 0, lat = north; y < (int)height; y++, lat = (north - ((1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree))*(double)y)))
	{
		for (x = 0, lon = maximumLongitudeWest; x < (int)width; x++, lon = (maximumLongitudeWest - ((1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree))*(double)x)))
		{
			if (lon < 0.0)
			{
				lon += 360.0;
			}

			for (indx = 0, found = 0; (indx < digitalElevationModelWrapper->maxPages) && (found == 0);)
			{
				x0 = (int)rint(((double)digitalElevationModelWrapper->demPixelsPerDegree)*(lat - (double)digitalElevationModelWrapper->digitalElevationModel[indx].min_north));
				y0 = (digitalElevationModelWrapper->demPixelsPerDegree - 1) - (int)rint(((double)digitalElevationModelWrapper->demPixelsPerDegree) * LongitudeDifference((double)digitalElevationModelWrapper->digitalElevationModel[indx].max_west, lon));

				if ((x0 >= 0) && (x0 <= (digitalElevationModelWrapper->demPixelsPerDegree - 1)) && (y0 >= 0) && (y0 <= (digitalElevationModelWrapper->demPixelsPerDegree - 1)))
				{
					found = 1;
				}
				else
				{
					indx++;
				}
			}

			if (found)
			{
				mask = digitalElevationModelWrapper->digitalElevationModel[indx].mask[x0][y0];
				loss = digitalElevationModelWrapper->digitalElevationModel[indx].signal[x0][y0];
				cityorcounty = 0;

				match = 255;

				red = 0;
				green = 0;
				blue = 0;

				if (loss <= region->level[0])
				{
					match = 0;
				}
				else
				{
					for (z = 1; (z < region->levels) && (match == 255); z++)
					{
						if ((loss >= region->level[z - 1]) && (loss < region->level[z]))
						{
							match = z;
						}
					}
				}

				if (match < region->levels)
				{
					if (displaySmoothContourLevels && (match > 0))
					{
						red = (unsigned)LinearInterpolation(region->color[match - 1][0], region->color[match][0], region->level[match - 1], region->level[match], loss);
						green = (unsigned)LinearInterpolation(region->color[match - 1][1], region->color[match][1], region->level[match - 1], region->level[match], loss);
						blue = (unsigned)LinearInterpolation(region->color[match - 1][2], region->color[match][2], region->level[match - 1], region->level[match], loss);
					}
					else
					{
						red = region->color[match][0];
						green = region->color[match][1];
						blue = region->color[match][2];
					}
				}

				if (mask & 2)
				{
					//| Text Labels: Red or otherwise

					if ((red >= 180) && (green <= 75) && (blue <= 75) && (loss != 0))
					{
						fprintf(fd, "%c%c%c", 255 ^ red, 255 ^ green, 255 ^ blue);
					}
					else
					{
						fprintf(fd, "%c%c%c", 255, 0, 0);
					}
					cityorcounty = 1;
				}
				else if (mask & 4)
				{
					//| County Boundaries: Black

					fprintf(fd, "%c%c%c", 0, 0, 0);

					cityorcounty = 1;
				}

				if (cityorcounty == 0)
				{
					if ((loss == 0) || ((contourDisplayThreshold != 0) && (loss > abs(contourDisplayThreshold))))
					{
						if (ngs)  //| No terrain
						{
							fprintf(fd, "%c%c%c", 255, 255, 255);
						}
						else
						{
							//| Display land or sea elevation

							if (digitalElevationModelWrapper->digitalElevationModel[indx].data[x0][y0] == 0)
							{
								fprintf(fd, "%c%c%c", 0, 0, 170);
							}
							else
							{
								terrain = (unsigned)(0.5 + pow((double)(digitalElevationModelWrapper->digitalElevationModel[indx].data[x0][y0] - minimumElevation), one_over_gamma)*conversion);
								fprintf(fd, "%c%c%c", terrain, terrain, terrain);
							}
						}
					}
					else
					{
						//| Plot path loss in color

						if ((red != 0) || (green != 0) || (blue != 0))
						{
							fprintf(fd, "%c%c%c", red, green, blue);
						}
						else  //| terrain / sea-level
						{
							if (digitalElevationModelWrapper->digitalElevationModel[indx].data[x0][y0] == 0)
							{
								fprintf(fd, "%c%c%c", 0, 0, 170);
							}
							else
							{
								//| Elevation: Greyscale
								terrain = (unsigned)(0.5 + pow((double)(digitalElevationModelWrapper->digitalElevationModel[indx].data[x0][y0] - minimumElevation), one_over_gamma)*conversion);
								fprintf(fd, "%c%c%c", terrain, terrain, terrain);
							}
						}
					}
				}
			}
			else
			{
				//| We should never get here, but if
				//| we do, display the region as black

				fprintf(fd, "%c%c%c", 0, 0, 0);
			}
		}
	}

	if ((kml == 0) && (geo == 0))
	{
		//| Display legend along bottom of image
		//| if not generating .kml or .geo output.

		colorwidth = (int)rint((float)width / (float)region->levels);

		for (y0 = 0; y0 < 30; y0++)
		{
			for (x0 = 0; x0 < (int)width; x0++)
			{
				indx = x0 / colorwidth;
				x = x0 % colorwidth;
				level = region->level[indx];

				hundreds = level / 100;

				if (hundreds > 0)
				{
					level -= (hundreds * 100);
				}

				tens = level / 10;

				if (tens > 0)
				{
					level -= (tens * 10);
				}

				units = level;

				if ((y0 >= 8) && (y0 <= 23))
				{
					if (hundreds > 0)
					{
						if ((x >= 11) && (x <= 18))
						{
							if (fontdata[16 * (hundreds + '0') + (y0 - 8)] & (128 >> (x - 11)))
							{
								indx = 255;
							}
						}
					}

					if ((tens > 0) || (hundreds > 0))
					{
						if ((x >= 19) && (x <= 26))
						{
							if (fontdata[16 * (tens + '0') + (y0 - 8)] & (128 >> (x - 19)))
							{
								indx = 255;
							}
						}
					}

					if ((x >= 27) && (x <= 34))
					{
						if (fontdata[16 * (units + '0') + (y0 - 8)] & (128 >> (x - 27)))
						{
							indx = 255;
						}
					}

					if ((x >= 42) && (x <= 49))
					{
						if (fontdata[16 * ('d') + (y0 - 8)] & (128 >> (x - 42)))
						{
							indx = 255;
						}
					}

					if ((x >= 50) && (x <= 57))
					{
						if (fontdata[16 * ('B') + (y0 - 8)] & (128 >> (x - 50)))
						{
							indx = 255;
						}
					}
				}

				if (indx > region->levels)
				{
					fprintf(fd, "%c%c%c", 0, 0, 0);
				}
				else
				{
					red = region->color[indx][0];
					green = region->color[indx][1];
					blue = region->color[indx][2];

					fprintf(fd, "%c%c%c", red, green, blue);
				}
			}
		}
	}

	fclose(fd);

	if (kml)
	{
		//| Write colorkey image file

		err = fopen_s(&fd, ckfile, "wb");

		height = 30 * region->levels;
		width = 100;

		fprintf(fd, "P6\n%u %u\n255\n", width, height);

		for (y0 = 0; y0 < (int)height; y0++)
		{
			for (x0 = 0; x0 < (int)width; x0++)
			{
				indx = y0 / 30;
				x = x0;
				level = region->level[indx];

				hundreds = level / 100;

				if (hundreds > 0)
				{
					level -= (hundreds * 100);
				}

				tens = level / 10;

				if (tens > 0)
				{
					level -= (tens * 10);
				}

				units = level;

				if (((y0 % 30) >= 8) && ((y0 % 30) <= 23))
				{
					if (hundreds > 0)
					{
						if ((x >= 11) && (x <= 18))
						{
							if (fontdata[16 * (hundreds + '0') + ((y0 % 30) - 8)] & (128 >> (x - 11)))
							{
								indx = 255;
							}
						}
					}

					if ((tens > 0) || (hundreds > 0))
					{
						if ((x >= 19) && (x <= 26))
						{
							if (fontdata[16 * (tens + '0') + ((y0 % 30) - 8)] & (128 >> (x - 19)))
							{
								indx = 255;
							}
						}
					}

					if ((x >= 27) && (x <= 34))
					{
						if (fontdata[16 * (units + '0') + ((y0 % 30) - 8)] & (128 >> (x - 27)))
						{
							indx = 255;
						}
					}

					if ((x >= 42) && (x <= 49))
					{
						if (fontdata[16 * ('d') + ((y0 % 30) - 8)] & (128 >> (x - 42)))
						{
							indx = 255;
						}
					}

					if ((x >= 50) && (x <= 57))
					{
						if (fontdata[16 * ('B') + ((y0 % 30) - 8)] & (128 >> (x - 50)))
						{
							indx = 255;
						}
					}
				}

				if (indx > region->levels)
				{
					fprintf(fd, "%c%c%c", 0, 0, 0);
				}
				else
				{
					red = region->color[indx][0];
					green = region->color[indx][1];
					blue = region->color[indx][2];

					fprintf(fd, "%c%c%c", red, green, blue);
				}
			}
		}

		fclose(fd);
	}

	fprintf(stdout, "Done\n");
	fflush(stdout);
}


//| ------------------------------
//| 
//| FUNCTION: WritePortablePixMapSignalStrengthFile
//| 
//| OLD NAME: WritePPMSS
//| 
//| NOTES: 
//|   This function generates a topographic map in Portable Pix Map
//|   (PPM) format based on the signal strength values held in the
//|   signal[][] array. The image created is rotated counter-clockwise
//|   90 degrees from its representation in digitalElevationModel[][] so that north
//|   points up and east points right in the image generated.
//| 
//| ------------------------------
void
WritePortablePixMapSignalStrengthFile
   (char *filename,
	unsigned char geo,
	unsigned char kml,
	unsigned char ngs,
	Site *xmtr,
	unsigned char txsites,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	ColorRegion *region,
	unsigned char displaySmoothContourLevels,
	int contourDisplayThreshold,
	int maximumElevation,
	int minimumElevation,
	int minimumLatitudeNorth,
	int maximumLatitudeNorth,
	int minimumLongitudeWest,
	int maximumLongitudeWest)
{
	char mapfile[255], geofile[255], kmlfile[255], ckfile[255];
	unsigned width, height, terrain, red, green, blue;
	unsigned char found, mask, cityorcounty;
	int indx, x, y, z = 1, x0, y0, signal, level, hundreds,
		tens, units, match, colorwidth;
	double conversion, one_over_gamma, lat, lon,
		north, south, east, west, minwest;
	FILE *fd;
	errno_t err;

	one_over_gamma = 1.0 / GAMMA;
	conversion = 255.0 / pow((double)(maximumElevation - minimumElevation), one_over_gamma);

	width = (unsigned)(digitalElevationModelWrapper->demPixelsPerDegree * ConvertToNormalizedAngle(maximumLongitudeWest - minimumLongitudeWest));
	height = (unsigned)(digitalElevationModelWrapper->demPixelsPerDegree * ConvertToNormalizedAngle(maximumLatitudeNorth - minimumLatitudeNorth));

	LoadSplatSignalColorsForSite(xmtr[0], region);

	if (filename[0] == 0)
	{
		err = strncpy_s(filename, 255, xmtr[0].filename, 254);
		filename[strlen(filename) - 4] = 0;  //| Remove .qth
	}

	y = (int)strlen(filename);

	if (y > 240)
	{
		y = 240;
	}

	if (y > 4)
	{
		if ((filename[y - 1] == 'm') && (filename[y - 2] == 'p') && (filename[y - 3] == 'p') && (filename[y - 4] == '.'))
		{
			y -= 4;
		}
	}

	for (x = 0; x < y; x++)
	{
		mapfile[x] = filename[x];
		geofile[x] = filename[x];
		kmlfile[x] = filename[x];
		ckfile[x] = filename[x];
	}

	mapfile[x] = '.';
	geofile[x] = '.';
	kmlfile[x] = '.';
	mapfile[x + 1] = 'p';
	geofile[x + 1] = 'g';
	kmlfile[x + 1] = 'k';
	mapfile[x + 2] = 'p';
	geofile[x + 2] = 'e';
	kmlfile[x + 2] = 'm';
	mapfile[x + 3] = 'm';
	geofile[x + 3] = 'o';
	kmlfile[x + 3] = 'l';
	mapfile[x + 4] = 0;
	geofile[x + 4] = 0;
	kmlfile[x + 4] = 0;

	ckfile[x] = '-';
	ckfile[x + 1] = 'c';
	ckfile[x + 2] = 'k';
	ckfile[x + 3] = '.';
	ckfile[x + 4] = 'p';
	ckfile[x + 5] = 'p';
	ckfile[x + 6] = 'm';
	ckfile[x + 7] = 0;

	minwest = ((double)minimumLongitudeWest) + (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree));

	if (minwest > 360.0)
	{
		minwest -= 360.0;
	}

	north = (double)maximumLatitudeNorth - (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree));

	if (kml || geo)
	{
		south = (double)minimumLatitudeNorth;	//| No bottom legend
	}
	else
	{
		south = (double)minimumLatitudeNorth - (30.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree));	//| 30 pixels for bottom legend
	}

	east = (minwest<180.0 ? -minwest : 360.0 - minimumLongitudeWest);
	west = (double)(maximumLongitudeWest < 180 ? -maximumLongitudeWest : 360 - maximumLongitudeWest);

	if (geo && (kml == 0))
	{
		err = fopen_s(&fd, geofile, "wb");

		fprintf(fd, "FILENAME\t%s\n", mapfile);
		fprintf(fd, "#\t\tX\tY\tLong\t\tLat\n");
		fprintf(fd, "TIEPOINT\t0\t0\t%.3f\t\t%.3f\n", west, north);

		fprintf(fd, "TIEPOINT\t%u\t%u\t%.3f\t\t%.3f\n", width - 1, height - 1, east, south);
		fprintf(fd, "IMAGESIZE\t%u\t%u\n", width, height);

		fprintf(fd, "#\n# Auto Generated by %s v%s\n#\n", SPLAT_NAME, SPLAT_VERSION);

		fclose(fd);
	}

	if (kml && (geo == 0))
	{
		err = fopen_s(&fd, kmlfile, "wb");

		fprintf(fd, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		fprintf(fd, "<kml xmlns=\"http://earth.google.com/kml/2.1\">\n");
		fprintf(fd, "<!-- Generated by %s Version %s -->\n", SPLAT_NAME, SPLAT_VERSION);
		fprintf(fd, "  <Folder>\n");
		fprintf(fd, "   <name>%s</name>\n", SPLAT_NAME);
		fprintf(fd, "     <description>%s Transmitter Contours</description>\n", xmtr[0].name);
		fprintf(fd, "       <GroundOverlay>\n");
		fprintf(fd, "         <name>SPLAT Signal Strength Contours</name>\n");
		fprintf(fd, "           <description>SPLAT Coverage</description>\n");
		fprintf(fd, "		<Icon>\n");
		fprintf(fd, "              <href>%s</href>\n", mapfile);
		fprintf(fd, "		</Icon>\n");
		//fprintf(fd,"            <opacity>128</opacity>\n");
		fprintf(fd, "            <LatLonBox>\n");
		fprintf(fd, "               <north>%.5f</north>\n", north);
		fprintf(fd, "               <south>%.5f</south>\n", south);
		fprintf(fd, "               <east>%.5f</east>\n", east);
		fprintf(fd, "               <west>%.5f</west>\n", west);
		fprintf(fd, "               <rotation>0.0</rotation>\n");
		fprintf(fd, "            </LatLonBox>\n");
		fprintf(fd, "       </GroundOverlay>\n");
		fprintf(fd, "       <ScreenOverlay>\n");
		fprintf(fd, "          <name>Color Key</name>\n");
		fprintf(fd, "            <description>Contour Color Key</description>\n");
		fprintf(fd, "          <Icon>\n");
		fprintf(fd, "            <href>%s</href>\n", ckfile);
		fprintf(fd, "          </Icon>\n");
		fprintf(fd, "          <overlayXY x=\"0\" y=\"1\" xunits=\"fraction\" yunits=\"fraction\"/>\n");
		fprintf(fd, "          <screenXY x=\"0\" y=\"1\" xunits=\"fraction\" yunits=\"fraction\"/>\n");
		fprintf(fd, "          <rotationXY x=\"0\" y=\"0\" xunits=\"fraction\" yunits=\"fraction\"/>\n");
		fprintf(fd, "          <size x=\"0\" y=\"0\" xunits=\"fraction\" yunits=\"fraction\"/>\n");
		fprintf(fd, "       </ScreenOverlay>\n");

		for (x = 0; x < txsites; x++)
		{
			fprintf(fd, "     <Placemark>\n");
			fprintf(fd, "       <name>%s</name>\n", xmtr[x].name);
			fprintf(fd, "       <visibility>1</visibility>\n");
			fprintf(fd, "       <Style>\n");
			fprintf(fd, "       <IconStyle>\n");
			fprintf(fd, "        <Icon>\n");
			fprintf(fd, "          <href>root://icons/palette-5.png</href>\n");
			fprintf(fd, "          <x>224</x>\n");
			fprintf(fd, "          <y>224</y>\n");
			fprintf(fd, "          <w>32</w>\n");
			fprintf(fd, "          <h>32</h>\n");
			fprintf(fd, "        </Icon>\n");
			fprintf(fd, "       </IconStyle>\n");
			fprintf(fd, "       </Style>\n");
			fprintf(fd, "      <Point>\n");
			fprintf(fd, "        <extrude>1</extrude>\n");
			fprintf(fd, "        <altitudeMode>relativeToGround</altitudeMode>\n");
			fprintf(fd, "        <coordinates>%f,%f,%f</coordinates>\n", (xmtr[x].lon<180.0 ? -xmtr[x].lon : 360.0 - xmtr[x].lon), xmtr[x].lat, xmtr[x].alt);
			fprintf(fd, "      </Point>\n");
			fprintf(fd, "     </Placemark>\n");
		}

		fprintf(fd, "  </Folder>\n");
		fprintf(fd, "</kml>\n");

		fclose(fd);
	}

	err = fopen_s(&fd, mapfile, "wb");

	if (kml || geo)
	{
		//| No bottom legend

		fprintf(fd, "P6\n%u %u\n255\n", width, height);
		fprintf(stdout, "\nWriting \"%s\" (%ux%u pixmap image)... ", mapfile, width, height);
	}
	else
	{
		//| Allow space for bottom legend

		fprintf(fd, "P6\n%u %u\n255\n", width, height + 30);
		fprintf(stdout, "\nWriting \"%s\" (%ux%u pixmap image)... ", mapfile, width, height + 30);
	}

	fflush(stdout);

	for (y = 0, lat = north; y < (int)height; y++, lat = (north - ((1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree))*(double)y)))
	{
		for (x = 0, lon = maximumLongitudeWest; x < (int)width; x++, lon = (maximumLongitudeWest - ((1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree))*(double)x)))
		{
			if (lon < 0.0)
			{
				lon += 360.0;
			}

			for (indx = 0, found = 0; (indx < digitalElevationModelWrapper->maxPages) && (found == 0);)
			{
				x0 = (int)rint(((double)digitalElevationModelWrapper->demPixelsPerDegree)*(lat - (double)digitalElevationModelWrapper->digitalElevationModel[indx].min_north));
				y0 = (digitalElevationModelWrapper->demPixelsPerDegree - 1) - (int)rint(((double)digitalElevationModelWrapper->demPixelsPerDegree) * LongitudeDifference((double)digitalElevationModelWrapper->digitalElevationModel[indx].max_west, lon));

				if ((x0 >= 0) && (x0 <= (digitalElevationModelWrapper->demPixelsPerDegree - 1)) && (y0 >= 0) && (y0 <= (digitalElevationModelWrapper->demPixelsPerDegree - 1)))
				{
					found = 1;
				}
				else
				{
					indx++;
				}
			}

			if (found)
			{
				mask = digitalElevationModelWrapper->digitalElevationModel[indx].mask[x0][y0];
				signal = digitalElevationModelWrapper->digitalElevationModel[indx].signal[x0][y0] - 100;
				cityorcounty = 0;

				match = 255;

				red = 0;
				green = 0;
				blue = 0;

				if (signal >= region->level[0])
				{
					match = 0;
				}
				else
				{
					for (z = 1; (z < region->levels) && (match == 255); z++)
					{
						if ((signal < region->level[z - 1]) && (signal >= region->level[z]))
						{
							match = z;
						}
					}
				}

				if (match < region->levels)
				{
					if (displaySmoothContourLevels && (match > 0))
					{
						red = (unsigned)LinearInterpolation(region->color[match][0], region->color[match - 1][0], region->level[match], region->level[match - 1], signal);
						green = (unsigned)LinearInterpolation(region->color[match][1], region->color[match - 1][1], region->level[match], region->level[match - 1], signal);
						blue = (unsigned)LinearInterpolation(region->color[match][2], region->color[match - 1][2], region->level[match], region->level[match - 1], signal);
					}
					else
					{
						red = region->color[match][0];
						green = region->color[match][1];
						blue = region->color[match][2];
					}
				}

				if (mask & 2)
				{
					//| Text Labels: Red or otherwise

					if ((red >= 180) && (green <= 75) && (blue <= 75))
					{
						fprintf(fd, "%c%c%c", 255 ^ red, 255 ^ green, 255 ^ blue);
					}
					else
					{
						fprintf(fd, "%c%c%c", 255, 0, 0);
					}

					cityorcounty = 1;
				}
				else if (mask & 4)
				{
					//| County Boundaries: Black

					fprintf(fd, "%c%c%c", 0, 0, 0);

					cityorcounty = 1;
				}

				if (cityorcounty == 0)
				{
					if ((contourDisplayThreshold != 0) && (signal < contourDisplayThreshold))
					{
						if (ngs)
						{
							fprintf(fd, "%c%c%c", 255, 255, 255);
						}
						else
						{
							//| Display land or sea elevation

							if (digitalElevationModelWrapper->digitalElevationModel[indx].data[x0][y0] == 0)
							{
								fprintf(fd, "%c%c%c", 0, 0, 170);
							}
							else
							{
								terrain = (unsigned)(0.5 + pow((double)(digitalElevationModelWrapper->digitalElevationModel[indx].data[x0][y0] - minimumElevation), one_over_gamma)*conversion);
								fprintf(fd, "%c%c%c", terrain, terrain, terrain);
							}
						}
					}
					else
					{
						//| Plot field strength regions in color

						if ((red != 0) || (green != 0) || (blue != 0))
						{
							fprintf(fd, "%c%c%c", red, green, blue);
						}
						else  //| terrain / sea-level
						{
							if (ngs)
							{
								fprintf(fd, "%c%c%c", 255, 255, 255);
							}
							else
							{
								if (digitalElevationModelWrapper->digitalElevationModel[indx].data[x0][y0] == 0)
								{
									fprintf(fd, "%c%c%c", 0, 0, 170);
								}
								else
								{
									//| Elevation: Greyscale
									terrain = (unsigned)(0.5 + pow((double)(digitalElevationModelWrapper->digitalElevationModel[indx].data[x0][y0] - minimumElevation), one_over_gamma)*conversion);
									fprintf(fd, "%c%c%c", terrain, terrain, terrain);
								}
							}
						}
					}
				}
			}
			else
			{
				//| We should never get here, but if
				//| we do, display the region as black

				fprintf(fd, "%c%c%c", 0, 0, 0);
			}
		}
	}

	if ((kml == 0) && (geo == 0))
	{
		//| Display legend along bottom of image
		//| if not generating .kml or .geo output.

		colorwidth = (int)rint((float)width / (float)region->levels);

		for (y0 = 0; y0 < 30; y0++)
		{
			for (x0 = 0; x0 < (int)width; x0++)
			{
				indx = x0 / colorwidth;
				x = x0 % colorwidth;
				level = region->level[indx];

				hundreds = level / 100;

				if (hundreds > 0)
				{
					level -= (hundreds * 100);
				}

				tens = level / 10;

				if (tens > 0)
				{
					level -= (tens * 10);
				}

				units = level;

				if ((y0 >= 8) && (y0 <= 23))
				{
					if (hundreds > 0)
					{
						if ((x >= 5) && (x <= 12))
						{
							if (fontdata[16 * (hundreds + '0') + (y0 - 8)] & (128 >> (x - 5)))
							{
								indx = 255;
							}
						}
					}

					if ((tens > 0) || (hundreds > 0))
					{
						if ((x >= 13) && (x <= 20))
						{
							if (fontdata[16 * (tens + '0') + (y0 - 8)] & (128 >> (x - 13)))
							{
								indx = 255;
							}
						}
					}

					if ((x >= 21) && (x <= 28))
					{
						if (fontdata[16 * (units + '0') + (y0 - 8)] & (128 >> (x - 21)))
						{
							indx = 255;
						}
					}

					if ((x >= 36) && (x <= 43))
					{
						if (fontdata[16 * ('d') + (y0 - 8)] & (128 >> (x - 36)))
						{
							indx = 255;
						}
					}

					if ((x >= 44) && (x <= 51))
					{
						if (fontdata[16 * ('B') + (y0 - 8)] & (128 >> (x - 44)))
						{
							indx = 255;
						}
					}

					if ((x >= 52) && (x <= 59))
					{
						if (fontdata[16 * (230) + (y0 - 8)] & (128 >> (x - 52)))
						{
							indx = 255;
						}
					}

					if ((x >= 60) && (x <= 67))
					{
						if (fontdata[16 * ('V') + (y0 - 8)] & (128 >> (x - 60)))
						{
							indx = 255;
						}
					}

					if ((x >= 68) && (x <= 75))
					{
						if (fontdata[16 * ('\\') + (y0 - 8)] & (128 >> (x - 68)))
						{
							indx = 255;
						}
					}

					if ((x >= 76) && (x <= 83))
					{
						if (fontdata[16 * ('m') + (y0 - 8)] & (128 >> (x - 76)))
						{
							indx = 255;
						}
					}
				}

				if (indx > region->levels)
				{
					fprintf(fd, "%c%c%c", 0, 0, 0);
				}
				else
				{
					red = region->color[indx][0];
					green = region->color[indx][1];
					blue = region->color[indx][2];

					fprintf(fd, "%c%c%c", red, green, blue);
				}
			}
		}
	}

	fclose(fd);

	if (kml)
	{
		//| Write colorkey image file

		err = fopen_s(&fd, ckfile, "wb");

		height = 30 * region->levels;
		width = 100;

		fprintf(fd, "P6\n%u %u\n255\n", width, height);

		for (y0 = 0; y0 < (int)height; y0++)
		{
			for (x0 = 0; x0 < (int)width; x0++)
			{
				indx = y0 / 30;
				x = x0;
				level = region->level[indx];

				hundreds = level / 100;

				if (hundreds > 0)
				{
					level -= (hundreds * 100);
				}

				tens = level / 10;

				if (tens > 0)
				{
					level -= (tens * 10);
				}

				units = level;

				if (((y0 % 30) >= 8) && ((y0 % 30) <= 23))
				{
					if (hundreds > 0)
					{
						if ((x >= 5) && (x <= 12))
						{
							if (fontdata[16 * (hundreds + '0') + ((y0 % 30) - 8)] & (128 >> (x - 5)))
							{
								indx = 255;
							}
						}
					}

					if ((tens > 0) || (hundreds > 0))
					{
						if ((x >= 13) && (x <= 20))
						{
							if (fontdata[16 * (tens + '0') + ((y0 % 30) - 8)] & (128 >> (x - 13)))
							{
								indx = 255;
							}
						}
					}

					if ((x >= 21) && (x <= 28))
					{
						if (fontdata[16 * (units + '0') + ((y0 % 30) - 8)] & (128 >> (x - 21)))
						{
							indx = 255;
						}
					}

					if ((x >= 36) && (x <= 43))
					{
						if (fontdata[16 * ('d') + ((y0 % 30) - 8)] & (128 >> (x - 36)))
						{
							indx = 255;
						}
					}

					if ((x >= 44) && (x <= 51))
					{
						if (fontdata[16 * ('B') + ((y0 % 30) - 8)] & (128 >> (x - 44)))
						{
							indx = 255;
						}
					}

					if ((x >= 52) && (x <= 59))
					{
						if (fontdata[16 * (230) + ((y0 % 30) - 8)] & (128 >> (x - 52)))
						{
							indx = 255;
						}
					}

					if ((x >= 60) && (x <= 67))
					{
						if (fontdata[16 * ('V') + ((y0 % 30) - 8)] & (128 >> (x - 60)))
						{
							indx = 255;
						}
					}

					if ((x >= 68) && (x <= 75))
					{
						if (fontdata[16 * ('\\') + ((y0 % 30) - 8)] & (128 >> (x - 68)))
						{
							indx = 255;
						}
					}

					if ((x >= 76) && (x <= 83))
					{
						if (fontdata[16 * ('m') + ((y0 % 30) - 8)] & (128 >> (x - 76)))
						{
							indx = 255;
						}
					}
				}

				if (indx > region->levels)
				{
					fprintf(fd, "%c%c%c", 0, 0, 0);
				}
				else
				{
					red = region->color[indx][0];
					green = region->color[indx][1];
					blue = region->color[indx][2];

					fprintf(fd, "%c%c%c", red, green, blue);
				}
			}
		}

		fclose(fd);
	}

	fprintf(stdout, "Done\n");
	fflush(stdout);
}


//| ------------------------------
//| 
//| FUNCTION: WritePortablePixMapPowerLevelFile
//| 
//| OLD NAME: WritePPMDBM
//| 
//| NOTES: 
//|   This function generates a topographic map in Portable Pix Map
//|   (PPM) format based on the signal power level values held in the
//|   signal[][] array. The image created is rotated counter-clockwise
//|   90 degrees from its representation in digitalElevationModel[][] so that north
//|   points up and east points right in the image generated.
//| 
//| ------------------------------
void
WritePortablePixMapPowerLevelFile
   (char *filename,
	unsigned char geo,
	unsigned char kml,
	unsigned char ngs,
	Site *xmtr,
	unsigned char txsites,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	ColorRegion *region,
	unsigned char displaySmoothContourLevels,
	int contourDisplayThreshold,
	int maximumElevation,
	int minimumElevation,
	int minimumLatitudeNorth,
	int maximumLatitudeNorth,
	int minimumLongitudeWest,
	int maximumLongitudeWest)
{
	char mapfile[255], geofile[255], kmlfile[255], ckfile[255];
	unsigned width, height, terrain, red, green, blue;
	unsigned char found, mask, cityorcounty;
	int indx, x, y, z = 1, x0, y0, dBm, level, hundreds,
		tens, units, match, colorwidth;
	double conversion, one_over_gamma, lat, lon,
		north, south, east, west, minwest;
	FILE *fd;
	errno_t err;

	one_over_gamma = 1.0 / GAMMA;
	conversion = 255.0 / pow((double)(maximumElevation - minimumElevation), one_over_gamma);

	width = (unsigned)(digitalElevationModelWrapper->demPixelsPerDegree * ConvertToNormalizedAngle(maximumLongitudeWest - minimumLongitudeWest));
	height = (unsigned)(digitalElevationModelWrapper->demPixelsPerDegree * ConvertToNormalizedAngle(maximumLatitudeNorth - minimumLatitudeNorth));

	LoadSplatPowerColorsForSite(xmtr[0], region);

	if (filename[0] == 0)
	{
		err = strncpy_s(filename, 255, xmtr[0].filename, 254);
		filename[strlen(filename) - 4] = 0;  //| Remove .qth
	}

	y = (int)strlen(filename);

	if (y > 240)
	{
		y = 240;
	}

	if (y > 4)
	{
		if ((filename[y - 1] == 'm') && (filename[y - 2] == 'p') && (filename[y - 3] == 'p') && (filename[y - 4] == '.'))
		{
			y -= 4;
		}
	}

	for (x = 0; x < y; x++)
	{
		mapfile[x] = filename[x];
		geofile[x] = filename[x];
		kmlfile[x] = filename[x];
		ckfile[x] = filename[x];
	}

	mapfile[x] = '.';
	geofile[x] = '.';
	kmlfile[x] = '.';
	mapfile[x + 1] = 'p';
	geofile[x + 1] = 'g';
	kmlfile[x + 1] = 'k';
	mapfile[x + 2] = 'p';
	geofile[x + 2] = 'e';
	kmlfile[x + 2] = 'm';
	mapfile[x + 3] = 'm';
	geofile[x + 3] = 'o';
	kmlfile[x + 3] = 'l';
	mapfile[x + 4] = 0;
	geofile[x + 4] = 0;
	kmlfile[x + 4] = 0;

	ckfile[x] = '-';
	ckfile[x + 1] = 'c';
	ckfile[x + 2] = 'k';
	ckfile[x + 3] = '.';
	ckfile[x + 4] = 'p';
	ckfile[x + 5] = 'p';
	ckfile[x + 6] = 'm';
	ckfile[x + 7] = 0;

	minwest = ((double)minimumLongitudeWest) + (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree));

	if (minwest > 360.0)
	{
		minwest -= 360.0;
	}

	north = (double)maximumLatitudeNorth - (1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree));

	if (kml || geo)
	{
		south = (double)minimumLatitudeNorth;	//| No bottom legend
	}
	else
	{
		south = (double)minimumLatitudeNorth - (30.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree));	//| 30 pixels for bottom legend
	}

	east = (minwest < 180.0 ? -minwest : 360.0 - minimumLongitudeWest);
	west = (double)(maximumLongitudeWest < 180 ? -maximumLongitudeWest : 360 - maximumLongitudeWest);

	if (geo && (kml == 0))
	{
		err = fopen_s(&fd, geofile, "wb");

		fprintf(fd, "FILENAME\t%s\n", mapfile);
		fprintf(fd, "#\t\tX\tY\tLong\t\tLat\n");
		fprintf(fd, "TIEPOINT\t0\t0\t%.3f\t\t%.3f\n", west, north);

		fprintf(fd, "TIEPOINT\t%u\t%u\t%.3f\t\t%.3f\n", width - 1, height - 1, east, south);
		fprintf(fd, "IMAGESIZE\t%u\t%u\n", width, height);

		fprintf(fd, "#\n# Auto Generated by %s v%s\n#\n", SPLAT_NAME, SPLAT_VERSION);

		fclose(fd);
	}

	if (kml && (geo == 0))
	{
		err = fopen_s(&fd, kmlfile, "wb");

		fprintf(fd, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		fprintf(fd, "<kml xmlns=\"http://earth.google.com/kml/2.1\">\n");
		fprintf(fd, "<!-- Generated by %s Version %s -->\n", SPLAT_NAME, SPLAT_VERSION);
		fprintf(fd, "  <Folder>\n");
		fprintf(fd, "   <name>%s</name>\n", SPLAT_NAME);
		fprintf(fd, "     <description>%s Transmitter Contours</description>\n", xmtr[0].name);
		fprintf(fd, "       <GroundOverlay>\n");
		fprintf(fd, "         <name>SPLAT Signal Power Level Contours</name>\n");
		fprintf(fd, "           <description>SPLAT Coverage</description>\n");
		fprintf(fd, "		<Icon>\n");
		fprintf(fd, "              <href>%s</href>\n", mapfile);
		fprintf(fd, "		</Icon>\n");
		//fprintf(fd,"            <opacity>128</opacity>\n");
		fprintf(fd, "            <LatLonBox>\n");
		fprintf(fd, "               <north>%.5f</north>\n", north);
		fprintf(fd, "               <south>%.5f</south>\n", south);
		fprintf(fd, "               <east>%.5f</east>\n", east);
		fprintf(fd, "               <west>%.5f</west>\n", west);
		fprintf(fd, "               <rotation>0.0</rotation>\n");
		fprintf(fd, "            </LatLonBox>\n");
		fprintf(fd, "       </GroundOverlay>\n");
		fprintf(fd, "       <ScreenOverlay>\n");
		fprintf(fd, "          <name>Color Key</name>\n");
		fprintf(fd, "            <description>Contour Color Key</description>\n");
		fprintf(fd, "          <Icon>\n");
		fprintf(fd, "            <href>%s</href>\n", ckfile);
		fprintf(fd, "          </Icon>\n");
		fprintf(fd, "          <overlayXY x=\"0\" y=\"1\" xunits=\"fraction\" yunits=\"fraction\"/>\n");
		fprintf(fd, "          <screenXY x=\"0\" y=\"1\" xunits=\"fraction\" yunits=\"fraction\"/>\n");
		fprintf(fd, "          <rotationXY x=\"0\" y=\"0\" xunits=\"fraction\" yunits=\"fraction\"/>\n");
		fprintf(fd, "          <size x=\"0\" y=\"0\" xunits=\"fraction\" yunits=\"fraction\"/>\n");
		fprintf(fd, "       </ScreenOverlay>\n");

		for (x = 0; x < txsites; x++)
		{
			fprintf(fd, "     <Placemark>\n");
			fprintf(fd, "       <name>%s</name>\n", xmtr[x].name);
			fprintf(fd, "       <visibility>1</visibility>\n");
			fprintf(fd, "       <Style>\n");
			fprintf(fd, "       <IconStyle>\n");
			fprintf(fd, "        <Icon>\n");
			fprintf(fd, "          <href>root://icons/palette-5.png</href>\n");
			fprintf(fd, "          <x>224</x>\n");
			fprintf(fd, "          <y>224</y>\n");
			fprintf(fd, "          <w>32</w>\n");
			fprintf(fd, "          <h>32</h>\n");
			fprintf(fd, "        </Icon>\n");
			fprintf(fd, "       </IconStyle>\n");
			fprintf(fd, "       </Style>\n");
			fprintf(fd, "      <Point>\n");
			fprintf(fd, "        <extrude>1</extrude>\n");
			fprintf(fd, "        <altitudeMode>relativeToGround</altitudeMode>\n");
			fprintf(fd, "        <coordinates>%f,%f,%f</coordinates>\n", (xmtr[x].lon < 180.0 ? -xmtr[x].lon : 360.0 - xmtr[x].lon), xmtr[x].lat, xmtr[x].alt);
			fprintf(fd, "      </Point>\n");
			fprintf(fd, "     </Placemark>\n");
		}

		fprintf(fd, "  </Folder>\n");
		fprintf(fd, "</kml>\n");

		fclose(fd);
	}

	err = fopen_s(&fd, mapfile, "wb");

	if (kml || geo)
	{
		//| No bottom legend

		fprintf(fd, "P6\n%u %u\n255\n", width, height);
		fprintf(stdout, "\nWriting \"%s\" (%ux%u pixmap image)... ", mapfile, width, height);
	}
	else
	{
		//| Allow for bottom legend

		fprintf(fd, "P6\n%u %u\n255\n", width, height + 30);
		fprintf(stdout, "\nWriting \"%s\" (%ux%u pixmap image)... ", mapfile, width, height + 30);
	}

	fflush(stdout);

	for (y = 0, lat = north; y < (int)height; y++, lat = (north - ((1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree))*(double)y)))
	{
		for (x = 0, lon = maximumLongitudeWest; x < (int)width; x++, lon = (maximumLongitudeWest - ((1.0 / ((double)digitalElevationModelWrapper->demPixelsPerDegree))*(double)x)))
		{
			if (lon < 0.0)
			{
				lon += 360.0;
			}

			for (indx = 0, found = 0; (indx < digitalElevationModelWrapper->maxPages) && (found == 0);)
			{
				x0 = (int)rint(((double)digitalElevationModelWrapper->demPixelsPerDegree)*(lat - (double)digitalElevationModelWrapper->digitalElevationModel[indx].min_north));
				y0 = (digitalElevationModelWrapper->demPixelsPerDegree - 1) - (int)rint(((double)digitalElevationModelWrapper->demPixelsPerDegree) * LongitudeDifference((double)digitalElevationModelWrapper->digitalElevationModel[indx].max_west, lon));

				if ((x0 >= 0) && (x0 <= (digitalElevationModelWrapper->demPixelsPerDegree - 1)) && (y0 >= 0) && (y0 <= (digitalElevationModelWrapper->demPixelsPerDegree - 1)))
				{
					found = 1;
				}
				else
				{
					indx++;
				}
			}

			if (found)
			{
				mask = digitalElevationModelWrapper->digitalElevationModel[indx].mask[x0][y0];
				dBm = digitalElevationModelWrapper->digitalElevationModel[indx].signal[x0][y0] - 200;
				cityorcounty = 0;

				match = 255;

				red = 0;
				green = 0;
				blue = 0;

				if (dBm >= region->level[0])
				{
					match = 0;
				}
				else
				{
					for (z = 1; (z < region->levels) && (match == 255); z++)
					{
						if ((dBm < region->level[z - 1]) && (dBm >= region->level[z]))
						{
							match = z;
						}
					}
				}

				if (match < region->levels)
				{
					if (displaySmoothContourLevels && (match > 0))
					{
						red = (unsigned)LinearInterpolation(region->color[match][0], region->color[match - 1][0], region->level[match], region->level[match - 1], dBm);
						green = (unsigned)LinearInterpolation(region->color[match][1], region->color[match - 1][1], region->level[match], region->level[match - 1], dBm);
						blue = (unsigned)LinearInterpolation(region->color[match][2], region->color[match - 1][2], region->level[match], region->level[match - 1], dBm);
					}
					else
					{
						red = region->color[match][0];
						green = region->color[match][1];
						blue = region->color[match][2];
					}
				}

				if (mask & 2)
				{
					//| Text Labels: Red or otherwise

					if ((red >= 180) && (green <= 75) && (blue <= 75) && (dBm != 0))
					{
						fprintf(fd, "%c%c%c", 255 ^ red, 255 ^ green, 255 ^ blue);
					}
					else
					{
						fprintf(fd, "%c%c%c", 255, 0, 0);
					}

					cityorcounty = 1;
				}
				else if (mask & 4)
				{
					//| County Boundaries: Black

					fprintf(fd, "%c%c%c", 0, 0, 0);

					cityorcounty = 1;
				}

				if (cityorcounty == 0)
				{
					if ((contourDisplayThreshold != 0) && (dBm < contourDisplayThreshold))
					{
						if (ngs) //| No terrain
						{
							fprintf(fd, "%c%c%c", 255, 255, 255);
						}
						else
						{
							//| Display land or sea elevation

							if (digitalElevationModelWrapper->digitalElevationModel[indx].data[x0][y0] == 0)
							{
								fprintf(fd, "%c%c%c", 0, 0, 170);
							}
							else
							{
								terrain = (unsigned)(0.5 + pow((double)(digitalElevationModelWrapper->digitalElevationModel[indx].data[x0][y0] - minimumElevation), one_over_gamma)*conversion);
								fprintf(fd, "%c%c%c", terrain, terrain, terrain);
							}
						}
					}
					else
					{
						//| Plot signal power level regions in color

						if ((red != 0) || (green != 0) || (blue != 0))
						{
							fprintf(fd, "%c%c%c", red, green, blue);
						}
						else  //| terrain / sea-level
						{
							if (ngs)
							{
								fprintf(fd, "%c%c%c", 255, 255, 255);
							}
							else
							{
								if (digitalElevationModelWrapper->digitalElevationModel[indx].data[x0][y0] == 0)
								{
									fprintf(fd, "%c%c%c", 0, 0, 170);
								}
								else
								{
									//| Elevation: Greyscale
									terrain = (unsigned)(0.5 + pow((double)(digitalElevationModelWrapper->digitalElevationModel[indx].data[x0][y0] - minimumElevation), one_over_gamma)*conversion);
									fprintf(fd, "%c%c%c", terrain, terrain, terrain);
								}
							}
						}
					}
				}
			}
			else
			{
				//| We should never get here, but if
				//| we do, display the region as black

				fprintf(fd, "%c%c%c", 0, 0, 0);
			}
		}
	}

	if ((kml == 0) && (geo == 0))
	{
		//| Display legend along bottom of image
		//| if not generating .kml or .geo output.

		colorwidth = (int)rint((float)width / (float)region->levels);

		for (y0 = 0; y0 < 30; y0++)
		{
			for (x0 = 0; x0 < (int)width; x0++)
			{
				indx = x0 / colorwidth;
				x = x0 % colorwidth;

				level = abs(region->level[indx]);

				hundreds = level / 100;

				if (hundreds > 0)
				{
					level -= (hundreds * 100);
				}

				tens = level / 10;

				if (tens > 0)
				{
					level -= (tens * 10);
				}

				units = level;

				if ((y0 >= 8) && (y0 <= 23))
				{
					if (hundreds > 0)
					{
						if (region->level[indx] < 0)
						{
							if ((x >= 5) && (x <= 12))
							{
								if (fontdata[16 * ('-') + (y0 - 8)] & (128 >> (x - 5)))
								{
									indx = 255;
								}
							}
						}
						else
						{
							if ((x >= 5) && (x <= 12))
							{
								if (fontdata[16 * ('+') + (y0 - 8)] & (128 >> (x - 5)))
								{
									indx = 255;
								}
							}
						}

						if ((x >= 13) && (x <= 20))
						{
							if (fontdata[16 * (hundreds + '0') + (y0 - 8)] & (128 >> (x - 13)))
							{
								indx = 255;
							}
						}
					}

					if ((tens > 0) || (hundreds > 0))
					{
						if (hundreds == 0)
						{
							if (region->level[indx] < 0)
							{
								if ((x >= 13) && (x <= 20))
								{
									if (fontdata[16 * ('-') + (y0 - 8)] & (128 >> (x - 13)))
									{
										indx = 255;
									}
								}
							}
							else
							{
								if ((x >= 13) && (x <= 20))
								{
									if (fontdata[16 * ('+') + (y0 - 8)] & (128 >> (x - 13)))
									{
										indx = 255;
									}
								}
							}
						}

						if ((x >= 21) && (x <= 28))
						{
							if (fontdata[16 * (tens + '0') + (y0 - 8)] & (128 >> (x - 21)))
							{
								indx = 255;
							}
						}
					}

					if ((hundreds == 0) && (tens == 0))
					{
						if (region->level[indx] < 0)
						{
							if ((x >= 21) && (x <= 28))
							{
								if (fontdata[16 * ('-') + (y0 - 8)] & (128 >> (x - 21)))
								{
									indx = 255;
								}
							}
						}
						else
						{
							if ((x >= 21) && (x <= 28))
							{
								if (fontdata[16 * ('+') + (y0 - 8)] & (128 >> (x - 21)))
								{
									indx = 255;
								}
							}
						}
					}

					if ((x >= 29) && (x <= 36))
					{
						if (fontdata[16 * (units + '0') + (y0 - 8)] & (128 >> (x - 29)))
						{
							indx = 255;
						}
					}

					if ((x >= 37) && (x <= 44))
					{
						if (fontdata[16 * ('d') + (y0 - 8)] & (128 >> (x - 37)))
						{
							indx = 255;
						}
					}

					if ((x >= 45) && (x <= 52))
					{
						if (fontdata[16 * ('B') + (y0 - 8)] & (128 >> (x - 45)))
						{
							indx = 255;
						}
					}

					if ((x >= 53) && (x <= 60))
					{
						if (fontdata[16 * ('m') + (y0 - 8)] & (128 >> (x - 53)))
						{
							indx = 255;
						}
					}
				}

				if (indx > region->levels)
				{
					fprintf(fd, "%c%c%c", 0, 0, 0);
				}
				else
				{
					red = region->color[indx][0];
					green = region->color[indx][1];
					blue = region->color[indx][2];

					fprintf(fd, "%c%c%c", red, green, blue);
				}
			}
		}
	}

	fclose(fd);

	if (kml)
	{
		//| Write colorkey image file

		err = fopen_s(&fd, ckfile, "wb");

		height = 30 * region->levels;
		width = 100;

		fprintf(fd, "P6\n%u %u\n255\n", width, height);

		for (y0 = 0; y0 < (int)height; y0++)
		{
			for (x0 = 0; x0 < (int)width; x0++)
			{
				indx = y0 / 30;
				x = x0;

				level = abs(region->level[indx]);

				hundreds = level / 100;

				if (hundreds > 0)
				{
					level -= (hundreds * 100);
				}

				tens = level / 10;

				if (tens > 0)
				{
					level -= (tens * 10);
				}

				units = level;

				if (((y0 % 30) >= 8) && ((y0 % 30) <= 23))
				{
					if (hundreds > 0)
					{
						if (region->level[indx] < 0)
						{
							if ((x >= 5) && (x <= 12))
							{
								if (fontdata[16 * ('-') + ((y0 % 30) - 8)] & (128 >> (x - 5)))
								{
									indx = 255;
								}
							}
						}
						else
						{
							if ((x >= 5) && (x <= 12))
							{
								if (fontdata[16 * ('+') + ((y0 % 30) - 8)] & (128 >> (x - 5)))
								{
									indx = 255;
								}
							}
						}

						if ((x >= 13) && (x <= 20))
						{
							if (fontdata[16 * (hundreds + '0') + ((y0 % 30) - 8)] & (128 >> (x - 13)))
							{
								indx = 255;
							}
						}
					}

					if ((tens > 0) || (hundreds > 0))
					{
						if (hundreds == 0)
						{
							if (region->level[indx] < 0)
							{
								if ((x >= 13) && (x <= 20))
								{
									if (fontdata[16 * ('-') + ((y0 % 30) - 8)] & (128 >> (x - 13)))
									{
										indx = 255;
									}
								}
							}
							else
							{
								if ((x >= 13) && (x <= 20))
								{
									if (fontdata[16 * ('+') + ((y0 % 30) - 8)] & (128 >> (x - 13)))
									{
										indx = 255;
									}
								}
							}
						}

						if ((x >= 21) && (x <= 28))
						{
							if (fontdata[16 * (tens + '0') + ((y0 % 30) - 8)] & (128 >> (x - 21)))
							{
								indx = 255;
							}
						}
					}

					if ((hundreds == 0) && (tens == 0))
					{
						if (region->level[indx] < 0)
						{
							if ((x >= 21) && (x <= 28))
							{
								if (fontdata[16 * ('-') + ((y0 % 30) - 8)] & (128 >> (x - 21)))
								{
									indx = 255;
								}
							}
						}
						else
						{
							if ((x >= 21) && (x <= 28))
							{
								if (fontdata[16 * ('+') + ((y0 % 30) - 8)] & (128 >> (x - 21)))
								{
									indx = 255;
								}
							}
						}
					}

					if ((x >= 29) && (x <= 36))
					{
						if (fontdata[16 * (units + '0') + ((y0 % 30) - 8)] & (128 >> (x - 29)))
						{
							indx = 255;
						}
					}

					if ((x >= 37) && (x <= 44))
					{
						if (fontdata[16 * ('d') + ((y0 % 30) - 8)] & (128 >> (x - 37)))
						{
							indx = 255;
						}
					}

					if ((x >= 45) && (x <= 52))
					{
						if (fontdata[16 * ('B') + ((y0 % 30) - 8)] & (128 >> (x - 45)))
						{
							indx = 255;
						}
					}

					if ((x >= 53) && (x <= 60))
					{
						if (fontdata[16 * ('m') + ((y0 % 30) - 8)] & (128 >> (x - 53)))
						{
							indx = 255;
						}
					}
				}

				if (indx > region->levels)
				{
					fprintf(fd, "%c%c%c", 0, 0, 0);
				}
				else
				{
					red = region->color[indx][0];
					green = region->color[indx][1];
					blue = region->color[indx][2];

					fprintf(fd, "%c%c%c", red, green, blue);
				}
			}
		}

		fclose(fd);
	}

	fprintf(stdout, "Done\n");
	fflush(stdout);
}


//| ------------------------------
//| 
//| FUNCTION: GenerateGnuPlotTerrainProfileBetweenSites
//| 
//| OLD NAME: GraphTerrain
//| 
//| NOTES: 
//|   This function invokes gnuplot to generate an appropriate
//|   output file indicating the terrain profile between the source
//|   and destination locations when the -p command line option
//|   is used. "basename" is the name assigned to the output
//|   file generated by gnuplot. The filename extension is used
//|   to set gnuplot's terminal setting and output file type.
//|   If no extension is found, .png is assumed.
//| 
//| ------------------------------
void
GenerateGnuPlotTerrainProfileBetweenSites
   (Site source,
	Site destination,
	char *name,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	Path *path,
	char saveGnuPlotWorkingFiles,
	unsigned char useMetricUnits,
	double groundClutterHeight,
	char *reportSavePath)
{
	int	x, y, z;
	char basename[255], term[30], ext[15];
	double minheight = 100000.0, maxheight = -100000.0;
	FILE *fd = NULL, *fd1 = NULL;
	errno_t err;

	GeneratePathBetweenSites(destination, source, digitalElevationModelWrapper, path);

	char plotNameAndPath[335];
	sprintf_s(plotNameAndPath, _countof(plotNameAndPath), "%s%s", (reportSavePath[0] ? reportSavePath : ""), "splat.gp");

	char profileNameAndPath[335];
	sprintf_s(profileNameAndPath, _countof(profileNameAndPath), "%s%s", (reportSavePath[0] ? reportSavePath : ""), "profile.gp");

	char clutterNameAndPath[335];
	sprintf_s(clutterNameAndPath, _countof(clutterNameAndPath), "%s%s", (reportSavePath[0] ? reportSavePath : ""), "clutter.gp");

	err = fopen_s(&fd, profileNameAndPath, "wb");

	if (groundClutterHeight > 0.0)
	{
		err = fopen_s(&fd1, clutterNameAndPath, "wb");
	}

	for (x = 0; x < path->length; x++)
	{
		if ((path->elevation[x] + groundClutterHeight) > maxheight)
		{
			maxheight = path->elevation[x] + groundClutterHeight;
		}

		if (path->elevation[x] < minheight)
		{
			minheight = path->elevation[x];
		}

		if (useMetricUnits)
		{
			fprintf(fd, "%f\t%f\n", KM_PER_MILE*path->distance[x], METERS_PER_FOOT*path->elevation[x]);

			if ((fd1 != NULL) && (x > 0) && (x < (path->length - 2)))
			{
				fprintf(fd1, "%f\t%f\n", KM_PER_MILE*path->distance[x], METERS_PER_FOOT*(path->elevation[x] == 0.0 ? path->elevation[x] : (path->elevation[x] + groundClutterHeight)));
			}
		}
		else
		{
			fprintf(fd, "%f\t%f\n", path->distance[x], path->elevation[x]);

			if ((fd1 != NULL) && (x > 0) && (x < (path->length - 2)))
			{
				fprintf(fd1, "%f\t%f\n", path->distance[x], (path->elevation[x] == 0.0 ? path->elevation[x] : (path->elevation[x] + groundClutterHeight)));
			}
		}
	}

	fclose(fd);

	if (fd1 != NULL)
	{
		fclose(fd1);
	}

	if (name[0] == '.')
	{
		//| Default filename and output file type

		err = strncpy_s(basename, _countof(basename), "profile\0", 8);
		err = strncpy_s(term, _countof(term), "png\0", 4);
		err = strncpy_s(ext, _countof(ext), "png\0", 4);
	}
	else
	{
		//| Extract extension and terminal type from "name"

		ext[0] = 0;
		y = (int)strlen(name);
		err = strncpy_s(basename, _countof(basename), name, 254);

		for (x = y - 1; (x > 0) && (name[x] != '.'); x--);

		if (x > 0)  //| Extension found
		{
			for (z = x + 1; (z <= y) && ((z - (x + 1)) < 10); z++)
			{
				ext[z - (x + 1)] = tolower(name[z]);
				term[z - (x + 1)] = name[z];
			}

			ext[z - (x + 1)] = 0;  //| Ensure an ending 0
			term[z - (x + 1)] = 0;
			basename[x] = 0;
		}

		if (ext[0] == 0)	//| No extension -- Default is png
		{
			err = strncpy_s(term, _countof(term), "png\0", 4);
			err = strncpy_s(ext, _countof(ext), "png\0", 4);
		}
	}

	//| Either .ps or .postscript may be used
	//| as an extension for postscript output.

	if (strncmp(term, "postscript", 10) == 0)
	{
		err = strncpy_s(ext, _countof(ext), "ps\0", 3);
	}
	else if (strncmp(ext, "ps", 2) == 0)
	{
		err = strncpy_s(term, _countof(term), "postscript enhanced color\0", 26);
	}

	if (maxheight < 1.0)
	{
		maxheight = 1.0;	//| Avoid a gnuplot y-range error
		minheight = -1.0;	//| over a completely sea-level path
	}
	else
	{
		minheight -= (0.01*maxheight);
	}

	err = fopen_s(&fd, plotNameAndPath, "w");
	fprintf(fd, "set grid\n");
	fprintf(fd, "set yrange [%2.3f to %2.3f]\n", (useMetricUnits ? minheight * METERS_PER_FOOT : minheight), (useMetricUnits ? maxheight * METERS_PER_FOOT : maxheight));
	fprintf(fd, "set encoding iso_8859_1\n");
	fprintf(fd, "set term %s\n", term);
	fprintf(fd, "set title \"%s Terrain Profile Between %s and %s (%.2f%c Azimuth)\"\n", SPLAT_NAME, destination.name, source.name, AzimuthAngleBetweenSites(destination, source), 176);

	if (useMetricUnits)
	{
		fprintf(fd, "set xlabel \"Distance Between %s and %s (%.2f kilometers)\"\n", destination.name, source.name, KM_PER_MILE * GreatCircleDistanceBetweenSiteLocations(source, destination));
		fprintf(fd, "set ylabel \"Ground Elevation Above Sea Level (meters)\"\n");
	}
	else
	{
		fprintf(fd, "set xlabel \"Distance Between %s and %s (%.2f miles)\"\n", destination.name, source.name, GreatCircleDistanceBetweenSiteLocations(source, destination));
		fprintf(fd, "set ylabel \"Ground Elevation Above Sea Level (feet)\"\n");
	}

	fprintf(fd, "set output \"%s.%s\"\n", basename, ext);

	if (groundClutterHeight > 0.0)
	{
		if (useMetricUnits)
		{
			fprintf(fd, "plot \"%s\" title \"Terrain Profile\" with lines, \"%s\" title \"Clutter Profile (%.2f meters)\" with lines\n", profileNameAndPath, clutterNameAndPath, groundClutterHeight*METERS_PER_FOOT);
		}
		else
		{
			fprintf(fd, "plot \"%s\" title \"Terrain Profile\" with lines, \"%s\" title \"Clutter Profile (%.2f feet)\" with lines\n", profileNameAndPath, clutterNameAndPath, groundClutterHeight);
		}
	}
	else
	{
		fprintf(fd, "plot \"%s\" title \"\" with lines\n", profileNameAndPath);
	}

	fclose(fd);

	char gnuPlotAndSplat[335];
	sprintf_s(gnuPlotAndSplat, _countof(gnuPlotAndSplat), "run gnuplot %s", plotNameAndPath);

	//| Invokve gnuplot and run the splat.gp script.
	x = system(gnuPlotAndSplat);

	if (x != -1)
	{
		if (saveGnuPlotWorkingFiles == 0)
		{
			_unlink(plotNameAndPath);
			_unlink(profileNameAndPath);
		}

		fprintf(stdout, "Terrain plot written to: \"%s.%s\"\n", basename, ext);
		fflush(stdout);
	}
	else
	{
		fprintf(stderr, "\n*** ERROR: Error occurred invoking gnuplot\n");
	}
}


//| ------------------------------
//| 
//| FUNCTION: GenerateGnuPlotElevationProfileBetweenSites
//| 
//| OLD NAME: GraphElevation
//| 
//| NOTES: 
//|   This function invokes gnuplot to generate an appropriate
//|   output file indicating the elevation profile between
//|   the source and destination locations when the -e command line
//|   option is used. "basename" is the name assigned to the output
//|   file generated by gnuplot. The filename extension is used
//|   to set gnuplot's terminal setting and output file type.
//|   If no extension is found, .png is assumed.
//| 
//| ------------------------------
void
GenerateGnuPlotElevationProfileBetweenSites
   (Site source,
	Site destination,
	char *name,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	Path *path,
	char saveGnuPlotWorkingFiles,
	unsigned char useMetricUnits,
	double groundClutterHeight,
	double sphereRadius,
	char *reportSavePath)
{
	int	x, y, z;
	char basename[255], term[30], ext[15];
	double angle, clutter_angle = 0.0, refangle, maxangle = -90.0,
		minangle = 90.0, distance;
	Site remote, remote2;
	FILE *fd = NULL, *fd1 = NULL, *fd2 = NULL;
	errno_t err;

	GeneratePathBetweenSites(destination, source, digitalElevationModelWrapper, path);  //| destination=RX, source=TX
	refangle = ElevationAngleBetweenSites(destination, source, digitalElevationModelWrapper, sphereRadius);
	distance = GreatCircleDistanceBetweenSiteLocations(source, destination);

	char plotNameAndPath[335];
	sprintf_s(plotNameAndPath, _countof(plotNameAndPath), "%s%s", (reportSavePath[0] ? reportSavePath : ""), "splat.gp");

	char profileNameAndPath[335];
	sprintf_s(profileNameAndPath, _countof(profileNameAndPath), "%s%s", (reportSavePath[0] ? reportSavePath : ""), "profile.gp");

	char clutterNameAndPath[335];
	sprintf_s(clutterNameAndPath, _countof(clutterNameAndPath), "%s%s", (reportSavePath[0] ? reportSavePath : ""), "clutter.gp");

	char referenceNameAndPath[335];
	sprintf_s(referenceNameAndPath, _countof(referenceNameAndPath), "%s%s", (reportSavePath[0] ? reportSavePath : ""), "reference.gp");

	err = fopen_s(&fd, profileNameAndPath, "wb");

	if (groundClutterHeight > 0.0)
	{
		err = fopen_s(&fd1, clutterNameAndPath, "wb");
	}

	err = fopen_s(&fd2, referenceNameAndPath, "wb");

	for (x = 1; x < path->length - 1; x++)
	{
		remote.lat = path->lat[x];
		remote.lon = path->lon[x];
		remote.alt = 0.0;
		angle = ElevationAngleBetweenSites(destination, remote, digitalElevationModelWrapper, sphereRadius);

		if (groundClutterHeight > 0.0)
		{
			remote2.lat = path->lat[x];
			remote2.lon = path->lon[x];

			if (path->elevation[x] != 0.0)
			{
				remote2.alt = (float)groundClutterHeight;
			}
			else
			{
				remote2.alt = 0.0;
			}

			clutter_angle = ElevationAngleBetweenSites(destination, remote2, digitalElevationModelWrapper, sphereRadius);
		}

		if (useMetricUnits)
		{
			fprintf(fd, "%f\t%f\n", KM_PER_MILE*path->distance[x], angle);

			if (fd1 != NULL)
			{
				fprintf(fd1, "%f\t%f\n", KM_PER_MILE*path->distance[x], clutter_angle);
			}

			fprintf(fd2, "%f\t%f\n", KM_PER_MILE*path->distance[x], refangle);
		}
		else
		{
			fprintf(fd, "%f\t%f\n", path->distance[x], angle);

			if (fd1 != NULL)
			{
				fprintf(fd1, "%f\t%f\n", path->distance[x], clutter_angle);
			}

			fprintf(fd2, "%f\t%f\n", path->distance[x], refangle);
		}

		if (angle > maxangle)
		{
			maxangle = angle;
		}

		if (clutter_angle > maxangle)
		{
			maxangle = clutter_angle;
		}

		if (angle < minangle)
		{
			minangle = angle;
		}
	}

	if (useMetricUnits)
	{
		fprintf(fd, "%f\t%f\n", KM_PER_MILE*path->distance[path->length - 1], refangle);
		fprintf(fd2, "%f\t%f\n", KM_PER_MILE*path->distance[path->length - 1], refangle);
	}
	else
	{
		fprintf(fd, "%f\t%f\n", path->distance[path->length - 1], refangle);
		fprintf(fd2, "%f\t%f\n", path->distance[path->length - 1], refangle);
	}

	fclose(fd);

	if (fd1 != NULL)
	{
		fclose(fd1);
	}

	fclose(fd2);

	if (name[0] == '.')
	{
		//| Default filename and output file type

		err = strncpy_s(basename, _countof(basename), "profile\0", 8);
		err = strncpy_s(term, _countof(term), "png\0", 4);
		err = strncpy_s(ext, _countof(ext), "png\0", 4);
	}
	else
	{
		//| Extract extension and terminal type from "name"

		ext[0] = 0;
		y = (int)strlen(name);
		err = strncpy_s(basename, _countof(basename), name, 254);

		for (x = y - 1; (x > 0) && (name[x] != '.'); x--);

		if (x > 0)  //| Extension found
		{
			for (z = x + 1; (z <= y) && ((z - (x + 1)) < 10); z++)
			{
				ext[z - (x + 1)] = tolower(name[z]);
				term[z - (x + 1)] = name[z];
			}

			ext[z - (x + 1)] = 0;  //| Ensure an ending 0
			term[z - (x + 1)] = 0;
			basename[x] = 0;
		}

		if (ext[0] == 0)	//| No extension -- Default is png
		{
			err = strncpy_s(term, _countof(term), "png\0", 4);
			err = strncpy_s(ext, _countof(ext), "png\0", 4);
		}
	}

	//| Either .ps or .postscript may be used
	//| as an extension for postscript output.

	if (strncmp(term, "postscript", 10) == 0)
	{
		err = strncpy_s(ext, _countof(ext), "ps\0", 3);
	}
	else if (strncmp(ext, "ps", 2) == 0)
	{
		err = strncpy_s(term, _countof(term), "postscript enhanced color\0", 26);
	}

	err = fopen_s(&fd, plotNameAndPath, "w");

	fprintf(fd, "set grid\n");

	if (distance > 2.0)
	{
		fprintf(fd, "set yrange [%2.3f to %2.3f]\n", (-fabs(refangle) - 0.25), maxangle + 0.25);
	}
	else
	{
		fprintf(fd, "set yrange [%2.3f to %2.3f]\n", minangle, refangle + (-minangle / 8.0));
	}

	fprintf(fd, "set encoding iso_8859_1\n");
	fprintf(fd, "set term %s\n", term);
	fprintf(fd, "set title \"%s Elevation Profile Between %s and %s (%.2f%c azimuth)\"\n", SPLAT_NAME, destination.name, source.name, AzimuthAngleBetweenSites(destination, source), 176);

	if (useMetricUnits)
	{
		fprintf(fd, "set xlabel \"Distance Between %s and %s (%.2f kilometers)\"\n", destination.name, source.name, KM_PER_MILE*distance);
	}
	else
	{
		fprintf(fd, "set xlabel \"Distance Between %s and %s (%.2f miles)\"\n", destination.name, source.name, distance);
	}

	fprintf(fd, "set ylabel \"Elevation Angle Along LOS Path Between\\n%s and %s (degrees)\"\n", destination.name, source.name);
	fprintf(fd, "set output \"%s.%s\"\n", basename, ext);

	if (groundClutterHeight > 0.0)
	{
		if (useMetricUnits)
		{
			fprintf(fd, "plot \"%s\" title \"Real Earth Profile\" with lines, \"%s\" title \"Clutter Profile (%.2f meters)\" with lines, \"%s\" title \"Line of Sight Path (%.2f%c elevation)\" with lines\n", profileNameAndPath, clutterNameAndPath, groundClutterHeight*METERS_PER_FOOT, referenceNameAndPath, refangle, 176);
		}
		else
		{
			fprintf(fd, "plot \"%s\" title \"Real Earth Profile\" with lines, \"%s\" title \"Clutter Profile (%.2f feet)\" with lines, \"%s\" title \"Line of Sight Path (%.2f%c elevation)\" with lines\n", profileNameAndPath, clutterNameAndPath, groundClutterHeight, referenceNameAndPath, refangle, 176);
		}
	}
	else
	{
		fprintf(fd, "plot \"%s\" title \"Real Earth Profile\" with lines, \"%s\" title \"Line of Sight Path (%.2f%c elevation)\" with lines\n", profileNameAndPath, referenceNameAndPath, refangle, 176);
	}

	fclose(fd);

	char gnuPlotAndSplat[335];
	sprintf_s(gnuPlotAndSplat, _countof(gnuPlotAndSplat), "run gnuplot %s", plotNameAndPath);

	//| Invokve gnuplot and run the splat.gp script.
	x = system(gnuPlotAndSplat);

	if (x != -1)
	{
		if (saveGnuPlotWorkingFiles == 0)
		{
			_unlink(plotNameAndPath);
			_unlink(profileNameAndPath);
			_unlink(referenceNameAndPath);

			if (groundClutterHeight > 0.0)
			{
				_unlink(clutterNameAndPath);
			}
		}

		fprintf(stdout, "Elevation plot written to: \"%s.%s\"\n", basename, ext);
		fflush(stdout);
	}
	else
	{
		fprintf(stderr, "\n*** ERROR: Error occurred invoking gnuplot\n");
	}
}


//| ------------------------------
//| 
//| FUNCTION: GenerateGnuPlotHeightProfileBetweenSites
//| 
//| OLD NAME: GraphHeight
//| 
//| NOTES: 
//|   This function invokes gnuplot to generate an appropriate
//|   output file indicating the terrain height profile between
//|   the source and destination locations referenced to the
//|   line-of-sight path between the receive and transmit sites
//|   when the -h or -H command line option is used. "basename"
//|   is the name assigned to the output file generated by gnuplot.
//|   The filename extension is used to set gnuplot's terminal
//|   setting and output file type. If no extension is found,
//|   .png is assumed.
//| 
//| ------------------------------
void
GenerateGnuPlotHeightProfileBetweenSites
   (Site source,
	Site destination,
	char *name,
	unsigned char fresnel_plot,
	unsigned char normalized,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	Path *path,
	IrregularTerrainModelParameters *itmParameters,
	char saveGnuPlotWorkingFiles,
	unsigned char useMetricUnits,
	double groundClutterHeight,
	double fresnelZoneClearanceRatio,
	double sphereRadius,
	char *reportSavePath)
{
	int	x, y, z;
	char basename[255], term[30], ext[15];
	double a, b, c, height = 0.0, refangle, cangle, maxheight = -100000.0,
		minheight = 100000.0, lambda = 0.0, f_zone = 0.0, fpt6_zone = 0.0,
		nm = 0.0, nb = 0.0, ed = 0.0, es = 0.0, r = 0.0, d = 0.0, d1 = 0.0,
		terrain, azimuth, distance, dheight = 0.0, minterrain = 100000.0,
		minearth = 100000.0, miny, maxy, min2y, max2y;
	Site remote;
	FILE *fd = NULL, *fd1 = NULL, *fd2 = NULL, *fd3 = NULL, *fd4 = NULL, *fd5 = NULL;
	errno_t err;

	GeneratePathBetweenSites(destination, source, digitalElevationModelWrapper, path);  //| destination=RX, source=TX
	azimuth = AzimuthAngleBetweenSites(destination, source);
	distance = GreatCircleDistanceBetweenSiteLocations(destination, source);
	refangle = ElevationAngleBetweenSites(destination, source, digitalElevationModelWrapper, sphereRadius);
	b = GetSiteLocationElevation(destination, digitalElevationModelWrapper) + destination.alt + sphereRadius;

	//| Wavelength and path distance (great circle) in feet.

	if (fresnel_plot)
	{
		lambda = 9.8425e8 / (itmParameters->frq_mhz*1e6);
		d = 5280.0*path->distance[path->length - 1];
	}

	if (normalized)
	{
		ed = GetSiteLocationElevation(destination, digitalElevationModelWrapper);
		es = GetSiteLocationElevation(source, digitalElevationModelWrapper);
		nb = -destination.alt - ed;
		nm = (-source.alt - es - nb) / (path->distance[path->length - 1]);
	}

	char plotNameAndPath[335];
	sprintf_s(plotNameAndPath, _countof(plotNameAndPath), "%s%s", (reportSavePath[0] ? reportSavePath : ""), "splat.gp");

	char profileNameAndPath[335];
	sprintf_s(profileNameAndPath, _countof(profileNameAndPath), "%s%s", (reportSavePath[0] ? reportSavePath : ""), "profile.gp");

	char clutterNameAndPath[335];
	sprintf_s(clutterNameAndPath, _countof(clutterNameAndPath), "%s%s", (reportSavePath[0] ? reportSavePath : ""), "clutter.gp");

	char referenceNameAndPath[335];
	sprintf_s(referenceNameAndPath, _countof(referenceNameAndPath), "%s%s", (reportSavePath[0] ? reportSavePath : ""), "reference.gp");

	char curvatureNameAndPath[335];
	sprintf_s(curvatureNameAndPath, _countof(curvatureNameAndPath), "%s%s", (reportSavePath[0] ? reportSavePath : ""), "curvature.gp");

	char fresnelNameAndPath[335];
	sprintf_s(fresnelNameAndPath, _countof(fresnelNameAndPath), "%s%s", (reportSavePath[0] ? reportSavePath : ""), "fresnel.gp");

	char fresnelPtNameAndPath[335];
	sprintf_s(fresnelPtNameAndPath, _countof(fresnelPtNameAndPath), "%s%s", (reportSavePath[0] ? reportSavePath : ""), "fresnel_pt_6.gp");

	err = fopen_s(&fd, profileNameAndPath, "wb");

	if (groundClutterHeight > 0.0)
	{
		err = fopen_s(&fd1, clutterNameAndPath, "wb");
	}

	err = fopen_s(&fd2, referenceNameAndPath, "wb");
	err = fopen_s(&fd5, curvatureNameAndPath, "wb");

	if ((itmParameters->frq_mhz >= 20.0) && (itmParameters->frq_mhz <= 20000.0) && fresnel_plot)
	{
		err = fopen_s(&fd3, fresnelNameAndPath, "wb");
		err = fopen_s(&fd4, fresnelPtNameAndPath, "wb");
	}

	for (x = 0; x < (path->length - 1); x++)
	{
		remote.lat = path->lat[x];
		remote.lon = path->lon[x];
		remote.alt = 0.0;

		terrain = GetSiteLocationElevation(remote, digitalElevationModelWrapper);

		if (x == 0)
		{
			terrain += destination.alt;  //| RX antenna spike
		}

		a = terrain + sphereRadius;
		cangle = 5280.0 * GreatCircleDistanceBetweenSiteLocations(destination, remote) / sphereRadius;
		c = b * sin(refangle*DEGREES_TO_RADIANS + HALF_PI) / sin(HALF_PI - refangle * DEGREES_TO_RADIANS - cangle);

		height = a - c;

		//| Per Fink and Christiansen, Electronics
		//| Engineers' Handbook, 1989:
		//| 
		//| H = sqrt(lamba * d1 * (d - d1)/d)
		//| 
		//| where H is the distance from the LOS
		//| path to the first Fresnel zone boundary.

		if ((itmParameters->frq_mhz >= 20.0) && (itmParameters->frq_mhz <= 20000.0) && fresnel_plot)
		{
			d1 = 5280.0*path->distance[x];
			f_zone = -1.0*sqrt(lambda*d1*(d - d1) / d);
			fpt6_zone = f_zone * fresnelZoneClearanceRatio;
		}

		if (normalized)
		{
			r = -(nm*path->distance[x]) - nb;
			height += r;

			if ((itmParameters->frq_mhz >= 20.0) && (itmParameters->frq_mhz <= 20000.0) && fresnel_plot)
			{
				f_zone += r;
				fpt6_zone += r;
			}
		}
		else
		{
			r = 0.0;
		}

		if (useMetricUnits)
		{
			fprintf(fd, "%f\t%f\n", KM_PER_MILE*path->distance[x], METERS_PER_FOOT*height);

			if ((fd1 != NULL) && (x > 0) && (x < (path->length - 2)))
			{
				fprintf(fd1, "%f\t%f\n", KM_PER_MILE*path->distance[x], METERS_PER_FOOT*(terrain == 0.0 ? height : (height + groundClutterHeight)));
			}

			fprintf(fd2, "%f\t%f\n", KM_PER_MILE*path->distance[x], METERS_PER_FOOT*r);
			fprintf(fd5, "%f\t%f\n", KM_PER_MILE*path->distance[x], METERS_PER_FOOT*(height - terrain));
		}
		else
		{
			fprintf(fd, "%f\t%f\n", path->distance[x], height);

			if ((fd1 != NULL) && (x > 0) && (x < (path->length - 2)))
			{
				fprintf(fd1, "%f\t%f\n", path->distance[x], (terrain == 0.0 ? height : (height + groundClutterHeight)));
			}

			fprintf(fd2, "%f\t%f\n", path->distance[x], r);
			fprintf(fd5, "%f\t%f\n", path->distance[x], height - terrain);
		}

		if ((itmParameters->frq_mhz >= 20.0) && (itmParameters->frq_mhz <= 20000.0) && fresnel_plot)
		{
			if (useMetricUnits)
			{
				fprintf(fd3, "%f\t%f\n", KM_PER_MILE*path->distance[x], METERS_PER_FOOT*f_zone);
				fprintf(fd4, "%f\t%f\n", KM_PER_MILE*path->distance[x], METERS_PER_FOOT*fpt6_zone);
			}
			else
			{
				fprintf(fd3, "%f\t%f\n", path->distance[x], f_zone);
				fprintf(fd4, "%f\t%f\n", path->distance[x], fpt6_zone);
			}

			if (f_zone < minheight)
			{
				minheight = f_zone;
			}
		}

		if ((height + groundClutterHeight) > maxheight)
		{
			maxheight = height + groundClutterHeight;
		}

		if (height < minheight)
		{
			minheight = height;
		}

		if (r > maxheight)
		{
			maxheight = r;
		}

		if (terrain < minterrain)
		{
			minterrain = terrain;
		}

		if ((height - terrain) < minearth)
		{
			minearth = height - terrain;
		}
	}

	if (normalized)
	{
		r = -(nm*path->distance[path->length - 1]) - nb;
	}
	else
	{
		r = 0.0;
	}

	if (useMetricUnits)
	{
		fprintf(fd, "%f\t%f\n", KM_PER_MILE*path->distance[path->length - 1], METERS_PER_FOOT*r);
		fprintf(fd2, "%f\t%f\n", KM_PER_MILE*path->distance[path->length - 1], METERS_PER_FOOT*r);
	}
	else
	{
		fprintf(fd, "%f\t%f\n", path->distance[path->length - 1], r);
		fprintf(fd2, "%f\t%f\n", path->distance[path->length - 1], r);
	}

	if ((itmParameters->frq_mhz >= 20.0) && (itmParameters->frq_mhz <= 20000.0) && fresnel_plot)
	{
		if (useMetricUnits)
		{
			fprintf(fd3, "%f\t%f\n", KM_PER_MILE*path->distance[path->length - 1], METERS_PER_FOOT*r);
			fprintf(fd4, "%f\t%f\n", KM_PER_MILE*path->distance[path->length - 1], METERS_PER_FOOT*r);
		}
		else
		{
			fprintf(fd3, "%f\t%f\n", path->distance[path->length - 1], r);
			fprintf(fd4, "%f\t%f\n", path->distance[path->length - 1], r);
		}
	}

	if (r > maxheight)
	{
		maxheight = r;
	}

	if (r < minheight)
	{
		minheight = r;
	}

	fclose(fd);

	if (fd1 != NULL)
	{
		fclose(fd1);
	}

	fclose(fd2);
	fclose(fd5);

	if ((itmParameters->frq_mhz >= 20.0) && (itmParameters->frq_mhz <= 20000.0) && fresnel_plot)
	{
		fclose(fd3);
		fclose(fd4);
	}

	if (name[0] == '.')
	{
		//| Default filename and output file type

		err = strncpy_s(basename, _countof(basename), "profile\0", 8);
		err = strncpy_s(term, _countof(term), "png\0", 4);
		err = strncpy_s(ext, _countof(ext), "png\0", 4);
	}
	else
	{
		//| Extract extension and terminal type from "name"

		ext[0] = 0;
		y = (int)strlen(name);
		err = strncpy_s(basename, _countof(basename), name, 254);

		for (x = y - 1; (x > 0) && (name[x] != '.'); x--);

		if (x > 0)  //| Extension found
		{
			for (z = x + 1; (z <= y) && ((z - (x + 1)) < 10); z++)
			{
				ext[z - (x + 1)] = tolower(name[z]);
				term[z - (x + 1)] = name[z];
			}

			ext[z - (x + 1)] = 0;  //| Ensure an ending 0
			term[z - (x + 1)] = 0;
			basename[x] = 0;
		}

		if (ext[0] == 0)	//| No extension -- Default is png
		{
			err = strncpy_s(term, _countof(term), "png\0", 4);
			err = strncpy_s(ext, _countof(ext), "png\0", 4);
		}
	}

	//| Either .ps or .postscript may be used
	//| as an extension for postscript output.

	if (strncmp(term, "postscript", 10) == 0)
	{
		err = strncpy_s(ext, _countof(ext), "ps\0", 3);
	}
	else if (strncmp(ext, "ps", 2) == 0)
	{
		err = strncpy_s(term, _countof(term), "postscript enhanced color\0", 26);
	}

	err = fopen_s(&fd, plotNameAndPath, "w");

	dheight = maxheight - minheight;
	miny = minheight - 0.15*dheight;
	maxy = maxheight + 0.05*dheight;

	if (maxy < 20.0)
	{
		maxy = 20.0;
	}

	dheight = maxheight - minheight;
	min2y = miny - minterrain + 0.05*dheight;

	if (minearth < min2y)
	{
		miny -= min2y - minearth + 0.05*dheight;
		min2y = minearth - 0.05*dheight;
	}

	max2y = min2y + maxy - miny;

	fprintf(fd, "set grid\n");
	fprintf(fd, "set yrange [%2.3f to %2.3f]\n", (useMetricUnits ? miny * METERS_PER_FOOT : miny), (useMetricUnits ? maxy * METERS_PER_FOOT : maxy));
	fprintf(fd, "set y2range [%2.3f to %2.3f]\n", (useMetricUnits ? min2y * METERS_PER_FOOT : min2y), (useMetricUnits ? max2y * METERS_PER_FOOT : max2y));
	fprintf(fd, "set xrange [-0.5 to %2.3f]\n", (useMetricUnits ? KM_PER_MILE * rint(distance + 0.5) : rint(distance + 0.5)));
	fprintf(fd, "set encoding iso_8859_1\n");
	fprintf(fd, "set term %s\n", term);

	if ((itmParameters->frq_mhz >= 20.0) && (itmParameters->frq_mhz <= 20000.0) && fresnel_plot)
	{
		fprintf(fd, "set title \"%s Path Profile Between %s and %s (%.2f%c azimuth)\\nWith First Fresnel Zone\"\n", SPLAT_NAME, destination.name, source.name, azimuth, 176);
	}
	else
	{
		fprintf(fd, "set title \"%s Height Profile Between %s and %s (%.2f%c azimuth)\"\n", SPLAT_NAME, destination.name, source.name, azimuth, 176);
	}

	if (useMetricUnits)
	{
		fprintf(fd, "set xlabel \"Distance Between %s and %s (%.2f kilometers)\"\n", destination.name, source.name, KM_PER_MILE * GreatCircleDistanceBetweenSiteLocations(source, destination));
	}
	else
	{
		fprintf(fd, "set xlabel \"Distance Between %s and %s (%.2f miles)\"\n", destination.name, source.name, GreatCircleDistanceBetweenSiteLocations(source, destination));
	}

	if (normalized)
	{
		if (useMetricUnits)
		{
			fprintf(fd, "set ylabel \"Normalized Height Referenced To LOS Path Between\\n%s and %s (meters)\"\n", destination.name, source.name);
		}
		else
		{
			fprintf(fd, "set ylabel \"Normalized Height Referenced To LOS Path Between\\n%s and %s (feet)\"\n", destination.name, source.name);
		}
	}
	else
	{
		if (useMetricUnits)
		{
			fprintf(fd, "set ylabel \"Height Referenced To LOS Path Between\\n%s and %s (meters)\"\n", destination.name, source.name);
		}
		else
		{
			fprintf(fd, "set ylabel \"Height Referenced To LOS Path Between\\n%s and %s (feet)\"\n", destination.name, source.name);
		}
	}

	fprintf(fd, "set output \"%s.%s\"\n", basename, ext);

	if ((itmParameters->frq_mhz >= 20.0) && (itmParameters->frq_mhz <= 20000.0) && fresnel_plot)
	{
		if (groundClutterHeight > 0.0)
		{
			if (useMetricUnits)
			{
				fprintf(fd, "plot \"%s\" title \"Point-to-Point Profile\" with lines, \"%s\" title \"Ground Clutter (%.2f meters)\" with lines, \"%s\" title \"Line of Sight Path\" with lines, \"%s\" axes x1y2 title \"Earth's Curvature Contour\" with lines, \"%s\" axes x1y1 title \"First Fresnel Zone (%.3f MHz)\" with lines, \"%s\" title \"%.0f%% of First Fresnel Zone\" with lines\n", profileNameAndPath, clutterNameAndPath, groundClutterHeight*METERS_PER_FOOT, referenceNameAndPath, curvatureNameAndPath, fresnelNameAndPath, itmParameters->frq_mhz, fresnelPtNameAndPath, fresnelZoneClearanceRatio*100.0);
			}
			else
			{
				fprintf(fd, "plot \"%s\" title \"Point-to-Point Profile\" with lines, \"%s\" title \"Ground Clutter (%.2f feet)\" with lines, \"%s\" title \"Line of Sight Path\" with lines, \"%s\" axes x1y2 title \"Earth's Curvature Contour\" with lines, \"%s\" axes x1y1 title \"First Fresnel Zone (%.3f MHz)\" with lines, \"%s\" title \"%.0f%% of First Fresnel Zone\" with lines\n", profileNameAndPath, clutterNameAndPath, groundClutterHeight, referenceNameAndPath, curvatureNameAndPath, fresnelNameAndPath, itmParameters->frq_mhz, fresnelPtNameAndPath, fresnelZoneClearanceRatio*100.0);
			}
		}
		else
		{
			fprintf(fd, "plot \"%s\" title \"Point-to-Point Profile\" with lines, \"%s\" title \"Line of Sight Path\" with lines, \"%s\" axes x1y2 title \"Earth's Curvature Contour\" with lines, \"%s\" axes x1y1 title \"First Fresnel Zone (%.3f MHz)\" with lines, \"%s\" title \"%.0f%% of First Fresnel Zone\" with lines\n", profileNameAndPath, referenceNameAndPath, curvatureNameAndPath, fresnelNameAndPath, itmParameters->frq_mhz, fresnelPtNameAndPath, fresnelZoneClearanceRatio*100.0);
		}
	}
	else
	{
		if (groundClutterHeight > 0.0)
		{
			if (useMetricUnits)
			{
				fprintf(fd, "plot \"%s\" title \"Point-to-Point Profile\" with lines, \"%s\" title \"Ground Clutter (%.2f meters)\" with lines, \"%s\" title \"Line Of Sight Path\" with lines, \"%s\" axes x1y2 title \"Earth's Curvature Contour\" with lines\n", profileNameAndPath, clutterNameAndPath, groundClutterHeight*METERS_PER_FOOT, referenceNameAndPath, curvatureNameAndPath);
			}
			else
			{
				fprintf(fd, "plot \"%s\" title \"Point-to-Point Profile\" with lines, \"%s\" title \"Ground Clutter (%.2f feet)\" with lines, \"%s\" title \"Line Of Sight Path\" with lines, \"%s\" axes x1y2 title \"Earth's Curvature Contour\" with lines\n", profileNameAndPath, clutterNameAndPath, groundClutterHeight, referenceNameAndPath, curvatureNameAndPath);
			}
		}
		else
		{
			fprintf(fd, "plot \"%s\" title \"Point-to-Point Profile\" with lines, \"%s\" title \"Line Of Sight Path\" with lines, \"%s\" axes x1y2 title \"Earth's Curvature Contour\" with lines\n", profileNameAndPath, referenceNameAndPath, curvatureNameAndPath);
		}
	}

	fclose(fd);

	char gnuPlotAndSplat[335];
	sprintf_s(gnuPlotAndSplat, _countof(gnuPlotAndSplat), "run gnuplot %s", plotNameAndPath);

	//| Invokve gnuplot and run the splat.gp script.
	x = system(gnuPlotAndSplat);

	if (x != -1)
	{
		if (saveGnuPlotWorkingFiles == 0)
		{
			_unlink(plotNameAndPath);
			_unlink(profileNameAndPath);
			_unlink(referenceNameAndPath);
			_unlink(curvatureNameAndPath);

			if (fd1 != NULL)
			{
				_unlink(clutterNameAndPath);
			}

			if ((itmParameters->frq_mhz >= 20.0) && (itmParameters->frq_mhz <= 20000.0) && fresnel_plot)
			{
				_unlink(fresnelNameAndPath);
				_unlink(fresnelPtNameAndPath);
			}
		}

		fprintf(stdout, "\nHeight plot written to: \"%s.%s\"", basename, ext);
		fflush(stdout);
	}
	else
	{
		fprintf(stderr, "\n*** ERROR: Error occurred invoking gnuplot\n");
	}
}


//| ------------------------------
//| 
//| FUNCTION: PerformObstructionAnalysisBetweenSites
//| 
//| OLD NAME: ObstructionAnalysis
//| 
//| NOTES: 
//|   Perform an obstruction analysis along the
//|   path between receiver and transmitter.
//| 
//| ------------------------------
void
PerformObstructionAnalysisBetweenSites
   (Site xmtr,
	Site rcvr,
	double f,
	FILE *outfile,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	Path *path,
	unsigned char useMetricUnits,
	double groundClutterHeight,
	double fresnelZoneClearanceRatio,
	double sphereRadius)
{
	int	x;
	Site site_x;
	double h_r, h_t, h_x, h_r_orig, cos_tx_angle, cos_test_angle,
		cos_tx_angle_f1, cos_tx_angle_fpt6, d_tx, d_x,
		h_r_f1, h_r_fpt6, h_f, h_los, lambda = 0.0;
	char ObsString[255], string_fpt6[255], string_f1[255];

	GeneratePathBetweenSites(xmtr, rcvr, digitalElevationModelWrapper, path);
	h_r = GetSiteLocationElevation(rcvr, digitalElevationModelWrapper) + rcvr.alt + sphereRadius;
	h_r_f1 = h_r;
	h_r_fpt6 = h_r;
	h_r_orig = h_r;
	h_t = GetSiteLocationElevation(xmtr, digitalElevationModelWrapper) + xmtr.alt + sphereRadius;
	d_tx = 5280.0 * GreatCircleDistanceBetweenSiteLocations(rcvr, xmtr);
	cos_tx_angle = ((h_r*h_r) + (d_tx*d_tx) - (h_t*h_t)) / (2.0*h_r*d_tx);
	cos_tx_angle_f1 = cos_tx_angle;
	cos_tx_angle_fpt6 = cos_tx_angle;

	if (f)
	{
		lambda = 9.8425e8 / (f*1e6);
	}

	if (groundClutterHeight > 0.0)
	{
		fprintf(outfile, "Terrain has been raised by");

		if (useMetricUnits)
		{
			fprintf(outfile, " %.2f meters", METERS_PER_FOOT*groundClutterHeight);
		}
		else
		{
			fprintf(outfile, " %.2f feet", groundClutterHeight);
		}

		fprintf(outfile, " to account for ground clutter.\n\n");
	}

	//| At each point along the path calculate the cosine
	//| of a sort of "inverse elevation angle" at the receiver.
	//| From the antenna, 0 deg. looks at the ground, and 90 deg.
	//| is parallel to the ground.
	//|
	//| Start at the receiver. If this is the lowest antenna,
	//| then terrain obstructions will be nearest to it. Plus,
	//| that's the way SPLAT's original los() did it.
	//|
	//| Calculate cosines only. That's sufficient to compare
	//| angles and it saves the extra computational burden of
	//| acos(). However, note the inverted comparison: if
	//| acos(A) > acos(B), then B > A.

	for (x = path->length - 1; x > 0; x--)
	{
		site_x.lat = path->lat[x];
		site_x.lon = path->lon[x];
		site_x.alt = 0.0;

		h_x = GetSiteLocationElevation(site_x, digitalElevationModelWrapper) + sphereRadius + groundClutterHeight;
		d_x = 5280.0 * GreatCircleDistanceBetweenSiteLocations(rcvr, site_x);

		//| Deal with the LOS path first.

		cos_test_angle = ((h_r*h_r) + (d_x*d_x) - (h_x*h_x)) / (2.0*h_r*d_x);

		if (cos_tx_angle > cos_test_angle)
		{
			if (h_r == h_r_orig)
			{
				fprintf(outfile, "Between %s and %s, %s detected obstructions at:\n\n", rcvr.name, xmtr.name, SPLAT_NAME);
			}

			if (site_x.lat >= 0.0)
			{
				if (useMetricUnits)
				{
					fprintf(outfile, "   %8.4f N,%9.4f W, %5.2f kilometers, %6.2f meters AMSL\n", site_x.lat, site_x.lon, KM_PER_MILE*(d_x / 5280.0), METERS_PER_FOOT*(h_x - sphereRadius));
				}
				else
				{
					fprintf(outfile, "   %8.4f N,%9.4f W, %5.2f miles, %6.2f feet AMSL\n", site_x.lat, site_x.lon, d_x / 5280.0, h_x - sphereRadius);
				}
			}
			else
			{
				if (useMetricUnits)
				{
					fprintf(outfile, "   %8.4f S,%9.4f W, %5.2f kilometers, %6.2f meters AMSL\n", -site_x.lat, site_x.lon, KM_PER_MILE*(d_x / 5280.0), METERS_PER_FOOT*(h_x - sphereRadius));
				}
				else
				{
					fprintf(outfile, "   %8.4f S,%9.4f W, %5.2f miles, %6.2f feet AMSL\n", -site_x.lat, site_x.lon, d_x / 5280.0, h_x - sphereRadius);
				}
			}
		}

		while (cos_tx_angle > cos_test_angle)
		{
			h_r += 1;
			cos_test_angle = ((h_r*h_r) + (d_x*d_x) - (h_x*h_x)) / (2.0*h_r*d_x);
			cos_tx_angle = ((h_r*h_r) + (d_tx*d_tx) - (h_t*h_t)) / (2.0*h_r*d_tx);
		}

		if (f)
		{
			//| Now clear the first Fresnel zone,

			cos_tx_angle_f1 = ((h_r_f1*h_r_f1) + (d_tx*d_tx) - (h_t*h_t)) / (2.0*h_r_f1*d_tx);
			h_los = sqrt(h_r_f1*h_r_f1 + d_x * d_x - 2 * h_r_f1*d_x*cos_tx_angle_f1);
			h_f = h_los - sqrt(lambda*d_x*(d_tx - d_x) / d_tx);

			while (h_f < h_x)
			{
				h_r_f1 += 1;
				cos_tx_angle_f1 = ((h_r_f1*h_r_f1) + (d_tx*d_tx) - (h_t*h_t)) / (2.0*h_r_f1*d_tx);
				h_los = sqrt(h_r_f1*h_r_f1 + d_x * d_x - 2 * h_r_f1*d_x*cos_tx_angle_f1);
				h_f = h_los - sqrt(lambda*d_x*(d_tx - d_x) / d_tx);
			}

			//| and clear the 60% F1 zone.

			cos_tx_angle_fpt6 = ((h_r_fpt6*h_r_fpt6) + (d_tx*d_tx) - (h_t*h_t)) / (2.0*h_r_fpt6*d_tx);
			h_los = sqrt(h_r_fpt6*h_r_fpt6 + d_x * d_x - 2 * h_r_fpt6*d_x*cos_tx_angle_fpt6);
			h_f = h_los - fresnelZoneClearanceRatio * sqrt(lambda*d_x*(d_tx - d_x) / d_tx);

			while (h_f < h_x)
			{
				h_r_fpt6 += 1;
				cos_tx_angle_fpt6 = ((h_r_fpt6*h_r_fpt6) + (d_tx*d_tx) - (h_t*h_t)) / (2.0*h_r_fpt6*d_tx);
				h_los = sqrt(h_r_fpt6*h_r_fpt6 + d_x * d_x - 2 * h_r_fpt6*d_x*cos_tx_angle_fpt6);
				h_f = h_los - fresnelZoneClearanceRatio * sqrt(lambda*d_x*(d_tx - d_x) / d_tx);
			}
		}
	}

	if (h_r > h_r_orig)
	{
		if (useMetricUnits)
		{
			snprintf(ObsString, 150, "\nAntenna at %s must be raised to at least %.2f meters AGL\nto clear all obstructions detected by %s.\n", rcvr.name, METERS_PER_FOOT*(h_r - GetSiteLocationElevation(rcvr, digitalElevationModelWrapper) - sphereRadius), SPLAT_NAME);
		}
		else
		{
			snprintf(ObsString, 150, "\nAntenna at %s must be raised to at least %.2f feet AGL\nto clear all obstructions detected by %s.\n", rcvr.name, h_r - GetSiteLocationElevation(rcvr, digitalElevationModelWrapper) - sphereRadius, SPLAT_NAME);
		}
	}
	else
	{
		snprintf(ObsString, 150, "\nNo obstructions to LOS path due to terrain were detected by %s\n", SPLAT_NAME);
	}

	if (f)
	{
		if (h_r_fpt6 > h_r_orig)
		{
			if (useMetricUnits)
			{
				snprintf(string_fpt6, 150, "\nAntenna at %s must be raised to at least %.2f meters AGL\nto clear %.0f%c of the first Fresnel zone.\n", rcvr.name, METERS_PER_FOOT*(h_r_fpt6 - GetSiteLocationElevation(rcvr, digitalElevationModelWrapper) - sphereRadius), fresnelZoneClearanceRatio*100.0, 37);
			}
			else
			{
				snprintf(string_fpt6, 150, "\nAntenna at %s must be raised to at least %.2f feet AGL\nto clear %.0f%c of the first Fresnel zone.\n", rcvr.name, h_r_fpt6 - GetSiteLocationElevation(rcvr, digitalElevationModelWrapper) - sphereRadius, fresnelZoneClearanceRatio*100.0, 37);
			}
		}
		else
		{
			snprintf(string_fpt6, 150, "\n%.0f%c of the first Fresnel zone is clear.\n", fresnelZoneClearanceRatio*100.0, 37);
		}

		if (h_r_f1 > h_r_orig)
		{
			if (useMetricUnits)
			{
				snprintf(string_f1, 150, "\nAntenna at %s must be raised to at least %.2f meters AGL\nto clear the first Fresnel zone.\n", rcvr.name, METERS_PER_FOOT*(h_r_f1 - GetSiteLocationElevation(rcvr, digitalElevationModelWrapper) - sphereRadius));
			}
			else
			{
				snprintf(string_f1, 150, "\nAntenna at %s must be raised to at least %.2f feet AGL\nto clear the first Fresnel zone.\n", rcvr.name, h_r_f1 - GetSiteLocationElevation(rcvr, digitalElevationModelWrapper) - sphereRadius);
			}
		}
		else
		{
			snprintf(string_f1, 150, "\nThe first Fresnel zone is clear.\n");
		}
	}

	fprintf(outfile, "%s", ObsString);

	if (f)
	{
		fprintf(outfile, "%s", string_f1);
		fprintf(outfile, "%s", string_fpt6);
	}
}


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
WriteSplatPathReport
   (Site source,
	Site destination,
	char *name,
	char graph_it,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	Path *path,
	IrregularTerrainModelParameters *itmParameters,
	double *pathElevation,
	char useOldLongleyRiceModel,
	char saveGnuPlotWorkingFiles,
	unsigned char gotAntennaElevationAnglePattern,
	unsigned char gotAntennaAzimuthAnglePattern,
	unsigned char useMetricUnits,
	double groundClutterHeight,
	double fresnelZoneClearanceRatio,
	double sphereRadius,
	char *reportSavePath)
{
	int	x, y, z, errnum;
	char basename[255], term[30], ext[15], strmode[100],
		report_name[80], block = 0, propstring[20];
	double maxloss = -100000.0, minloss = 100000.0, loss, haavt,
		angle1, angle2, azimuth, pattern = 1.0, patterndB = 0.0,
		total_loss = 0.0, cos_xmtr_angle, cos_test_angle = 0.0,
		source_alt, test_alt, dest_alt, source_alt2, dest_alt2,
		distance, elevation, four_thirds_earth, field_strength,
		free_space_loss = 0.0, eirp = 0.0, voltage, rxp, dBm,
		power_density;
	FILE *fd = NULL, *fd2 = NULL;
	errno_t err;

	char dmsString[255];

	char lineOfDashes[80];	//| OLD NAME: dashes
	err = strncpy_s(lineOfDashes, _countof(lineOfDashes), "---------------------------------------------------------------------------\0", 76);

	sprintf_s(report_name, _countof(report_name), "%s-to-%s.txt", source.name, destination.name);

	char plotNameAndPath[335];
	sprintf_s(plotNameAndPath, _countof(plotNameAndPath), "%s%s", (reportSavePath[0] ? reportSavePath : ""), "splat.gp");

	char profileNameAndPath[335];
	sprintf_s(profileNameAndPath, _countof(profileNameAndPath), "%s%s", (reportSavePath[0] ? reportSavePath : ""), "profile.gp");

	char referenceNameAndPath[335];
	sprintf_s(referenceNameAndPath, _countof(referenceNameAndPath), "%s%s", (reportSavePath[0] ? reportSavePath : ""), "reference.gp");

	four_thirds_earth = FOUR_THIRDS * EARTH_RADIUS;

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

	fprintf(fd2, "\n\t\t--==[ %s v%s Path Analysis ]==--\n\n", SPLAT_NAME, SPLAT_VERSION);
	fprintf(fd2, "%s\n\n", lineOfDashes);
	fprintf(fd2, "Transmitter site: %s\n", source.name);

	if (source.lat >= 0.0)
	{
		ConvertDecimalToDegreesMinutesSeconds(source.lat, dmsString);
		fprintf(fd2, "Site location: %.4f North / %.4f West", source.lat, source.lon);
		fprintf(fd2, " (%s N / ", dmsString);
	}
	else
	{
		ConvertDecimalToDegreesMinutesSeconds(source.lat, dmsString);
		fprintf(fd2, "Site location: %.4f South / %.4f West", -source.lat, source.lon);
		fprintf(fd2, " (%s S / ", dmsString);
	}

	ConvertDecimalToDegreesMinutesSeconds(source.lon, dmsString);
	fprintf(fd2, "%s W)\n", dmsString);

	if (useMetricUnits)
	{
		fprintf(fd2, "Ground elevation: %.2f meters AMSL\n", METERS_PER_FOOT * GetSiteLocationElevation(source, digitalElevationModelWrapper));
		fprintf(fd2, "Antenna height: %.2f meters AGL / %.2f meters AMSL\n", METERS_PER_FOOT * source.alt, METERS_PER_FOOT * (source.alt + GetSiteLocationElevation(source, digitalElevationModelWrapper)));
	}
	else
	{
		fprintf(fd2, "Ground elevation: %.2f feet AMSL\n", GetSiteLocationElevation(source, digitalElevationModelWrapper));
		fprintf(fd2, "Antenna height: %.2f feet AGL / %.2f feet AMSL\n", source.alt, source.alt + GetSiteLocationElevation(source, digitalElevationModelWrapper));
	}

	haavt = AntennaHeightAboveAverageTerrain(source, digitalElevationModelWrapper, path, groundClutterHeight);

	if (haavt > -4999.0)
	{
		if (useMetricUnits)
		{
			fprintf(fd2, "Antenna height above average terrain: %.2f meters\n", METERS_PER_FOOT*haavt);
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
		x = (int)rint(10.0*(10.0 - angle2));

		if ((x >= 0) && (x <= 1000))
		{
			pattern = (double)itmParameters->antenna_pattern[(int)rint(azimuth)][x];
		}

		patterndB = 20.0*log10(pattern);
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

	//| Receiver

	fprintf(fd2, "Receiver site: %s\n", destination.name);

	if (destination.lat >= 0.0)
	{
		ConvertDecimalToDegreesMinutesSeconds(destination.lat, dmsString);
		fprintf(fd2, "Site location: %.4f North / %.4f West", destination.lat, destination.lon);
		fprintf(fd2, " (%s N / ", dmsString);
	}
	else
	{
		ConvertDecimalToDegreesMinutesSeconds(destination.lat, dmsString);
		fprintf(fd2, "Site location: %.4f South / %.4f West", -destination.lat, destination.lon);
		fprintf(fd2, " (%s S / ", dmsString);
	}

	ConvertDecimalToDegreesMinutesSeconds(destination.lon, dmsString);
	fprintf(fd2, "%s W)\n", dmsString);

	if (useMetricUnits)
	{
		fprintf(fd2, "Ground elevation: %.2f meters AMSL\n", METERS_PER_FOOT * GetSiteLocationElevation(destination, digitalElevationModelWrapper));
		fprintf(fd2, "Antenna height: %.2f meters AGL / %.2f meters AMSL\n", METERS_PER_FOOT * destination.alt, METERS_PER_FOOT * (destination.alt + GetSiteLocationElevation(destination, digitalElevationModelWrapper)));
	}
	else
	{
		fprintf(fd2, "Ground elevation: %.2f feet AMSL\n", GetSiteLocationElevation(destination, digitalElevationModelWrapper));
		fprintf(fd2, "Antenna height: %.2f feet AGL / %.2f feet AMSL\n", destination.alt, destination.alt + GetSiteLocationElevation(destination, digitalElevationModelWrapper));
	}

	haavt = AntennaHeightAboveAverageTerrain(destination, digitalElevationModelWrapper, path, groundClutterHeight);

	if (haavt > -4999.0)
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

	if (itmParameters->frq_mhz > 0.0)
	{
		if (useOldLongleyRiceModel)
		{
			fprintf(fd2, "Longley-Rice Parameters Used In This Analysis:\n\n");
		}
		else
		{
			fprintf(fd2, "ITWOM Version %.1f Parameters Used In This Analysis:\n\n", IrregularTerrainModelVersion());
		}

		fprintf(fd2, "Earth's Dielectric Constant: %.3lf\n", itmParameters->eps_dielect);
		fprintf(fd2, "Earth's Conductivity: %.3lf Siemens/meter\n", itmParameters->sgm_conductivity);
		fprintf(fd2, "Atmospheric Bending Constant (N-units): %.3lf ppm\n", itmParameters->eno_ns_surfref);
		fprintf(fd2, "Frequency: %.3lf MHz\n", itmParameters->frq_mhz);
		fprintf(fd2, "Radio Climate: %d (", itmParameters->radio_climate);

		switch (itmParameters->radio_climate)
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

		fprintf(fd2, ")\nPolarization: %d (", itmParameters->pol);

		if (itmParameters->pol == 0)
		{
			fprintf(fd2, "Horizontal");
		}

		if (itmParameters->pol == 1)
		{
			fprintf(fd2, "Vertical");
		}

		fprintf(fd2, ")\nFraction of Situations: %.1lf%c\n", itmParameters->conf*100.0, 37);
		fprintf(fd2, "Fraction of Time: %.1lf%c\n", itmParameters->rel*100.0, 37);

		if (itmParameters->erp != 0.0)
		{
			fprintf(fd2, "Transmitter ERP: ");

			if (itmParameters->erp < 1.0)
			{
				fprintf(fd2, "%.1lf milliwatts", 1000.0*itmParameters->erp);
			}

			if ((itmParameters->erp >= 1.0) && (itmParameters->erp < 10.0))
			{
				fprintf(fd2, "%.1lf Watts", itmParameters->erp);
			}

			if ((itmParameters->erp >= 10.0) && (itmParameters->erp < 10.0e3))
			{
				fprintf(fd2, "%.0lf Watts", itmParameters->erp);
			}

			if (itmParameters->erp >= 10.0e3)
			{
				fprintf(fd2, "%.3lf kilowatts", itmParameters->erp / 1.0e3);
			}

			dBm = 10.0*log10(itmParameters->erp*1000.0);
			fprintf(fd2, " (%+.2f dBm)\n", dBm);

			//| EIRP = ERP + 2.14 dB

			fprintf(fd2, "Transmitter EIRP: ");

			eirp = itmParameters->erp*1.636816521;

			if (eirp < 1.0)
			{
				fprintf(fd2, "%.1lf milliwatts", 1000.0*eirp);
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

			dBm = 10.0*log10(eirp*1000.0);
			fprintf(fd2, " (%+.2f dBm)\n", dBm);
		}

		fprintf(fd2, "\n%s\n\n", lineOfDashes);

		fprintf(fd2, "Summary For The Link Between %s and %s:\n\n", source.name, destination.name);

		if (patterndB != 0.0)
		{
			fprintf(fd2, "%s antenna pattern towards %s: %.3f (%.2f dB)\n", source.name, destination.name, pattern, patterndB);
		}

		GeneratePathBetweenSites(source, destination, digitalElevationModelWrapper, path);  //| source=TX, destination=RX

		//| Copy elevations plus clutter along
		//| path into the pathElevation[] array.

		for (x = 1; x < path->length - 1; x++)
		{
			pathElevation[x + 2] = METERS_PER_FOOT * (path->elevation[x] == 0.0 ? path->elevation[x] : (groundClutterHeight + path->elevation[x]));
		}

		//| Copy ending points without clutter

		pathElevation[2] = path->elevation[0] * METERS_PER_FOOT;
		pathElevation[path->length + 1] = path->elevation[path->length - 1] * METERS_PER_FOOT;

		err = fopen_s(&fd, profileNameAndPath, "w");

		azimuth = rint(AzimuthAngleBetweenSites(source, destination));

		for (y = 2; y < (path->length - 1); y++)  //| path->length-1 avoids itmParameters error
		{
			distance = 5280.0*path->distance[y];
			source_alt = four_thirds_earth + source.alt + path->elevation[0];
			dest_alt = four_thirds_earth + destination.alt + path->elevation[y];
			dest_alt2 = dest_alt * dest_alt;
			source_alt2 = source_alt * source_alt;

			//| Calculate the cosine of the elevation of
			//| the receiver as seen by the transmitter.

			cos_xmtr_angle = (source_alt2 + (distance*distance) - dest_alt2) / (2.0 * source_alt * distance);

			if (gotAntennaElevationAnglePattern)
			{
				//| If an antenna elevation pattern is available, the
				//| following code determines the elevation angle to
				//| the first obstruction along the path.

				for (x = 2, block = 0; (x < y) && (block == 0); x++)
				{
					distance = 5280.0 * (path->distance[y] - path->distance[x]);
					test_alt = four_thirds_earth + path->elevation[x];

					//| Calculate the cosine of the elevation
					//| angle of the terrain (test point)
					//| as seen by the transmitter.

					cos_test_angle = (source_alt2 + (distance*distance) - (test_alt*test_alt)) / (2.0 * source_alt * distance);

					//| Compare these two angles to determine if
					//| an obstruction exists. Since we're comparing
					//| the cosines of these angles rather than
					//| the angles themselves, the sense of the
					//| following "if" statement is reversed from
					//| what it would be if the angles themselves
					//| were compared.

					if (cos_xmtr_angle >= cos_test_angle)
					{
						block = 1;
					}
				}

				//| At this point, we have the elevation angle
				//| to the first obstruction (if it exists).
			}

			//| Determine path loss for each point along
			//| the path using ITWOM's PointToPointCalculation mode
			//| starting at x=2 (number_of_points = 1), the
			//| shortest distance terrain can play a role in
			//| path loss.

			pathElevation[0] = y - 1;	//| (number of points - 1)

			//| Distance between elevation samples

			pathElevation[1] = METERS_PER_MILE * (path->distance[y] - path->distance[y - 1]);

			if (useOldLongleyRiceModel)
			{
				PointToPointCalculationLegacy(pathElevation, (source.alt * METERS_PER_FOOT),
					(destination.alt * METERS_PER_FOOT), itmParameters->eps_dielect,
					itmParameters->sgm_conductivity, itmParameters->eno_ns_surfref, itmParameters->frq_mhz,
					itmParameters->radio_climate, itmParameters->pol, itmParameters->conf, itmParameters->rel, loss,
					strmode, 100, errnum);
			}
			else
			{
				PointToPointCalculation(pathElevation, (source.alt * METERS_PER_FOOT),
					(destination.alt * METERS_PER_FOOT), itmParameters->eps_dielect,
					itmParameters->sgm_conductivity, itmParameters->eno_ns_surfref, itmParameters->frq_mhz,
					itmParameters->radio_climate, itmParameters->pol, itmParameters->conf, itmParameters->rel, loss,
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

			//| Integrate the antenna's radiation
			//| pattern into the overall path loss.

			x = (int)rint(10.0*(10.0 - elevation));

			if ((x >= 0) && (x <= 1000))
			{
				pattern = (double)itmParameters->antenna_pattern[(int)azimuth][x];

				if (pattern != 0.0)
				{
					patterndB = 20.0*log10(pattern);
				}
			}
			else
			{
				patterndB = 0.0;
			}

			total_loss = loss - patterndB;

			if (useMetricUnits)
			{
				fprintf(fd, "%f\t%f\n", KM_PER_MILE*path->distance[y], total_loss);
			}
			else
			{
				fprintf(fd, "%f\t%f\n", path->distance[y], total_loss);
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
			free_space_loss = 36.6 + (20.0*log10(itmParameters->frq_mhz)) + (20.0*log10(distance));

			fprintf(fd2, "Free space path loss: %.2f dB\n", free_space_loss);
		}

		if (useOldLongleyRiceModel)
		{
			fprintf(fd2, "Longley-Rice path loss: %.2f dB\n", loss);
		}
		else
		{
			fprintf(fd2, "ITWOM Version %.1f path loss: %.2f dB\n", IrregularTerrainModelVersion(), loss);
		}

		if (free_space_loss != 0.0)
		{
			fprintf(fd2, "Attenuation due to terrain shielding: %.2f dB\n", loss - free_space_loss);
		}

		if (patterndB != 0.0)
		{
			fprintf(fd2, "Total path loss including %s antenna pattern: %.2f dB\n", source.name, total_loss);
		}

		if (itmParameters->erp != 0.0)
		{
			field_strength = (139.4 + (20.0*log10(itmParameters->frq_mhz)) - total_loss) + (10.0*log10(itmParameters->erp / 1000.0));

			//| dBm is referenced to EIRP

			rxp = eirp / (pow(10.0, (total_loss / 10.0)));
			dBm = 10.0*(log10(rxp*1000.0));
			power_density = (eirp / (pow(10.0, (total_loss - free_space_loss) / 10.0)));
			//| divide by 4*PI*distance_in_meters squared
			power_density /= (4.0*PI*distance*distance*2589988.11);

			fprintf(fd2, "Field strength at %s: %.2f dBuV/meter\n", destination.name, field_strength);
			fprintf(fd2, "Signal power level at %s: %+.2f dBm\n", destination.name, dBm);
			fprintf(fd2, "Signal power density at %s: %+.2f dBW per square meter\n", destination.name, 10.0*log10(power_density));
			voltage = 1.0e6*sqrt(50.0*(eirp / (pow(10.0, (total_loss - 2.14) / 10.0))));
			fprintf(fd2, "Voltage across a 50 ohm dipole at %s: %.2f uV (%.2f dBuV)\n", destination.name, voltage, 20.0*log10(voltage));

			voltage = 1.0e6*sqrt(75.0*(eirp / (pow(10.0, (total_loss - 2.14) / 10.0))));
			fprintf(fd2, "Voltage across a 75 ohm dipole at %s: %.2f uV (%.2f dBuV)\n", destination.name, voltage, 20.0*log10(voltage));
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

	PerformObstructionAnalysisBetweenSites(source, destination, itmParameters->frq_mhz, fd2, digitalElevationModelWrapper, path,
		useMetricUnits, groundClutterHeight, fresnelZoneClearanceRatio, sphereRadius);

	fclose(fd2);

	//| Skip plotting the graph if ONLY a path-loss report is needed.

	if (graph_it)
	{
		if (name[0] == '.')
		{
			//| Default filename and output file type

			err = strncpy_s(basename, _countof(basename), "profile\0", 8);
			err = strncpy_s(term, _countof(term), "png\0", 4);
			err = strncpy_s(ext, _countof(ext), "png\0", 4);
		}
		else
		{
			//| Extract extension and terminal type from "name"

			ext[0] = 0;
			y = (int)strlen(name);
			err = strncpy_s(basename, _countof(basename), name, 254);

			for (x = y - 1; (x > 0) && (name[x] != '.'); x--);

			if (x > 0)  //| Extension found
			{
				for (z = x + 1; (z <= y) && ((z - (x + 1)) < 10); z++)
				{
					ext[z - (x + 1)] = tolower(name[z]);
					term[z - (x + 1)] = name[z];
				}

				ext[z - (x + 1)] = 0;  //| Ensure an ending 0
				term[z - (x + 1)] = 0;
				basename[x] = 0;
			}
		}

		if (ext[0] == 0)	//| No extension -- Default is png
		{
			err = strncpy_s(term, _countof(term), "png\0", 4);
			err = strncpy_s(ext, _countof(ext), "png\0", 4);
		}

		//| Either .ps or .postscript may be used
		//| as an extension for postscript output.

		if (strncmp(term, "postscript", 10) == 0)
		{
			err = strncpy_s(ext, _countof(ext), "ps\0", 3);
		}
		else if (strncmp(ext, "ps", 2) == 0)
		{
			err = strncpy_s(term, _countof(term), "postscript enhanced color\0", 26);
		}

		err = fopen_s(&fd, plotNameAndPath, "w");

		fprintf(fd, "set grid\n");
		fprintf(fd, "set yrange [%2.3f to %2.3f]\n", minloss, maxloss);
		fprintf(fd, "set encoding iso_8859_1\n");
		fprintf(fd, "set term %s\n", term);
		fprintf(fd, "set title \"%s Loss Profile Along Path Between %s and %s (%.2f%c azimuth)\"\n", SPLAT_NAME, destination.name, source.name, AzimuthAngleBetweenSites(destination, source), 176);

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
				fprintf(fd, "set ylabel \"ITWOM Version %.1f Path Loss (dB)", IrregularTerrainModelVersion());
			}
		}

		fprintf(fd, "\"\nset output \"%s.%s\"\n", basename, ext);
		fprintf(fd, "plot \"%s\" title \"Path Loss\" with lines\n", profileNameAndPath);

		fclose(fd);

		char gnuPlotAndSplat[335];
		sprintf_s(gnuPlotAndSplat, _countof(gnuPlotAndSplat), "run gnuplot %s", plotNameAndPath);

		//| Invokve gnuplot and run the splat.gp script.
		x = system(gnuPlotAndSplat);

		if (x != -1)
		{
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
WriteSplatSiteReport
   (Site xmtr,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	Path *path,
	unsigned char useMetricUnits,
	double groundClutterHeight,
	char *reportSavePath)
{
	char report_name[80];
	double terrain;
	int	x, azi;
	FILE *fd;
	errno_t err;

	char dmsString[255];

	char lineOfDashes[80];	//| OLD NAME: dashes
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

	fprintf(fd, "\n\t--==[ %s v%s Site Analysis Report For: %s ]==--\n\n", SPLAT_NAME, SPLAT_VERSION, xmtr.name);

	fprintf(fd, "%s\n\n", lineOfDashes);

	if (xmtr.lat >= 0.0)
	{
		ConvertDecimalToDegreesMinutesSeconds(xmtr.lat, dmsString);
		fprintf(fd, "Site location: %.4f North / %.4f West", xmtr.lat, xmtr.lon);
		fprintf(fd, " (%s N / ", dmsString);
	}
	else
	{
		ConvertDecimalToDegreesMinutesSeconds(xmtr.lat, dmsString);
		fprintf(fd, "Site location: %.4f South / %.4f West", -xmtr.lat, xmtr.lon);
		fprintf(fd, " (%s S / ", dmsString);
	}

	ConvertDecimalToDegreesMinutesSeconds(xmtr.lon, dmsString);
	fprintf(fd, "%s W)\n", dmsString);

	if (useMetricUnits)
	{
		fprintf(fd, "Ground elevation: %.2f meters AMSL\n", METERS_PER_FOOT * GetSiteLocationElevation(xmtr, digitalElevationModelWrapper));
		fprintf(fd, "Antenna height: %.2f meters AGL / %.2f meters AMSL\n", METERS_PER_FOOT * xmtr.alt, METERS_PER_FOOT * (xmtr.alt + GetSiteLocationElevation(xmtr, digitalElevationModelWrapper)));
	}
	else
	{
		fprintf(fd, "Ground elevation: %.2f feet AMSL\n", GetSiteLocationElevation(xmtr, digitalElevationModelWrapper));
		fprintf(fd, "Antenna height: %.2f feet AGL / %.2f feet AMSL\n", xmtr.alt, xmtr.alt + GetSiteLocationElevation(xmtr, digitalElevationModelWrapper));
	}

	terrain = AntennaHeightAboveAverageTerrain(xmtr, digitalElevationModelWrapper, path, groundClutterHeight);

	if (terrain > -4999.0)
	{
		if (useMetricUnits)
		{
			fprintf(fd, "Antenna height above average terrain: %.2f meters\n\n", METERS_PER_FOOT * terrain);
		}
		else
		{
			fprintf(fd, "Antenna height above average terrain: %.2f feet\n\n", terrain);
		}

		//| Display the average terrain between 2 and 10 miles
		//| from the transmitter site at azimuths of 0, 45, 90,
		//| 135, 180, 225, 270, and 315 degrees.

		for (azi = 0; azi <= 315; azi += 45)
		{
			fprintf(fd, "Average terrain at %3d degrees azimuth: ", azi);
			terrain = AverageTerrainOverDistanceAtAzimuthFromSite(xmtr, (double)azi, 2.0, 10.0, digitalElevationModelWrapper, path, groundClutterHeight);

			if (terrain > -4999.0)
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
	fprintf(stdout, "\nSite analysis report written to: \"%s\"\n", reportNameAndPath);
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
LoadSplatDataFilesForRegion
   (int max_lon,
	int min_lon,
	int max_lat,
	int min_lat,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	int *maximumElevation,
	int *minimumElevation,
	int *minimumLatitudeNorth,
	int *maximumLatitudeNorth,
	int *minimumLongitudeWest,
	int *maximumLongitudeWest,
	char *splatDataFilePath)
{
	int x, y, width, ymin, ymax;
	char nameString[255];	//| OLD NAME: string

	width = ConvertToNormalizedAngle(max_lon - min_lon);

	if ((max_lon - min_lon) <= 180.0)
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

				if (digitalElevationModelWrapper->demPixelsPerDegree == 3600)
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

				if (digitalElevationModelWrapper->demPixelsPerDegree == 3600)
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
LoadSplatAlphanumericOutputFile
   (char *filename,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	IrregularTerrainModelParameters *itmParameters,
	unsigned char plotSignalPowerLevelContours,
	int contourDisplayThreshold,
	int *maximumElevation,
	int *minimumElevation,
	int *minimumLatitudeNorth,
	int *maximumLatitudeNorth,
	int *minimumLongitudeWest,
	int *maximumLongitudeWest,
	char *splatDataFilePath)
{
	int	error = 0, max_west, min_west, max_north, min_north;
	char AnoString[80], *pointer = NULL;
	double latitude = 0.0, longitude = 0.0, azimuth = 0.0, elevation = 0.0,
		ano = 0.0;
	FILE *fd;
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
			if (itmParameters->erp == 0.0)
			{
				//| Path loss

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

			if ((itmParameters->erp != 0.0) && (plotSignalPowerLevelContours != 0))
			{
				//| signal power level in dBm

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

			if ((itmParameters->erp != 0.0) && (plotSignalPowerLevelContours == 0))
			{
				//| field strength dBuV/m

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
//| FUNCTION: WriteKeyholeMarkupLanguageFile
//| 
//| OLD NAME: WriteKML
//| 
//| NOTES: 
//| 
//| ------------------------------
void
WriteKeyholeMarkupLanguageFile
   (Site source,
	Site destination,
	DigitalElevationModelWrapper *digitalElevationModelWrapper,
	Path *path,
	unsigned char useMetricUnits,
	double sphereRadius)
{
	int	x, y;
	char block, report_name[80];
	double distance, rx_alt, tx_alt, cos_xmtr_angle,
		azimuth, cos_test_angle, test_alt;
	FILE *fd = NULL;
	errno_t err;

	char dmsString[255];

	GeneratePathBetweenSites(source, destination, digitalElevationModelWrapper, path);

	sprintf_s(report_name, _countof(report_name), "%s-to-%s.kml", source.name, destination.name);

	for (x = 0; report_name[x] != 0; x++)
	{
		if ((report_name[x] == 32) || (report_name[x] == 17) || (report_name[x] == 92) || (report_name[x] == 42) || (report_name[x] == 47))
		{
			report_name[x] = '_';
		}
	}

	err = fopen_s(&fd, report_name, "w");

	fprintf(fd, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(fd, "<kml xmlns=\"http://earth.google.com/kml/2.0\">\n");
	fprintf(fd, "<!-- Generated by %s Version %s -->\n", SPLAT_NAME, SPLAT_VERSION);
	fprintf(fd, "<Folder>\n");
	fprintf(fd, "<name>SPLAT Path</name>\n");
	fprintf(fd, "<open>1</open>\n");
	fprintf(fd, "<description>Path Between %s and %s</description>\n", source.name, destination.name);

	fprintf(fd, "<Placemark>\n");
	fprintf(fd, "    <name>%s</name>\n", source.name);
	fprintf(fd, "    <description>\n");
	fprintf(fd, "       Transmit Site\n");

	if (source.lat >= 0.0)
	{
		ConvertDecimalToDegreesMinutesSeconds(source.lat, dmsString);
		fprintf(fd, "       <BR>%s North</BR>\n", dmsString);
	}
	else
	{
		ConvertDecimalToDegreesMinutesSeconds(source.lat, dmsString);
		fprintf(fd, "       <BR>%s South</BR>\n", dmsString);
	}

	ConvertDecimalToDegreesMinutesSeconds(source.lon, dmsString);
	fprintf(fd, "       <BR>%s West</BR>\n", dmsString);

	azimuth = AzimuthAngleBetweenSites(source, destination);
	distance = GreatCircleDistanceBetweenSiteLocations(source, destination);

	if (useMetricUnits)
	{
		fprintf(fd, "       <BR>%.2f km", distance * KM_PER_MILE);
	}
	else
	{
		fprintf(fd, "       <BR>%.2f miles", distance);
	}

	fprintf(fd, " to %s</BR>\n       <BR>toward an azimuth of %.2f%c</BR>\n", destination.name, azimuth, 176);

	fprintf(fd, "    </description>\n");
	fprintf(fd, "    <visibility>1</visibility>\n");
	fprintf(fd, "    <Style>\n");
	fprintf(fd, "      <IconStyle>\n");
	fprintf(fd, "        <Icon>\n");
	fprintf(fd, "          <href>root://icons/palette-5.png</href>\n");
	fprintf(fd, "          <x>224</x>\n");
	fprintf(fd, "          <y>224</y>\n");
	fprintf(fd, "          <w>32</w>\n");
	fprintf(fd, "          <h>32</h>\n");
	fprintf(fd, "        </Icon>\n");
	fprintf(fd, "      </IconStyle>\n");
	fprintf(fd, "    </Style>\n");
	fprintf(fd, "    <Point>\n");
	fprintf(fd, "      <extrude>1</extrude>\n");
	fprintf(fd, "      <altitudeMode>relativeToGround</altitudeMode>\n");
	fprintf(fd, "      <coordinates>%f,%f,30</coordinates>\n", (source.lon < 180.0 ? -source.lon : 360.0 - source.lon), source.lat);
	fprintf(fd, "    </Point>\n");
	fprintf(fd, "</Placemark>\n");

	fprintf(fd, "<Placemark>\n");
	fprintf(fd, "    <name>%s</name>\n", destination.name);
	fprintf(fd, "    <description>\n");
	fprintf(fd, "       Receive Site\n");

	if (destination.lat >= 0.0)
	{
		ConvertDecimalToDegreesMinutesSeconds(destination.lat, dmsString);
		fprintf(fd, "       <BR>%s North</BR>\n", dmsString);
	}
	else
	{
		ConvertDecimalToDegreesMinutesSeconds(destination.lat, dmsString);
		fprintf(fd, "       <BR>%s South</BR>\n", dmsString);
	}

	ConvertDecimalToDegreesMinutesSeconds(destination.lon, dmsString);
	fprintf(fd, "       <BR>%s West</BR>\n", dmsString);

	if (useMetricUnits)
	{
		fprintf(fd, "       <BR>%.2f km", distance * KM_PER_MILE);
	}
	else
	{
		fprintf(fd, "       <BR>%.2f miles", distance);
	}

	fprintf(fd, " to %s</BR>\n       <BR>toward an azimuth of %.2f%c</BR>\n", source.name, AzimuthAngleBetweenSites(destination, source), 176);

	fprintf(fd, "    </description>\n");
	fprintf(fd, "    <visibility>1</visibility>\n");
	fprintf(fd, "    <Style>\n");
	fprintf(fd, "      <IconStyle>\n");
	fprintf(fd, "        <Icon>\n");
	fprintf(fd, "          <href>root://icons/palette-5.png</href>\n");
	fprintf(fd, "          <x>224</x>\n");
	fprintf(fd, "          <y>224</y>\n");
	fprintf(fd, "          <w>32</w>\n");
	fprintf(fd, "          <h>32</h>\n");
	fprintf(fd, "        </Icon>\n");
	fprintf(fd, "      </IconStyle>\n");
	fprintf(fd, "    </Style>\n");
	fprintf(fd, "    <Point>\n");
	fprintf(fd, "      <extrude>1</extrude>\n");
	fprintf(fd, "      <altitudeMode>relativeToGround</altitudeMode>\n");
	fprintf(fd, "      <coordinates>%f,%f,30</coordinates>\n", (destination.lon < 180.0 ? -destination.lon : 360.0 - destination.lon), destination.lat);
	fprintf(fd, "    </Point>\n");
	fprintf(fd, "</Placemark>\n");

	fprintf(fd, "<Placemark>\n");
	fprintf(fd, "<name>Point-to-Point Path</name>\n");
	fprintf(fd, "  <visibility>1</visibility>\n");
	fprintf(fd, "  <open>0</open>\n");
	fprintf(fd, "  <Style>\n");
	fprintf(fd, "    <LineStyle>\n");
	fprintf(fd, "      <color>7fffffff</color>\n");
	fprintf(fd, "    </LineStyle>\n");
	fprintf(fd, "    <PolyStyle>\n");
	fprintf(fd, "       <color>7fffffff</color>\n");
	fprintf(fd, "    </PolyStyle>\n");
	fprintf(fd, "  </Style>\n");
	fprintf(fd, "  <LineString>\n");
	fprintf(fd, "    <extrude>1</extrude>\n");
	fprintf(fd, "    <tessellate>1</tessellate>\n");
	fprintf(fd, "    <altitudeMode>relativeToGround</altitudeMode>\n");
	fprintf(fd, "    <coordinates>\n");

	for (x = 0; x < path->length; x++)
	{
		fprintf(fd, "      %f,%f,5\n", (path->lon[x] < 180.0 ? -path->lon[x] : 360.0 - path->lon[x]), path->lat[x]);
	}

	fprintf(fd, "    </coordinates>\n");
	fprintf(fd, "   </LineString>\n");
	fprintf(fd, "</Placemark>\n");

	fprintf(fd, "<Placemark>\n");
	fprintf(fd, "<name>Line-of-Sight Path</name>\n");
	fprintf(fd, "  <visibility>1</visibility>\n");
	fprintf(fd, "  <open>0</open>\n");
	fprintf(fd, "  <Style>\n");
	fprintf(fd, "    <LineStyle>\n");
	fprintf(fd, "      <color>ff00ff00</color>\n");
	fprintf(fd, "    </LineStyle>\n");
	fprintf(fd, "    <PolyStyle>\n");
	fprintf(fd, "       <color>7f00ff00</color>\n");
	fprintf(fd, "    </PolyStyle>\n");
	fprintf(fd, "  </Style>\n");
	fprintf(fd, "  <LineString>\n");
	fprintf(fd, "    <extrude>1</extrude>\n");
	fprintf(fd, "    <tessellate>1</tessellate>\n");
	fprintf(fd, "    <altitudeMode>relativeToGround</altitudeMode>\n");
	fprintf(fd, "    <coordinates>\n");

	//| Walk across the "path", indentifying obstructions along the way

	for (y = 0; y < path->length; y++)
	{
		distance = 5280.0*path->distance[y];
		tx_alt = sphereRadius + source.alt + path->elevation[0];
		rx_alt = sphereRadius + destination.alt + path->elevation[y];

		//| Calculate the cosine of the elevation of the
		//| transmitter as seen at the temp rx point.

		cos_xmtr_angle = ((rx_alt*rx_alt) + (distance*distance) - (tx_alt*tx_alt)) / (2.0*rx_alt*distance);

		for (x = y, block = 0; (x >= 0) && (block == 0); x--)
		{
			distance = 5280.0*(path->distance[y] - path->distance[x]);
			test_alt = sphereRadius + path->elevation[x];

			cos_test_angle = ((rx_alt*rx_alt) + (distance*distance) - (test_alt*test_alt)) / (2.0*rx_alt*distance);

			//| Compare these two angles to determine if
			//| an obstruction exists. Since we're comparing
			//| the cosines of these angles rather than
			//| the angles themselves, the following "if"
			//| statement is reversed from what it would
			//| be if the actual angles were compared.

			if (cos_xmtr_angle >= cos_test_angle)
			{
				block = 1;
			}
		}

		if (block)
		{
			fprintf(fd, "      %f,%f,-30\n", (path->lon[y] < 180.0 ? -path->lon[y] : 360.0 - path->lon[y]), path->lat[y]);
		}
		else
		{
			fprintf(fd, "      %f,%f,5\n", (path->lon[y] < 180.0 ? -path->lon[y] : 360.0 - path->lon[y]), path->lat[y]);
		}
	}

	fprintf(fd, "    </coordinates>\n");
	fprintf(fd, "  </LineString>\n");
	fprintf(fd, "</Placemark>\n");

	fprintf(fd, "    <LookAt>\n");
	fprintf(fd, "      <longitude>%f</longitude>\n", (source.lon < 180.0 ? -source.lon : 360.0 - source.lon));
	fprintf(fd, "      <latitude>%f</latitude>\n", source.lat);
	fprintf(fd, "      <range>300.0</range>\n");
	fprintf(fd, "      <tilt>45.0</tilt>\n");
	fprintf(fd, "      <heading>%f</heading>\n", azimuth);
	fprintf(fd, "    </LookAt>\n");

	fprintf(fd, "</Folder>\n");
	fprintf(fd, "</kml>\n");

	fclose(fd);

	fprintf(stdout, "\nKML file written to: \"%s\"", report_name);

	fflush(stdout);
}
