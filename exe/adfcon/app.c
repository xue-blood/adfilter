#include <stdio.h>

#define  ADF_STATIC
#include "adf.h"

HANDLE h;

void promot()
{
	char s[] = \
"Usage:\n\
	adfcon start/stop	: start or stop the driver.\n\
	adfcon show		: show the status of driver.\n\
	adfcon add/del (hostname): add or delete host.\n\
	adfcon addE/delE (hostname): add or delete a host which will not be filtered.";

	printf("%s",s);
}

#define Cmd(name,func)						\
		if (strcmp(cmd, name) == 0)			\
		{									\
			func;							\
			break;							\
		}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		promot();
		return 0;
	}

	h = adf_open();
	printf("open device %s\n", h == INVALID_HANDLE_VALUE ? "failed." : "success.");
	if (h == INVALID_HANDLE_VALUE) return -1;

	char *cmd = argv[1];
	do 
	{
		// argc == 2
		Cmd("start",	adf_set_pause(h, false));
		Cmd("stop",		adf_set_pause(h, true));
		Cmd("show",		printf("driver is %s\n", 
							adf_get_pause(h) ?"paused" : "not paused"));

		// argc == 3
		if (argc != 3)  printf("please input the host name\n");
		Cmd("add",		printf("add host %s.\n",
							adf_add_user_host(h,argv[2],strlen(argv[2]))?"success":"failed"));
		Cmd("del",		printf("delete host %s.\n", 
							adf_del_user_host(h, argv[2], strlen(argv[2])) ? "success" : "failed"));

		Cmd("addE",		printf("add host %s.\n",
							adf_add_except_host(h, argv[2], strlen(argv[2])) ? "success" : "failed"));
		Cmd("delE",		printf("delete host %s.\n",
							adf_del_except_host(h, argv[2], strlen(argv[2])) ? "success" : "failed"));


		// invalid command
		printf("parameter is invalid\n");
		promot();
		return 0;


	} while (0);
	
	printf("command success.\n");
	adf_close(h);

	return 1;
}