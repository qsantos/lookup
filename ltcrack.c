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
#include <assert.h>
#include <time.h>

#include "md5.h"
#include "lookup.h"

#define ERROR(...)                    \
{                                     \
	fprintf(stderr, __VA_ARGS__); \
	fprintf(stderr, "\n");        \
	usage(argc, argv);            \
	exit(1);                      \
}

// some variables used internally by reverseHash()
static LTable* lt     = NULL;
static u32     n_lt   = 0;
static char*   bufstr = NULL;

static char reverseHash(const char hash[16])
{
	for (u32 i = 0; i < n_lt; i++)
		if (LTable_Reverse(&lt[i], hash, bufstr))
			return 1;
	return 0;
}

static inline void rewriteLine(void)
{
	printf("\r\33[K");
}

static void usage(int argc, char** argv)
{
	(void) argc;

	fprintf(stderr,
		"Usage: %s TARGET src1 [src2 [...]]\n"
		"Try and reverse one or several hashes\n"
		"\n"
		"TARGET:\n"
		" -x, --hash HASH  sets the target to HASH\n"
		" -f, --file FILE  read the targets from FILE (- for stdin)\n"
		" -r, --random N   targets are N random string hashes (bench)\n"
		,
		argv[0]
	);
}

typedef enum
{
	T_HASH,
	T_FILE,
	T_RAND,
} Target;

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
		exit(1);
	}

	Target ttype;
	char* tstr = argv[1];
	if (strcmp(tstr, "-x") == 0 || strcmp(tstr, "--hash") == 0)
		ttype = T_HASH;
	else if (strcmp(tstr, "-f") == 0 || strcmp(tstr, "--file") == 0)
		ttype = T_FILE;
	else if (strcmp(tstr, "-r") == 0 || strcmp(tstr, "--random") == 0)
		ttype = T_RAND;
	else
		ERROR("Invalid target '%s'\n", tstr);

	char* tparam = argv[2];

	// load tables
	n_lt = argc-3;
	lt = malloc(sizeof(LTable) * n_lt);
	assert(lt);
	for (u32 i = 0; i < n_lt; i++)
		if (!LTable_FromFile(&lt[i], argv[i+3]))
			ERROR("Could no load table '%s'\n", argv[i+3])

	// some parameters
	u32   l_string  = lt[0].l_string;
	char* charset   = lt[0].charset;
	u32   n_charset = lt[0].n_charset;

	// some buffers
	char hash[16];
	bufstr = malloc(l_string);
	assert(bufstr);

	// try and crack hash(es)
	switch (ttype)
	{
	case T_HASH:
		hex2hash(tparam, hash, 16);
		if (reverseHash(hash))
		{
			printHash(hash, 16);
			printf(" ");
			printString(bufstr, l_string);
			printf("\n");
		}
		else
			printf("Could not reverse hash\n");
		break;
	case T_FILE:
		(void) 0;
		FILE* f = strcmp(tparam, "-") == 0 ? stdin : fopen(tparam, "r");
		assert(f);

		while (1)
		{
			char hashstr[33];
			fread(hashstr, 1, 33, f);
			if (feof(f))
				break;

			hex2hash(hashstr, hash, 16);
			if (reverseHash(hash))
			{
				printHash(hash, 16);
				printf(" ");
				printString(bufstr, l_string);
				printf("\n");
			}
			else
				printf("Could not reverse hash\n");
		}

		fclose(f);
		break;
	case T_RAND:
		srandom(time(NULL));
		u32 n = atoi(tparam);
		u32 n_crack = 0;
		for (u32 i = 0; i < n; i++)
		{
			for (u32 j = 0; j < l_string; j++)
				bufstr[j] = charset[random() % n_charset];

			char hash[16];
			MD5((u8*) hash, (u8*) bufstr, l_string);
			if (reverseHash(hash))
				n_crack++;
			rewriteLine();
			printf("%lu / %lu", n_crack, i+1);
			fflush(stdout);
		}
		printf("\n");
	}
	free(bufstr);

	for (u32 i = 0; i < n_lt; i++)
		LTable_Delete(&lt[i]);
	free(lt);

	return 0;
}
