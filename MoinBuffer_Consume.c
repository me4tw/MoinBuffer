// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

