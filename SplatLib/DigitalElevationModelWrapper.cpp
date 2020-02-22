/****************************************************************************
*					Class: DigitalElevationModelWrapper		                *
****************************************************************************/

#include "stdafx.h"
#include "DigitalElevationModelWrapper.h"


DigitalElevationModelWrapper::DigitalElevationModelWrapper() :
	demPixelsPerDegree(0),
	maxPages(0),
	digitalElevationModel(NULL)
{
}


DigitalElevationModelWrapper::DigitalElevationModelWrapper(
	int pages,
	int pixelsPerDegree)
{
	maxPages = pages;
	demPixelsPerDegree = pixelsPerDegree;

	digitalElevationModel = new DigitalElevationModel[maxPages];	// OLD NAME: dem

	for (int i = 0; i < maxPages; i++)
	{
		digitalElevationModel[i].AllocateArrays(demPixelsPerDegree);

		digitalElevationModel[i].minimumElevation = 32768;
		digitalElevationModel[i].maximumElevation = -32768;
		digitalElevationModel[i].minimumLatitudeNorth = 90;
		digitalElevationModel[i].maximumLatitudeNorth = -90;
		digitalElevationModel[i].minimumLongitudeWest = 360;
		digitalElevationModel[i].maximumLongitudeWest = -1;
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
