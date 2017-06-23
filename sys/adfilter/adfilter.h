#include<ntddk.h>

#pragma once
#ifndef bool
#define true 1
#define false 0
typedef unsigned char bool;
#endif

struct 
{
	bool paused;

	struct 
	{
		// the path of ad host file
		UNICODE_STRING  path;
		wchar_t			_path[256];

		// link list header
		SLIST_HEADER		list;

	}AdHost;
}g_adf;

typedef struct _HostList
{
	SLIST_ENTRY list;

	// our member
	char* name;
}HostList,*PHostList;

#define isalpha(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))

#define MEM 'ADF_'