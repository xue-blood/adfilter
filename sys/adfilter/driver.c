
#include <ntddk.h>
#include <ntstrsafe.h>
#include <stdio.h>
#include <string.h>
#include "tdi_fw_lib.h"

#include "adfilter.h"



NTSTATUS create_device(PDRIVER_OBJECT driver)
{
	// create device
	UNICODE_STRING name;
	RtlInitUnicodeString(&name, CONTROL_DEVICE_NAME);
	
	NTSTATUS status = IoCreateDevice(driver, 0, &name, FILE_DEVICE_CONTROLLER, 
		0, false, &Adf.device);
	chk("Create device failed.\n");

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
	fail
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
	NTSTATUS status = STATUS_SUCCESS;
	
	status = load_cfg(theRegistryPath);
// 	if (!NT_SUCCESS(status))
// 	{
// 		KdPrint(("[adf] load config failed.\n"));
// 		unload_cfg();
// 		return status;
// 	}
	
	status = create_device(theDriverObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("[adf] create device failed.\n"));
		unload_cfg();
		return status;
	}


	return STATUS_SUCCESS;
}

VOID
tdifw_driver_unload(
			IN PDRIVER_OBJECT DriverObject)
{
    
	delete_device();
	unload_cfg();

	return;
}

