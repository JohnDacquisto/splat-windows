/*****************************************************************************
*				Usgs2SdfLib: Functions used by Usgs2Sdf						 *
*****************************************************************************/

#include "stdafx.h"
#include "usgs2sdfLib.h"


//| ------------------------------
//| 
//| FUNCTION: ConvertExponentialNotation
//| 
//| OLD NAME: d2e
//| 
//| NOTES: 
//|   This function is used to replace 'D's with 'E's for proper
//|   exponential notation of numeric strings read from delimited
//|   USGS data files. It returns a pointer to a string.
//| 
//| ------------------------------
char*
ConvertExponentialNotation(
	char *UsgsString)
{
	for (int x = 0; UsgsString[x] != 0; x++)
	{
		if (UsgsString[x] == 'D')
		{
			UsgsString[x] = 'E';
		}
	}

	return UsgsString;
}
