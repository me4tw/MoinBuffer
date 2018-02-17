// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "MoinBuffer.h"

static char mbStreamGetC(struct MoinBuffer *m);
static char mbRamGetC(struct MoinBuffer *m);

char MoinBuffer_GetC(struct MoinBuffer *m)
{
	if (!MoinBuffer_Eof(m))
	{
		switch (m->mode)
		{
		case MoinStream:
			return mbStreamGetC(m);
			break;
		case MoinRandomAccess:
			return mbRamGetC(m);
			break;
		}
	}
	return '\0';
}

/**
if read pos is less than static size, get it from static,
else get it from heap (minus static size offset) if heap is not null
*/
static char mbStreamGetC(struct MoinBuffer *m)
{
	char ret = '\0';
	if (m->readPos < m->staticStorageSize)
	{
		ret= m->staticStorage[m->readPos];
		m->readPos++;
	}
	else if(m->heapStorage)
	{
		ret = m->heapStorage[m->readPos - m->staticStorageSize];
		m->readPos++;
	}
	return ret;
}

/**
get it from heap if heap is not null
if heap is null then get it from static
*/
static char mbRamGetC(struct MoinBuffer *m)
{
	if (m->heapStorage && m->readPos < m->heapAllocationSize)
	{
		return m->heapStorage[m->readPos++];
	}
	else if(m->heapStorage == NULL && m->readPos < m->staticStorageSize)
	{
		return m->staticStorage[m->readPos++];
	}
	return '\0';
}
