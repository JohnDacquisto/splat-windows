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

	int min_north;
	int max_north;
	int min_west;
	int max_west;
	int max_el;
	int min_el;

	short **data;
	unsigned char **mask;
	unsigned char **signal;

	void AllocateArrays(int pixelsPerDegree);
	void DeallocateArrays(int pixelsPerDegree);
};
