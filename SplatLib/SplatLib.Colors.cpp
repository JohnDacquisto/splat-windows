/****************************************************************************\
*					SplatLib.Colors: SPLAT Colors							 *
*****************************************************************************/

#include "stdafx.h"
#include <math.h>
#include <io.h>
#include <stdlib.h>
#include <ctype.h>
#include <Windows.h>
#include "SplatLib.h"
#include "itwom.h"
#include "fontdata.h"
#include "..\Common\Common.h"
#include "..\Common\Site.h"
#include "..\Common\version.h"
#include "..\Common\constants.h"


//| ------------------------------
//| 
//| FUNCTION: LoadSplatSignalColorsForSite
//| 
//| OLD NAME: LoadSignalColors
//| 
//| NOTES: 
//| 
//| ------------------------------
void
LoadSplatSignalColorsForSite(
	Site xmtr,
	ColorRegion* region)
{
	int x, y, ok, val[4];
	char filename[255], ColorString[80], * pointer = NULL;
	FILE* fd = NULL;
	errno_t err;

	for (x = 0; (xmtr.filename[x] != '.') && (xmtr.filename[x] != 0) && (x < 250); x++)
	{
		filename[x] = xmtr.filename[x];
	}

	filename[x] = '.';
	filename[x + 1] = 's';
	filename[x + 2] = 'c';
	filename[x + 3] = 'm'; // Can't use .scf files in Windows.
	filename[x + 4] = 0;

	// Default values

	region->level[0] = 128;
	region->color[0][0] = 255;
	region->color[0][1] = 0;
	region->color[0][2] = 0;

	region->level[1] = 118;
	region->color[1][0] = 255;
	region->color[1][1] = 165;
	region->color[1][2] = 0;

	region->level[2] = 108;
	region->color[2][0] = 255;
	region->color[2][1] = 206;
	region->color[2][2] = 0;

	region->level[3] = 98;
	region->color[3][0] = 255;
	region->color[3][1] = 255;
	region->color[3][2] = 0;

	region->level[4] = 88;
	region->color[4][0] = 184;
	region->color[4][1] = 255;
	region->color[4][2] = 0;

	region->level[5] = 78;
	region->color[5][0] = 0;
	region->color[5][1] = 255;
	region->color[5][2] = 0;

	region->level[6] = 68;
	region->color[6][0] = 0;
	region->color[6][1] = 208;
	region->color[6][2] = 0;

	region->level[7] = 58;
	region->color[7][0] = 0;
	region->color[7][1] = 196;
	region->color[7][2] = 196;

	region->level[8] = 48;
	region->color[8][0] = 0;
	region->color[8][1] = 148;
	region->color[8][2] = 255;

	region->level[9] = 38;
	region->color[9][0] = 80;
	region->color[9][1] = 80;
	region->color[9][2] = 255;

	region->level[10] = 28;
	region->color[10][0] = 0;
	region->color[10][1] = 38;
	region->color[10][2] = 255;

	region->level[11] = 18;
	region->color[11][0] = 142;
	region->color[11][1] = 63;
	region->color[11][2] = 255;

	region->level[12] = 8;
	region->color[12][0] = 140;
	region->color[12][1] = 0;
	region->color[12][2] = 128;

	region->levels = 13;

	err = fopen_s(&fd, "splat.scm", "r");	// Can't use .scf files in Windows.

	if (fd == NULL)
	{
		err = fopen_s(&fd, filename, "r");
	}

	if (fd == NULL)
	{
		err = fopen_s(&fd, filename, "w");

		fprintf(fd, "; SPLAT Auto-generated Signal Color Definition (\"%s\") File\n", filename);
		fprintf(fd, ";\n; Format for the parameters held in this file is as follows:\n;\n");
		fprintf(fd, ";    dBuV/m: red, green, blue\n;\n");
		fprintf(fd, "; ...where \"dBuV/m\" is the signal strength (in dBuV/m) and\n");
		fprintf(fd, "; \"red\", \"green\", and \"blue\" are the corresponding RGB color\n");
		fprintf(fd, "; definitions ranging from 0 to 255 for the region specified.\n");
		fprintf(fd, ";\n; The following parameters may be edited and/or expanded\n");
		fprintf(fd, "; for future runs of SPLAT.  A total of 32 contour regions\n");
		fprintf(fd, "; may be defined in this file.\n;\n;\n");

		for (x = 0; x < region->levels; x++)
		{
			fprintf(fd, "%3d: %3d, %3d, %3d\n", region->level[x], region->color[x][0], region->color[x][1], region->color[x][2]);
		}

		fclose(fd);
	}
	else
	{
		x = 0;
		fgets(ColorString, 80, fd);

		while ((x < 32) && (feof(fd) == 0))
		{
			pointer = strchr(ColorString, ';');

			if (pointer != NULL)
			{
				*pointer = 0;
			}

			ok = sscanf_s(ColorString, "%d: %d, %d, %d", &val[0], &val[1], &val[2], &val[3]);

			if (ok == 4)
			{
				for (y = 0; y < 4; y++)
				{
					if (val[y] > 255)
					{
						val[y] = 255;
					}

					if (val[y] < 0)
					{
						val[y] = 0;
					}
				}

				region->level[x] = val[0];
				region->color[x][0] = val[1];
				region->color[x][1] = val[2];
				region->color[x][2] = val[3];
				x++;
			}

			fgets(ColorString, 80, fd);
		}

		fclose(fd);
		region->levels = x;
	}
}


//| ------------------------------
//| 
//| FUNCTION: LoadSplatLossColorsForSite
//| 
//| OLD NAME: LoadLossColors
//| 
//| NOTES: 
//| 
//| ------------------------------
void
LoadSplatLossColorsForSite(
	Site xmtr,
	ColorRegion* region)
{
	int x, y, ok, val[4];
	char filename[255], LossString[80], * pointer = NULL;
	FILE* fd = NULL;
	errno_t err;

	for (x = 0; (xmtr.filename[x] != '.') && (xmtr.filename[x] != 0) && (x < 250); x++)
	{
		filename[x] = xmtr.filename[x];
	}

	filename[x] = '.';
	filename[x + 1] = 'l';
	filename[x + 2] = 'c';
	filename[x + 3] = 'f';
	filename[x + 4] = 0;

	// Default values

	region->level[0] = 80;
	region->color[0][0] = 255;
	region->color[0][1] = 0;
	region->color[0][2] = 0;

	region->level[1] = 90;
	region->color[1][0] = 255;
	region->color[1][1] = 128;
	region->color[1][2] = 0;

	region->level[2] = 100;
	region->color[2][0] = 255;
	region->color[2][1] = 165;
	region->color[2][2] = 0;

	region->level[3] = 110;
	region->color[3][0] = 255;
	region->color[3][1] = 206;
	region->color[3][2] = 0;

	region->level[4] = 120;
	region->color[4][0] = 255;
	region->color[4][1] = 255;
	region->color[4][2] = 0;

	region->level[5] = 130;
	region->color[5][0] = 184;
	region->color[5][1] = 255;
	region->color[5][2] = 0;

	region->level[6] = 140;
	region->color[6][0] = 0;
	region->color[6][1] = 255;
	region->color[6][2] = 0;

	region->level[7] = 150;
	region->color[7][0] = 0;
	region->color[7][1] = 208;
	region->color[7][2] = 0;

	region->level[8] = 160;
	region->color[8][0] = 0;
	region->color[8][1] = 196;
	region->color[8][2] = 196;

	region->level[9] = 170;
	region->color[9][0] = 0;
	region->color[9][1] = 148;
	region->color[9][2] = 255;

	region->level[10] = 180;
	region->color[10][0] = 80;
	region->color[10][1] = 80;
	region->color[10][2] = 255;

	region->level[11] = 190;
	region->color[11][0] = 0;
	region->color[11][1] = 38;
	region->color[11][2] = 255;

	region->level[12] = 200;
	region->color[12][0] = 142;
	region->color[12][1] = 63;
	region->color[12][2] = 255;

	region->level[13] = 210;
	region->color[13][0] = 196;
	region->color[13][1] = 54;
	region->color[13][2] = 255;

	region->level[14] = 220;
	region->color[14][0] = 255;
	region->color[14][1] = 0;
	region->color[14][2] = 255;

	region->level[15] = 230;
	region->color[15][0] = 255;
	region->color[15][1] = 194;
	region->color[15][2] = 204;

	region->levels = 16;

	err = fopen_s(&fd, "splat.lcf", "r");

	if (fd == NULL)
	{
		err = fopen_s(&fd, filename, "r");
	}

	if (fd == NULL)
	{
		err = fopen_s(&fd, filename, "w");

		fprintf(fd, "; SPLAT Auto-generated Path-Loss Color Definition (\"%s\") File\n", filename);
		fprintf(fd, ";\n; Format for the parameters held in this file is as follows:\n;\n");
		fprintf(fd, ";    dB: red, green, blue\n;\n");
		fprintf(fd, "; ...where \"dB\" is the path loss (in dB) and\n");
		fprintf(fd, "; \"red\", \"green\", and \"blue\" are the corresponding RGB color\n");
		fprintf(fd, "; definitions ranging from 0 to 255 for the region specified.\n");
		fprintf(fd, ";\n; The following parameters may be edited and/or expanded\n");
		fprintf(fd, "; for future runs of SPLAT.  A total of 32 contour regions\n");
		fprintf(fd, "; may be defined in this file.\n;\n;\n");

		for (x = 0; x < region->levels; x++)
		{
			fprintf(fd, "%3d: %3d, %3d, %3d\n", region->level[x], region->color[x][0], region->color[x][1], region->color[x][2]);
		}

		fclose(fd);
	}
	else
	{
		x = 0;
		fgets(LossString, 80, fd);

		while ((x < 32) && (feof(fd) == 0))
		{
			pointer = strchr(LossString, ';');

			if (pointer != NULL)
			{
				*pointer = 0;
			}

			ok = sscanf_s(LossString, "%d: %d, %d, %d", &val[0], &val[1], &val[2], &val[3]);

			if (ok == 4)
			{
				for (y = 0; y < 4; y++)
				{
					if (val[y] > 255)
					{
						val[y] = 255;
					}

					if (val[y] < 0)
					{
						val[y] = 0;
					}
				}

				region->level[x] = val[0];
				region->color[x][0] = val[1];
				region->color[x][1] = val[2];
				region->color[x][2] = val[3];
				x++;
			}

			fgets(LossString, 80, fd);
		}

		fclose(fd);
		region->levels = x;
	}
}


//| ------------------------------
//| 
//| FUNCTION: LoadSplatPowerColorsForSite
//| 
//| OLD NAME: LoadDBMColors
//| 
//| NOTES: 
//| 
//| ------------------------------
void
LoadSplatPowerColorsForSite(
	Site xmtr,
	ColorRegion* region)
{
	int x, y, ok, val[4];
	char filename[255], DbmString[80], * pointer = NULL;
	FILE* fd = NULL;
	errno_t err;

	for (x = 0; (xmtr.filename[x] != '.') && (xmtr.filename[x] != 0) && (x < 250); x++)
	{
		filename[x] = xmtr.filename[x];
	}

	filename[x] = '.';
	filename[x + 1] = 'd';
	filename[x + 2] = 'c';
	filename[x + 3] = 'f';
	filename[x + 4] = 0;

	// Default values

	region->level[0] = 0;
	region->color[0][0] = 255;
	region->color[0][1] = 0;
	region->color[0][2] = 0;

	region->level[1] = -10;
	region->color[1][0] = 255;
	region->color[1][1] = 128;
	region->color[1][2] = 0;

	region->level[2] = -20;
	region->color[2][0] = 255;
	region->color[2][1] = 165;
	region->color[2][2] = 0;

	region->level[3] = -30;
	region->color[3][0] = 255;
	region->color[3][1] = 206;
	region->color[3][2] = 0;

	region->level[4] = -40;
	region->color[4][0] = 255;
	region->color[4][1] = 255;
	region->color[4][2] = 0;

	region->level[5] = -50;
	region->color[5][0] = 184;
	region->color[5][1] = 255;
	region->color[5][2] = 0;

	region->level[6] = -60;
	region->color[6][0] = 0;
	region->color[6][1] = 255;
	region->color[6][2] = 0;

	region->level[7] = -70;
	region->color[7][0] = 0;
	region->color[7][1] = 208;
	region->color[7][2] = 0;

	region->level[8] = -80;
	region->color[8][0] = 0;
	region->color[8][1] = 196;
	region->color[8][2] = 196;

	region->level[9] = -90;
	region->color[9][0] = 0;
	region->color[9][1] = 148;
	region->color[9][2] = 255;

	region->level[10] = -100;
	region->color[10][0] = 80;
	region->color[10][1] = 80;
	region->color[10][2] = 255;

	region->level[11] = -110;
	region->color[11][0] = 0;
	region->color[11][1] = 38;
	region->color[11][2] = 255;

	region->level[12] = -120;
	region->color[12][0] = 142;
	region->color[12][1] = 63;
	region->color[12][2] = 255;

	region->level[13] = -130;
	region->color[13][0] = 196;
	region->color[13][1] = 54;
	region->color[13][2] = 255;

	region->level[14] = -140;
	region->color[14][0] = 255;
	region->color[14][1] = 0;
	region->color[14][2] = 255;

	region->level[15] = -150;
	region->color[15][0] = 255;
	region->color[15][1] = 194;
	region->color[15][2] = 204;

	region->levels = 16;

	err = fopen_s(&fd, "splat.dcf", "r");

	if (fd == NULL)
	{
		err = fopen_s(&fd, filename, "r");
	}

	if (fd == NULL)
	{
		err = fopen_s(&fd, filename, "w");

		fprintf(fd, "; SPLAT Auto-generated DBM Signal Level Color Definition (\"%s\") File\n", filename);
		fprintf(fd, ";\n; Format for the parameters held in this file is as follows:\n;\n");
		fprintf(fd, ";    dBm: red, green, blue\n;\n");
		fprintf(fd, "; ...where \"dBm\" is the received signal power level between +40 dBm\n");
		fprintf(fd, "; and -200 dBm, and \"red\", \"green\", and \"blue\" are the corresponding\n");
		fprintf(fd, "; RGB color definitions ranging from 0 to 255 for the region specified.\n");
		fprintf(fd, ";\n; The following parameters may be edited and/or expanded\n");
		fprintf(fd, "; for future runs of SPLAT.  A total of 32 contour regions\n");
		fprintf(fd, "; may be defined in this file.\n;\n;\n");

		for (x = 0; x < region->levels; x++)
		{
			fprintf(fd, "%+4d: %3d, %3d, %3d\n", region->level[x], region->color[x][0], region->color[x][1], region->color[x][2]);
		}

		fclose(fd);
	}
	else
	{
		x = 0;
		fgets(DbmString, 80, fd);

		while ((x < 32) && (feof(fd) == 0))
		{
			pointer = strchr(DbmString, ';');

			if (pointer != NULL)
			{
				*pointer = 0;
			}

			ok = sscanf_s(DbmString, "%d: %d, %d, %d", &val[0], &val[1], &val[2], &val[3]);

			if (ok == 4)
			{
				if (val[0] < -200)
				{
					val[0] = -200;
				}

				if (val[0] > +40)
				{
					val[0] = +40;
				}

				region->level[x] = val[0];

				for (y = 1; y < 4; y++)
				{
					if (val[y] > 255)
					{
						val[y] = 255;
					}

					if (val[y] < 0)
					{
						val[y] = 0;
					}
				}

				region->color[x][0] = val[1];
				region->color[x][1] = val[2];
				region->color[x][2] = val[3];
				x++;
			}

			fgets(DbmString, 80, fd);
		}

		fclose(fd);
		region->levels = x;
	}
}
