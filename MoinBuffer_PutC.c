#include "MoinBuffer.h"


static void mbStreamPutC(struct MoinBuffer *m, char c);
static void mbRamPutC(struct MoinBuffer *m, char c);
void MoinBuffer_PutC(struct MoinBuffer *m, char c)
{
	MoinBuffer_EnsureFree(m, 1);
	switch (m->mode)
	{
	case MoinStream:
		mbStreamPutC(m, c);
		break;
	case MoinRandomAccess:
		mbRamPutC(m, c);
		break;
	}
}
static void mbStreamPutC(struct MoinBuffer *m, char c)
{
	if (m->writePos < m->staticStorageSize)
	{
		m->staticStorage[m->writePos++] = c;
	}
	else
	{
		m->heapStorage[m->writePos - m->staticStorageSize] = c;
		m->writePos++;
	}
}
static void mbRamPutC(struct MoinBuffer *m, char c)
{
	if (m->heapStorage)
	{
		m->heapStorage[m->writePos++] = c;
	}
	else
	{
		m->staticStorage[m->writePos++] = c;
	}
}
