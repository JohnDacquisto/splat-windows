/****************************************************************************\
*						Common: Shared functions							 *
*****************************************************************************/

#include "stdafx.h"
#include <math.h>
#include "Common.h"
#include "constants.h"


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

	lat1 = site1.lat * DEGREES_TO_RADIANS;
	lon1 = site1.lon * DEGREES_TO_RADIANS;
	lat2 = site2.lat * DEGREES_TO_RADIANS;
	lon2 = site2.lon * DEGREES_TO_RADIANS;

	distance = EARTH_RADIUS_MILES * acos(sin(lat1)*sin(lat2) + cos(lat1)*cos(lat2)*cos(lon1 - lon2));

	return distance;
}
