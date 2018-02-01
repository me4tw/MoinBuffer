# MoinBuffer
Malloc Only If Needed Buffers

These can be expanded even further, such as circular buffer capability perhaps.

But they are super-memory effecient for character-by-character access (getc/putc)

And they still save you typing and runtime performance for user-input / dynamic-sized data so long
as your static buffer size is big enough to cover the common case.

[![HitCount](http://hits.dwyl.io/me4tw/MoinBuffer.svg)](http://hits.dwyl.io/me4tw/MoinBuffer)


### Have code like:
~~~{.c}
char *buffer = malloc(size_needed);
doStuff(buffer);
free(buffer);
~~~
Well it will be slow if run in a tight loop because of the malloc() and the free()

### Or do you have code like:
~~~{.c}
char buffer[100];
doStuff(buffer);
~~~
Well it will blow out the stack if called recursively. Or even if called once for example
if you use a 4096 buffer that will blow out the stack on DOS or embedded systems sometimes.
But even 4096 is small for a string if it holds paragraphs of text.

### try this
~~~{.c}
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
~~~

The best of both worlds, fast for all <100 size_needed situations, but allows infinite size
allocated on demand!

### And for convenience there is a macro like so:
~~~{.c}
float myNum = 0.88f;
MOINBUFFER(MyBuf,128);
int somethingElse = 123;

MoinBuffer_EnsureTotal(MyBuf,size_needed);
doStuff(MoinBuffer_Expose(MyBuf));
MoinBuffer_Free(MyBuf);
~~~

### note
This macro (MOINBUFFER()) should work not only for C++/C99 variable-defined-anywhere
    capability but also in strict (pedantic mode) C89.
