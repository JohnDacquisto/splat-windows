/****************************************************************************
*							Class: Path						                *
****************************************************************************/

#pragma once
#include "stdafx.h"
#include "../Common/constants.h"


class _declspec(dllexport) Path
{
public:
	Path();
	~Path();

	double lat[ARRAY_SIZE];
	double lon[ARRAY_SIZE];
	double elevation[ARRAY_SIZE];
	double distance[ARRAY_SIZE];
	int length;
};
