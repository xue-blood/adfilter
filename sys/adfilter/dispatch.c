
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
	void* buff = irp->AssociatedIrp.SystemBuffer;
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
	case IOCTL_ADF_ADD_HOST:
		{
#if 1 // add host
			PHostList invalid_ptr = NULL;	// store the first invalid entry
			
			KeAcquireInStackQueuedSpinLock(&Adf.lock, &que);
			// search in the list
			for (h = Adf.AdHost.list.Next.Next;
				h;
				h= h->Next)
			{
				char * name = CONTAINING_RECORD(h, HostList, list)->name;
				bool invalid = CONTAINING_RECORD(h, HostList, list)->invalid;

				if (invalid && !invalid_ptr)
				{
					// find a invalid entry,and it should be the first
					invalid_ptr = (PHostList)h;
				}

				if (memcmp(buff, name, strlen(name)) == 0)
				{
					// found the same entry
					CONTAINING_RECORD(h, HostList, list)->invalid = false;

					KeReleaseInStackQueuedSpinLock(&que);
					KdPrint((APP "renable host:%s.\n",buff));
					return STATUS_SUCCESS;
				}
			}

			int newlen = strnlen(buff, ADF_HOST_MAX_LEN);
			
			if (invalid_ptr)
#if 1 // found a invalid entry
			{
				char * oldbuff = CONTAINING_RECORD(h, HostList, list)->name;
				int oldlen = strlen(oldbuff);
				
				if (newlen <= oldlen)
				{
					// old buffer is enough, so just copy it
					RtlZeroMemory(oldbuff, oldlen + 1);
					RtlCopyMemory(oldbuff, buff, newlen + 1);
					KeReleaseInStackQueuedSpinLock(&que);

					KdPrint((APP"add host: %s success.\n", buff));
					return STATUS_SUCCESS;
				}

				// buffer is small, so we should realloc it
				char *newbuff = ExAllocatePoolWithTag(NonPagedPool, newlen + 1, MEM);
				if (!newbuff)
				{
					KeReleaseInStackQueuedSpinLock(&que);
					KdPrint(("[adf] allocate memory failed.\n"));
					KdPrint((APP"add host: %s failed.\n", buff));
					return STATUS_MEMORY_NOT_ALLOCATED;
				}

				// update new buffer
				ExFreePoolWithTag(oldbuff, MEM);
				oldbuff = newbuff;
				CONTAINING_RECORD(h, HostList, list)->name = newbuff;

				// copy data to buffer
				RtlZeroMemory(oldbuff, newlen + 1);
				RtlCopyMemory(oldbuff, buff, newlen +1);

				// all work done
				KdPrint((APP"add host: %s success.\n", buff));
			}
#endif// found a invalid entry
			else
#if 1 // no invalid entry, so we should insert a new
			{
				PHostList e = ExAllocatePoolWithTag(NonPagedPool, sizeof(HostList), MEM);
				if (!e)
				{
					KdPrint(("[adf] allocate memory failed.\n"));
					KdPrint((APP"add host: %s failed.\n", buff)); 
					KeReleaseInStackQueuedSpinLock(&que);
					return STATUS_MEMORY_NOT_ALLOCATED;
				}
				e->name = ExAllocatePoolWithTag(NonPagedPool, newlen + 1, MEM);
				if (!e->name)
				{
					KdPrint(("[adf] allocate memory failed.\n"));
					KdPrint((APP"add host: %s failed.\n", buff)); 
					KeReleaseInStackQueuedSpinLock(&que);
					return STATUS_MEMORY_NOT_ALLOCATED;
				}

				// add host to list
				e->invalid = false;
				RtlZeroMemory(e->name, newlen + 1);
				RtlCopyMemory(e->name, buff, newlen + 1);
				InterlockedPushEntrySList(&Adf.AdHost.list, (PSLIST_ENTRY)e);
				KdPrint((APP"add host: %s success.\n", buff));
			}
#endif// no invalid entry, so we should insert a new
			KeReleaseInStackQueuedSpinLock(&que);

#endif// add host
		}
		break;

	case IOCTL_ADF_DEL_HOST:
		{
			KeAcquireInStackQueuedSpinLock(&Adf.lock, &que);
			for (h = Adf.AdHost.list.Next.Next;
				h;
				h = h->Next)
			{
				char * name = CONTAINING_RECORD(h, HostList, list)->name;

				if (memcmp(buff, name, strlen(name)) == 0)
				{
					// set the host to invalid
					CONTAINING_RECORD(h, HostList, list)->invalid = true;
					KeReleaseInStackQueuedSpinLock(&que);
					KdPrint((APP "delete host:%s\n", buff));
					return STATUS_SUCCESS;
				}
			}

			KeReleaseInStackQueuedSpinLock(&que);
			KdPrint((APP "no host:%s to delete\n", buff));
			return STATUS_UNSUCCESSFUL;
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

