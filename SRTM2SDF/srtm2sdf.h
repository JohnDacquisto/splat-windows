#pragma once
#include "stdafx.h"


//| Global variables

char	sdf_filename[30], sdf_path[255], replacement_flag, opened = 0,
	hgt = 0, bil = 0;

int	srtm[3601][3601], usgs[1201][1201], max_north, max_west, n,
	min_north, min_west, merge = 0, min_elevation, bzerror, ippd, mpi;


//| Functions

int ReadSRTM(char *filename);

int LoadSDF_SDF(char *name);

char LoadSDF(char *name);

int ReadUSGS();

void average_terrain(int y, int x, int z);

void WriteSDF(char *filename);

int main(int argc, char *argv[]);
