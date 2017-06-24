#include <stdio.h>

#define  ADF_STATIC
#include "adf.h"

HANDLE h;

int main(int argc, char** argv)
{
	h = adf_open();

	printf("open device %s\n", h == INVALID_HANDLE_VALUE ? "failed." : "success.");
	if (h == INVALID_HANDLE_VALUE) return -1;

	adf_set_pause(h, true);

				
	adf_close(h);
}