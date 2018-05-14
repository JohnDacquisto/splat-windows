/***************************************************************************\
*					BearingLib: Functions used by Bearing					*
****************************************************************************/

#include "stdafx.h"
#include <math.h>
#include "BearingLib.h"
#include "..\Common\constants.h"
#include "..\Common\Common.h"
#include "..\Common\Site.h"


//| ------------------------------
//| 
//| FUNCTION: LoadSplatSiteLocationFileForBearing
//| 
//| OLD NAME: LoadQTH
//| 
//| NOTES: 
//|   This function reads SPLAT .qth (site location) files.
//|   The latitude and longitude may be expressed either in
//|   decimal degrees, or in degree, minute, second format.
//|   Antenna height is assumed to be expressed in feet above
//|   ground level (AGL), unless followed by the letter 'M',
//|   or 'm', or by the word "meters" or "Meters", in which
//|   case meters is assumed, and is handled accordingly.
//| 
//| ------------------------------
Site
LoadSplatSiteLocationFileForBearing
   (char *filename)
{
	int	x;
	char QthString[50], qthfile[255];
	Site tempsite;
	FILE *fd = NULL;
	errno_t err;

	for (x = 0; (filename[x] != '.') && (filename[x] != 0) && (x < 250); x++)
	{
		qthfile[x] = filename[x];
	}

	qthfile[x] = '.';
	qthfile[x + 1] = 'q';
	qthfile[x + 2] = 't';
	qthfile[x + 3] = 'h';
	qthfile[x + 4] = 0;

	tempsite.latitude = 91.0;
	tempsite.longitude = 361.0;
	tempsite.name[0] = 0;
	tempsite.azimuth = 0.0;

	err = fopen_s(&fd, qthfile, "r");

	if (fd != NULL)
	{
		//| Site Name
		fgets(QthString, 49, fd);

		//| Strip <CR> and/or <LF> from end of site name

		for (x = 0; (QthString[x] != 13) && (QthString[x] != 10) && (QthString[x] != 0); tempsite.name[x] = QthString[x], x++);

		tempsite.name[x] = 0;

		//| Site Latitude
		fgets(QthString, 49, fd);
		tempsite.latitude = BearingStringToDecimalDegrees(QthString);

		//| Site Longitude
		fgets(QthString, 49, fd);
		tempsite.longitude = BearingStringToDecimalDegrees(QthString);

		fclose(fd);
	}

	return tempsite;
}
