// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "MoinBuffer.h"
#include <stdlib.h>//malloc
#include <string.h>//memcpy

char *MoinBuffer_Expose(struct MoinBuffer *m)
{
	if(m->mode == MoinStream)
	{
		size_t bytesleftfree = MoinBuffer_BytesLeftFree(m);
		size_t bytesofdata = MoinBuffer_BytesLeft(m);
		MoinBuffer_Optimise(m, bytesleftfree);
		if (bytesofdata + bytesleftfree <= m->staticStorageSize)
		{
			if (m->heapStorage)
				free(m->heapStorage);
			m->heapStorage = NULL;
			m->heapAllocationSize = 0;
			m->mode = MoinRandomAccess;
			//return &m->staticStorage[0];
		}
		else if(bytesleftfree + bytesofdata <= m->heapAllocationSize)
		{
			// we got [xxxxxxxxxx][xxx----------------]
			// we need[xxxxxxxxxx][----------xxx------]
			// then   [----------][xxxxxxxxxxxxx------]
			size_t amountInStatic = bytesofdata <= m->staticStorageSize ? bytesofdata : m->staticStorageSize;
			size_t amountInHeap = bytesofdata > m->staticStorageSize ? bytesofdata - m->staticStorageSize : 0;
			memmove(m->heapStorage + amountInStatic, m->heapStorage, amountInHeap);
			memcpy(m->heapStorage, m->staticStorage, amountInStatic);
			m->mode = MoinRandomAccess;
			//return &m->heapStorage[0];
		}
		else
		{
			size_t newmemsz = bytesofdata + bytesleftfree;
			char *newmem = malloc(newmemsz + 1);
			if (!newmem)
			{
				if (m->heapStorage)
				{
					free(m->heapStorage);
					m->heapStorage = NULL;
					m->staticStorageSize = 0;
				}
				return NULL;
			}
			memcpy(newmem, m->staticStorage, m->staticStorageSize);
			if (m->heapStorage && bytesofdata > m->staticStorageSize)
			{
				memcpy(newmem + m->staticStorageSize, m->heapStorage, bytesofdata - m->staticStorageSize);
				free(m->heapStorage);
				m->heapStorage = NULL;
				m->heapAllocationSize = 0;
			}
			m->heapStorage = newmem;
			m->heapAllocationSize = newmemsz;
			m->mode = MoinRandomAccess;
			//return &m->heapStorage[0];
		}
	}
	//this may change it from heap storage to static storage
	if (m->readPos > 4096)
		MoinBuffer_Optimise(m, MoinBuffer_BytesLeftFree(m));
	//now we can test that status to get the final location of the data
	if (m->heapStorage)
	{
		m->heapStorage[m->writePos] = '\0';
		return m->heapStorage + m->readPos;
	}
	else
	{
		m->staticStorage[m->writePos] = '\0';
		return m->staticStorage + m->readPos;
	}
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
