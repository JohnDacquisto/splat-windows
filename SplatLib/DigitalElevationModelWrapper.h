/****************************************************************************
*					Class: DigitalElevationModelWrapper		                *
****************************************************************************/

#pragma once
#include "stdafx.h"
#include "DigitalElevationModel.h"


class _declspec(dllexport) DigitalElevationModelWrapper
{
public:
	DigitalElevationModelWrapper();
	DigitalElevationModelWrapper(int pages, int pixelsPerDegree);
	~DigitalElevationModelWrapper();

	DigitalElevationModel *digitalElevationModel;
	int maxPages;
	int demPixelsPerDegree;
};
