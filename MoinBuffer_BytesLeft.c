#include "MoinBuffer.h"



size_t MoinBuffer_BytesLeft(const struct MoinBuffer *m)
{
	return m->writePos - m->readPos;
}




#if MOINBUFFER_TESTS == 1

void tMoinBuffer_BytesLeft_test1(CuTest *tc)
{
	const char *input = "ABCDEFG";
	int actual;
	int expected = 3;
	char outVal;

	MOINBUFFER(testbuf, 100);

	MoinBuffer_WriteStr(testbuf, input);

	outVal = MoinBuffer_GetC(testbuf);
	//CuAssertIntEquals(tc, 'A', outVal);

	outVal = MoinBuffer_GetC(testbuf);
	//CuAssertIntEquals(tc, 'B', outVal);

	outVal = MoinBuffer_GetC(testbuf);
	//CuAssertIntEquals(tc, 'C', outVal);

	outVal = MoinBuffer_GetC(testbuf);
	//CuAssertIntEquals(tc, 'D', outVal);

	actual = MoinBuffer_BytesLeft(testbuf);
	CuAssertIntEquals(tc, 3, actual);


	MoinBuffer_Free(testbuf);
}

CuSuite* tMoinBuffer_BytesLeft_GetSuite(void)
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, tMoinBuffer_BytesLeft_test1);
	return suite;
}
#endif




