# MoinBuffer
Malloc Only If Needed Buffers

These can be expanded even further, such as circular buffer capability perhaps.

But they are super-memory efficient for character-by-character access (getc/putc)

And they still save you typing and runtime performance for user-input / dynamic-sized data so long
as your static buffer size is big enough to cover the common case.

Another idea would be using a garbage collector for the heap buffer so that the free function never
needs to be called, that would be even better.

[![HitCount](http://hits.dwyl.io/me4tw/MoinBuffer.svg)](http://hits.dwyl.io/me4tw/MoinBuffer)


## Use them as circular buffers
These are great, you can use them as a circular buffer that grants you access to the entire range of data. And it can all
be done without malloc so long as you never push too much on at once. Just remember that to use them as a circular buffer, every time you append a character, you must test if the length is >= your "circular" size and if so then consume a character first. That way it will never call malloc() even if you push gigabytes through the circular buffer, and you can at any point get access to a contigous array of the contents, and it is optimised in that it does not memcpy() at every allocation only when needed.

So if you have a static backing buffer of 1024 and your cicular size is 640kb then you will only have memcpy() called every so often, and never a malloc().

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
#### Not to mention what will happen if you need to write more than 99 characters or 100 bytes of data

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
