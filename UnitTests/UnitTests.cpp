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


// ItwomTests

TEST(ItwomTests, MinimumIntegerTests)
{
	EXPECT_EQ(0, MinimumInteger(0, 0));

	EXPECT_EQ(-5, MinimumInteger(-5, -4));

	EXPECT_EQ(2, MinimumInteger(31, 2));

	EXPECT_EQ(-3, MinimumInteger(-3, 3));

	EXPECT_EQ(-3, MinimumInteger(3, -3));
}

TEST(ItwomTests, MaximumIntegerTests)
{
	EXPECT_EQ(0, MaximumInteger(0, 0));

	EXPECT_EQ(-4, MaximumInteger(-5, -4));

	EXPECT_EQ(31, MaximumInteger(31, 2));

	EXPECT_EQ(3, MaximumInteger(-3, 3));

	EXPECT_EQ(3, MaximumInteger(3, -3));
}

TEST(ItwomTests, MinimumDoubleTests)
{
	EXPECT_DOUBLE_EQ(0.0, MinimumDouble(0.0, 0.0));

	EXPECT_DOUBLE_EQ(-5.4, MinimumDouble(-5.4, -4.9));

	EXPECT_DOUBLE_EQ(2.03, MinimumDouble(31.98, 2.03));

	EXPECT_DOUBLE_EQ(-3.21, MinimumDouble(-3.21, 3.21));

	EXPECT_DOUBLE_EQ(-3.21, MinimumDouble(3.21, -3.21));
}

TEST(ItwomTests, MaximumDoubleTests)
{
	EXPECT_DOUBLE_EQ(0.0, MaximumDouble(0.0, 0.0));

	EXPECT_DOUBLE_EQ(-4.9, MaximumDouble(-5.4, -4.9));

	EXPECT_DOUBLE_EQ(31.98, MaximumDouble(31.98, 2.03));

	EXPECT_DOUBLE_EQ(3.21, MaximumDouble(-3.21, 3.21));

	EXPECT_DOUBLE_EQ(3.21, MaximumDouble(3.21, -3.21));
}

TEST(ItwomTests, PositiveDifferenceOrZeroTests)
{
	EXPECT_DOUBLE_EQ(0.0, PositiveDifferenceOrZero(2.0, 3.2));

	EXPECT_DOUBLE_EQ(1.2, PositiveDifferenceOrZero(3.2, 2.0));

	EXPECT_DOUBLE_EQ(0.0, PositiveDifferenceOrZero(0.0, 0.0));

	EXPECT_DOUBLE_EQ(0.0, PositiveDifferenceOrZero(-3.2, -2.0));

	EXPECT_DOUBLE_EQ(1.2, PositiveDifferenceOrZero(-2.0, -3.2));

	EXPECT_DOUBLE_EQ(5.0, PositiveDifferenceOrZero(3.2, -1.8));

	EXPECT_DOUBLE_EQ(0.0, PositiveDifferenceOrZero(-1.8, 3.2));
}

TEST(ItwomTests, AttenuationFromSingleKnifeEdgeTests)
{
	EXPECT_DOUBLE_EQ(19.048675701455156, AttenuationFromSingleKnifeEdge(3.8912));

	EXPECT_DOUBLE_EQ(20.572954568645088, AttenuationFromSingleKnifeEdge(5.7809));

	EXPECT_DOUBLE_EQ(6.9182999999999995, AttenuationFromSingleKnifeEdge(0.01));
}

TEST(ItwomTests, HeightGainOverSmoothSphereTests)
{
	EXPECT_DOUBLE_EQ(-40.789758050061948, HeightGainOverSmoothSphere(80.4, 5.4e-6));

	EXPECT_DOUBLE_EQ(-64.603075180534304, HeightGainOverSmoothSphere(10.13, 0.003));

	EXPECT_DOUBLE_EQ(-23.354445456218272, HeightGainOverSmoothSphere(216.6, 0.0));

	EXPECT_DOUBLE_EQ(132.94092511472209, HeightGainOverSmoothSphere(2914.03, 0.0));
}

TEST(ItwomTests, H01ScatterFieldsTests)
{
	EXPECT_DOUBLE_EQ(5.1365065331287685, H01ScatterFields(3.4, -2.1));

	EXPECT_DOUBLE_EQ(15.046265731341633, H01ScatterFields(2.6, 6.8));

	EXPECT_DOUBLE_EQ(22.058824499738321, H01ScatterFields(1.2, 3.3));

	EXPECT_DOUBLE_EQ(28.413081749794216, H01ScatterFields(0.92, 4.13));
}

TEST(ItwomTests, FThetaDScatterFieldsTests)
{
	EXPECT_DOUBLE_EQ(96.983764191385063, FThetaDScatterFields(8.2e3));

	EXPECT_DOUBLE_EQ(96.872073663060505, FThetaDScatterFields(11.4e3));

	EXPECT_DOUBLE_EQ(111.21997454574499, FThetaDScatterFields(92.9e3));
}

TEST(ItwomTests, AbsoluteValueOfComplexNumberTests)
{
	EXPECT_DOUBLE_EQ(0.0, AbsoluteValueOfComplexNumber(complex<double>(0.0, 0.0)));

	EXPECT_DOUBLE_EQ(11.559999999999999, AbsoluteValueOfComplexNumber(complex<double>(0.0, 3.4)));

	EXPECT_DOUBLE_EQ(1.4399999999999999, AbsoluteValueOfComplexNumber(complex<double>(1.2, 0.0)));

	EXPECT_DOUBLE_EQ(12.999999999999998, AbsoluteValueOfComplexNumber(complex<double>(1.2, 3.4)));

	EXPECT_DOUBLE_EQ(11.559999999999999, AbsoluteValueOfComplexNumber(complex<double>(0.0, -3.4)));

	EXPECT_DOUBLE_EQ(1.4399999999999999, AbsoluteValueOfComplexNumber(complex<double>(-1.2, 0.0)));

	EXPECT_DOUBLE_EQ(12.999999999999998, AbsoluteValueOfComplexNumber(complex<double>(-1.2, -3.4)));

	EXPECT_DOUBLE_EQ(12.999999999999998, AbsoluteValueOfComplexNumber(complex<double>(1.2, -3.4)));

	EXPECT_DOUBLE_EQ(12.999999999999998, AbsoluteValueOfComplexNumber(complex<double>(-1.2, 3.4)));
}

TEST(ItwomTests, StandardNormalTailDistributionFunctionInverseTests)
{
	EXPECT_DOUBLE_EQ(-4.7532584794995731, StandardNormalTailDistributionFunctionInverse(1.2));

	EXPECT_DOUBLE_EQ(0.84145678947617653, StandardNormalTailDistributionFunctionInverse(0.2));
}

TEST(ItwomTests, CurveTests)
{
	EXPECT_DOUBLE_EQ(1.2117610438486284, Curve(1.3, -0.2, 1.1, 1.3, -0.2, 4.1));

	EXPECT_DOUBLE_EQ(0.34671811760647009, Curve(0.4, -0.9, -0.8, 4.4, 1.5, -3.3));
}

TEST(ItwomTests, CalculateLinearLeastSquaresFitTests)
{
	double z0, zn;
	double z[] = { 34.0, 2.6, 1.2, 0.9, 0.6, 0.2, 0.1, 0.0 };

	CalculateLinearLeastSquaresFit(z, 0.0, 34.0, z0, zn);

	EXPECT_DOUBLE_EQ(9.7167578364543524e+60, z0);
	EXPECT_DOUBLE_EQ(-3.1043538696053672e+62, zn);

	CalculateLinearLeastSquaresFit(z, 34.0, 0.0, z0, zn);

	EXPECT_DOUBLE_EQ(-1.0466831926419531e+61, z0);
	EXPECT_DOUBLE_EQ(6.6289935533990367e+61, zn);
}

TEST(ItwomTests, CalculateLinearLeastSquaresFitV2Tests)
{
	double z0, zn;
	double z[] = { 34.0, 2.6, 1.2, 0.9, 0.6, 0.2, 0.1, 0.0 };

	CalculateLinearLeastSquaresFitV2(z, 0.0, 34.0, z0, zn);

	EXPECT_DOUBLE_EQ(6.9419723511988156e+60, z0);
	EXPECT_DOUBLE_EQ(-2.9652139042977889e+62, zn);

	CalculateLinearLeastSquaresFitV2(z, 34.0, 0.0, z0, zn);

	EXPECT_DOUBLE_EQ(6.5820182292848243e+61, z0);
	EXPECT_DOUBLE_EQ(-2.4888256429483241e+62, zn);
}

TEST(ItwomTests, CreateQuantileTests)
{
	double a[] = { 2.0, 6.0, 4.0, 5.0, 3.0, 0.0, -2.0, 0.8, 5.1 };

	EXPECT_DOUBLE_EQ(3.0, CreateQuantile(8, a, 4));

	EXPECT_DOUBLE_EQ(6.0, CreateQuantile(8, a, -2));
}

TEST(ItwomTests, StandardNormalTailDistributionFunctionTests)
{
	EXPECT_DOUBLE_EQ(0.99931279796095918, StandardNormalTailDistributionFunction(-3.2));

	EXPECT_DOUBLE_EQ(0.00068720203904081587, StandardNormalTailDistributionFunction(3.2));

	EXPECT_DOUBLE_EQ(1.0, StandardNormalTailDistributionFunction(-24.5));

	EXPECT_DOUBLE_EQ(0.0, StandardNormalTailDistributionFunction(24.5));

	EXPECT_DOUBLE_EQ(0.99999999999998501, StandardNormalTailDistributionFunction(-7.6));

	EXPECT_DOUBLE_EQ(1.4934676229806036e-14, StandardNormalTailDistributionFunction(7.6));
}

TEST(ItwomTests, CalculateTerrainInterdecileRangeDeltaTests)
{
	double pf1[] = { 3.0, 5.0, 2.3, 4.9, 8.4, 0.1, 2.1, 0.0, 9.3 };

	EXPECT_DOUBLE_EQ(0.0, CalculateTerrainInterdecileRangeDelta(pf1, 3.1, 5.2));

	EXPECT_DOUBLE_EQ(40.672045536691222, CalculateTerrainInterdecileRangeDelta(pf1, 3.1, 25.2));
}

TEST(ItwomTests, PointToPointCalculationLegacyTests)
{
	double dbLoss;
	int errNum;
	char strMode[100];

	double elev[4960];
	elev[0] = 12.0;
	elev[1] = 83.8170;
	for (int i = 2; i < 4960; i++)
	{
		elev[i] = 0.0;
	}

	PointToPointCalculationLegacy(elev, 20.0, 7.620, 15.0, 0.005, 301.0, 605.0, 5, 0, 0.50, 0.90, dbLoss, strMode, 100, errNum);

	EXPECT_DOUBLE_EQ(88.138262130231965, dbLoss);
	EXPECT_EQ(0, errNum);
	EXPECT_STREQ("Line-Of-Sight Mode", strMode);

	elev[0] = 11.0;
	elev[1] = 84.42810;
	for (int i = 2; i < 4960; i++)
	{
		elev[i] = 0.0;
	}

	PointToPointCalculationLegacy(elev, 20.0, 7.620, 15.0, 0.005, 301.0, 605.0, 5, 0, 0.50, 0.90, dbLoss, strMode, 100, errNum);

	EXPECT_DOUBLE_EQ(87.445162313824255, dbLoss);
	EXPECT_EQ(4, errNum);
	EXPECT_STREQ("Line-Of-Sight Mode", strMode);
}

TEST(ItwomTests, PointToPointCalculationTests)
{
	double dbLoss;
	int errNum;
	char strMode[100];

	double elev[4960];
	elev[0] = 12.0;
	elev[1] = 83.8170;
	for (int i = 2; i < 4960; i++)
	{
		elev[i] = 0.0;
	}

	PointToPointCalculation(elev, 20.0, 7.620, 15.0, 0.005, 301.0, 605.0, 5, 0, 0.50, 0.90, dbLoss, strMode, 100, errNum);

	EXPECT_DOUBLE_EQ(110.13892003947502, dbLoss);
	EXPECT_EQ(0, errNum);
	EXPECT_STREQ("L-o-S", strMode);

	elev[0] = 11.0;
	elev[1] = 84.42810;
	for (int i = 2; i < 4960; i++)
	{
		elev[i] = 0.0;
	}

	PointToPointCalculation(elev, 20.0, 7.620, 15.0, 0.005, 301.0, 605.0, 5, 0, 0.50, 0.90, dbLoss, strMode, 100, errNum);

	EXPECT_DOUBLE_EQ(109.44593397671387, dbLoss);
	EXPECT_EQ(4, errNum);
	EXPECT_STREQ("L-o-S", strMode);
}

TEST(ItwomTests, PointToPointCalculationMdhTwoTests)
{
	double dbLoss, deltaH;
	int propMode, errNum;

	double elev[4960];
	elev[0] = 12.0;
	elev[1] = 83.8170;
	for (int i = 2; i < 4960; i++)
	{
		elev[i] = 0.0;
	}

	PointToPointCalculationMdhTwo(elev, 20.0, 7.620, 15.0, 0.005, 301.0, 1000.0, 10.0, 0.5, 0.0, 605.0, 5, 0, 1, 0.9, 0.5, 0.75, dbLoss, propMode, deltaH, errNum);

	EXPECT_DOUBLE_EQ(115.49231165878668, dbLoss);
	EXPECT_EQ(0, propMode);
	EXPECT_DOUBLE_EQ(0.0, deltaH);
	EXPECT_EQ(0, errNum);

	elev[0] = 11.0;
	elev[1] = 84.42810;
	for (int i = 2; i < 4960; i++)
	{
		elev[i] = 0.0;
	}

	PointToPointCalculationMdhTwo(elev, 20.0, 7.620, 15.0, 0.005, 301.0, 1000.0, 10.0, 0.5, 0.0, 605.0, 5, 0, 1, 0.9, 0.5, 0.75, dbLoss, propMode, deltaH, errNum);

	EXPECT_DOUBLE_EQ(114.80220932417001, dbLoss);
	EXPECT_EQ(0, propMode);
	EXPECT_DOUBLE_EQ(0.0, deltaH);
	EXPECT_EQ(4, errNum);
}

TEST(ItwomTests, PointToPointCalculationDhTests)
{
	double dbLoss, deltaH;
	int errNum;

	double elev[4960];
	elev[0] = 12.0;
	elev[1] = 83.8170;
	for (int i = 2; i < 4960; i++)
	{
		elev[i] = 0.0;
	}

	PointToPointCalculationDh(elev, 20.0, 7.620, 15.0, 0.005, 301.0, 1000.0, 10.0, 0.5, 0.0, 605.0, 5, 0, 1.2, 0.2, 0.0, dbLoss, deltaH, errNum);

	EXPECT_DOUBLE_EQ(147.87972600143414, dbLoss);
	EXPECT_DOUBLE_EQ(0.0, deltaH);
	EXPECT_EQ(1, errNum);

	elev[0] = 11.0;
	elev[1] = 84.42810;
	for (int i = 2; i < 4960; i++)
	{
		elev[i] = 0.0;
	}

	PointToPointCalculationDh(elev, 20.0, 7.620, 15.0, 0.005, 301.0, 1000.0, 10.0, 0.5, 0.0, 605.0, 5, 0, 1.2, 0.2, 0.0, dbLoss, deltaH, errNum);

	EXPECT_DOUBLE_EQ(147.20870079856931, dbLoss);
	EXPECT_DOUBLE_EQ(0.0, deltaH);
	EXPECT_EQ(4, errNum);
}

TEST(ItwomTests, AreaCalculationTests)
{
	double dbLoss;
	int errNum;

	AreaCalculation(3, 2.3, 20.0, 7.620, 13.0, 1, 2, 15.0, 0.005, 301.0, 1000.0, 10.0, 0.5, 0.0, 605.0, 5, 0, 0, 0.9, 0.5, 0.75, dbLoss, NULL, errNum);

	EXPECT_DOUBLE_EQ(1208.8422817277913, dbLoss);
	EXPECT_EQ(0, errNum);
}

TEST(ItwomTests, IrregularTerrainModelAreaCalculationDbLossTests)
{
	double dbLoss;

	dbLoss = IrregularTerrainModelAreaCalculationDbLoss(3, 2.3, 20.0, 7.620, 13.0, 1, 2, 15.0, 0.005, 301.0, 1000.0, 10.0, 0.5, 0.0, 605.0, 5, 0, 0, 0.9, 0.5, 0.75);

	EXPECT_DOUBLE_EQ(1208.8422817277913, dbLoss);
}

TEST(ItwomTests, CalculateClutterLossTests)
{
	double clutterLoss;
	prop_type prop;
	propa_type propa;
	double d;

	d = 0.0;
	prop.antennaHeightAboveGroundLevel[0] = 20.0;
	prop.antennaHeightAboveGroundLevel[1] = 7.620;
	prop.antennaHeightAndPathElevation[0] = 21.0;
	prop.antennaHeightAndPathElevation[1] = 8.620;
	prop.canopyClutterHeight = 2.0;
	prop.transmitterGroundHeight = 30.0;
	prop.tsgh = 15.0;
	prop.waveNumber = 100.0;
	prop.surfaceRefractivity = 325.0;
	prop.encc = 1000.0;
	prop.transmitterPolarization = 0;

	clutterLoss = CalculateClutterLoss(d, prop, propa);

	EXPECT_DOUBLE_EQ(0.0, clutterLoss);

	d = 2.0;

	clutterLoss = CalculateClutterLoss(d, prop, propa);

	EXPECT_DOUBLE_EQ(0.0, clutterLoss);

	prop.canopyClutterHeight = 22.5;

	clutterLoss = CalculateClutterLoss(d, prop, propa);

	EXPECT_DOUBLE_EQ(0.02634783281894764, clutterLoss);

	prop.transmitterGroundHeight = 15.0;

	clutterLoss = CalculateClutterLoss(d, prop, propa);

	EXPECT_DOUBLE_EQ(39.193931594212195, clutterLoss);

	prop.transmitterPolarization = 1;

	clutterLoss = CalculateClutterLoss(d, prop, propa);

	EXPECT_DOUBLE_EQ(39.193931594212195, clutterLoss);

	prop.transmitterPolarization = 2;

	clutterLoss = CalculateClutterLoss(d, prop, propa);

	EXPECT_DOUBLE_EQ(39.193931594212195, clutterLoss);
}

TEST(ItwomTests, CalculateDiffractionAttenuationTests)
{
	double diffractionAttenuation;
	prop_type prop;
	propa_type propa;
	double d;

	d = 0.0;
	prop.surfaceImpedanceReal = 1.0;
	prop.surfaceImpedanceImag = 0.5;
	prop.antennaHeightAboveGroundLevel[0] = 20.0;
	prop.antennaHeightAboveGroundLevel[1] = 7.620;
	prop.antennaEffectiveHeight[0] = 21.0;
	prop.antennaEffectiveHeight[1] = 8.620;
	prop.horizonDistance[0] = 1.0;
	prop.horizonDistance[1] = 2.0;
	prop.surfaceRefractivity = 325.0;
	prop.relativeCurvature = 157e-9 * (1.0 - 0.04665 * exp(prop.surfaceRefractivity / 179.3));
	prop.analysisMode = -1;
	prop.interdecileElevationRangeBetweenPoints = 1.0;
	prop.waveNumber = 100.0;
	propa.dla = 3.0;
	propa.tha = 20.0;
	propa.dlsa = 1000000.0;

	diffractionAttenuation = CalculateDiffractionAttenuation(d, prop, propa);

	EXPECT_DOUBLE_EQ(0.0, diffractionAttenuation);

	prop.analysisMode = 0;

	diffractionAttenuation = CalculateDiffractionAttenuation(d, prop, propa);

	EXPECT_DOUBLE_EQ(0.0, diffractionAttenuation);

	d = 4.0;

	diffractionAttenuation = CalculateDiffractionAttenuation(d, prop, propa);

	EXPECT_DOUBLE_EQ(94.63238012826875, diffractionAttenuation);
}

TEST(ItwomTests, CalculateDiffractionAttenuationV2Tests)
{
	double diffractionAttenuation;
	prop_type prop;
	propa_type propa;
	double d;

	d = 0.0;
	prop.surfaceImpedanceReal = 1.0;
	prop.surfaceImpedanceImag = 0.5;
	prop.antennaHeightAboveGroundLevel[0] = 20.0;
	prop.antennaHeightAboveGroundLevel[1] = 7.620;
	prop.antennaEffectiveHeight[0] = 21.0;
	prop.antennaEffectiveHeight[1] = 8.620;
	prop.antennaHeightAndPathElevation[0] = 21.0;
	prop.antennaHeightAndPathElevation[1] = 8.620;
	prop.horizonDistance[0] = 1.0;
	prop.horizonDistance[1] = 2.0;
	prop.surfaceRefractivity = 325.0;
	prop.relativeCurvature = 157e-9 * (1.0 - 0.04665 * exp(prop.surfaceRefractivity / 179.3));
	prop.analysisMode = -1;
	prop.interdecileElevationRangeBetweenPoints = 1.0;
	prop.waveNumber = 100.0;
	prop.grazingAngle[0] = 0.0;
	prop.grazingAngle[1] = 5.0;
	prop.hht = 3300.0;
	prop.hhr = 3200.0;
	prop.distance = 4.0;
	prop.canopyClutterHeight = 22.5;
	propa.dla = 3.0;
	propa.tha = 20.0;
	propa.dlsa = 1000000.0;

	diffractionAttenuation = CalculateDiffractionAttenuationV2(d, prop, propa);

	EXPECT_DOUBLE_EQ(0.0, diffractionAttenuation);

	prop.grazingAngle[1] = 0.1;

	diffractionAttenuation = CalculateDiffractionAttenuationV2(d, prop, propa);

	EXPECT_DOUBLE_EQ(0.0, diffractionAttenuation);

	prop.analysisMode = 0;

	diffractionAttenuation = CalculateDiffractionAttenuationV2(d, prop, propa);

	EXPECT_DOUBLE_EQ(0.0, diffractionAttenuation);

	d = 4.0;
	prop.grazingAngle[1] = 5.0;

	diffractionAttenuation = CalculateDiffractionAttenuationV2(d, prop, propa);

	EXPECT_DOUBLE_EQ(30.800000000000001, diffractionAttenuation);

	prop.hht = 3500.0;

	diffractionAttenuation = CalculateDiffractionAttenuationV2(d, prop, propa);

	EXPECT_DOUBLE_EQ(30.800000000000001, diffractionAttenuation);

	prop.grazingAngle[1] = 1.15;

	diffractionAttenuation = CalculateDiffractionAttenuationV2(d, prop, propa);

	EXPECT_DOUBLE_EQ(164.08214416555981, diffractionAttenuation);

	prop.grazingAngle[1] = 0.55;

	diffractionAttenuation = CalculateDiffractionAttenuationV2(d, prop, propa);

	EXPECT_DOUBLE_EQ(160.14159400852918, diffractionAttenuation);

	prop.grazingAngle[1] = 0.15;
	prop.hht = 3300.0;

	diffractionAttenuation = CalculateDiffractionAttenuationV2(d, prop, propa);

	EXPECT_DOUBLE_EQ(123.27607770858398, diffractionAttenuation);

	prop.hht = 3500.0;
	prop.hhr = 3600.0;

	diffractionAttenuation = CalculateDiffractionAttenuationV2(d, prop, propa);

	EXPECT_DOUBLE_EQ(159.3628643671268, diffractionAttenuation);

	prop.horizonDistance[0] = 0.0;

	diffractionAttenuation = CalculateDiffractionAttenuationV2(d, prop, propa);

	EXPECT_DOUBLE_EQ(157.55607568116454, diffractionAttenuation);

	d = 0.5;
	prop.horizonDistance[1] = 0.0;

	diffractionAttenuation = CalculateDiffractionAttenuationV2(d, prop, propa);

	EXPECT_DOUBLE_EQ(5.7999999999999998, diffractionAttenuation);

	prop.horizonDistance[1] = 2.0;
	prop.hht = 3300.0;

	diffractionAttenuation = CalculateDiffractionAttenuationV2(d, prop, propa);

	EXPECT_DOUBLE_EQ(36.282342495122933, diffractionAttenuation);

	prop.hht = 3500.0;

	diffractionAttenuation = CalculateDiffractionAttenuationV2(d, prop, propa);

	EXPECT_DOUBLE_EQ(91.465970576819103, diffractionAttenuation);

	prop.grazingAngle[1] = 5.0;

	diffractionAttenuation = CalculateDiffractionAttenuationV2(d, prop, propa);

	EXPECT_DOUBLE_EQ(30.800000000000001, diffractionAttenuation);

	prop.grazingAngle[1] = 1.15;

	diffractionAttenuation = CalculateDiffractionAttenuationV2(d, prop, propa);

	EXPECT_DOUBLE_EQ(22.0, diffractionAttenuation);

	prop.grazingAngle[1] = 0.55;

	diffractionAttenuation = CalculateDiffractionAttenuationV2(d, prop, propa);

	EXPECT_DOUBLE_EQ(91.465970576819103, diffractionAttenuation);
}

TEST(ItwomTests, CalculateScatterAttenuationTests)
{
	double scatterAttenuation;
	prop_type prop;
	propa_type propa;
	double d;

	d = 0.0;
	prop.antennaEffectiveHeight[0] = 21.0;
	prop.antennaEffectiveHeight[1] = 8.620;
	prop.horizonDistance[0] = 1.0;
	prop.horizonDistance[1] = 2.0;
	prop.antennaHeightAndPathElevation[0] = 21.0;
	prop.antennaHeightAndPathElevation[1] = 8.620;
	prop.surfaceRefractivity = 325.0;
	prop.relativeCurvature = 157e-9 * (1.0 - 0.04665 * exp(prop.surfaceRefractivity / 179.3));
	prop.grazingAngle[0] = 0.0;
	prop.grazingAngle[1] = 5.0;
	prop.waveNumber = 100.0;
	propa.tha = 20.0;

	scatterAttenuation = CalculateScatterAttenuation(d, prop, propa);

	EXPECT_DOUBLE_EQ(0.0, scatterAttenuation);

	prop.horizonDistance[0] = 3.0;

	scatterAttenuation = CalculateScatterAttenuation(d, prop, propa);

	EXPECT_DOUBLE_EQ(0.0, scatterAttenuation);

	d = 2.0;

	scatterAttenuation = CalculateScatterAttenuation(d, prop, propa);

	EXPECT_DOUBLE_EQ(203.82398136037997, scatterAttenuation);

	prop.antennaEffectiveHeight[0] = 0.00001;
	prop.antennaEffectiveHeight[1] = 0.00001;

	scatterAttenuation = CalculateScatterAttenuation(d, prop, propa);

	EXPECT_DOUBLE_EQ(1001.0, scatterAttenuation);
}

TEST(ItwomTests, QlrpsTests)
{
	prop_type prop1, prop2;

	Qlrps(605.0, 0.0, 325.0, 0, 12.0, 1.2, prop1);

	EXPECT_DOUBLE_EQ(12.683438155136267, prop1.waveNumber);
	EXPECT_DOUBLE_EQ(325.0, prop1.surfaceRefractivity);
	EXPECT_DOUBLE_EQ(1.1213006250331002e-07, prop1.relativeCurvature);
	EXPECT_DOUBLE_EQ(4.9138508618991086, prop1.surfaceImpedanceReal);
	EXPECT_DOUBLE_EQ(3.6257316907055608, prop1.surfaceImpedanceImag);

	Qlrps(605.0, 3.0, 325.0, 3, 12.0, 1.2, prop2);

	EXPECT_DOUBLE_EQ(12.683438155136267, prop2.waveNumber);
	EXPECT_DOUBLE_EQ(324.89695080147652, prop2.surfaceRefractivity);
	EXPECT_DOUBLE_EQ(1.1215584322070243e-07, prop2.relativeCurvature);
	EXPECT_DOUBLE_EQ(0.13309950673826831, prop2.surfaceImpedanceReal);
	EXPECT_DOUBLE_EQ(-0.093079255757644211, prop2.surfaceImpedanceImag);
}

TEST(ItwomTests, CalculateLineOfSightAttenuationTests)
{
	double lineOfSightAttenuation;
	prop_type prop;
	propa_type propa;
	double d;

	d = 0.0;
	prop.surfaceImpedanceReal = 1.0;
	prop.surfaceImpedanceImag = 0.5;
	prop.interdecileElevationRangeBetweenPoints = 1.0;
	prop.waveNumber = 100.0;
	prop.antennaEffectiveHeight[0] = 21.0;
	prop.antennaEffectiveHeight[1] = 8.620;
	propa.emd = 1.2;
	propa.aed = 5.0;
	propa.dlsa = 1000000.0;

	lineOfSightAttenuation = CalculateLineOfSightAttenuation(d, prop, propa);

	EXPECT_DOUBLE_EQ(0.0, lineOfSightAttenuation);

	d = 2.0;

	lineOfSightAttenuation = CalculateLineOfSightAttenuation(d, prop, propa);

	EXPECT_DOUBLE_EQ(-3.9167149496685898, lineOfSightAttenuation);

	prop.antennaEffectiveHeight[0] = 0.00001;
	prop.antennaEffectiveHeight[1] = 0.00001;

	lineOfSightAttenuation = CalculateLineOfSightAttenuation(d, prop, propa);

	EXPECT_DOUBLE_EQ(77.528123150083999, lineOfSightAttenuation);
}

TEST(ItwomTests, CalculateLineOfSightAttenuationV2Tests)
{
	double lineOfSightAttenuation;
	prop_type prop;
	propa_type propa;
	double d;

	d = 0.0;
	prop.surfaceImpedanceReal = 1.0;
	prop.surfaceImpedanceImag = 0.5;
	prop.interdecileElevationRangeBetweenPoints = 1.0;
	prop.waveNumber = 100.0;
	prop.antennaHeightAboveGroundLevel[0] = 20.0;
	prop.antennaHeightAboveGroundLevel[1] = 7.620;
	prop.antennaEffectiveHeight[0] = 21.0;
	prop.antennaEffectiveHeight[1] = 8.620;
	prop.antennaHeightAndPathElevation[0] = 21.0;
	prop.antennaHeightAndPathElevation[1] = 8.620;
	prop.horizonDistance[0] = 1.0;
	prop.horizonDistance[1] = 2.0;
	prop.transmitterTotalHeight = 21.0;
	prop.receiverTotalHeight = 8.620;
	prop.horizonHeight = 1.0;
	prop.distance = 5.0;
	prop.analysisMode = -1;
	prop.canopyClutterHeight = 22.5;
	prop.thera = 0.5;
	prop.thenr = 0.5;

	lineOfSightAttenuation = CalculateLineOfSightAttenuationV2(d, prop, propa);

	EXPECT_DOUBLE_EQ(0.0, lineOfSightAttenuation);

	d = 4.0;

	lineOfSightAttenuation = CalculateLineOfSightAttenuationV2(d, prop, propa);

	EXPECT_DOUBLE_EQ(22.0, lineOfSightAttenuation);

	prop.analysisMode = 0;

	lineOfSightAttenuation = CalculateLineOfSightAttenuationV2(d, prop, propa);

	EXPECT_DOUBLE_EQ(22.0, lineOfSightAttenuation);

	prop.surfaceImpedanceReal = 0.25;
	prop.surfaceImpedanceImag = 0.25;

	lineOfSightAttenuation = CalculateLineOfSightAttenuationV2(d, prop, propa);

	EXPECT_DOUBLE_EQ(22.0, lineOfSightAttenuation);

	prop.antennaHeightAboveGroundLevel[1] = 25.0;

	lineOfSightAttenuation = CalculateLineOfSightAttenuationV2(d, prop, propa);

	EXPECT_DOUBLE_EQ(-1.7931089645393468, lineOfSightAttenuation);
}

TEST(ItwomTests, QlraTests)
{
	int kst[2];
	prop_type prop1, prop2, prop3;
	propv_type propv1, propv2, propv3;
	int mdvarx, klimx;

	kst[0] = 0;
	kst[1] = 0;
	prop1.antennaHeightAboveGroundLevel[0] = 20.0;
	prop1.antennaHeightAboveGroundLevel[1] = 7.620;
	prop1.interdecileElevationRangeBetweenPoints = 1.0;
	prop1.surfaceRefractivity = 325.0;
	prop1.relativeCurvature = 157e-9 * (1.0 - 0.04665 * exp(prop1.surfaceRefractivity / 179.3));
	propv1.recomputeParameters = 0;
	propv1.variabilityMode = 0;
	propv1.radioClimate = 0;
	mdvarx = -1;
	klimx = 0;

	Qlra(kst, klimx, mdvarx, prop1, propv1);

	EXPECT_DOUBLE_EQ(20.0, prop1.antennaEffectiveHeight[0]);
	EXPECT_DOUBLE_EQ(7.620, prop1.antennaEffectiveHeight[1]);
	EXPECT_DOUBLE_EQ(18593.929486598805, prop1.horizonDistance[0]);
	EXPECT_DOUBLE_EQ(11366.28315718191, prop1.horizonDistance[1]);
	EXPECT_DOUBLE_EQ(-0.0021172867537533244, prop1.grazingAngle[0]);
	EXPECT_DOUBLE_EQ(-0.0013058026177807953, prop1.grazingAngle[1]);
	EXPECT_EQ(1, prop1.analysisMode);
	EXPECT_EQ(3, propv1.recomputeParameters);
	EXPECT_EQ(0, propv1.variabilityMode);
	EXPECT_EQ(0, propv1.radioClimate);

	kst[0] = 1;
	kst[1] = 1;
	prop2.antennaHeightAboveGroundLevel[0] = 20.0;
	prop2.antennaHeightAboveGroundLevel[1] = 7.620;
	prop2.interdecileElevationRangeBetweenPoints = 1.0;
	prop2.surfaceRefractivity = 325.0;
	prop2.relativeCurvature = 157e-9 * (1.0 - 0.04665 * exp(prop2.surfaceRefractivity / 179.3));
	propv2.recomputeParameters = 0;
	propv2.variabilityMode = 0;
	propv2.radioClimate = 0;
	mdvarx = 2;
	klimx = 3;

	Qlra(kst, klimx, mdvarx, prop2, propv2);

	EXPECT_DOUBLE_EQ(20.000000010305769, prop2.antennaEffectiveHeight[0]);
	EXPECT_DOUBLE_EQ(7.6200012031564404, prop2.antennaEffectiveHeight[1]);
	EXPECT_DOUBLE_EQ(18593.929491464409, prop2.horizonDistance[0]);
	EXPECT_DOUBLE_EQ(11366.284077273896, prop2.horizonDistance[1]);
	EXPECT_DOUBLE_EQ(-0.0021172867542992519, prop2.grazingAngle[0]);
	EXPECT_DOUBLE_EQ(-0.0013058027212109261, prop2.grazingAngle[1]);
	EXPECT_EQ(1, prop2.analysisMode);
	EXPECT_EQ(5, propv2.recomputeParameters);
	EXPECT_EQ(2, propv2.variabilityMode);
	EXPECT_EQ(3, propv2.radioClimate);

	kst[0] = 2;
	kst[1] = 3;
	prop3.antennaHeightAboveGroundLevel[0] = 3.0;
	prop3.antennaHeightAboveGroundLevel[1] = 4.0;
	prop3.interdecileElevationRangeBetweenPoints = 1.0;
	prop3.surfaceRefractivity = 325.0;
	prop3.relativeCurvature = 157e-9 * (1.0 - 0.04665 * exp(prop3.surfaceRefractivity / 179.3));
	propv3.recomputeParameters = 0;
	propv3.variabilityMode = 0;
	propv3.radioClimate = 0;
	mdvarx = 3;
	klimx = 0;

	Qlra(kst, klimx, mdvarx, prop3, propv3);

	EXPECT_DOUBLE_EQ(3.020526927261288, prop3.antennaEffectiveHeight[0]);
	EXPECT_DOUBLE_EQ(4.0032068580213407, prop3.antennaEffectiveHeight[1]);
	EXPECT_DOUBLE_EQ(7113.7689219010153, prop3.horizonDistance[0]);
	EXPECT_DOUBLE_EQ(8189.5947891973183, prop3.horizonDistance[1]);
	EXPECT_DOUBLE_EQ(-0.00082021716914179241, prop3.grazingAngle[0]);
	EXPECT_DOUBLE_EQ(-0.0009450556502009612, prop3.grazingAngle[1]);
	EXPECT_EQ(1, prop3.analysisMode);
	EXPECT_EQ(4, propv3.recomputeParameters);
	EXPECT_EQ(3, propv3.variabilityMode);
	EXPECT_EQ(0, propv3.radioClimate);
}

TEST(ItwomTests, CalculateLongleyRicePropagationTests)
{
	double d = 0.0;
	prop_type prop;
	propa_type propa;

	prop.surfaceImpedanceReal = 1.0;
	prop.surfaceImpedanceImag = 0.5;
	prop.antennaHeightAboveGroundLevel[0] = 20.0;
	prop.antennaHeightAboveGroundLevel[1] = 7.620;
	prop.antennaEffectiveHeight[0] = 21.0;
	prop.antennaEffectiveHeight[1] = 8.620;
	prop.antennaHeightAndPathElevation[0] = 21.0;
	prop.antennaHeightAndPathElevation[1] = 8.620;
	prop.interdecileElevationRangeBetweenPoints = 1.0;
	prop.surfaceRefractivity = 325.0;
	prop.relativeCurvature = 157e-9 * (1.0 - 0.04665 * exp(prop.surfaceRefractivity / 179.3));
	prop.grazingAngle[0] = 0.0;
	prop.grazingAngle[1] = 5.0;
	prop.horizonDistance[0] = 1.0;
	prop.horizonDistance[1] = 2.0;
	prop.waveNumber = 100.0;
	prop.distance = 4.0;
	prop.analysisMode = -1;

	CalculateLongleyRicePropagation(d, prop, propa);

	EXPECT_DOUBLE_EQ(36.34131182280791, prop.referenceAttenuation);
	EXPECT_DOUBLE_EQ(31753.280350337467, propa.dlsa);
	EXPECT_DOUBLE_EQ(36.275972255121474, propa.ael);
	EXPECT_DOUBLE_EQ(0.016334891921609335, propa.ak1);
	EXPECT_DOUBLE_EQ(0.0, propa.ak2);
	EXPECT_DOUBLE_EQ(304.79019257778907, propa.aed);
	EXPECT_DOUBLE_EQ(0.0078786248096725398, propa.emd);
	EXPECT_DOUBLE_EQ(19353.682543364997, propa.dls[0]);
	EXPECT_DOUBLE_EQ(12399.59780697247, propa.dls[1]);
	EXPECT_DOUBLE_EQ(3.0, propa.dla);
	EXPECT_DOUBLE_EQ(5.0, propa.tha);
	EXPECT_EQ(4, prop.errorCode);
}

TEST(ItwomTests, CalculateLongleyRicePropagationV2Tests)
{
	double d = 0.0;
	prop_type prop;
	propa_type propa;

	prop.surfaceImpedanceReal = 1.0;
	prop.surfaceImpedanceImag = 0.5;
	prop.antennaHeightAboveGroundLevel[0] = 20.0;
	prop.antennaHeightAboveGroundLevel[1] = 7.620;
	prop.antennaEffectiveHeight[0] = 21.0;
	prop.antennaEffectiveHeight[1] = 8.620;
	prop.antennaHeightAndPathElevation[0] = 21.0;
	prop.antennaHeightAndPathElevation[1] = 8.620;
	prop.interdecileElevationRangeBetweenPoints = 1.0;
	prop.surfaceRefractivity = 325.0;
	prop.relativeCurvature = 157e-9 * (1.0 - 0.04665 * exp(prop.surfaceRefractivity / 179.3));
	prop.grazingAngle[0] = 0.0;
	prop.grazingAngle[1] = 5.0;
	prop.horizonDistance[0] = 1.0;
	prop.horizonDistance[1] = 2.0;
	prop.waveNumber = 100.0;
	prop.distance = 4.0;
	prop.analysisMode = -1;
	prop.canopyClutterHeight = 22.5;
	prop.encc = 1000.0;
	prop.hht = 3300.0;
	prop.hhr = 3200.0;
	prop.horizonHeight = 1.0;
	prop.receiverTotalHeight = 8.620;
	prop.thera = 0.5;
	prop.thenr = 0.5;
	prop.transmitterPolarization = 0;
	prop.transmitterTotalHeight = 21.0;
	prop.lineOfSight = 1;
	prop.transmitterIntervalWidth = 83.8170;

	CalculateLongleyRicePropagationV2(d, prop, propa);

	EXPECT_DOUBLE_EQ(22.0, prop.referenceAttenuation);
	EXPECT_DOUBLE_EQ(20.0, prop.transmitterGroundHeight);
	EXPECT_DOUBLE_EQ(1.0, prop.tsgh);
	EXPECT_DOUBLE_EQ(4.0, prop.distance);
	EXPECT_DOUBLE_EQ(31753.280350337467, propa.dlsa);
	EXPECT_DOUBLE_EQ(2000000.0, propa.dx);
	EXPECT_DOUBLE_EQ(30.800000000000001, propa.aed);
	EXPECT_DOUBLE_EQ(0.0, propa.emd);
	EXPECT_DOUBLE_EQ(19353.682543364997, propa.dls[0]);
	EXPECT_DOUBLE_EQ(12399.59780697247, propa.dls[1]);
	EXPECT_DOUBLE_EQ(3.0, propa.dla);
	EXPECT_DOUBLE_EQ(5.0, propa.tha);
	EXPECT_EQ(4, prop.errorCode);
	EXPECT_EQ(-1, prop.analysisMode);
}

TEST(ItwomTests, CalculateLongleyRiceVariabilityTests)
{
	double zt = 2.37;
	double zl = 2.58;
	double zc = 2.43;
	prop_type prop;
	propv_type propv;
	prop.waveNumber = 100.0;
	prop.antennaEffectiveHeight[0] = 21.0;
	prop.antennaEffectiveHeight[1] = 8.620;
	prop.distance = 4.0;
	prop.interdecileElevationRangeBetweenPoints = 1.0;
	prop.referenceAttenuation = 22.0;
	propv.recomputeParameters = 5;
	propv.variabilityMode = 0;
	propv.radioClimate = 0;

	double longleyRiceVariability = CalculateLongleyRiceVariability(zt, zl, zc, prop, propv);

	EXPECT_DOUBLE_EQ(-1.7048997061336799, longleyRiceVariability);
	EXPECT_DOUBLE_EQ(10.391812490576163, propv.sgc);
	EXPECT_EQ(0, propv.recomputeParameters);
	EXPECT_EQ(0, propv.variabilityMode);
	EXPECT_EQ(5, propv.radioClimate);
}

TEST(ItwomTests, CalculateHorizonDistancesTests)
{
	// JAD TODO: Finish unit tests.
}

TEST(ItwomTests, CalculateHorizonDistancesV2Tests)
{
	// JAD TODO: Finish unit tests.
}

TEST(ItwomTests, CalculateTerrainInterdecileRangeDeltaV2Tests)
{
	// JAD TODO: Finish unit tests.
}

TEST(ItwomTests, PrepareForPointToPointModeAnalysisTests)
{
	// JAD TODO: Finish unit tests.
}

TEST(ItwomTests, PrepareForPointToPointModeAnalysisV2Tests)
{
	// JAD TODO: Finish unit tests.
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
