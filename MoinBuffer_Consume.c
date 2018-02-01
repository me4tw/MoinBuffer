#include "MoinBuffer.h"

void MoinBuffer_Consume(struct MoinBuffer *m, size_t n)
{
	m->readPos += n;
	if(m->readPos > m->writePos)
	{
		m->readPos=m->writePos;
		MoinBuffer_Optimise(m,0);
	}
}

