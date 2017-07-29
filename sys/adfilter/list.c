#include "adfilter.h"
#include "slre.h"

bool isSameHost(PLIST_ENTRY entry, char *name, int len)
{
	char*host = CONTAINING_RECORD(entry, HostList, list)->name.Buffer;
	int  hostlen = CONTAINING_RECORD(entry, HostList, list)->name.Length;

	if (hostlen != len) return false;

	return (memcmp(host, name, hostlen) == 0);
}

bool isInHostList(PLIST_ENTRY header, char* dst, int len)
{
	for (PLIST_ENTRY h = header->Blink;
		h != header;
		h = h->Blink)
	{
		// is the same host
		if (isSameHost(h, dst, len))
			return true;
	}

	return false;
}


PHostList newHostList(char *src, int len)
{
	// allocate memory
	PHostList e = ExAllocatePoolWithTag(NonPagedPool, sizeof(HostList), MEM);
	if (!e)	return NULL;

	e->name.Buffer = ExAllocatePoolWithTag(NonPagedPool, len + 1, MEM);
	if (!e->name.Buffer)
	{
		ExFreePoolWithTag(e, MEM);
		return NULL;
	}

	// copy data
	RtlCopyMemory(e->name.Buffer, src, len);
	e->name.Buffer[len] = 0;
	e->name.Length = (USHORT)len;
	e->name.MaximumLength = (USHORT)(len + 1);

	return e;
}

bool addHostList(PLIST_ENTRY header, char*src, int len)
{
	PHostList h = newHostList(src, len);
	if (!h)	return false;

	KdPrint((APP"add host:%s .", src));

	InsertTailList(header, (PLIST_ENTRY)h);
	return true;
}

bool removeHostList(PLIST_ENTRY header, char* dst, int len)
{
	for (PLIST_ENTRY h = header->Blink;
		h != header;
		h = h->Blink)
	{
		// is the same host
		if (isSameHost(h, dst, len))
		{
			// remove from list
			RemoveEntryList(h);

			// simply we just free it
			ExFreePoolWithTag(CONTAINING_RECORD(h, HostList, list)->name.Buffer, MEM);
			ExFreePoolWithTag(h, MEM);
			return true;
		}
	}

	return false;
}

void clearHostList(PLIST_ENTRY header)
{
	for (PLIST_ENTRY entry = RemoveTailList(header);
		entry != header;
		entry = RemoveTailList(header))
	{
		ExFreePoolWithTag(CONTAINING_RECORD(entry, HostList, list)->name.Buffer, MEM);
		ExFreePoolWithTag(entry, MEM);
	}
}

/*
 *	try to match host in list use regular expersion
 */
bool isMatchHostList(PLIST_ENTRY header, char* dst, int len)
{
	for (PLIST_ENTRY h = header->Blink;
		h != header;
		h = h->Blink)
	{
		char* reg = CONTAINING_RECORD(h, HostList, list)->name.Buffer;

		// match it
		if( slre_match(reg, dst, len, NULL, 0, SLRE_IGNORE_CASE) == len)
			return true;
	}

	return false;
}