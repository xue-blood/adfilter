
#include <ntddk.h>
#include <ntstrsafe.h>
#include <stdio.h>
#include <string.h>
#include "tdi_fw_lib.h"

#include "adfilter.h"




int load_host(PUNICODE_STRING reg)
{

#if 1// query file path
	
	DefUnicodeString(file, 1024);

	RTL_QUERY_REGISTRY_TABLE rtl[2];
	RtlZeroMemory(rtl, sizeof(rtl));

	rtl[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
	rtl[0].Name = L"AdHostFilePath";
	rtl[0].EntryContext = &file;
	rtl[0].DefaultType = REG_SZ;
	rtl[0].DefaultData = file.Buffer;
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
	KdPrint(("[adf] load ad hosts from %wZ\n", &file));

	ExInitializeSListHead(&Adf.AdHost.list);

	OBJECT_ATTRIBUTES attr;
	InitializeObjectAttributes(&attr, &file,
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
		KdPrint(("[adf] open file failed.(%wZ)\n", &file));
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
			//KdPrint(("[adf] read : %s", len));

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
			RtlZeroMemory(e->name, l + 1);
			RtlCopyMemory(e->name, len, l + 1);
			e->invalid = false;
			InterlockedPushEntrySList(&Adf.AdHost.list, (PSLIST_ENTRY)e);

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
		//KdPrint(("[adf] read : %s", buff));
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
	e->invalid = false;
	// add host to list
	RtlCopyMemory(e->name, buff, l + 1);
	InterlockedPushEntrySList(&Adf.AdHost.list, (PSLIST_ENTRY)e);

#endif// read the last line


	ZwClose(handle);
#endif// load ad hosts from file
	return status;
}

void unload_host()
{
	while (true)
	{
		// get each entry in list
		PSLIST_ENTRY e = InterlockedPopEntrySList(&Adf.AdHost.list);
		if (!e) break;

		ExFreePoolWithTag(CONTAINING_RECORD(e, HostList, list)->name, MEM);
		ExFreePoolWithTag(e, MEM);
	}
}

NTSTATUS create_device(PDRIVER_OBJECT driver)
{
	// create device
	UNICODE_STRING name;
	RtlInitUnicodeString(&name, CONTROL_DEVICE_NAME);
	
	NTSTATUS status = IoCreateDevice(driver, 0, &name, FILE_DEVICE_CONTROLLER, 
		0, false, &Adf.device);
	test("Create device failed.\n");

	Adf.device->Flags |= DO_BUFFERED_IO;

	// add to tdifw engine
	if (!tdifw_register_user_device(Adf.device))
	{
		log("Registry device failed.\n");
		IoDeleteDevice(Adf.device);
		return status;
	}

	// create symbol link
	UNICODE_STRING symb;
	RtlInitUnicodeString(&symb, CONTROL_DEVICE_SYMB);

	status = IoCreateSymbolicLink(&symb, &name);
	chk
	{
		log("Create symbol link failed.\n");
		IoDeleteDevice(Adf.device);
		return status;
	};

	KdPrint((APP"Create device: %wZ\n", &symb));
	
	return status;
}

void delete_device()
{
	// delete symbol link
	UNICODE_STRING symb;
	RtlInitUnicodeString(&symb, CONTROL_DEVICE_SYMB);
	
	IoDeleteSymbolicLink(&symb);

	// delete device
	IoDeleteDevice(Adf.device);
}

NTSTATUS
tdifw_driver_entry(
			IN PDRIVER_OBJECT theDriverObject,
            IN PUNICODE_STRING theRegistryPath)
{
	KdPrint(("[adf] driver start.(%wZ)\n", theRegistryPath));
	
	KeInitializeSpinLock(&Adf.lock);

	Adf.paused = false;
	NTSTATUS status = load_host(theRegistryPath);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("[adf] load host failed.\n"));
		return status;
	}

	status = create_device(theDriverObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("[adf] create device failed.\n"));
		
		return status;
	}


	return STATUS_SUCCESS;
}

VOID
tdifw_driver_unload(
			IN PDRIVER_OBJECT DriverObject)
{
    
	delete_device();
	unload_host();

	return;
}

