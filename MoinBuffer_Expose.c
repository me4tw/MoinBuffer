// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
		{
			free(m->heapStorage);
			m->heapStorage = NULL;
			m->heapAllocationSize = 0;
		}
		m->staticStorage[0]='\0';
		m->heapStorage = newmem;
		m->mode = MoinRandomAccess;
		MoinBuffer_Optimise(m,m->heapAllocationSize + m->staticStorageSize);
	}
	m->heapStorage[m->writePos]='\0';
	return m->heapStorage + m->readPos;
}





#if MOINBUFFER_TESTS == 1

void tMoinBuffer_Expose_test1(CuTest *tc)
{
	char *str;
	MOINBUFFER(test,10);
	MoinBuffer_WriteStr(test,"abcdefgh");
	CuAssertIntEquals(tc,'a',MoinBuffer_GetC(test));
	MoinBuffer_WriteStr(test,"ijklmnop");
	str = MoinBuffer_Expose(test);
	CuAssertStrEquals(tc,"bcdefghijklmnop",str);
	MoinBuffer_Free(test);
}


void tMoinBuffer_Expose_test2(CuTest *tc)
{
	char *str;
	MOINBUFFER(test,10);
	MoinBuffer_WriteStr(test,"abcdefgh");
	MoinBuffer_WriteStr(test,"ijklmnop");
	MoinBuffer_Consume(test,10);
	str = MoinBuffer_Expose(test);
	CuAssertStrEquals(tc,"klmnop",str);
	MoinBuffer_Free(test);
}

CuSuite* tMoinBuffer_Expose_GetSuite(void)
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, tMoinBuffer_Expose_test1);
	SUITE_ADD_TEST(suite, tMoinBuffer_Expose_test2);
	return suite;
}
#endif
