#include "MoinBuffer.h"
#include "MoinBuffer.h"
#include <stdlib.h>//malloc
#include <string.h>//memcpy

char *MoinBuffer_Expose(struct MoinBuffer *m)
{
	if(m->mode == MoinStream)
	{
		//convert to random access
		char *newmem = malloc(m->heapAllocationSize + m->staticStorageSize + 1);
		memcpy(newmem,m->staticStorage,m->staticStorageSize);
		if(m->heapAllocationSize)
			memcpy(newmem+m->staticStorageSize,m->heapStorage,m->heapAllocationSize);
		newmem[m->heapAllocationSize + m->staticStorageSize]='\0';
		if(m->heapStorage)
			free(m->heapStorage);
		m->staticStorage[0]='\0';
		m->heapStorage = newmem;
		m->mode = MoinRandomAccess;
		MoinBuffer_Optimise(m,m->heapAllocationSize + m->staticStorageSize);
	}
	m->heapStorage[m->writePos]='\0';
	return m->heapStorage + m->readPos;
}

