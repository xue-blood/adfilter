#include<ntddk.h>

#pragma once
#ifndef bool
#define true 1
#define false 0
#define bool unsigned long
#endif

#define ADF_FAST_LIST_LEN  128

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
		LIST_ENTRY		sys;
		LIST_ENTRY		user;
		LIST_ENTRY		excpt;
	}AdHost;

}Adf;

typedef struct _HostList
{
	LIST_ENTRY list;

	// our member
	ANSI_STRING name;
}HostList, *PHostList;

#define isalpha(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))

// easy use macro
#define APP			"[adf]"
#define log(msg)	KdPrint((APP ## msg))
#define fail		if(!NT_SUCCESS(status))
#define success		if(NT_SUCCESS(status))
#define chk(msg)	fail {log(msg); return status;}

#define DefUnicodeString(name,buff_name,size)					\
			UNICODE_STRING name; wchar_t buff_name[size];	\
			RtlInitEmptyUnicodeString(&name,buff_name,sizeof(buff_name));

#define MEM 'ADF_'

#define CONTROL_DEVICE_NAME L"\\Device\\Ad_filter_control_device"
#define CONTROL_DEVICE_SYMB L"\\??\\Ad_filter_control_device"

#define ADF_HOST_MAX_LEN 128

/*
// define io control code
// see:https://msdn.microsoft.com/en-us/library/windows/hardware/ff543023(v=vs.85).aspx
*/
// pause
#define IOCTL_ADF_GET_PAUSE CTL_CODE(FILE_DEVICE_CONTROLLER,0x801,METHOD_BUFFERED,FILE_READ_ACCESS|FILE_WRITE_ACCESS)
#define IOCTL_ADF_SET_PAUSE CTL_CODE(FILE_DEVICE_CONTROLLER,0x802,METHOD_BUFFERED,FILE_READ_ACCESS|FILE_WRITE_ACCESS)

// host list
#define IOCTL_ADF_ADD_USER_HOST CTL_CODE(FILE_DEVICE_CONTROLLER,0x803,METHOD_BUFFERED,FILE_READ_ACCESS|FILE_WRITE_ACCESS)
#define IOCTL_ADF_DEL_USER_HOST CTL_CODE(FILE_DEVICE_CONTROLLER,0x804,METHOD_BUFFERED,FILE_READ_ACCESS|FILE_WRITE_ACCESS)

#define IOCTL_ADF_ADD_EXCEPT_HOST CTL_CODE(FILE_DEVICE_CONTROLLER,0x805,METHOD_BUFFERED,FILE_READ_ACCESS|FILE_WRITE_ACCESS)
#define IOCTL_ADF_DEL_EXCEPT_HOST CTL_CODE(FILE_DEVICE_CONTROLLER,0x806,METHOD_BUFFERED,FILE_READ_ACCESS|FILE_WRITE_ACCESS)

// statistic
#define IOCTL_ADF_SYS_HOST CTL_CODE(FILE_DEVICE_CONTROLLER,0x807,METHOD_BUFFERED,FILE_READ_ACCESS|FILE_WRITE_ACCESS)
#define IOCTL_ADF_STATISTIC CTL_CODE(FILE_DEVICE_CONTROLLER,0x808,METHOD_BUFFERED,FILE_READ_ACCESS|FILE_WRITE_ACCESS)


NTSTATUS load_cfg(PUNICODE_STRING path);
void unload_cfg();

void dump_hex(PUCHAR data, int len);
void dump_hex(PUCHAR data, int len);

PHostList newHostList(char *src, int len);
bool addHostList(PLIST_ENTRY header, char*src, int len);
bool removeHostList(PLIST_ENTRY header, char* dst, int len);

bool isSameHost(PLIST_ENTRY entry, char *name, int len);
bool isInHostList(PLIST_ENTRY header, char* dst, int len);
bool isMatchHostList(PLIST_ENTRY header, char* dst, int len);


void clearHostList(PLIST_ENTRY header);
