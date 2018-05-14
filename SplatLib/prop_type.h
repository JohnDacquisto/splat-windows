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

	double referenceAttenuation;				//| OLD NAME: aref
	double distance;							//| OLD NAME: dist
	double antennaHeightAboveGroundLevel[2];	//| OLD NAME: hg
	double antennaHeightAndPathElevation[2];	//| OLD NAME: rch
	double waveNumber;							//| OLD NAME: wn
	double interdecileElevationRangeBetweenPoints;	//| OLD NAME: dh
	double surfaceRefractivity;					//| OLD NAME ens
	double encc;
	double canopyClutterHeight;					//| OLD NAME: cch
	double clutterDensity;						//| OLD NAME: cd
	double relativeCurvature;					//| OLD NAME: gme
	double surfaceImpedanceReal;				//| OLD NAME: zgndreal
	double surfaceImpedanceImag;				//| OLD NAME: zgndimag
	double antennaEffectiveHeight[2];			//| OLD NAME: he
	double horizonDistance[2];					//| OLD NAME: dl
	double grazingAngle[2];						//| OLD NAME: the
	double transmitterIntervalWidth;			//| OLD NAME: tiw
	double transmitterTotalHeight;				//| OLD NAME: ght
	double receiverTotalHeight;					//| OLD NAME: ghr
	double horizonHeight;						//| OLD NAME: rph
	double hht;
	double hhr;
	double transmitterGroundHeight;				//| OLD NAME: tgh
	double tsgh;
	double thera;
	double thenr;
	int horizonHeightIndex;						//| OLD NAME: rpl
	int errorCode;								//| OLD NAME: kwx
	int analysisMode;							//| OLD NAME: mdp
	int transmitterPolarization;				//| OLD NAME: ptx
	int lineOfSight;							//| OLD NAME: los
};
