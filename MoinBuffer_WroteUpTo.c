// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "MoinBuffer.h"


void MoinBuffer_WroteUpTo(struct MoinBuffer *m, size_t endPoint)
{
	m->writePos = m->readPos + endPoint;
}


#if MOINBUFFER_TESTS == 1
void tMoinBuffer_WroteUpTo_test1(CuTest *tc)
{
	//create
	MOINBUFFER(t, 6);
	//act
	MoinBuffer_WriteStr(t, "ABCD");
	//test
	CuAssertIntEquals(tc, 4, MoinBuffer_BytesLeft(t));
	//act
	MoinBuffer_EnsureFree(t,100);
	//test
	CuAssertIntEquals(tc, 4, MoinBuffer_BytesLeft(t));
	//act
	MoinBuffer_Expose(t);
	MoinBuffer_WroteUpTo(t,14);
	MoinBuffer_Consume(t,6);
	//test
	CuAssertIntEquals(tc, 8, MoinBuffer_BytesLeft(t));

	//cleanup
	MoinBuffer_Free(t);
}
CuSuite* tMoinBuffer_WroteUpTo_GetSuite(void)
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, tMoinBuffer_WroteUpTo_test1);
	return suite;
}
#endif


