#pragma once

//| JAD FIX
#include "C:/DEV/SPLAT/Common/constants.h"


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
	short data[INTEGER_PIXELS_PER_DEGREE][INTEGER_PIXELS_PER_DEGREE];
	unsigned char mask[INTEGER_PIXELS_PER_DEGREE][INTEGER_PIXELS_PER_DEGREE];
	unsigned char signal[INTEGER_PIXELS_PER_DEGREE][INTEGER_PIXELS_PER_DEGREE];
};
