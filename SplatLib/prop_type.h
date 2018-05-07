/****************************************************************************
*							Class: prop_type				                *
****************************************************************************/

#pragma once
#include "stdafx.h"


class _declspec(dllexport) prop_type
{
public:
	prop_type();
	~prop_type();

	double aref;
	double dist;
	double hg[2];
	double rch[2];
	double wn;
	double dh;
	double dhd;
	double ens;
	double encc;
	double cch;
	double cd;
	double gme;
	double zgndreal;
	double zgndimag;
	double he[2];
	double dl[2];
	double the[2];
	double tiw;
	double ght;
	double ghr;
	double rph;
	double hht;
	double hhr;
	double tgh;
	double tsgh;
	double thera;
	double thenr;
	int rpl;
	int kwx;
	int mdp;
	int ptx;
	int los;
};
