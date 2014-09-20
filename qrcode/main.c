#include <stdlib.h>
#include "qrencode.h"

int main(int argc, char **argv)
{
	if (argc == 2) {
		qrencode(argv[1]);
	}
	return EXIT_SUCCESS;
}
