/****************************************************************************
*							Constants						                *
****************************************************************************/

#pragma once

#include "stdafx.h"


// Constants

#define GAMMA 2.5

#define SPLAT_NAME "SPLAT\0"	// OLD NAME: splat_name

#define SPLAT_VERSION "1.4.2\0"	// OLD NAME: splat_version

#define ITWOM_VERSION 3.0		// OLD NAME: ITWOMVersion

#ifndef PI
	#define PI 3.141592653589793
#endif

#ifndef TWO_PI
	#define TWO_PI 6.283185307179586			// OLD NAME: TWOPI
#endif

#ifndef HALF_PI
	#define HALF_PI 1.570796326794896			// OLD NAME: HALFPI
#endif

#define DEGREES_TO_RADIANS 1.74532925199e-02	// OLD NAME: DEG2RAD

#define PIXELS_PER_DEGREE_STANDARD 1200

#define PIXELS_PER_DEGREE_HIGH_DEF 3600

#define EARTH_RADIUS_FEET 20902230.97			// OLD NAME: EARTHRADIUS

#define EARTH_RADIUS_MILES 3959.0

#define	METERS_PER_MILE 1609.344

#define FEET_PER_MILE 5280.0

#define METERS_PER_FOOT 0.3048

#define FEET_PER_METER 3.28084

#define	KM_PER_MILE 1.609344

#define FOUR_THIRDS 1.3333333333333

#define ONE_THIRD (1.0/3.0)

#define MINIMUM_FREQUENCY 20.0				// MHz

#define MAXIMUM_FREQUENCY 20000.0			// MHz

#define AVERAGE_TERRAIN_MIN_DISTANCE 2.0	// miles

#define AVERAGE_TERRAIN_MAX_DISTANCE 10.0	// miles

#define DIPOLE_50_OHM 50.0

#define DIPOLE_75_OHM 75.0

#define LOCATION_NOT_IN_MEMORY -5000.0

#define LOCATION_CRITICAL_ERROR -9999.0

#define SAMPLES_PER_RADIAN_STANDARD 68755.0

#define SAMPLES_PER_RADIAN_HIGH_DEF 206265.0

#define DIPOLE_TO_ISOTROPIC_ANTENNA 2.14				// dB : ERP [dBd] on dipole to EIRP [dBi] on isotropic

#define DIPOLE_TO_ISOTROPIC_ANTENNA_RATIO 1.636816521	// ERP [W] on dipole to EIRP [W] on isotropic

#define DBW_TO_DBM 30.0

#define FREE_SPACE_PATH_LOSS_CONSTANT 36.6	// dB : Adjustment to free-space path loss calculation, for freq in MHz and distance in miles.

#define SPEED_OF_LIGHT_FEET_PER_SECOND 9.8425e8

// JAD TODO: Not sure the significance of this value. May have to do with going from ERP watts
// to field strength volts, and/or going from frequency to meters via speed-of-light?
#define FIELD_STRENGTH_MAGIC_NUMBER 139.4

// JAD TODO: Not sure the significance of this value.
#define POWER_DENSITY_MAGIC_NUMBER 2589988.11
