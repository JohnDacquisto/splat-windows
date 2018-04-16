#pragma once


class IrregularTerrainModelParameters
{
public:
	IrregularTerrainModelParameters();
	~IrregularTerrainModelParameters();

	double eps_dielect;
	double sgm_conductivity;
	double eno_ns_surfref;
	double frq_mhz;
	double conf;
	double rel;
	double erp;
	int radio_climate;
	int pol;
	float antenna_pattern[361][1001];
};
