/****************************************************************************
*							Class: Path						                *
****************************************************************************/

#pragma once

#include "stdafx.h"


class _declspec(dllexport) Path
{
public:
	Path();
	Path(
		long arraySize);
	~Path();

	double *latitudes;		// OLD NAME: lat
	double *longitudes;		// OLD NAME: lon
	double *elevations;		// OLD NAME: elevation
	double *distances;		// OLD NAME: distance

	long pathArraySize;

	int pathLength;			// OLD NAME: length
};
