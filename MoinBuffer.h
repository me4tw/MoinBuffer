// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/**@file
@brief Malloc Only If Needed Buffers

The Malloc Only If Needed buffers allow you to have the best of both worlds, and with full
efficiency if you only need char-by-char access.

@ingroup MoinBuffer

This header file has unit tests, using CuTest.



@defgroup MoinBuffer MoinBuffer: Malloc-Only-If-Needed Buffers

Have code like:
@code
char *buffer = malloc(size_needed);
doStuff(buffer);
free(buffer);
@endcode
Well it will be slow if run in a tight loop because of the malloc() and the free()

Or do you have code like:
@code
char buffer[100];
doStuff(buffer);
@endcode
Well it will blow out the stack if called recursively. Or even if called once for example
if you use a 4096 buffer that will blow out the stack on DOS or embedded systems sometimes.
But even 4096 is small for a string if it holds paragraphs of text.

Not to mention what will happen if you need to write more than 99 characters or 100 bytes of data

@code
char backingstorage_for_buffer[100];
struct MoinBuffer buffer;
MoinBuffer_Init(&buffer,&backingstorage_for_buffer,sizeof(backingstorage_for_buffer));
doStuff(buffer);
//or even
char *buff;
MoinBuffer_EnsureTotal(&buffer,size_needed);
buff = MoinBuffer_Expose(&buffer);
doStuff(buff);
//and finally, this will only call free() if malloc() was actually needed at all
MoinBuffer_Free(&buffer);
@endcode

The best of both worlds, fast for all <100 size_needed situations, but allows infinite size
allocated on demand!

And for convenience there is a macro like so:
@code
float myNum = 0.88f;
MOINBUFFER(MyBuf,128);
int somethingElse = 123;

MoinBuffer_EnsureTotal(MyBuf,size_needed);
doStuff(MoinBuffer_Expose(MyBuf));
MoinBuffer_Free(MyBuf);
@endcode

@note This macro (MOINBUFFER()) should work not only for C++/C99 variable-defined-anywhere
    capability but also in strict (pedantic mode) C89.
*/
#ifndef DS_MoinBuf_h
#define DS_MoinBuf_h

#include "StdInt.h"//for size_t
#include <string.h>//for strlen() in the macro

/**
change this to 0 for your project, I leave it as 1 because I don't want to set project defines
*/
#define DEFAULT_TEST_ENABLE_STATE 1

/**Tells what mode the MOIN buffer is in*/
enum MoinBufferMode
{
	/**only char-by-char access is allowed, will save ram*/
	MoinStream,
	/**full access is allowed, might waste whatever static storage amount */
	MoinRandomAccess
};
struct MoinBuffer
{
	char *staticStorage;
	char *heapStorage;

	/**always -1 to make room for null terminator*/
	size_t staticStorageSize;
	/**always -1 to make room for null terminator*/
	size_t heapAllocationSize;

	enum MoinBufferMode mode;
	/**in stream mode, this is from beginning of static+heap, else is from EITHER start of static OR heap*/
	size_t readPos;
	size_t writePos;
};

/**
Will create the MoinBuffer in the stream mode, where this data structure is most useful and efficient
@code
struct MoinbufExample
{
	struct MoinBuffer memory;
	char data[100];
};
struct MoinBufExample ex;
MoinBuffer_Init(&ex.mem,&ex.data,sizeof(ex.data));
@endcode
@return It returns the parameter m. It does this to allow the macro to be C89-compatible by making the initialize calling line also be a variable instantiating line.
*/
struct MoinBuffer * MoinBuffer_Init(struct MoinBuffer *m, char *staticStorage, size_t sz);
/**
A convenience macro on MoinBuffer_Init(). Note that this can only be used inside functions,
not inside a struct, because it calls a function.
@param BUFFERNAME         The name you want for your pointer variable (of type struct MoinBuffer *)
@param FASTSIZE           The size of the auto variable array you want, data usage beyond this size will incur the performance penalty of malloc() / free()
*/
#define MOINBUFFER(BUFFERNAME,FASTSIZE) \
 struct MoinBuffer header_##BUFFERNAME; \
 char backingSpace_##BUFFERNAME[FASTSIZE+1]; \
 struct MoinBuffer * BUFFERNAME = MoinBuffer_Init(&header_##BUFFERNAME,backingSpace_##BUFFERNAME,FASTSIZE+1)

char MoinBuffer_GetC(struct MoinBuffer *m);
int MoinBuffer_Eof(const struct MoinBuffer *m);
void MoinBuffer_PutC(struct MoinBuffer *m, char c);
/**@return how many bytes are left to still be read*/
size_t MoinBuffer_BytesLeft(const struct MoinBuffer *m);
#define MoinBuffer_Length MoinBuffer_BytesLeft
/**@return how many bytes can be written without an alloc or optimise*/
size_t MoinBuffer_BytesLeftFree(const struct MoinBuffer *m);

/**
Called by other functions at certain times.
Will shift data down to the start of the memory area as appropriate.
*/
void MoinBuffer_Optimise(struct MoinBuffer *m, size_t minFree);

/**
@param m      the buffer object to modify
@param n      the amount of total RAM space needed in the buffer object m
*/
void MoinBuffer_EnsureTotal(struct MoinBuffer *m, size_t n);

/**
Use this to guarantee that there is enough space to insert all this data.
@param m      the buffer object to modify
@param n      the amount of free RAM space needed at the end of the buffer object m. The end is determined by the write position.
*/
void MoinBuffer_EnsureFree(struct MoinBuffer *m, size_t n);

/**
Will convert the MoinBuffer to MoinRandomAccess mode if need be.
@warning This will shift the data around. If you call MoinBuffer_EnsureFree()
and then MoinBuffer_WroteUpTo() and then MoinBuffer_Consume()
then this MoinBuffer_Expose(), what can happen is that the data may be shifted
down and the amount that is still free will be retained but excess free beyond
that may be reallocated away which you might not expect (so the full amount
originally free before the wrote up to will no longer be guaranteed to be available).
In this case if you care about this you should call MoinBuffer_EnsureFree() again
before MoinBuffer_Expose()
@return a pointer to internal memory
*/
char * MoinBuffer_Expose(struct MoinBuffer *m);
/**
Advances the read pointer by this much.
@warning If you call this before calling MoinBuffer_WroteUpTo() you will get errors.
Call MoinBuffer_WroteUpTo() first in that case.
*/
void MoinBuffer_Consume(struct MoinBuffer *m, size_t n);
/**
Adds the data and advances the write pointer.
Calls MoinBuffer_PutC() in a loop if in stream mode,
otherwise does it all in a single memcpy() operation.
*/
void MoinBuffer_Write(struct MoinBuffer *m, const char *src, size_t sz);
/**
A little convenience macro
@param M       Of type struct MoinBuffer *
@param SRC      Of type const char *
*/
#define MoinBuffer_WriteStr(M, SRC) \
	MoinBuffer_Write(M, SRC, strlen(SRC))
/**
Notify the system that you wrote up to the given offset, important or else optimize function will clobber your manually written data
@warning if you use MoinBuffer_EnsureTotal() and MoinBuffer_Expose() and write to that buffer and don't use this to mark how high you've written, then MoinBuffer_GetC() and MoinBuffer_Eof() and so on won't work properly.
@param m        the MoinBuffer to operate on
@param endPoint         The offset from the pointer returned from MoinBuffer_Expose() that you have written up to
@warning If you call this after calling MoinBuffer_Consume() you will get errors. Call this first in that case.
*/
void MoinBuffer_WroteUpTo(struct MoinBuffer *m, size_t endPoint);

void MoinBuffer_Free(struct MoinBuffer *m);

/**
@return a number between 0 and 255 if a byte was pushed out, -1 if no byte was pushed out. Useful mainly when nBytes = 1
*/
int MoinBuffer_CircularAdd(struct MoinBuffer *m, size_t circularSize, const char *src, size_t nBytes);


#ifdef ENABLE_TESTS
/**
If this is enabled (set to 1) then unit tests are turned on
*/
#define MOINBUFFER_TESTS 1
#endif

#ifdef DISABLE_TESTS
/**
If this is disabled (set to 0) then unit tests are not turned on
*/
#define MOINBUFFER_TESTS 0
#endif

#ifndef MOINBUFFER_TESTS
/**
if this is set then CuTest.h will be included and unit tests functions will be turned on
*/
#define MOINBUFFER_TESTS DEFAULT_TEST_ENABLE_STATE
#endif





#if MOINBUFFER_TESTS == 1
#include "CuTest.h"
CuSuite* tMoinBuffer_Eof_GetSuite(void);
CuSuite* tMoinBuffer_Write_GetSuite(void);
CuSuite* tMoinBuffer_BytesLeft_GetSuite(void);
CuSuite* tMoinBuffer_Expose_GetSuite(void);
CuSuite* tMoinBuffer_PutC_GetSuite(void);
CuSuite* tMoinBuffer_WroteUpTo_GetSuite(void);
/**A convenience macro for the RunAllTests() function*/
#define ADD_MOINBUFFER_TESTS(SUITE) \
CuSuiteAddSuite(SUITE, tMoinBuffer_PutC_GetSuite()); \
CuSuiteAddSuite(SUITE, tMoinBuffer_Expose_GetSuite()); \
CuSuiteAddSuite(SUITE, tMoinBuffer_Eof_GetSuite()); \
CuSuiteAddSuite(SUITE, tMoinBuffer_Write_GetSuite()); \
CuSuiteAddSuite(SUITE, tMoinBuffer_BytesLeft_GetSuite()); \
CuSuiteAddSuite(SUITE, tMoinBuffer_WroteUpTo_GetSuite());
#endif

#endif
