/*\
 *  Lookup table for hash cracking
 *  Copyright (C) 2012  Quentin SANTOS
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
\*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lookup.h"

static void usage(int argc, char** argv)
{
	(void) argc;

	fprintf(stderr,
		"Usage: %s l_string dst\n"
		"Create a lookup table\n"
		"\n"
		"l_string  string length\n"
		"dst       the destination file\n"
		,
		argv[0]
	);
}

int main(int argc, char** argv)
{
	if (argc == 1 || !strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))
	{
		usage(argc, argv);
		exit(0);
	}
	else if (!strcmp(argv[1], "--version") || !strcmp(argv[1], "-v"))
	{
		printf("ltgen\n");
		printf("Compiled on %s at %s\n", __DATE__, __TIME__);
		exit(0);
	}

	if (argc < 3)
	{
		usage(argc, argv);
		exit(0);
	}

	char* charset  = "0123456789abcdefghijklmnopqrstuvwxyz";
	u32   l_string = atoi(argv[1]);
	char* filename = argv[2];

	LTable lt;
	LTable_New(&lt, l_string, charset);

	// load table, if exists
	LTable_FromFile(&lt, filename);

	// generate more rows
	while (LTable_Next(&lt));

	// finish generation
	// TODO
	LTable_Sort(&lt);

	// save table
	LTable_ToFile(&lt, filename);
	LTable_Delete(&lt);
	return 0;
}
