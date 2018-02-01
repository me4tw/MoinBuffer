#include "MoinBuffer.h"
#include <stdlib.h>
static void mbStreamShiftDown(struct MoinBuffer *m, size_t minFree);
static void mbRamShiftDown(struct MoinBuffer *m, size_t minFree);
void MoinBuffer_Optimise(struct MoinBuffer *m, size_t minFree)
{
	if (m->readPos == m->writePos && m->readPos > 0)
	{
		m->readPos = 0;
		m->writePos = 0;
		if (m->heapStorage && m->staticStorageSize >= minFree)
		{
			free(m->heapStorage);
		}
	}

	if (m->readPos > 0)
	{
		switch (m->mode)
		{
		case MoinStream:
			mbStreamShiftDown(m,minFree);
			break;
		case MoinRandomAccess:
			mbRamShiftDown(m, minFree);
		}
	}
}
#include <string.h>
static void mbStreamShiftDown(struct MoinBuffer *m, size_t minFree)
{
	if (m->readPos < m->staticStorageSize)
	{
		// from: [      xxxxxxx]----
		// to:   [xxxxxxx      ]----
		memmove(m->staticStorage, m->staticStorage + m->readPos, m->staticStorageSize - m->readPos);
		if (m->heapStorage)
		{
			//from: [     xxxxxxx][xxxxxxx       ]
			//to:   [xxxxxxxxxxxx][xx            ]
			memcpy(m->staticStorage + m->staticStorageSize - m->readPos, m->heapStorage, m->readPos);
			memmove(m->heapStorage, m->heapStorage + m->readPos, m->heapAllocationSize - m->readPos);
		}
	}
	else
	{
		if (m->heapAllocationSize > m->staticStorageSize)
		{
			// from: [                  ][  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx]
			// to:   [xxxxxxxxxxxxxxxxxx][xxxxxxxxx             ]
			memcpy(m->staticStorage, m->heapStorage + m->readPos - m->staticStorageSize, m->staticStorageSize);
			memmove(m->heapStorage, m->heapStorage + m->readPos/*+-staticsz*/, m->heapAllocationSize - m->readPos);	
		}
		else
		{
			// from: [                  ][  xxx]
			// to:   [xxx               ][     ]
			memcpy(m->staticStorage, m->heapStorage + m->readPos - m->staticStorageSize, m->heapAllocationSize);
		}
	}
	m->writePos -= m->readPos;
	m->readPos = 0;
	if ((m->writePos < m->staticStorageSize - minFree) && m->heapStorage)
	{
		free(m->heapStorage);
		m->heapStorage = NULL;
		m->heapAllocationSize = 0;
	}
}

static void mbRamShiftDown(struct MoinBuffer *m, size_t minFree)
{
	if (!m->heapStorage)
	{
		memmove(m->staticStorage, m->staticStorage + m->readPos, m->staticStorageSize - m->readPos);
	}
	else
	{
		if ((m->writePos - m->readPos) + minFree <= m->staticStorageSize)
		{
			//from: [         ][      xxxxxxrrr    ]
			//to:   [xxxxxrrr ]---------------------
			memcpy(m->staticStorage, m->heapStorage + m->readPos, m->writePos - m->readPos);
			free(m->heapStorage);
			m->heapAllocationSize = 0;
		}
		else
		{
			//from:[         ][      xxxxxxxxxrrrrrrrrrrrrrrrrrrrrr ]
			//to:  [         ][xxxxxxxxxrrrrrrrrrrrrrrrrrrrrr       ]
			memmove(m->heapStorage, m->heapStorage + m->readPos, m->writePos - m->readPos);
		}
	}
	m->writePos -= m->readPos;
	m->readPos = 0;
}


