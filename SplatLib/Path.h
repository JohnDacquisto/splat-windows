/****************************************************************************
*							Class: Path						                *
****************************************************************************/

#pragma once
#include "stdafx.h"


class _declspec(dllexport) Path
{
public:
	Path();
	Path(long arraySize);
	~Path();

	double *lat;
	double *lon;
	double *elevation;
	double *distance;

	long pathArraySize;

	int length;
};
