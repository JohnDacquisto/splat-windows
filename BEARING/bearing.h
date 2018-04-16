#pragma once
#include "stdafx.h"

//| JAD FIX
#include "C:/DEV/SPLAT/Common/Site.h"


//| Global variables

char BearingString[255];						//| OLD NAME: string

double TWO_PI = 6.283185307179586;				//| OLD NAME: TWOPI
double PI = 3.141592653589793;
double DEGREES_TO_RADIANS = 1.74532925199e-02;	//| OLD NAME: deg2rad
double KM_PER_MILE = 1.609344;

Site site;


//| Functions

double ArcCosine(double x, double y);

char *ConvertDecimalToDegreesMinutesSeconds(double decimal);

double GreatCircleDistanceBetweenSiteLocations(Site site1, Site site2);

double AzimuthAngleBetweenSites(Site source, Site destination);

double BearingStringToDecimalDegrees(char *input);

Site LoadSplatSiteLocationFile(char *filename);

int main(int argc, char *argv[]);
