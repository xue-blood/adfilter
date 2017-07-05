#pragma once

#include <windows.h>
#ifndef bool
#define true 1
#define false 0
#define bool long
#endif


#ifdef  ADF_STATIC	// is use static library
#define ADF_API 
#else
#ifdef  ADF_IMPLEMENT
#define ADF_API __declspec(dllexport)
#else
#define ADF_API __declspec(dllimport)
#endif
#endif

#define ADF_DEVICE_NAME L"\\\\.\\Ad_filter_control_device"



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

#if _DEBUG
#define log printf
#else
#define log 
#endif

#define ADF_HOST_MAX_LEN 128


ADF_API HANDLE	adf_open();
ADF_API void	adf_close(HANDLE h);


ADF_API bool	adf_set_pause(HANDLE h,bool pause);
ADF_API bool	adf_get_pause(HANDLE h);

ADF_API bool	adf_host(HANDLE h, char* host, int len,bool add,bool except);

#define adf_add_user_host(h,host,len) adf_host(h,host,len,1,0)
#define adf_del_user_host(h,host,len) adf_host(h,host,len,0,0)

#define adf_add_except_host(h,host,len) adf_host(h,host,len,1,1)
#define adf_del_except_host(h,host,len) adf_host(h,host,len,0,1)
