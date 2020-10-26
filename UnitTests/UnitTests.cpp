#include "pch.h"
#include "..\Common\Common.h"
#include "..\Common\Site.h"
#include "..\SplatLib\SplatLib.h"
#include "..\SplatLib\itwom.h"
#include "..\Usgs2SdfLib\usgs2sdfLib.h"


// CommonTests

TEST(CommonTests, ArcCosineTests)
{
	EXPECT_DOUBLE_EQ(0.0, ArcCosine(0.0, 0.0));

	EXPECT_DOUBLE_EQ(1.5707963267948966, ArcCosine(0.0, 0.634));

	EXPECT_DOUBLE_EQ(0.0, ArcCosine(0.211, 0.0));

	EXPECT_DOUBLE_EQ(4.7123889803846897, ArcCosine(0.0, -0.986));

	EXPECT_DOUBLE_EQ(0.0, ArcCosine(-0.755, 0.0));

	EXPECT_DOUBLE_EQ(1.4274487578895312, ArcCosine(0.2, 1.4));

	EXPECT_DOUBLE_EQ(2.3288370922211326, ArcCosine(-3.3, 4.8));

	EXPECT_DOUBLE_EQ(5.2741333517637212, ArcCosine(10.6, -19.9));
}

TEST(CommonTests, AzimuthAngleBetweenSitesTests)
{
	Site source, destination;

	source.latitude = 43.0;
	source.longitude = 72.4;
	destination.latitude = 45.0;
	destination.longitude = 76.9;

	EXPECT_DOUBLE_EQ(303.25902754187035, AzimuthAngleBetweenSites(source, destination));

	source.latitude = 47.2;
	source.longitude = 70.8;

	EXPECT_DOUBLE_EQ(244.74261202238631, AzimuthAngleBetweenSites(source, destination));

	destination.latitude = 50.3;
	destination.longitude = 67.1;

	EXPECT_DOUBLE_EQ(36.822823764035398, AzimuthAngleBetweenSites(source, destination));

	source.latitude = 0.0;
	source.longitude = 0.0;

	EXPECT_DOUBLE_EQ(322.59186048560031, AzimuthAngleBetweenSites(source, destination));
}

TEST(CommonTests, BearingStringToDecimalDegreesTests)
{
	EXPECT_DOUBLE_EQ(40.139721999999999, BearingStringToDecimalDegrees("40.139722"));

	EXPECT_DOUBLE_EQ(40.139722222222218, BearingStringToDecimalDegrees("40 08 23"));

	EXPECT_DOUBLE_EQ(0.0, BearingStringToDecimalDegrees("0 0 0"));

	EXPECT_DOUBLE_EQ(0.0, BearingStringToDecimalDegrees("0.0"));
}

TEST(CommonTests, GreatCircleDistanceBetweenSiteLocationsTests)
{
	Site site1, site2;

	site1.latitude = 43.0;
	site1.longitude = 72.4;
	site2.latitude = 45.0;
	site2.longitude = 76.9;

	EXPECT_DOUBLE_EQ(262.8590286501888, GreatCircleDistanceBetweenSiteLocations(site1, site2));

	site1.latitude = 47.2;
	site1.longitude = 70.8;

	EXPECT_DOUBLE_EQ(329.30418009139476, GreatCircleDistanceBetweenSiteLocations(site1, site2));

	site2.latitude = 50.3;
	site2.longitude = 67.1;

	EXPECT_DOUBLE_EQ(272.50446636914228, GreatCircleDistanceBetweenSiteLocations(site1, site2));

	site1.latitude = 0.0;
	site1.longitude = 0.0;

	EXPECT_DOUBLE_EQ(5224.3089129927603, GreatCircleDistanceBetweenSiteLocations(site1, site2));
}


// SplatLibTests

TEST(SplatLibTests, LinearInterpolationTests)
{
	EXPECT_EQ(2, LinearInterpolation(1, 2, 3, 4, 5));

	EXPECT_EQ(5, LinearInterpolation(5, 5, 5, 5, 5));

	EXPECT_EQ(9, LinearInterpolation(9, 9, 10, 10, 1));

	EXPECT_EQ(9, LinearInterpolation(8, 9, 4, 5, 16));

	EXPECT_EQ(4, LinearInterpolation(4, 4, 1, 9, 6));

	EXPECT_EQ(7, LinearInterpolation(2, 10, 1, 9, 6));
}

TEST(SplatLibTests, LongitudeDifferenceTests)
{
	EXPECT_DOUBLE_EQ(-168.89999999999998, LongitudeDifference(12.3, 181.2));

	EXPECT_DOUBLE_EQ(0.0, LongitudeDifference(0.0, 0.0));

	EXPECT_DOUBLE_EQ(15.739999999999998, LongitudeDifference(36.98, 21.24));

	EXPECT_DOUBLE_EQ(149.202, LongitudeDifference(548.1, 398.898));

	EXPECT_DOUBLE_EQ(97.759999999999991, LongitudeDifference(-719.02, -456.78));
}

TEST(SplatLibTests, ConvertToNormalizedAnglTests)
{
	EXPECT_EQ(12, ConvertToNormalizedAngle(12.4));

	EXPECT_EQ(41, ConvertToNormalizedAngle(-40.9));

	EXPECT_EQ(3, ConvertToNormalizedAngle(363.0));

	EXPECT_EQ(0, ConvertToNormalizedAngle(0.37));

	EXPECT_EQ(180, ConvertToNormalizedAngle(179.999));
}

TEST(SplatLibTests, ConvertDecimalToDegreesMinutesSecondsTests)
{
	// \xB0 is unicode for degree symbol.

	char dmsString[255];

	ConvertDecimalToDegreesMinutesSeconds(9.52, dmsString);

	EXPECT_STREQ("9\xB0 31' 11\"", dmsString);

	ConvertDecimalToDegreesMinutesSeconds(-61.89, dmsString);

	EXPECT_STREQ("-61\xB0 53' 24\"", dmsString);

	ConvertDecimalToDegreesMinutesSeconds(0.0, dmsString);

	EXPECT_STREQ("0\xB0 0' 0\"", dmsString);
}


// Usgs2SdfLibTests

TEST(Usgs2SdfLibTests, ConvertExponentialNotationTests)
{
	char usgsString[30];

	snprintf(usgsString, 30, "%s", "29D14A3");

	EXPECT_STREQ("29E14A3", ConvertExponentialNotation(usgsString));

	snprintf(usgsString, 30, "%s", "DD314AD");

	EXPECT_STREQ("EE314AE", ConvertExponentialNotation(usgsString));

	snprintf(usgsString, 30, "%s", "_EDE_D");

	EXPECT_STREQ("_EEE_E", ConvertExponentialNotation(usgsString));
}


//| ------------------------------
//| 
//| FUNCTION: main
//| 
//| NOTES: 
//| 
//| ------------------------------
int
main(
	int argc,
	char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
