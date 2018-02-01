#include "MoinBuffer.h"
#include <stdlib.h>


void MoinBuffer_Free(struct MoinBuffer *m)
{
	if(m->heapStorage)
		free(m->heapStorage);
	m->staticStorage = NULL;
	m->staticStorageSize = 0;
	m->mode = MoinStream;
	m->heapAllocationSize = 0;
	m->heapStorage = NULL;
	m->readPos = 0;
	m->writePos = 0;
}
