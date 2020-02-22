/***************************************************************************\
*					BearingLib: Functions used by Bearing					*
****************************************************************************/

#pragma once

#include "stdafx.h"
#include "..\Common\Site.h"


// Export functions

Site _declspec(dllexport)
LoadSplatSiteLocationFileForBearing(
	char *filename);
