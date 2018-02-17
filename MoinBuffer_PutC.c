// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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


#if MOINBUFFER_TESTS == 1
void tMoinBuffer_PutC_test1(CuTest *tc)
{
	MOINBUFFER(test,2);
	MoinBuffer_PutC(test,'a');
	MoinBuffer_PutC(test,'b');
	MoinBuffer_PutC(test,'c');
	CuAssertIntEquals(tc,'a',MoinBuffer_GetC(test));
	CuAssertIntEquals(tc,'b',MoinBuffer_GetC(test));
	CuAssertIntEquals(tc,'c',MoinBuffer_GetC(test));

	CuAssertIntEquals(tc,'\0',MoinBuffer_GetC(test));
	CuAssertIntEquals(tc,'\0',MoinBuffer_GetC(test));

	MoinBuffer_PutC(test,'x');
	CuAssertIntEquals(tc,'x',MoinBuffer_GetC(test));
	MoinBuffer_Free(test);
}
CuSuite* tMoinBuffer_PutC_GetSuite(void)
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, tMoinBuffer_PutC_test1);
	return suite;
}
#endif

