/**************************************************************\
**     Created originally by Jonathan Naylor, G4KLX.        **
**     Later embellished by John Magliacane, KD2BD to       **
**     detect and handle voids found in the SRTM data,      **
**     SRTM-3 data in .BIL and .HGT format, and high        **
**     resolution SRTM-1 one arc-second topography data.    **
**************************************************************
**                    Compile like this:                    **
**      cc -Wall -O3 -s -lbz2 srtm2sdf.c -o srtm2sdf        **
**              Last modification: 08-Jan-2014              **
\**************************************************************/

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#include <share.h>
#include "srtm2sdf.h"


//| ------------------------------
//| 
//| FUNCTION: ReadSRTM
//| 
//| OLD NAME: ReadSRTM
//| 
//| NOTES: 
//| 
//| ------------------------------
int 
ReadSRTM
   (char *filename)
{
	int x, y, infile, byte = 0, bytes_read;
	unsigned char error, buffer[2];
	char north[3], west[4], *base = NULL, blw_filename[255];
	double cell_size, deg_north, deg_west;
	FILE *fd = NULL;

	errno_t err;

	if (strstr(filename, ".zip") != NULL)
	{
		fprintf(stderr, "*** Error: \"%s\" must be uncompressed\n", filename);
		return -1;

	}

	if (strstr(filename, ".tgz") != NULL)
	{
		fprintf(stderr, "*** Error: \"%s\" must be uncompressed\n", filename);
		return -1;

	}

	if ((strstr(filename, ".hgt") == NULL) && (strstr(filename, ".bil") == NULL))
	{
		fprintf(stderr, "*** Error: \"%s\" does not have the correct extension (.hgt or .bil)\n", filename);
		return -1;
	}

	if (strstr(filename, ".hgt") != NULL)
		hgt = 1;

	if (strstr(filename, ".bil") != NULL)
		bil = 1;

	base = strrchr(filename, '\\');

	if (base == NULL)
		base = filename;
	else
		base += 1;

	if (hgt)
	{
		/* We obtain coordinates from the base of the .HGT filename */

		north[0] = base[1];
		north[1] = base[2];
		north[2] = 0;

		west[0] = base[4];
		west[1] = base[5];
		west[2] = base[6];
		west[3] = 0;

		if ((base[0] != 'N' && base[0] != 'S') || (base[3] != 'W' && base[3] != 'E'))
		{
			fprintf(stderr, "*** Error: \"%s\" doesn't look like a valid .hgt SRTM filename.\n", filename);
			return -1;
		}

		max_west = atoi(west);

		if (base[3] == 'E')
			max_west = 360 - max_west;

		min_west = max_west - 1;

		if (max_west == 360)
			max_west = 0;

		if (base[0] == 'N')
			min_north = atoi(north);
		else
			min_north = -atoi(north);

		max_north = min_north + 1;
	}

	if (bil)
	{
		/* We obtain .BIL file coordinates
		from the corresponding .BLW file */

		err = strncpy_s(blw_filename, _countof(blw_filename), filename, 250);
		x = (int)strlen(filename);

		if (x>3)
		{
			blw_filename[x - 2] = 'l';
			blw_filename[x - 1] = 'w';
			blw_filename[x] = 0;

			err = fopen_s(&fd, blw_filename, "rb");

			if (fd != NULL)
			{
				fscanf_s(fd, "%lf", &cell_size);

				if ((cell_size<0.0008) || (cell_size>0.0009))
				{
					printf("\n*** .BIL file's cell size is incompatible with SPLAT!!\n");
					exit(1);
				}

				fscanf_s(fd, "%lf", &deg_west);
				fscanf_s(fd, "%lf", &deg_west);
				fscanf_s(fd, "%lf", &deg_west);

				fscanf_s(fd, "%lf", &deg_west);

				fscanf_s(fd, "%lf", &deg_north);

				fclose(fd);
			}

			min_north = (int)(deg_north);
			max_north = max_north + 1;

			if (deg_west<0.0)
				deg_west = -deg_west;
			else
				deg_west = 360.0 - deg_west;

			min_west = (int)(deg_west);

			if (min_west == 360)
				min_west = 0;

			max_west = min_west + 1;
		}
	}

	err = _sopen_s(&infile, filename, (_O_RDONLY | _O_BINARY), _SH_DENYNO, 0);

	if (infile == 0)
	{
		fprintf(stderr, "*** Error: Cannot open \"%s\"\n", filename);
		return -1;
	}

	n = _read(infile, &buffer, 2);

	if ((buffer[0] == 'P') && (buffer[1] == 'K'))
	{
		fprintf(stderr, "*** Error: \"%s\" still appears to be compressed!\n", filename);
		_close(infile);
		return -1;
	}

	_lseek(infile, 0L, SEEK_SET);

	if (ippd == 3600)
		sprintf_s(sdf_filename, _countof(sdf_filename), "%ldx%ldx%ldx%ld-hd.sdf", min_north, max_north, min_west, max_west);
	else
		sprintf_s(sdf_filename, _countof(sdf_filename), "%ldx%ldx%ldx%ld.sdf", min_north, max_north, min_west, max_west);

	error = 0;
	replacement_flag = 0;

	printf("Reading %s... ", filename);
	fflush(stdout);

	for (x = 0; (x <= ippd && error == 0); x++)
		for (y = 0; (y <= ippd && error == 0); y++)
		{
			bytes_read = _read(infile, &buffer, 2);

			if (bytes_read == 2)
			{
				if (bil)
				{
					/* "little-endian" structure */

					byte = buffer[0] + (buffer[1] << 8);

					if (buffer[1] & 128)
						byte -= 0x10000;
				}

				if (hgt)
				{
					/* "big-endian" structure */

					byte = buffer[1] + (buffer[0] << 8);

					if (buffer[0] & 128)
						byte -= 0x10000;
				}

				/* Flag problem elevations here */

				if (byte<-32768)
					byte = -32768;

				if (byte>32767)
					byte = 32767;

				if (byte <= min_elevation)
					replacement_flag = 1;

				srtm[x][y] = byte;
			}

			else
				error = 1;
		}

	if (error)
	{
		fprintf(stderr, "\n*** Error: Premature EOF detected while reading \"%s\"!  :-(\n", filename);
	}

	_close(infile);

	return 0;
}


//| ------------------------------
//| 
//| FUNCTION: LoadSDF_SDF
//| 
//| OLD NAME: LoadSDF_SDF
//| 
//| NOTES: 
//| 
//| ------------------------------
int 
LoadSDF_SDF
   (char *name)
{
	/* This function reads uncompressed
	SPLAT Data Files (.sdf) into memory. */

	int x, y, dummy;
	char sdf_file[255], path_plus_name[512];
	FILE *infile;

	errno_t err;

	for (x = 0; name[x] != '.' && name[x] != 0 && x<250; x++)
		sdf_file[x] = name[x];

	sdf_file[x] = '.';
	sdf_file[x + 1] = 's';
	sdf_file[x + 2] = 'd';
	sdf_file[x + 3] = 'f';
	sdf_file[x + 4] = 0;

	err = strncpy_s(path_plus_name, _countof(path_plus_name), sdf_path, 255);
	err = strncat_s(path_plus_name, _countof(path_plus_name), sdf_file, 254);

	err = fopen_s(&infile, path_plus_name, "rb");

	if (infile == NULL)
		return 0;

	fscanf_s(infile, "%ld", &dummy);
	fscanf_s(infile, "%ld", &dummy);
	fscanf_s(infile, "%ld", &dummy);
	fscanf_s(infile, "%ld", &dummy);

	printf("\nReading %s... ", path_plus_name);
	fflush(stdout);

	for (x = 0; x<1200; x++)
		for (y = 0; y<1200; y++)
			fscanf_s(infile, "%ld", &usgs[x][y]);

	fclose(infile);

	return 1;
}


//| ------------------------------
//| 
//| FUNCTION: LoadSDF
//| 
//| OLD NAME: LoadSDF
//| 
//| NOTES: 
//| 
//| ------------------------------
char 
LoadSDF
   (char *name)
{
	/* This function loads the requested SDF file from the filesystem.
	First, it tries to invoke the LoadSDF_SDF() function to load an
	uncompressed SDF file (since uncompressed files load slightly
	faster). */

	int return_value = -1;

	/* Try to load an uncompressed SDF first. */

	return_value = LoadSDF_SDF(name);

	return return_value;
}


//| ------------------------------
//| 
//| FUNCTION: ReadUSGS
//| 
//| OLD NAME: ReadUSGS
//| 
//| NOTES: 
//| 
//| ------------------------------
int 
ReadUSGS()
{
	char usgs_filename[15];

	/* usgs_filename is a minimal filename ("40:41:74:75").
	Full path and extentions are added later though
	subsequent function calls. */

	sprintf_s(usgs_filename, _countof(usgs_filename), "%ldx%ldx%ldx%ld", min_north, max_north, min_west, max_west);

	return (LoadSDF(usgs_filename));
}


//| ------------------------------
//| 
//| FUNCTION: average_terrain
//| 
//| OLD NAME: average_terrain
//| 
//| NOTES: 
//| 
//| ------------------------------
void 
average_terrain
   (int y, 
	int x, 
	int z)
{
	long accum;
	int temp = 0, count, bad_value;
	double average;

	bad_value = srtm[y][x];

	accum = 0L;
	count = 0;

	if (y >= 2)
	{
		temp = srtm[y - 1][x];

		if (temp>bad_value)
		{
			accum += temp;
			count++;
		}
	}

	if (y <= mpi)
	{
		temp = srtm[y + 1][x];

		if (temp>bad_value)
		{
			accum += temp;
			count++;
		}
	}

	if ((y >= 2) && (x <= (mpi - 1)))
	{
		temp = srtm[y - 1][x + 1];

		if (temp>bad_value)
		{
			accum += temp;
			count++;
		}
	}

	if (x <= (mpi - 1))
	{
		temp = srtm[y][x + 1];

		if (temp>bad_value)
		{
			accum += temp;
			count++;
		}
	}

	if ((x <= (mpi - 1)) && (y <= mpi))
	{
		temp = srtm[y + 1][x + 1];

		if (temp>bad_value)
		{
			accum += temp;
			count++;
		}
	}

	if ((x >= 1) && (y >= 2))
	{
		temp = srtm[y - 1][x - 1];

		if (temp>bad_value)
		{
			accum += temp;
			count++;
		}
	}

	if (x >= 1)
	{
		temp = srtm[y][x - 1];

		if (temp>bad_value)
		{
			accum += temp;
			count++;
		}
	}

	if ((y <= mpi) && (x >= 1))
	{
		temp = srtm[y + 1][x - 1];

		if (temp>bad_value)
		{
			accum += temp;
			count++;
		}
	}

	if (count != 0)
	{
		average = (((double)accum) / ((double)count));
		temp = (int)(average + 0.5);
	}

	if (temp>min_elevation)
		srtm[y][x] = temp;
	else
		srtm[y][x] = min_elevation;
}


//| ------------------------------
//| 
//| FUNCTION: WriteSDF
//| 
//| OLD NAME: WriteSDF
//| 
//| NOTES: 
//| 
//| ------------------------------
void 
WriteSDF
   (char *filename)
{
	/* Like the HGT files, the extreme southwest corner
	* provides the point of reference for the SDF file.
	* The SDF file extends from min_north degrees to
	* the south to min_north+(mpi/ippd) degrees to
	* the north, and max_west degrees to the west to
	* max_west-(mpi/ippd) degrees to the east.  The
	* overlapping edge redundancy present in the HGT
	* and earlier USGS files is not necessary, nor
	* is it present in SDF files.
	*/

	int x, y, byte, last_good_byte = 0;
	FILE *outfile;

	errno_t err;

	printf("\nWriting %s... ", filename);
	fflush(stdout);

	err = fopen_s(&outfile, filename, "wb");

	fprintf(outfile, "%d\n%d\n%d\n%d\n", max_west, min_north, min_west, max_north);

	for (y = ippd; y >= 1; y--)		/* Omit the northern most edge */
		for (x = mpi; x >= 0; x--) /* Omit the eastern most edge  */
		{
			byte = srtm[y][x];

			if (byte>min_elevation)
				last_good_byte = byte;

			if (byte<min_elevation)
			{
				if (merge)
				{
					if (ippd == 3600)
						fprintf(outfile, "%d\n", usgs[1200 - (y / 3)][1199 - (x / 3)]);
					else
						fprintf(outfile, "%d\n", usgs[1200 - y][1199 - x]);
				}

				else
				{
					average_terrain(y, x, last_good_byte);
					fprintf(outfile, "%d\n", srtm[y][x]);
				}
			}
			else
				fprintf(outfile, "%d\n", byte);
		}

	printf("Done!\n");

	fclose(outfile);
}


//| ------------------------------
//| 
//| FUNCTION: main
//| 
//| OLD NAME: main
//| 
//| NOTES: 
//| 
//| ------------------------------
int 
main
   (int argc, 
	char *argv[])
{
	int x, y, z = 0;
	char *env = NULL, SrtmString[255];
	size_t envLen;
	FILE *fd;

	errno_t err;

	if (strstr(argv[0], "srtm2sdf-hd") != NULL)
	{
		ippd = 3600;	/* High Definition (1 arc-sec) Mode */
		err = strncpy_s(SrtmString, _countof(SrtmString), "srtm2sdf-hd\0", 12);
	}

	else
	{
		ippd = 1200;	/* Standard Definition (3 arc-sec) Mode */
		err = strncpy_s(SrtmString, _countof(SrtmString), "srtm2sdf\0", 9);
	}

	mpi = ippd - 1;		/* Maximum pixel index per degree */

	if (argc == 1 || (argc == 2 && strncmp(argv[1], "-h", 2) == 0))
	{
		if (ippd == 1200)
			fprintf(stderr, "\nsrtm2sdf: Generates SPLAT! elevation data files from unzipped\nSRTM-3 elevation data files, and replaces SRTM data voids with\nelevation data from older usgs2sdf derived SDF files.\n\n");

		if (ippd == 3600)
			fprintf(stderr, "\nsrtm2sdf-hd: Generates SPLAT! elevation data files from unzipped\nSRTM-1 elevation data files, and replaces SRTM data voids with\naverages, or elevation data from older usgs2sdf derived SDF files.\n\n");

		fprintf(stderr, "\tAvailable Options...\n\n");
		fprintf(stderr, "\t-d directory path of usgs2sdf derived SDF files\n\t    (overrides path in ~/.splat_path file)\n\n");
		fprintf(stderr, "\t-n elevation limit (in meters) below which SRTM data is\n\t    replaced by USGS-derived .sdf data (default = 0 meters)\n\n");
		fprintf(stderr, "Examples: %s N40W074.hgt\n", SrtmString);
		fprintf(stderr, "          %s -d /cdrom/sdf N40W074.hgt\n", SrtmString);
		fprintf(stderr, "          %s -d /dev/null N40W074.hgt  (prevents data replacement)\n", SrtmString);
		fprintf(stderr, "          %s -n -5 N40W074.hgt\n\n", SrtmString);

		return 1;
	}

	y = argc - 1;

	min_elevation = 0;

	for (x = 1, z = 0; x <= y; x++)
	{
		if (strcmp(argv[x], "-d") == 0)
		{
			z = x + 1;

			if (z <= y && argv[z][0] && argv[z][0] != '-')
				err = strncpy_s(sdf_path, _countof(sdf_path), argv[z], 253);
		}

		if (strcmp(argv[x], "-n") == 0)
		{
			z = x + 1;

			if (z <= y && argv[z][0])
			{
				sscanf_s(argv[z], "%d", &min_elevation);

				if (min_elevation<-32767)
					min_elevation = 0;
			}
		}
	}

	/* If no SDF path was specified on the command line (-d), check
	for a path specified in the $HOME/.splat_path file.  If the
	file is not found, then sdf_path[] remains NULL, and a data
	merge will not be attempted if voids are found in the SRTM file. */

	if (sdf_path[0] == 0)
	{
		char* hpath;
		size_t hpathLen;
		err = _dupenv_s(&env, &envLen, "HOMEPATH");
		err = _dupenv_s(&hpath, &hpathLen, "HOMEDRIVE");
		sprintf_s(SrtmString, _countof(SrtmString), "%s%s\\.splat_path", hpath, env); // JAD TODO
		err = fopen_s(&fd, SrtmString, "r");
		if (fd != NULL)
		{
			fgets(SrtmString, 253, fd);

			/* Remove <CR> and/or <LF> from string */

			for (x = 0; SrtmString[x] != 13 && SrtmString[x] != 10 && SrtmString[x] != 0 && x<253; x++);
			SrtmString[x] = 0;

			err = strncpy_s(sdf_path, _countof(sdf_path), SrtmString, 253);

			fclose(fd);
		}
	}

	/* Ensure a trailing '\\' is present in sdf_path */

	if (sdf_path[0])
	{
		x = (int)strlen(sdf_path);

		if (sdf_path[x - 1] != '\\' && x != 0)
		{
			sdf_path[x] = '\\';
			sdf_path[x + 1] = 0;
		}
	}

	//| JAD TODO - Process multiple SRTM files per call.
	while (argv[z + 1] != 0)
	{
		if (ReadSRTM(argv[z + 1]) == 0)
		{
			if (replacement_flag && sdf_path[0])
				merge = ReadUSGS();


			//| JAD TODO - Write SDFs to user-specified directory.
			char sdfPathAndName[285];
			sprintf_s(sdfPathAndName, _countof(sdfPathAndName), "%s%s", sdf_path, sdf_filename);
			WriteSDF(sdfPathAndName);
		}
		z++;
	}
	return 0;
}
