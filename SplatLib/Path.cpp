/****************************************************************************
*							Class: Path						                *
****************************************************************************/

#include "stdafx.h"
#include "Path.h"


Path::Path() :
	pathArraySize(0),
	latitudes(NULL),
	longitudes(NULL),
	elevations(NULL),
	distances(NULL)
{
}


Path::Path(
	long arraySize) :
	pathArraySize(arraySize)
{
	latitudes = new double[pathArraySize];

	longitudes = new double[pathArraySize];

	elevations = new double[pathArraySize];

	distances = new double[pathArraySize];
}


Path::~Path()
{
	if (latitudes)
	{
		delete[] latitudes;
		latitudes = NULL;
	}

	if (longitudes)
	{
		delete[] longitudes;
		longitudes = NULL;
	}

	if (elevations)
	{
		delete[] elevations;
		elevations = NULL;
	}

	if (distances)
	{
		delete[] distances;
		distances = NULL;
	}
}
