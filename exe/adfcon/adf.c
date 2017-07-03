#define ADF_IMPLEMENT
#include "adf.h"

// flush dns cache
// doc:http://blog.csdn.net/chenlycly/article/details/26254007
BOOL __stdcall DnsFlushResolverCache()
{
	BOOL bRet = FALSE;

	typedef BOOL(WINAPI *PfuncDnsFlushResolverCache)(VOID);
	HMODULE hDnsModule = LoadLibrary(TEXT("dnsapi.dll"));
	if (hDnsModule != NULL)
	{
		PfuncDnsFlushResolverCache pFlushFun = GetProcAddress(hDnsModule, "DnsFlushResolverCache");
		if (pFlushFun != NULL)
		{
			pFlushFun();
			bRet = TRUE;
		}

		FreeLibrary(hDnsModule);
	}

	return bRet;
}

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


ADF_API bool	adf_host(HANDLE h, char* host, int len, bool add, bool except)
{
	if (len > ADF_HOST_MAX_LEN || !host) return false;

	DWORD code = (except ?  ( add ? IOCTL_ADF_ADD_EXCEPT_HOST : IOCTL_ADF_DEL_EXCEPT_HOST) : 
							( add ? IOCTL_ADF_ADD_USER_HOST : IOCTL_ADF_DEL_USER_HOST));

	DWORD ret;
	bool status = DeviceIoControl(h, code,
		host, len + 1,	// IN 
		NULL, 0,				// OUT
		&ret, NULL);


	if (add)	DnsFlushResolverCache();
	

	return status;
}


ADF_API bool	adf_install()
{
	return system("install.bat");
}

ADF_API bool	adf_remove()
{
	return system("remove.bat");
}