// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "MoinBuffer.h"



int MoinBuffer_Eof(const struct MoinBuffer *m)
{
	int ret = m->readPos < m->writePos;
	return !ret;
}





#if MOINBUFFER_TESTS == 1

void tMoinBuffer_Eof_test1(CuTest *tc)
{
	//create
	MOINBUFFER(t, 100);
	//act

	//test
	CuAssertIntEquals(tc, 1, MoinBuffer_Eof(t));
	//cleanup
	MoinBuffer_Free(t);
}

void tMoinBuffer_Eof_test3(CuTest *tc)
{
	//create
	MOINBUFFER(t, 100);
	//act
	MoinBuffer_WriteStr(t, "ABCD");
	(void)MoinBuffer_GetC(t);
	(void)MoinBuffer_GetC(t);
	(void)MoinBuffer_GetC(t);
	(void)MoinBuffer_GetC(t);
	//test
	CuAssertIntEquals(tc, 1, MoinBuffer_Eof(t));
	//cleanup
	MoinBuffer_Free(t);
}

void tMoinBuffer_Eof_test2(CuTest *tc)
{
	//create
	MOINBUFFER(t, 100);
	//act
	MoinBuffer_WriteStr(t, "ABCD");
	//test
	CuAssertIntEquals(tc, 0, MoinBuffer_Eof(t));
	//cleanup
	MoinBuffer_Free(t);
}


CuSuite* tMoinBuffer_Eof_GetSuite(void)
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, tMoinBuffer_Eof_test1);
	SUITE_ADD_TEST(suite, tMoinBuffer_Eof_test2);
	SUITE_ADD_TEST(suite, tMoinBuffer_Eof_test3);
	return suite;
}
#endif

