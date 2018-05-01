/****************************************************************************
*							Class: Path						                *
****************************************************************************/

#include "stdafx.h"
#include "Path.h"


Path::Path()
	: pathArraySize(0),
	  lat(NULL),
      lon(NULL),
	  elevation(NULL),
	  distance(NULL)
{
}


Path::Path
   (long arraySize)
	: pathArraySize(arraySize)
{
	lat = new double[pathArraySize];

	lon = new double[pathArraySize];

	elevation = new double[pathArraySize];

	distance = new double[pathArraySize];
}


Path::~Path()
{
	if (lat)
	{
		delete[] lat;
		lat = NULL;
	}

	if (lon)
	{
		delete[] lon;
		lon = NULL;
	}

	if (elevation)
	{
		delete[] elevation;
		elevation = NULL;
	}

	if (distance)
	{
		delete[] distance;
		distance = NULL;
	}
}
