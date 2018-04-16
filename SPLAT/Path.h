#pragma once

//| JAD FIX
#include "C:/DEV/SPLAT/Common/constants.h"


class Path
{
public:
	Path();
	~Path();

	double lat[ARRAY_SIZE];
	double lon[ARRAY_SIZE];
	double elevation[ARRAY_SIZE];
	double distance[ARRAY_SIZE];
	int length;
};
