#include "MoinBuffer.h"


void MoinBuffer_WroteUpTo(struct MoinBuffer *m, size_t endPoint)
{
	m->writePos = m->readPos + endPoint;
}
