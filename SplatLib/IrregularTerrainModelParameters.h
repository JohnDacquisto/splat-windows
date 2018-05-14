/****************************************************************************
*					Class: IrregularTerrainModelParameters	                *
****************************************************************************/

#pragma once
#include "stdafx.h"


class _declspec(dllexport) IrregularTerrainModelParameters
{
public:
	IrregularTerrainModelParameters();
	~IrregularTerrainModelParameters();

	double dielectricRelativePermittivityConstant;	//| OLD NAME: eps_dielect
	double conductivitySiemensPerMeter;				//| OLD NAME: sgm_conductivity
	double atmosphericBendingConstant;				//| OLD NAME: eno_ns_surfref
	double referenceFrequency;						//| OLD NAME: frq_mhz
	double fractionOfSituations;					//| OLD NAME: conf
	double fractionOfTime;							//| OLD NAME: rel
	double effectiveRadiatedPower;					//| OLD NAME: erp
	int radioClimate;								//| OLD NAME: radio_climate
	int antennaPolarization;						//| OLD NAME: pol
	float antennaRadiationPattern[361][1001];		//| OLD NAME: antenna_pattern
};
