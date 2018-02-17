// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "MoinBuffer.h"


struct MoinBuffer * MoinBuffer_Init(struct MoinBuffer *m, char *staticStorage, size_t sz)
{
	m->staticStorage = staticStorage;
	m->staticStorageSize = sz > 0 ? sz - 1 : 0;
	m->mode = MoinStream;
	m->heapAllocationSize = 0;
	m->heapStorage = NULL;
	m->readPos = 0;
	m->writePos = 0;
	if (sz > 0)
		m->staticStorage[0] = '\0';
	return m;
}

