#include "MoinBuffer.h"

void MoinBuffer_EnsureTotal(struct MoinBuffer *m, size_t n)
{
	size_t sz;
	MoinBuffer_Optimise(m,n);
	sz = m->writePos - m->readPos;
	if(n > sz)
		MoinBuffer_EnsureFree(m,n - sz);
}

