#pragma once


class ColorRegion
{
public:
	ColorRegion();
	~ColorRegion();

	unsigned char color[32][3];
	int level[32];
	int levels;
};
