/***************************************************************************\
*					BearingLib: Functions used by Bearing					*
****************************************************************************/

#include "stdafx.h"
#include <math.h>
#include "BearingLib.h"
#include "..\Common\constants.h"
#include "..\Common\Site.h"


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

	distance = 3959.0 * acos(sin(lat1)*sin(lat2) + cos(lat1)*cos(lat2)*cos((lon1)-(lon2)));

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
	char ReadString[20];
	int	a, b, length, degrees, minutes;

	//| Copy "input" to "string", and ignore any extra
	//| spaces that might be present in the process.

	ReadString[0] = 0;
	length = (int)strlen(input);

	for (a = 0, b = 0; (a < length) && (a < 18); a++)
	{
		if (((input[a] != 32) && (input[a] != '\n')) || ((input[a] == 32) && (input[a + 1] != 32) && (input[a + 1] != '\n') && (b != 0)))
		{
			ReadString[b] = input[a];
			b++;
		}
	}

	ReadString[b] = 0;

	//| Count number of spaces in the clean string.

	length = (int)strlen(ReadString);

	for (a = 0, b = 0; a < length; a++)
	{
		if (ReadString[a] == 32)
		{
			b++;
		}
	}

	if (b == 0)  //| Decimal Format (40.139722)
	{
		sscanf_s(ReadString, "%lf", &bearing);
	}

	if (b == 2)  //| Degree, Minute, Second Format (40 08 23.xx)
	{
		sscanf_s(ReadString, "%d %d %lf", &degrees, &minutes, &seconds);

		bearing = fabs((double)degrees);
		bearing += fabs(((double)minutes) / 60.0);
		bearing += fabs(seconds / 3600.0);

		if ((degrees < 0) || (minutes < 0) || (seconds < 0.0))
		{
			bearing = -bearing;
		}
	}

	//| Anything else returns a 0.0

	if ((bearing > 360.0) || (bearing < -90.0))
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

	for (x = 0; (filename[x] != '.') && (filename[x] != 0) && (x < 250); x++)
	{
		qthfile[x] = filename[x];
	}

	qthfile[x] = '.';
	qthfile[x + 1] = 'q';
	qthfile[x + 2] = 't';
	qthfile[x + 3] = 'h';
	qthfile[x + 4] = 0;

	tempsite.lat = 91.0;
	tempsite.lon = 361.0;
	tempsite.name[0] = 0;
	tempsite.azimuth = 0.0;

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

		fclose(fd);
	}

	return tempsite;
}
