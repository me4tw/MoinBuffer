#include "MoinBuffer.h"
#include <stdlib.h>//malloc/free
#include <string.h>//memcpy et cetera
static void mbStreamEnsureFree(struct MoinBuffer *m, size_t n);
static void mbRamEnsureFree(struct MoinBuffer *m, size_t n);
void MoinBuffer_EnsureFree(struct MoinBuffer *m, size_t n)
{
	MoinBuffer_Optimise(m,n);
	switch(m->mode)
	{
	case MoinStream:
		mbStreamEnsureFree(m,n);
		break;
	case MoinRandomAccess:
		mbRamEnsureFree(m,n);
		break;
	}
}

static void mbStreamEnsureFree(struct MoinBuffer *m, size_t n)
{
	if(m->writePos < m->staticStorageSize)
	{
		//case: [  xxxxxnnnn ]???????
		if(m->staticStorageSize - m->writePos >= n)
			return;
		n -= m->staticStorageSize - m->writePos;
		//case: [   xxxxxnnnn][nnnnnn    ]
		if(m->heapAllocationSize >= n)
			return;
		
		if(m->heapStorage)
		{
			//case: [   xxxxxxxnnnnnn][nnnnnnn]nnnnnn
			m->heapStorage = (char*)realloc(m->heapStorage, n+1);
		} else
		{
			//case: [    xxxxxxxnnnnnnnn]--------
			m->heapStorage = (char*)malloc(n+1);
			m->heapAllocationSize = n;
		}
	} else
	{
		//case: [     xxxxxxxxxxx]-----------
		if(!m->heapStorage)
			m->heapStorage = (char*)malloc(n+1);
		else
		{
			//case: [         ][ xxxnnnnnnnnnn   ]
			if(m->heapAllocationSize > (n+(m->writePos - m->staticStorageSize)))
				return;
			//case : [        ][   xxxnnnnn]nnnnnnn
			m->heapStorage = (char*)realloc(m->heapStorage, m->heapAllocationSize + n + 1);
			m->heapAllocationSize += n;
		}
	}
}
static void mbRamEnsureFree(struct MoinBuffer *m, size_t n)
{
	if(m->heapStorage)
	{
		//case [       ][              xxxxxnnnnnnn]
		if(m->readPos > n)
		{
			memmove(m->heapStorage, m->heapStorage+m->readPos, m->writePos-m->readPos);
			m->writePos -= m->readPos;
			m->readPos = 0;
			return;
		}
		//case: [       ][      xxxxxxnnnn  ]
		if(m->heapAllocationSize > m->writePos + n)
			return;
		//case: [        ][        xxxxxxxnnnnnnnn]nnnn
		m->heapStorage = (char*)realloc(m->heapStorage,m->writePos+n+1);
	} else
	{
		//case: [    xxxxxnnnnn ]--------
		if(m->staticStorageSize > m->writePos + n)
			return;
		//case: [   xxxxnnnnnn]nnnnnn-------
		m->heapStorage = (char*)malloc(n + 1 + m->writePos - m->readPos);
		memcpy(m->heapStorage, m->staticStorage + m->readPos, m->writePos - m->readPos);
		m->heapAllocationSize = n + 1 + m->writePos - m->readPos;
		m->writePos -= m->readPos;
		m->readPos = 0;
	}
}
