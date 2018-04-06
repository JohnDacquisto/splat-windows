#pragma once
#include "stdafx.h"


//| Global variables

char SplatDataFileName[30];		//| OLD NAME: sdf_filename
char SplatDataFilePath[255];	//| OLD NAME: sdf_path
char ElevationProblem;			//| OLD NAME: replacement_flag
char FileExtentionHgt = 0;		//| OLD NAME: hgt
char FileExtentionBil = 0;		//| OLD NAME: bil

int	SpaceShuttleRadarTopographyMissionData[3601][3601];	//| OLD NAME: srtm
int UnitedStatesGeologicalSurveyData[1201][1201];		//| OLD NAME: usgs
int MaximumLatitudeNorth;	//| OLD NAME: max_north
int MaximumLongitudeWest;	//| OLD NAME: max_west
int MinimumLatitudeNorth;	//| OLD NAME: min_north
int MinimumLongitudeWest;	//| OLD NAME: min_west
int MergeData = 0;			//| OLD NAME: merge
int MinimumElevation;		//| OLD NAME: min_elevation
int IntegerPixelsPerDegree;	//| OLD NAME: ippd
int MaximumPixelIndexPerDegree;	//| OLD NAME: mpi


//| Functions

int ReadSRTM(char *filename);

int LoadSDF_SDF(char *name);

char LoadSDF(char *name);

int ReadUSGS();

void average_terrain(int y, int x, int z);

void WriteSDF(char *filename);

int main(int argc, char *argv[]);
