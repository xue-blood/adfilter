
#include <ntddk.h>
#include <ntstrsafe.h>
#include <stdio.h>
#include <string.h>
#include "tdi_fw_lib.h"

#include "adfilter.h"

// doc:https://msdn.microsoft.com/en-us/library/windows/hardware/ff550744(v=vs.85).aspx
NTSTATUS device_control(PIRP irp, PIO_STACK_LOCATION irpsp)
{
	KdPrint(("Device control here (0x%x) \n", irpsp->Parameters.DeviceIoControl.IoControlCode));

	KLOCK_QUEUE_HANDLE que;
	PSLIST_ENTRY h;
	
	// user input and output
	char* buff = irp->AssociatedIrp.SystemBuffer;
	int inlen = irpsp->Parameters.DeviceIoControl.InputBufferLength;
	int outlen = irpsp->Parameters.DeviceIoControl.OutputBufferLength;


	switch (irpsp->Parameters.DeviceIoControl.IoControlCode)
	{
		// set pause status
	case IOCTL_ADF_SET_PAUSE:
		{
			// is no buffer or buffer len is zero
			if (!buff || !inlen)	return STATUS_INVALID_PARAMETER;

			KeAcquireInStackQueuedSpinLock(&Adf.lock, &que);
			Adf.paused = *(bool*)buff;
			KeReleaseInStackQueuedSpinLock(&que);

			KdPrint((APP"driver %s\n", *(bool*)buff ? "paused" : "continue"));
		}
		break;
	case IOCTL_ADF_GET_PAUSE:
		{
			// is no buffer or buffer len is zero
			if (!buff || !outlen)	return STATUS_INVALID_PARAMETER;

			KeAcquireInStackQueuedSpinLock(&Adf.lock, &que);
			*(bool*)irp->AssociatedIrp.SystemBuffer = Adf.paused;
			KeReleaseInStackQueuedSpinLock(&que);

			irp->IoStatus.Information  = sizeof(bool);
			
		}
		break;
#if 1 // use list
	case IOCTL_ADF_ADD_USER_HOST:
		{
			int bufflen = strnlen(buff, ADF_HOST_MAX_LEN);
			KeAcquireInStackQueuedSpinLock(&Adf.lock, &que);
			
#if DBG
			KdPrint((APP "add host:%s to user list %s.\n", buff,
				addHostList(&Adf.AdHost.user, buff, bufflen) ? "success" : "failed"));
#else
			addHostList(&Adf.AdHost.user, buff, bufflen);
#endif
			
			KeReleaseInStackQueuedSpinLock(&que);
		}
		break;

	case IOCTL_ADF_DEL_USER_HOST:
		{
			int bufflen = strnlen(buff, ADF_HOST_MAX_LEN);
			KeAcquireInStackQueuedSpinLock(&Adf.lock, &que);
#if DBG
			KdPrint((APP "del host:%s from user list %s.\n", buff,
				removeHostList(&Adf.AdHost.user, buff, bufflen) ? "success" : "failed"));
#else
			removeHostList(&Adf.AdHost.user,buff,bufflen);
#endif
			KeReleaseInStackQueuedSpinLock(&que);
			return STATUS_SUCCESS;
		}
		break;
#endif// use list

#if 1 // except list
	case IOCTL_ADF_ADD_EXCEPT_HOST:
	{
		int bufflen = strnlen(buff, ADF_HOST_MAX_LEN);
		KeAcquireInStackQueuedSpinLock(&Adf.lock, &que);

#if DBG
		KdPrint((APP "add host:%s to except list %s.\n", buff,
			addHostList(&Adf.AdHost.excpt, buff, bufflen) ? "success" : "failed"));
#else
		addHostList(&Adf.AdHost.excpt, buff, bufflen);
#endif

		KeReleaseInStackQueuedSpinLock(&que);
	}
	break;

	case IOCTL_ADF_DEL_EXCEPT_HOST:
	{
		int bufflen = strnlen(buff, ADF_HOST_MAX_LEN);
		KeAcquireInStackQueuedSpinLock(&Adf.lock, &que);
#if DBG
		KdPrint((APP "del host:%s from except list %s.\n", buff,
			removeHostList(&Adf.AdHost.excpt, buff, bufflen) ? "success" : "failed"));
#else
		removeHostList(&Adf.AdHost.excpt, buff, bufflen);
#endif
		KeReleaseInStackQueuedSpinLock(&que);
		return STATUS_SUCCESS;
	}
	break;
#endif// except list
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

