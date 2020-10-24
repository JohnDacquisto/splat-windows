/****************************************************************************\
*					SplatLib.Math: Math Functions							 *
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
LinearInterpolation(
	int y0,
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

	delta_y = ToDouble(y0 - y1);
	delta_x = ToDouble(x0 - x1);

	result = y0 + (int)ceil((delta_y / delta_x) * ToDouble(n - x0));

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
ConvertToNormalizedAngle(
	double angle)
{
	double temp;

	temp = acos(cos(angle * DEGREES_TO_RADIANS));

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
LongitudeDifference(
	double lon1,
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
ConvertDecimalToDegreesMinutesSeconds(
	double decimal,
	char* dmsString)
{
	int	degrees, minutes, seconds, sign;
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
	b = 60.0 * (decimal - a);
	c = floor(b);
	d = 60.0 * (b - c);

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
	snprintf(dmsString, 250, "%d%c %d\' %d\"", degrees * sign, 176, minutes, seconds);
}
