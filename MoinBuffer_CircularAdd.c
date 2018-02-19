// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "MoinBuffer.h"

int MoinBuffer_CircularAdd(struct MoinBuffer *m, size_t circularSize, const char *src, size_t nBytes)
{
	size_t currentLen = MoinBuffer_BytesLeft(m);
	int ret = -1;
	if (currentLen >= circularSize)
	{
		if (nBytes == 1)
		{
			char c = MoinBuffer_GetC(m);
			unsigned char uc = (unsigned char)c;
			ret = uc;
		}
		else
		{
			MoinBuffer_Consume(m, (circularSize - currentLen) + nBytes);
			ret = 0;
		}
	}
	MoinBuffer_Optimise(m, nBytes);
	MoinBuffer_Write(m, src, nBytes);
	return ret;
}
