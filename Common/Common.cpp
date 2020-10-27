/****************************************************************************\
*						Common: Shared functions							 *
*****************************************************************************/

#include "stdafx.h"
#include <math.h>
#include <string>
#include <time.h>
#include <Windows.h>
#include <dbghelp.h>
#include "Common.h"
#include "version.h"
#include "constants.h"

using namespace std;

typedef BOOL(WINAPI *MINIDUMPWRITEDUMP)(
	HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
	CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);


//| ------------------------------
//| 
//| FUNCTION: ToDouble
//| 
//| NOTES: 
//|   Convenience function to cast int to double
//|   with cleaner syntax.
//| 
//| ------------------------------
double
ToDouble(
	int x)
{
	return (double)x;
}


//| ------------------------------
//| 
//| FUNCTION: ArcCosine
//| 
//| OLD NAME: arccos
//| 
//| NOTES: 
//|   This function implements the arc cosine function,
//|   returning a value between 0 and TWO_PI.
//| 
//| ------------------------------
double
ArcCosine(
	double x,
	double y)
{
	double result = 0.0;

	if (y > 0.0)
	{
		result = acos(x / y);
	}

	if (y < 0.0)
	{
		result = PI + acos(x / y);
	}

	return result;
}


//| ------------------------------
//| 
//| FUNCTION: AzimuthAngleBetweenSites
//| 
//| OLD NAME: Azimuth
//| 
//| NOTES: 
//|   This function returns the azimuth (in degrees) to the
//|   destination as seen from the location of the source.
//| 
//| ------------------------------
double
AzimuthAngleBetweenSites(
	Site source,
	Site destination)
{
	double dest_lat, dest_lon, src_lat, src_lon,
		beta, azimuth, diff, num, den, fraction;

	dest_lat = destination.latitude * DEGREES_TO_RADIANS;
	dest_lon = destination.longitude * DEGREES_TO_RADIANS;

	src_lat = source.latitude * DEGREES_TO_RADIANS;
	src_lon = source.longitude * DEGREES_TO_RADIANS;

	// Calculate Surface Distance

	beta = acos(sin(src_lat)*sin(dest_lat) + cos(src_lat)*cos(dest_lat)*cos(src_lon - dest_lon));

	// Calculate Azimuth

	num = sin(dest_lat) - (sin(src_lat)*cos(beta));
	den = cos(src_lat)*sin(beta);
	fraction = num / den;

	// Trap potential problems in acos() due to rounding

	if (fraction >= 1.0)
	{
		fraction = 1.0;
	}

	if (fraction <= -1.0)
	{
		fraction = -1.0;
	}

	// Calculate azimuth

	azimuth = acos(fraction);

	// Reference it to True North

	diff = dest_lon - src_lon;

	if (diff <= -PI)
	{
		diff += TWO_PI;
	}

	if (diff >= PI)
	{
		diff -= TWO_PI;
	}

	if (diff > 0.0)
	{
		azimuth = TWO_PI - azimuth;
	}

	return (azimuth / DEGREES_TO_RADIANS);
}


//| ------------------------------
//| 
//| FUNCTION: BearingStringToDecimalDegrees
//| 
//| OLD NAME: ReadBearing
//| 
//| NOTES: 
//|   This function takes numeric input in the form of a character
//|   string, and returns an equivalent bearing in degrees as a
//|   decimal number (double). The input may either be expressed
//|   in decimal format (40.139722) or degree, minute, second
//|   format (40 08 23). This function also safely handles
//|   extra spaces found either leading, trailing, or
//|   embedded within the numbers expressed in the
//|   input string. Decimal seconds are permitted.
//| 
//| ------------------------------
double
BearingStringToDecimalDegrees(
	char *input)
{
	double seconds, bearing = 0.0;
	char BearingString[20];
	int	a, b, length, degrees, minutes;

	// Copy "input" to "string", and ignore any extra
	// spaces that might be present in the process.

	BearingString[0] = 0;
	length = (int)strlen(input);

	for (a = 0, b = 0; (a < length) && (a < 18); a++)
	{
		if (((input[a] != 32) && (input[a] != '\n')) || ((input[a] == 32) && (input[a + 1] != 32) && (input[a + 1] != '\n') && (b != 0)))
		{
			BearingString[b] = input[a];
			b++;
		}
	}

	BearingString[b] = 0;

	// Count number of spaces in the clean string.

	length = (int)strlen(BearingString);

	for (a = 0, b = 0; a < length; a++)
	{
		if (BearingString[a] == 32)
		{
			b++;
		}
	}

	if (b == 0)  // Decimal Format (40.139722)
	{
		sscanf_s(BearingString, "%lf", &bearing);
	}

	if (b == 2)  // Degree, Minute, Second Format (40 08 23.xx)
	{
		sscanf_s(BearingString, "%d %d %lf", &degrees, &minutes, &seconds);

		bearing = fabs((double)degrees);
		bearing += fabs(((double)minutes) / 60.0);
		bearing += fabs(seconds / 3600.0);

		if ((degrees < 0) || (minutes < 0) || (seconds < 0.0))
		{
			bearing = -bearing;
		}
	}

	// Anything else returns a 0.0

	if ((bearing > 360.0) || (bearing < -360.0))
	{
		bearing = 0.0;
	}

	return bearing;
}


//| ------------------------------
//| 
//| FUNCTION: GreatCircleDistanceBetweenSiteLocations
//| 
//| OLD NAME: Distance
//| 
//| NOTES: 
//|   This function returns the great circle distance
//|   in miles between any two site locations.
//| 
//| ------------------------------
double
GreatCircleDistanceBetweenSiteLocations(
	Site site1,
	Site site2)
{
	double lat1, lon1, lat2, lon2, distance;

	lat1 = site1.latitude * DEGREES_TO_RADIANS;
	lon1 = site1.longitude * DEGREES_TO_RADIANS;
	lat2 = site2.latitude * DEGREES_TO_RADIANS;
	lon2 = site2.longitude * DEGREES_TO_RADIANS;

	distance = EARTH_RADIUS_MILES * acos(sin(lat1)*sin(lat2) + cos(lat1)*cos(lat2)*cos(lon1 - lon2));

	return distance;
}


//| ------------------------------
//| 
//| FUNCTION: ReplaceBackslashWithDoubleBackslash
//| 
//| NOTES: 
//|   This function takes the characters in inputStr and
//|   places them in outputStr, with the exception of replacing
//|   '\\' with '\\\\'.
//| 
//| ------------------------------
void
ReplaceBackslashWithDoubleBackslash(
	char* inputStr,
	char* outputStr)
{
	int inputStrLen = (int)strlen(inputStr);

	int outputStrIndex = 0;

	for (int inputStrIndex = 0; inputStrIndex < inputStrLen; inputStrIndex++)
	{
		outputStr[outputStrIndex] = inputStr[inputStrIndex];
		outputStrIndex++;

		if (inputStr[inputStrIndex] == '\\')
		{
			outputStr[outputStrIndex] = '\\';
			outputStrIndex++;
		}
	}

	outputStr[outputStrIndex] = 0;
}


//| ------------------------------
//| 
//| FUNCTION: ReturnCurrentDateAndTime
//| 
//| NOTES: 
//|   Return string of the current date and time.
//| 
//| ------------------------------
const string
ReturnCurrentDateAndTime()
{
	char buf[80];
	time_t now = time(0);
	struct tm tstruct;

	errno_t err = localtime_s(&tstruct, &now);
	strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

	// Look through the string, replace ':' with '-'
	for (int i = 0; i < strlen(buf); i++) {
		if (buf[i] == ':') {
			buf[i] = '-';
		}
	}

	return buf;
}


//| ------------------------------
//| 
//| FUNCTION: CommonUnhandledExceptionFilter
//| 
//| INFLOWS: 
//|   struct _EXCEPTION_POINTERS *ExceptionInfo - ignored
//|
//| OUTFLOWS: 
//|   none 
//|
//| RETURNS: (long WINAPI) - EXCEPTION_EXECUTE_HANDLER: execute the 
//|                          associated exception handler, usually 
//|                          results in process termination
//| 
//| NOTES: 
//|   Try to perform any clean up work before the application fails 
//|   on an unhandled exception, and write a minidump to capture it.
//| 
//| ------------------------------
long WINAPI
CommonUnhandledExceptionFilter(
	struct _EXCEPTION_POINTERS *ExceptionInfo)
{
	// Static variable ensures we pass through this only one (to prevent this
	// this from being called if it failed in the recovery effort as well.
	static BOOL firstPass = TRUE;

	if (firstPass) 
	{
		firstPass = FALSE;

		// Try to create a minidump file. Ideally this should be done from a separate watchdog process, to give the
		// best chance of successfully creating the file, rather than doing it in this process which
		// may be too unstable mattering on the fail scenario.
		// First, need to load the required library.

		HINSTANCE libraryInstance = LoadLibrary("dbghelp.dll");

		if (libraryInstance)
		{
			// Loaded the library, so find the required function address.
			MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)GetProcAddress(libraryInstance, "MiniDumpWriteDump");

			if (pDump)
			{
				// Found the function address, so save the minidump file.
				// Create a unique name for the .dmp file, using the current date & time when the exception occurred.

				string minidumpFileName = MINIDUMP_PATH;
				
				WIN32_FIND_DATA FindFileData;
				HANDLE hFind = FindFirstFile(minidumpFileName.c_str(), &FindFileData);
				if (hFind == INVALID_HANDLE_VALUE) {
					CreateDirectory(minidumpFileName.c_str(), NULL);
				}

				minidumpFileName += "splat ";
				minidumpFileName += ReturnCurrentDateAndTime();
				minidumpFileName += ".dmp";

				HANDLE hFile = CreateFile(minidumpFileName.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

				if (hFile != INVALID_HANDLE_VALUE)
				{
					// Have a valid file handle, so write the dump.
					_MINIDUMP_EXCEPTION_INFORMATION ExInfo;
					ExInfo.ThreadId = GetCurrentThreadId();
					ExInfo.ExceptionPointers = ExceptionInfo;
					ExInfo.ClientPointers = NULL;

					// Right now, uses general MiniDumpNormal when creating the file. This can be changed to give more comprehensive or useful results,
					// but needs to be considered carefully, because the file size can significantly increase mattering on which type is sent.
					BOOL bOK = pDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);

					if (bOK)
					{
						// Success - Created error log file.
					}
					else
					{
						// Error - Cannot create error log file.
						LPVOID lpMsgBuf;
						DWORD dw = GetLastError();
						FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
					}

					CloseHandle(hFile);
				}
				else 
				{
					// Error - Cannot create error log file.
				}
			}
			else 
			{
				// Error - Could not locate function MiniDumpWriteDump, cannot create error log file.
			}
			FreeLibrary(libraryInstance);
		}
		else
		{
			// Error - Could not locate dbghelp.dll, cannot create error log file.
		}
	}

	return EXCEPTION_EXECUTE_HANDLER;
}
