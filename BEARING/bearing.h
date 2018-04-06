#pragma once
#include "stdafx.h"


//| Global variables

char BearingString[255];						//| OLD NAME: string

double TWO_PI = 6.283185307179586;				//| OLD NAME: TWOPI
double PI = 3.141592653589793;
double DEGREES_TO_RADIANS = 1.74532925199e-02;	//| OLD NAME: deg2rad
double KM_PER_MILE = 1.609344;

struct site {
	double lat;
	double lon;
	double azimuth;
	char name[50];
} 	site;


//| Functions

double arccos(double x, double y);

char *dec2dms(double decimal);

double Distance(struct site site1, struct site site2);

double Azimuth(struct site source, struct site destination);

double ReadBearing(char *input);

struct site LoadQTH(char *filename);

int main(int argc, char *argv[]);
