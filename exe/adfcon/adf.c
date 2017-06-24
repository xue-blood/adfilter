#define ADF_IMPLEMENT
#include "adf.h"
#include <stdio.h>

ADF_API HANDLE adf_open()
{
	HANDLE h = CreateFile(ADF_DEVICE_NAME, GENERIC_ALL,
		0, NULL, OPEN_EXISTING,
		0, NULL);
	return h;
}

ADF_API void adf_close(HANDLE h)
{
	if (h != INVALID_HANDLE_VALUE)
		CloseHandle(h);
}

ADF_API bool	adf_set_pause(HANDLE h,bool pause)
{
	DWORD ret;
	log("ioctl : 0x%x", IOCTL_ADF_SET_PAUSE);

	bool status = DeviceIoControl(h, IOCTL_ADF_SET_PAUSE,
		&pause, sizeof(pause),	// IN 
		NULL, 0,				// OUT
		&ret, NULL);

	return status;
}

ADF_API bool	adf_get_pause(HANDLE h)
{

}