#include "MoinBuffer.h"
#include <string.h>



void MoinBuffer_Write(struct MoinBuffer *m, const char *src, size_t sz)
{
	MoinBuffer_EnsureFree(m,sz+1);
	switch(m->mode)
	{
	case MoinStream:
		{
			size_t i;
			for(i=0;i<sz;++i)
				MoinBuffer_PutC(m,src[i]);
		}
		break;
	case MoinRandomAccess:
		if(m->heapStorage)
		{
			memcpy(m->heapStorage+m->writePos, src, sz);
			m->heapStorage[m->writePos+sz]='\0';
			m->writePos += sz;
		} else
		{
			memcpy(m->staticStorage+m->writePos, src, sz);
			m->staticStorage[m->writePos+sz]='\0';
			m->writePos += sz;
		}
		break;
	}
}




#if MOINBUFFER_TESTS == 1

void tMoinBuffer_Write_test1(CuTest *tc)
{
	//create
	MOINBUFFER(t, 100);
	//act
	MoinBuffer_WriteStr(t, "ABCD");
	//test
	CuAssertIntEquals(tc, 4, MoinBuffer_BytesLeft(t));
	//cleanup
	MoinBuffer_Free(t);
}


void tMoinBuffer_Write_test2(CuTest *tc)
{
	//create
	MOINBUFFER(t, 100);
	//act
	MoinBuffer_WriteStr(t, "ABCD");
	//test
	CuAssertIntEquals(tc, 0, t->readPos);
	CuAssertIntEquals(tc, 4, t->writePos);
	//cleanup
	MoinBuffer_Free(t);
}


CuSuite* tMoinBuffer_Write_GetSuite(void)
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, tMoinBuffer_Write_test1);
	SUITE_ADD_TEST(suite, tMoinBuffer_Write_test2);
	return suite;
}
#endif




