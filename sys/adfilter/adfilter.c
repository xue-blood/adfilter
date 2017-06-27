#include "adfilter.h"


#if 1 // load config and data

NTSTATUS query_reg(PUNICODE_STRING  reg, wchar_t *key, PVOID dst, ULONG type, PVOID deftdata, int deftlen)
{
	RTL_QUERY_REGISTRY_TABLE rtl[2];
	RtlZeroMemory(rtl, sizeof(rtl));

	rtl[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
	rtl[0].Name = key;
	rtl[0].EntryContext = dst;
	rtl[0].DefaultType = type;
	rtl[0].DefaultData = deftdata;
	rtl[0].DefaultLength = deftlen;

	NTSTATUS status = RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE,
		reg->Buffer, rtl, NULL, NULL);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("[adf] query reg (%S) failed.\n", rtl[0].Name));
		return status;
	}

	return status;
}

NTSTATUS load_file(PUNICODE_STRING file_path, PLIST_ENTRY header)
{
	KdPrint(("[adf] load ad hosts from %wZ\n", file_path));

	OBJECT_ATTRIBUTES attr;
	InitializeObjectAttributes(&attr, file_path,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL, NULL);

	if (KeGetCurrentIrql() != PASSIVE_LEVEL)
		return STATUS_INVALID_DEVICE_STATE;

	// open file handle
	HANDLE handle;
	IO_STATUS_BLOCK iosb;
	NTSTATUS status = ZwCreateFile(&handle, GENERIC_READ, &attr, &iosb, NULL,
		FILE_ATTRIBUTE_NORMAL, 0, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT,
		NULL, 0);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("[adf] open file failed.(%wZ)\n", file_path));
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


		// each buffer
		buff[255] = 0;
		char len[128];
		int i = 0;
		int lines = 0; // store line number for current read buffer
		for (; i < 255;)
		{
			RtlZeroMemory(len, sizeof(len));

			// get each line
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
			int l = strnlen(len, ADF_HOST_MAX_LEN);
			//KdPrint(("[adf] read : %s", len));

			// add new host to list
			if (!addHostList(header,len,l))
			{
				KdPrint((APP "add host: %s failed.\n", len));
				return STATUS_MEMORY_NOT_ALLOCATED;
			}

			i += last_line_offset;
		}

		// is the last of file
		if (!lines)
		{
			// read the last line
			status = ZwReadFile(handle, NULL, NULL, NULL,
				&iosb, buff, 256, &offset, NULL);
			// get the len length
			int l = strnlen(len, ADF_HOST_MAX_LEN);
			//KdPrint(("[adf] read : %s", len));

			// add new host to list
			if (!addHostList(header, len, l))
			{
				KdPrint((APP "add host: %s failed.\n", len));
				return STATUS_MEMORY_NOT_ALLOCATED;
			}
			break;
		}

		// up date file offset
		offset.LowPart += i;
	}



	ZwClose(handle);

	return STATUS_SUCCESS;
}



NTSTATUS load_cfg(PUNICODE_STRING path)
{
	InitializeListHead(&Adf.AdHost.sys);
	InitializeListHead(&Adf.AdHost.user);
	InitializeListHead(&Adf.AdHost.excpt);
	KdBreakPoint();
	// load pause status
	query_reg(path, L"Pause", &Adf.paused, REG_DWORD, &Adf.paused, sizeof(ULONG));
	KdPrint((APP"driver status %s\n", Adf.paused ? "paused" : "not paused"));

	// load host list from file
	DefUnicodeString(file, _file, 1024);

	// load except
	NTSTATUS status = query_reg(path, L"ExceptFilePath", &file, REG_SZ, file.Buffer, 0);
	success
	{
		status = load_file(&file, &Adf.AdHost.excpt);
		fail log("load except file failed.\n");
	};

	// load user
	RtlZeroMemory(file.Buffer, file.MaximumLength);
	status = query_reg(path, L"UserFilePath", &file, REG_SZ, file.Buffer, 0);
	success
	{ 
		status = load_file(&file, &Adf.AdHost.user); 
		fail log("load user file failed.\n");
	};

	
	// load sys
	RtlZeroMemory(file.Buffer, file.MaximumLength);
	status = query_reg(path, L"SysFilePath", &file, REG_SZ, file.Buffer, 0);
	fail return status;
	status = load_file(&file, &Adf.AdHost.sys);
	chk("load system file failed.\n");

	return status;
}
#endif// load config and data

void unload_cfg()
{
	clearHostList(&Adf.AdHost.sys);
	clearHostList(&Adf.AdHost.user);
	clearHostList(&Adf.AdHost.excpt);
}
