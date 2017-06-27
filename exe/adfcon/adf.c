#define ADF_IMPLEMENT
#include "adf.h"

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
	if (h == INVALID_HANDLE_VALUE) return false;

	DWORD ret;
	bool status = DeviceIoControl(h, IOCTL_ADF_SET_PAUSE,
		&pause, sizeof(pause),	// IN 
		NULL, 0,				// OUT
		&ret, NULL);

	return status;
}

ADF_API bool	adf_get_pause(HANDLE h)
{
	if (h == INVALID_HANDLE_VALUE) return false;

	DWORD ret;
	bool pause = true;
	bool status = DeviceIoControl(h, IOCTL_ADF_GET_PAUSE,
		NULL, 0,						// IN 
		&pause, sizeof(bool),			// OUT
		&ret, NULL);

	return status && pause;
}

ADF_API bool	adf_add_user_host(HANDLE h, char* host, int len)
{
	if (len > ADF_HOST_MAX_LEN +1 || !host) return false;
	
	DWORD ret;
	bool status = DeviceIoControl(h, IOCTL_ADF_ADD_USER_HOST,
		host, len+1,	// IN 
		NULL, 0,				// OUT
		&ret, NULL);

	system("ipconfig /flushdns");

	return status;
}

ADF_API bool	adf_del_user_host(HANDLE h, char* host, int len)
{
	if (len > ADF_HOST_MAX_LEN || !host) return false;
	
	DWORD ret;
	bool status = DeviceIoControl(h, IOCTL_ADF_DEL_USER_HOST,
		host, len + 1,	// IN 
		NULL, 0,				// OUT
		&ret, NULL);

	return status;
}


ADF_API bool	adf_add_except_host(HANDLE h, char* host, int len)
{
	if (len > ADF_HOST_MAX_LEN + 1 || !host) return false;

	DWORD ret;
	bool status = DeviceIoControl(h, IOCTL_ADF_ADD_EXCEPT_HOST,
		host, len + 1,	// IN 
		NULL, 0,				// OUT
		&ret, NULL);

	system("ipconfig /flushdns");

	return status;
}

ADF_API bool	adf_del_except_host(HANDLE h, char* host, int len)
{
	if (len > ADF_HOST_MAX_LEN || !host) return false;

	DWORD ret;
	bool status = DeviceIoControl(h, IOCTL_ADF_DEL_EXCEPT_HOST,
		host, len + 1,	// IN 
		NULL, 0,				// OUT
		&ret, NULL);

	return status;
}