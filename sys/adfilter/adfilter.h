#include<ntddk.h>

#pragma once
#ifndef bool
#define true 1
#define false 0
typedef unsigned char bool;
#endif

struct 
{
	// is pause filter 
	bool paused;

	// queued spin lock 
	// see :https://msdn.microsoft.com/en-us/library/windows/hardware/ff559970(v=vs.85).aspx
	KSPIN_LOCK lock;

	// control device
	PDEVICE_OBJECT device;

	struct 
	{
		// link list header
		SLIST_HEADER		list;

	}AdHost;
}Adf;

typedef struct _HostList
{
	SLIST_ENTRY list;

	// our member
	char* name;
}HostList, *PHostList;

#define isalpha(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))

// easy use macro
#define APP			"[adf]"
#define log(msg)	KdPrint((APP ## msg))
#define chk			if(!NT_SUCCESS(status))
#define test(msg)	chk {log(msg); return status;}

#define DefUnicodeString(name,size)					\
			UNICODE_STRING name; wchar_t _##name[size];	\
			RtlInitEmptyUnicodeString(&name,_##name,sizeof(_##name));

#define MEM 'ADF_'

#define CONTROL_DEVICE_NAME L"\\Device\\Ad_filter_control_device"
#define CONTROL_DEVICE_SYMB L"\\??\\Ad_filter_control_device"

/*
// define io control code
// see:https://msdn.microsoft.com/en-us/library/windows/hardware/ff543023(v=vs.85).aspx
*/
// pause
#define IOCTL_ADF_GET_PAUSE CTL_CODE(FILE_DEVICE_CONTROLLER,0x801,METHOD_BUFFERED,FILE_READ_ACCESS|FILE_WRITE_ACCESS)
#define IOCTL_ADF_SET_PAUSE CTL_CODE(FILE_DEVICE_CONTROLLER,0x802,METHOD_BUFFERED,FILE_READ_ACCESS|FILE_WRITE_ACCESS)

// host list
#define IOCTL_ADF_ADD_HOST CTL_CODE(FILE_DEVICE_CONTROLLER,0x803,METHOD_BUFFERED,FILE_READ_ACCESS|FILE_WRITE_ACCESS)
#define IOCTL_ADF_DEL_HOST CTL_CODE(FILE_DEVICE_CONTROLLER,0x804,METHOD_BUFFERED,FILE_READ_ACCESS|FILE_WRITE_ACCESS)

// statistic
#define IOCTL_ADF_STATISTIC CTL_CODE(FILE_DEVICE_CONTROLLER,0x805,METHOD_BUFFERED,FILE_READ_ACCESS|FILE_WRITE_ACCESS)
