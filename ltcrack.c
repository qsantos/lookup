#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lookup.h"

static void usage(int argc, char** argv)
{
	(void) argc;

	fprintf(stderr,
		"Usage: %s TARGET src\n"
		"Try and reverse one or several hashes\n"
		"\n"
		"TARGET:\n"
		" -x, --hash HASH  sets the target to hash\n"
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
		printf("ltcrack\n");
		printf("Compiled on %s at %s\n", __DATE__, __TIME__);
		exit(0);
	}

	if (argc < 4)
	{
		usage(argc, argv);
		exit(0);
	}

	char* tparam   = argv[2];
	char* filename = argv[3];

	LTable lt;

	// load table, if exists
	if (!LTable_FromFile(&lt, filename))
	{
		fprintf(stderr, "Could not load table '%s'\n", filename);
		exit(1);
	}

	char hash[16];
	hex2hash(tparam, hash, 16);
	char* bufstr = malloc(lt.l_string);
	if (LTable_Reverse(&lt, hash, bufstr))
	{
		printHash(hash, 16);
		printf(" ");
		printString(bufstr, lt.l_string);
		printf("\n");
	}
	else
		printf("Could not reverse hash\n");
	free(bufstr);

	LTable_Delete(&lt);
	return 0;
}
