// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "MoinBuffer.h"

void MoinBuffer_EnsureTotal(struct MoinBuffer *m, size_t n)
{
	size_t sz;
	MoinBuffer_Optimise(m,n);
	sz = m->writePos - m->readPos;
	if(n > sz)
		MoinBuffer_EnsureFree(m,n - sz);
}

