#ifndef _CTIME
#define _CTIME

    #include <stddef.h>

    // Functions that are exported by this library.

    extern int compare(size_t length, char* x, char* y);
    extern int equbyte(unsigned char x, unsigned char y);
    extern void copy(size_t length, char* x, char* y);

#endif
