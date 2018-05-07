/****************************************************************************
*							Class: propa_type				                *
****************************************************************************/

#pragma once
#include "stdafx.h"


class _declspec(dllexport) propa_type
{
public:
	propa_type();
	~propa_type();

	double dlsa;
	double dx;
	double ael;
	double ak1;
	double ak2;
	double aed;
	double emd;
	double aes;
	double ems;
	double dls[2];
	double dla;
	double tha;
};
