#include <stdlib.h>
#include <stdio.h>

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
	usage(argc, argv);
	return 0;
}
