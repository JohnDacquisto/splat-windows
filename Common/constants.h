#pragma once


//| ------------------------------
//| Pick these values for each build.
#define HD_MODE 0			//| OLD NAME: HDMODE
#define MAXIMUM_PAGES 4		//| OLD NAME: MAXPAGES
//| ------------------------------


//| Constants

#define GAMMA 2.5

#if HD_MODE==0
	#if MAXIMUM_PAGES==4
		#define ARRAY_SIZE 4950
	#endif
	#if MAXIMUM_PAGES==9
		#define ARRAY_SIZE 10870
	#endif
	#if MAXIMUM_PAGES==16
		#define ARRAY_SIZE 19240
	#endif
	#if MAXIMUM_PAGES==25
		#define ARRAY_SIZE 30025
	#endif
	#if MAXIMUM_PAGES==36
		#define ARRAY_SIZE 43217
	#endif
	#if MAXIMUM_PAGES==49
		#define ARRAY_SIZE 58813
	#endif
	#if MAXIMUM_PAGES==64
		#define ARRAY_SIZE 76810
	#endif
	#define INTEGER_PIXELS_PER_DEGREE 1200	//| OLD NAME: IPPD
#endif
#if HD_MODE==1
	#if MAXIMUM_PAGES==1
		#define ARRAY_SIZE 5092 
	#endif
	#if MAXIMUM_PAGES==4
		#define ARRAY_SIZE 14844 
	#endif
	#if MAXIMUM_PAGES==9
		#define ARRAY_SIZE 32600
	#endif
	#if MAXIMUM_PAGES==16
		#define ARRAY_SIZE 57713
	#endif
	#if MAXIMUM_PAGES==25
		#define ARRAY_SIZE 90072
	#endif
	#if MAXIMUM_PAGES==36
		#define ARRAY_SIZE 129650
	#endif
	#if MAXIMUM_PAGES==49 
		#define ARRAY_SIZE 176437
	#endif
	#if MAXIMUM_PAGES==64
		#define ARRAY_SIZE 230430
	#endif
	#define INTEGER_PIXELS_PER_DEGREE 3600
#endif

#ifndef PI
	#define PI 3.141592653589793
#endif

#ifndef TWO_PI
	#define TWO_PI 6.283185307179586	//| OLD NAME: TWOPI
#endif

#ifndef HALF_PI
	#define HALF_PI 1.570796326794896	//| OLD NAME: HALFPI
#endif

#define DEGREES_TO_RADIANS 1.74532925199e-02	//| OLD NAME: DEG2RAD
#define EARTH_RADIUS 20902230.97				//| OLD NAME: EARTHRADIUS
#define	METERS_PER_MILE 1609.344
#define METERS_PER_FOOT 0.3048
#define	KM_PER_MILE 1.609344
#define FOUR_THIRDS 1.3333333333333

#define ONE_THIRD (1.0/3.0)
