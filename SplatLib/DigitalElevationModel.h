/****************************************************************************
*						Class: DigitalElevationModel		                *
****************************************************************************/

#pragma once

#include "stdafx.h"


class DigitalElevationModel
{
public:
	DigitalElevationModel();
	~DigitalElevationModel();

	void AllocateArrays(int pixelsPerDegree);
	void DeallocateArrays(int pixelsPerDegree);

	int minimumLatitudeNorth;	// OLD NAME: min_north
	int maximumLatitudeNorth;	// OLD NAME: max_north
	int minimumLongitudeWest;	// OLD NAME: min_west
	int maximumLongitudeWest;	// OLD NAME: max_west
	int maximumElevation;		// OLD NAME: max_el
	int minimumElevation;		// OLD NAME: min_el

	short **data;
	unsigned char **mask;
	unsigned char **signal;
};
