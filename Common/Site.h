/****************************************************************************
*							Class: Site						                *
****************************************************************************/

#pragma once
#include "stdafx.h"


class _declspec(dllexport) Site
{
public:
	Site();
	~Site();

	double lat;
	double lon;
	float alt;
	double azimuth;
	char name[50];
	char filename[255];
};
