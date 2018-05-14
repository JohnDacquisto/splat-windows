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

	double latitude;		//| OLD NAME: lat
	double longitude;		//| OLD NAME: lon
	float altitude;			//| OLD NAME: alt
	double azimuth;
	char name[50];
	char filename[255];
};
