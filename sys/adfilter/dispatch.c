
#include <ntddk.h>
#include <ntstrsafe.h>
#include <stdio.h>
#include <string.h>
#include "tdi_fw_lib.h"

#include "adfilter.h"

NTSTATUS device_control(PIRP irp, PIO_STACK_LOCATION irpsp)
{
	KdPrint(("Device control here (0x%x) \n", irpsp->Parameters.DeviceIoControl.IoControlCode));

	KLOCK_QUEUE_HANDLE que;
	switch (irpsp->Parameters.DeviceIoControl.IoControlCode)
	{
		// set pause status
	case IOCTL_ADF_SET_PAUSE:
		{
			bool *pause = irp->AssociatedIrp.SystemBuffer;
			if (!pause)				return STATUS_INVALID_PARAMETER;

			KeAcquireInStackQueuedSpinLock(&Adf.lock, &que);
			Adf.paused = *pause;
			KeReleaseInStackQueuedSpinLock(&que);

			KdPrint((APP"driver %s\n", *pause ? "paused" : "continue"));
		}
		break;
	default:
		break;
	}

	return STATUS_SUCCESS;
}

NTSTATUS tdifw_user_device_dispatch(
	IN PDEVICE_OBJECT DeviceObject, IN PIRP irp)
{
	// check dispatch
	if (!irp || DeviceObject != Adf.device) return STATUS_UNSUCCESSFUL;

	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(irp);
	NTSTATUS status = STATUS_SUCCESS;

	switch (irpsp->MajorFunction)
	{
	case IRP_MJ_CREATE:
		KdPrint((APP"process %d access in.\n", PsGetCurrentProcessId()));
		break;
	case IRP_MJ_CLOSE:
		KdPrint((APP"process %d leave.\n", PsGetCurrentProcessId()));
		break;
	case IRP_MJ_DEVICE_CONTROL:
		status = device_control(irp, irpsp);
		break;
	default:
		break;
	}

	// no need, just complete it
	IoSkipCurrentIrpStackLocation(irp);
	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return status;
}

