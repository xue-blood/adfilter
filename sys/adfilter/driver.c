
#include <ntddk.h>
#include <ntstrsafe.h>
#include <stdio.h>
#include <string.h>
#include "tdi_fw_lib.h"

#include "adfilter.h"




int load_host(PUNICODE_STRING reg)
{

#if 1// query file path
	
	RtlInitEmptyUnicodeString(&g_adf.AdHost.path, g_adf.AdHost._path, sizeof(g_adf.AdHost._path));

	RTL_QUERY_REGISTRY_TABLE rtl[2];
	RtlZeroMemory(rtl, sizeof(rtl));

	rtl[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
	rtl[0].Name = L"AdHostFilePath";
	rtl[0].EntryContext = &g_adf.AdHost.path;
	rtl[0].DefaultType = REG_SZ;
	rtl[0].DefaultData = g_adf.AdHost.path.Buffer;
	rtl[0].DefaultLength = 0;

	NTSTATUS status = RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE, 
		reg->Buffer, rtl, NULL, NULL);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("[adf] query reg (%S) failed.\n", rtl[0].Name));
		return status;
	}
#endif// query file path

#if 1 // load ad hosts from file
	KdPrint(("[adf] load ad hosts from %wZ\n", &g_adf.AdHost.path));

	ExInitializeSListHead(&g_adf.AdHost.list);

	OBJECT_ATTRIBUTES attr;
	InitializeObjectAttributes(&attr, &g_adf.AdHost.path,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL, NULL);

	if (KeGetCurrentIrql() != PASSIVE_LEVEL)
		return STATUS_INVALID_DEVICE_STATE;

	// open file handle
	HANDLE handle;
	IO_STATUS_BLOCK iosb;
	status = ZwCreateFile(&handle, GENERIC_READ, &attr, &iosb, NULL,
		FILE_ATTRIBUTE_NORMAL, 0, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT,
		NULL, 0);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("[adf] open file failed.(%wZ)\n", &g_adf.AdHost.path));
		return status;
	}
	
	// read file
	LARGE_INTEGER offset = { 0 };
	char buff[256];
	while (1)
	{
		RtlZeroMemory(buff, sizeof(buff));
		
		status = ZwReadFile(handle, NULL, NULL, NULL,
			&iosb, buff, 256, &offset, NULL);
		if (!NT_SUCCESS(status)) break;

		buff[255] = 0;

		char len[128];
		int i = 0;
		int lines = 0; // store line number for current read buffer
		for (; i < 255;)
		{
			RtlZeroMemory(len, sizeof(len));

			int last_line_offset = 0; // store the last '0d 0a' offset

			for (int j = 0;; j++)
			{
				// is next line
				if (buff[i + j] == 0x0d)
				{
					last_line_offset = j + 2;
					lines++;
					break;
				}
				// is end
				if (!buff[i + j]) break;

				// copy data to len
				len[j] = buff[i + j];
			}

			// is the last line for current read buff
			if (!last_line_offset) break;

			// get the len length
			int l = strlen(len);
			KdPrint(("[adf] read : %s", len));

			// allocate memory
			PHostList e = ExAllocatePoolWithTag(NonPagedPool, sizeof(HostList), MEM);
			if (!e)
			{
				KdPrint(("[adf] allocate memory failed.\n"));
				return STATUS_MEMORY_NOT_ALLOCATED;
			}
			e->name = ExAllocatePoolWithTag(NonPagedPool, l + 1, MEM);
			if (!e->name)
			{
				KdPrint(("[adf] allocate memory failed.\n"));
				return STATUS_MEMORY_NOT_ALLOCATED;
			}

			// add host to list
			RtlCopyMemory(e->name, len, l + 1);
			InterlockedPushEntrySList(&g_adf.AdHost.list, (PSLIST_ENTRY)e);

			i += last_line_offset;
		}
		
		// is the last of file
		if (!lines) break;

		// up date file offset
		offset.LowPart += i;
	}

#if 1 // read the last line
	status = ZwReadFile(handle, NULL, NULL, NULL,
		&iosb, buff, 256, &offset, NULL);
	if (NT_SUCCESS(status))
	{
		KdPrint(("[adf] read : %s", buff));
	}
	int l = strlen(buff);
	// allocate memory
	PHostList e = ExAllocatePoolWithTag(NonPagedPool, sizeof(HostList), MEM);
	if (!e)
	{
		KdPrint(("[adf] allocate memory failed.\n"));
		return STATUS_MEMORY_NOT_ALLOCATED;
	}
	e->name = ExAllocatePoolWithTag(NonPagedPool, l + 1, MEM);
	if (!e->name)
	{
		KdPrint(("[adf] allocate memory failed.\n"));
		return STATUS_MEMORY_NOT_ALLOCATED;
	}

	// add host to list
	RtlCopyMemory(e->name, buff, l + 1);
	InterlockedPushEntrySList(&g_adf.AdHost.list, (PSLIST_ENTRY)e);

#endif// read the last line


	ZwClose(handle);
#endif// load ad hosts from file
	return status;
}



NTSTATUS
tdifw_driver_entry(
			IN PDRIVER_OBJECT theDriverObject,
            IN PUNICODE_STRING theRegistryPath)
{
	KdPrint(("[adf] driver start.(%wZ)\n", theRegistryPath));

	g_adf.paused = false;
	load_host(theRegistryPath);

	return STATUS_SUCCESS;
}

VOID
tdifw_driver_unload(
			IN PDRIVER_OBJECT DriverObject)
{
    
#if 1 // clear memory for host list
	while (true)
	{
		// get each entry in list
		PSLIST_ENTRY e = InterlockedPopEntrySList(&g_adf.AdHost.list);
		ExFreePoolWithTag(CONTAINING_RECORD(e,HostList,list)->name, MEM);
		ExFreePoolWithTag(e, MEM);
	}
#endif// clear memory for host list

	return;
}

NTSTATUS tdifw_user_device_dispatch(
	IN PDEVICE_OBJECT DeviceObject, IN PIRP irp)
{
    // 不会有任何请求到达这里。我们没有注册过自定义设备。
    return STATUS_UNSUCCESSFUL;
}

