#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "constants.h"
#include "Common.h"
#include "Site.h"


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


