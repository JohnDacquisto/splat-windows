/****************************************************************************
*							Class: propv_type				                *
****************************************************************************/

#pragma once

#include "stdafx.h"


class _declspec(dllexport) propv_type
{
public:
	propv_type();
	~propv_type();

	double sgc;
	int recomputeParameters;	// OLD NAME: lvar
	int variabilityMode;		// OLD NAME: mdvar
	int radioClimate;			// OLD NAME: klim
};
