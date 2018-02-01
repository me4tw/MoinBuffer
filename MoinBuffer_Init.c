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

