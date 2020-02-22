/****************************************************************************\
*						Common: Shared functions							 *
*****************************************************************************/

#pragma once

#include "stdafx.h"
#include <windows.h>
#include "Site.h"


// Export functions

double _declspec(dllexport)
ArcCosine(
	double x,
	double y);

double _declspec(dllexport)
AzimuthAngleBetweenSites(
	Site source,
	Site destination);

double _declspec(dllexport)
BearingStringToDecimalDegrees(
	char *input);

double _declspec(dllexport)
GreatCircleDistanceBetweenSiteLocations(
	Site site1,
	Site site2);

long _declspec(dllexport) WINAPI
CommonUnhandledExceptionFilter(
	struct _EXCEPTION_POINTERS *ExceptionInfo);
