/****************************************************************************
*						Class: DigitalElevationModel		                *
****************************************************************************/

#include "stdafx.h"
#include "DigitalElevationModel.h"


DigitalElevationModel::DigitalElevationModel() : 
	data(NULL),
	mask(NULL),
	signal(NULL)
{
}


DigitalElevationModel::~DigitalElevationModel()
{
}


void 
DigitalElevationModel::AllocateArrays(
	int pixelsPerDegree)
{
	data = new short*[pixelsPerDegree];
	for (int i = 0; i < pixelsPerDegree; i++)
	{
		data[i] = new short[pixelsPerDegree];
	}

	mask = new unsigned char*[pixelsPerDegree];
	for (int i = 0; i < pixelsPerDegree; i++)
	{
		mask[i] = new unsigned char[pixelsPerDegree];
	}

	signal = new unsigned char*[pixelsPerDegree];
	for (int i = 0; i < pixelsPerDegree; i++)
	{
		signal[i] = new unsigned char[pixelsPerDegree];
	}
}


void
DigitalElevationModel::DeallocateArrays(
	int pixelsPerDegree)
{
	if (data)
	{
		for (int i = 0; i < pixelsPerDegree; i++)
		{
			delete[] data[i];
			data[i] = NULL;
		}

		delete[] data;
		data = NULL;
	}

	if (mask)
	{
		for (int i = 0; i < pixelsPerDegree; i++)
		{
			delete[] mask[i];
			mask[i] = NULL;
		}

		delete[] mask;
		mask = NULL;
	}

	if (signal)
	{
		for (int i = 0; i < pixelsPerDegree; i++)
		{
			delete[] signal[i];
			signal[i] = NULL;
		}

		delete[] signal;
		signal = NULL;
	}
}
