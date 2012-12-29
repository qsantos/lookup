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
	(void) filename;

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
