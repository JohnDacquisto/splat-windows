/****************************************************************************\
*					SplatLib.KeyholeMarkupLanguage: KML Output Files		 *
*****************************************************************************/

#include "stdafx.h"
#include <math.h>
#include <io.h>
#include <stdlib.h>
#include <ctype.h>
#include <Windows.h>
#include "SplatLib.h"
#include "itwom.h"
#include "fontdata.h"
#include "..\Common\Common.h"
#include "..\Common\Site.h"
#include "..\Common\version.h"
#include "..\Common\constants.h"


//| ------------------------------
//| 
//| FUNCTION: WriteKeyholeMarkupLanguageFile
//| 
//| OLD NAME: WriteKML
//| 
//| NOTES: 
//| 
//| ------------------------------
void
WriteKeyholeMarkupLanguageFile(
	Site source,
	Site destination,
	DigitalElevationModelWrapper* digitalElevationModelWrapper,
	Path* path,
	unsigned char useMetricUnits,
	double sphereRadius)
{
	int	x, y;
	char report_name[80];
	bool block;
	double distance, rx_alt, tx_alt, cos_xmtr_angle,
		azimuth, cos_test_angle, test_alt;
	FILE* fd = NULL;
	errno_t err;

	char dmsString[255];

	GeneratePathBetweenSites(source, destination, digitalElevationModelWrapper, path);

	sprintf_s(report_name, _countof(report_name), "%s-to-%s.kml", source.name, destination.name);

	for (x = 0; report_name[x] != 0; x++)
	{
		if ((report_name[x] == 32) || (report_name[x] == 17) || (report_name[x] == 92) || (report_name[x] == 42) || (report_name[x] == 47))
		{
			report_name[x] = '_';
		}
	}

	err = fopen_s(&fd, report_name, "w");

	fprintf(fd, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(fd, "<kml xmlns=\"http://earth.google.com/kml/2.0\">\n");
	fprintf(fd, "<!-- Generated by %s Version %s -->\n", APP_NAME, VERSION_HEADER);
	fprintf(fd, "<Folder>\n");
	fprintf(fd, "<name>SPLAT Path</name>\n");
	fprintf(fd, "<open>1</open>\n");
	fprintf(fd, "<description>Path Between %s and %s</description>\n", source.name, destination.name);

	fprintf(fd, "<Placemark>\n");
	fprintf(fd, "    <name>%s</name>\n", source.name);
	fprintf(fd, "    <description>\n");
	fprintf(fd, "       Transmit Site\n");

	if (source.latitude >= 0.0)
	{
		ConvertDecimalToDegreesMinutesSeconds(source.latitude, dmsString);
		fprintf(fd, "       <BR>%s North</BR>\n", dmsString);
	}
	else
	{
		ConvertDecimalToDegreesMinutesSeconds(source.latitude, dmsString);
		fprintf(fd, "       <BR>%s South</BR>\n", dmsString);
	}

	ConvertDecimalToDegreesMinutesSeconds(source.longitude, dmsString);
	fprintf(fd, "       <BR>%s West</BR>\n", dmsString);

	azimuth = AzimuthAngleBetweenSites(source, destination);
	distance = GreatCircleDistanceBetweenSiteLocations(source, destination);

	if (useMetricUnits)
	{
		fprintf(fd, "       <BR>%.2f km", distance * KM_PER_MILE);
	}
	else
	{
		fprintf(fd, "       <BR>%.2f miles", distance);
	}

	fprintf(fd, " to %s</BR>\n       <BR>toward an azimuth of %.2f%c</BR>\n", destination.name, azimuth, 176);

	fprintf(fd, "    </description>\n");
	fprintf(fd, "    <visibility>1</visibility>\n");
	fprintf(fd, "    <Style>\n");
	fprintf(fd, "      <IconStyle>\n");
	fprintf(fd, "        <Icon>\n");
	fprintf(fd, "          <href>root://icons/palette-5.png</href>\n");
	fprintf(fd, "          <x>224</x>\n");
	fprintf(fd, "          <y>224</y>\n");
	fprintf(fd, "          <w>32</w>\n");
	fprintf(fd, "          <h>32</h>\n");
	fprintf(fd, "        </Icon>\n");
	fprintf(fd, "      </IconStyle>\n");
	fprintf(fd, "    </Style>\n");
	fprintf(fd, "    <Point>\n");
	fprintf(fd, "      <extrude>1</extrude>\n");
	fprintf(fd, "      <altitudeMode>relativeToGround</altitudeMode>\n");
	fprintf(fd, "      <coordinates>%f,%f,30</coordinates>\n", (source.longitude < 180.0 ? -source.longitude : 360.0 - source.longitude), source.latitude);
	fprintf(fd, "    </Point>\n");
	fprintf(fd, "</Placemark>\n");

	fprintf(fd, "<Placemark>\n");
	fprintf(fd, "    <name>%s</name>\n", destination.name);
	fprintf(fd, "    <description>\n");
	fprintf(fd, "       Receive Site\n");

	if (destination.latitude >= 0.0)
	{
		ConvertDecimalToDegreesMinutesSeconds(destination.latitude, dmsString);
		fprintf(fd, "       <BR>%s North</BR>\n", dmsString);
	}
	else
	{
		ConvertDecimalToDegreesMinutesSeconds(destination.latitude, dmsString);
		fprintf(fd, "       <BR>%s South</BR>\n", dmsString);
	}

	ConvertDecimalToDegreesMinutesSeconds(destination.longitude, dmsString);
	fprintf(fd, "       <BR>%s West</BR>\n", dmsString);

	if (useMetricUnits)
	{
		fprintf(fd, "       <BR>%.2f km", distance * KM_PER_MILE);
	}
	else
	{
		fprintf(fd, "       <BR>%.2f miles", distance);
	}

	fprintf(fd, " to %s</BR>\n       <BR>toward an azimuth of %.2f%c</BR>\n", source.name, AzimuthAngleBetweenSites(destination, source), 176);

	fprintf(fd, "    </description>\n");
	fprintf(fd, "    <visibility>1</visibility>\n");
	fprintf(fd, "    <Style>\n");
	fprintf(fd, "      <IconStyle>\n");
	fprintf(fd, "        <Icon>\n");
	fprintf(fd, "          <href>root://icons/palette-5.png</href>\n");
	fprintf(fd, "          <x>224</x>\n");
	fprintf(fd, "          <y>224</y>\n");
	fprintf(fd, "          <w>32</w>\n");
	fprintf(fd, "          <h>32</h>\n");
	fprintf(fd, "        </Icon>\n");
	fprintf(fd, "      </IconStyle>\n");
	fprintf(fd, "    </Style>\n");
	fprintf(fd, "    <Point>\n");
	fprintf(fd, "      <extrude>1</extrude>\n");
	fprintf(fd, "      <altitudeMode>relativeToGround</altitudeMode>\n");
	fprintf(fd, "      <coordinates>%f,%f,30</coordinates>\n", (destination.longitude < 180.0 ? -destination.longitude : 360.0 - destination.longitude), destination.latitude);
	fprintf(fd, "    </Point>\n");
	fprintf(fd, "</Placemark>\n");

	fprintf(fd, "<Placemark>\n");
	fprintf(fd, "<name>Point-to-Point Path</name>\n");
	fprintf(fd, "  <visibility>1</visibility>\n");
	fprintf(fd, "  <open>0</open>\n");
	fprintf(fd, "  <Style>\n");
	fprintf(fd, "    <LineStyle>\n");
	fprintf(fd, "      <color>7fffffff</color>\n");
	fprintf(fd, "    </LineStyle>\n");
	fprintf(fd, "    <PolyStyle>\n");
	fprintf(fd, "       <color>7fffffff</color>\n");
	fprintf(fd, "    </PolyStyle>\n");
	fprintf(fd, "  </Style>\n");
	fprintf(fd, "  <LineString>\n");
	fprintf(fd, "    <extrude>1</extrude>\n");
	fprintf(fd, "    <tessellate>1</tessellate>\n");
	fprintf(fd, "    <altitudeMode>relativeToGround</altitudeMode>\n");
	fprintf(fd, "    <coordinates>\n");

	for (x = 0; x < path->pathLength; x++)
	{
		fprintf(fd, "      %f,%f,5\n", (path->longitudes[x] < 180.0 ? -path->longitudes[x] : 360.0 - path->longitudes[x]), path->latitudes[x]);
	}

	fprintf(fd, "    </coordinates>\n");
	fprintf(fd, "   </LineString>\n");
	fprintf(fd, "</Placemark>\n");

	fprintf(fd, "<Placemark>\n");
	fprintf(fd, "<name>Line-of-Sight Path</name>\n");
	fprintf(fd, "  <visibility>1</visibility>\n");
	fprintf(fd, "  <open>0</open>\n");
	fprintf(fd, "  <Style>\n");
	fprintf(fd, "    <LineStyle>\n");
	fprintf(fd, "      <color>ff00ff00</color>\n");
	fprintf(fd, "    </LineStyle>\n");
	fprintf(fd, "    <PolyStyle>\n");
	fprintf(fd, "       <color>7f00ff00</color>\n");
	fprintf(fd, "    </PolyStyle>\n");
	fprintf(fd, "  </Style>\n");
	fprintf(fd, "  <LineString>\n");
	fprintf(fd, "    <extrude>1</extrude>\n");
	fprintf(fd, "    <tessellate>1</tessellate>\n");
	fprintf(fd, "    <altitudeMode>relativeToGround</altitudeMode>\n");
	fprintf(fd, "    <coordinates>\n");

	// Walk across the "path", indentifying obstructions along the way

	for (y = 0; y < path->pathLength; y++)
	{
		distance = FEET_PER_MILE * path->distances[y];
		tx_alt = sphereRadius + source.altitude + path->elevations[0];
		rx_alt = sphereRadius + destination.altitude + path->elevations[y];

		// Calculate the cosine of the elevation of the
		// transmitter as seen at the temp rx point.

		cos_xmtr_angle = ((rx_alt * rx_alt) + (distance * distance) - (tx_alt * tx_alt)) / (2.0 * rx_alt * distance);

		for (x = y, block = false; (x >= 0) && (block == false); x--)
		{
			distance = FEET_PER_MILE * (path->distances[y] - path->distances[x]);
			test_alt = sphereRadius + path->elevations[x];

			cos_test_angle = ((rx_alt * rx_alt) + (distance * distance) - (test_alt * test_alt)) / (2.0 * rx_alt * distance);

			// Compare these two angles to determine if
			// an obstruction exists. Since we're comparing
			// the cosines of these angles rather than
			// the angles themselves, the following "if"
			// statement is reversed from what it would
			// be if the actual angles were compared.

			if (cos_xmtr_angle >= cos_test_angle)
			{
				block = true;
			}
		}

		if (block)
		{
			fprintf(fd, "      %f,%f,-30\n", (path->longitudes[y] < 180.0 ? -path->longitudes[y] : 360.0 - path->longitudes[y]), path->latitudes[y]);
		}
		else
		{
			fprintf(fd, "      %f,%f,5\n", (path->longitudes[y] < 180.0 ? -path->longitudes[y] : 360.0 - path->longitudes[y]), path->latitudes[y]);
		}
	}

	fprintf(fd, "    </coordinates>\n");
	fprintf(fd, "  </LineString>\n");
	fprintf(fd, "</Placemark>\n");

	fprintf(fd, "    <LookAt>\n");
	fprintf(fd, "      <longitude>%f</longitude>\n", (source.longitude < 180.0 ? -source.longitude : 360.0 - source.longitude));
	fprintf(fd, "      <latitude>%f</latitude>\n", source.latitude);
	fprintf(fd, "      <range>300.0</range>\n");
	fprintf(fd, "      <tilt>45.0</tilt>\n");
	fprintf(fd, "      <heading>%f</heading>\n", azimuth);
	fprintf(fd, "    </LookAt>\n");

	fprintf(fd, "</Folder>\n");
	fprintf(fd, "</kml>\n");

	fclose(fd);

	fprintf(stdout, "\nKML file written to: \"%s\"", report_name);

	fflush(stdout);
}