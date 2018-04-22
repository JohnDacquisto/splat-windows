/***************************************************************************\
*					BearingLib: Functions used by Bearing					*
****************************************************************************/

#pragma once
#include "stdafx.h"
#include "..\Common\Site.h"


//| Export functions

double _declspec(dllexport) GreatCircleDistanceBetweenSiteLocations(Site site1, Site site2);

double _declspec(dllexport) AzimuthAngleBetweenSites(Site source, Site destination);

Site _declspec(dllexport) LoadSplatSiteLocationFile(char *filename);


//| Functions

double ArcCosine(double x, double y);

double BearingStringToDecimalDegrees(char *input);
