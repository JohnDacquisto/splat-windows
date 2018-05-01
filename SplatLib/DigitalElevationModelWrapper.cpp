/****************************************************************************
*					Class: DigitalElevationModelWrapper		                *
****************************************************************************/

#include "stdafx.h"
#include "DigitalElevationModelWrapper.h"


DigitalElevationModelWrapper::DigitalElevationModelWrapper()
	: demPixelsPerDegree(0),
	  maxPages(0),
	  digitalElevationModel(NULL)
{
}


DigitalElevationModelWrapper::DigitalElevationModelWrapper
   (int pages,
	int pixelsPerDegree)
{
	maxPages = pages;
	demPixelsPerDegree = pixelsPerDegree;

	digitalElevationModel = new DigitalElevationModel[maxPages];	//| OLD NAME: dem

	for (int i = 0; i < maxPages; i++)
	{
		digitalElevationModel[i].AllocateArrays(demPixelsPerDegree);

		digitalElevationModel[i].min_el = 32768;
		digitalElevationModel[i].max_el = -32768;
		digitalElevationModel[i].min_north = 90;
		digitalElevationModel[i].max_north = -90;
		digitalElevationModel[i].min_west = 360;
		digitalElevationModel[i].max_west = -1;
	}
}


DigitalElevationModelWrapper::~DigitalElevationModelWrapper()
{
	if (digitalElevationModel)
	{
		for (int i = 0; i < this->maxPages; i++)
		{
			digitalElevationModel[i].DeallocateArrays(demPixelsPerDegree);
		}

		delete[] digitalElevationModel;
		digitalElevationModel = NULL;
	}
}
