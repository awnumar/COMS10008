/*
 * Cryptographic library that implements time-constant variations of common
 * cryptographic operations.
 *
 * See readme.txt for more information.
 */

#include <stdio.h>
#include <assert.h>

#include "ctime.h"

// Compares two arrays in constant-time.
// Size of `x` and `y` MUST be equal to `length`, or it's undefined behaviour.
// Returns `1` if and only if inputs are equal.
int compare(size_t length, char* x, char* y) {
    unsigned char v = 0; // Unsigned so that it doesn't overflow.

    for (size_t i = 0; i < length; i++) {
        v |= x[i] ^ y[i]; // v = v AND (x[i] XOR y[i])
    }

    return 1 & ((v - 1) >> 8);
}

// Compares two bytes in constant-time.
// Returns `1` if and only if inputs are equal.
int equbyte(unsigned char x, unsigned char y) {
    unsigned char c = ~(x ^ y); // c = NOT (x XOR y)
    c &= c >> 4;
    c &= c >> 2;
    c &= c >> 1;

    return c;
}

// Copies the contents of y into x, in constant-time.
// Size of `x` and `y` MUST be equal to `length`, or it's undefined behaviour.
void copy(size_t length, char* x, char* y) {
    // Define constants.
    unsigned char xmask = 0; // xmask = 0 => 00000000
    unsigned char ymask = ~0; // ymask = NOT 0 => 11111111

    for (size_t i = 0; i < length; i++) {
        // x[i] = (x[i] AND xmask) OR (y[i] AND ymask)
        x[i] = (x[i] & xmask) | (y[i] & ymask);
    }
}

int main() {
    // Runs tests if called directly.

    // Test compare.
    char x1[] = {0, 0, 0};
    char x2[] = {0, 0, 0};
    char y1[] = {1, 1, 1};
    char y2[] = {1, 1, 1};
    assert(compare(3, x1, y1) != 1);
    assert(compare(3, x1, x2) == 1);
    assert(compare(3, y2, y2) == 1);

    // Test equbyte.
    unsigned char a = 0;
    unsigned char b = 1;
    unsigned char c = 255;
    assert(equbyte(a, b) != 1);
    assert(equbyte(b, c) != 1);
    assert(equbyte(a, c) != 1);
    assert(equbyte(a, a) == 1);
    assert(equbyte(b, b) == 1);
    assert(equbyte(c, c) == 1);

    // Test copy.
    char u[] = {0, 1, 2, 3, 4, 5, 6, 7};
    char v[] = {0, 0, 0, 0, 0, 0, 0, 0};
    char w[] = {0, 0, 0, 0, 0, 0, 0, 0};
    copy(8, v, u); assert(compare(8, v, u) == 1);
    copy(8, u, w); assert(compare(8, u, w) == 1);

    // Everything went well.
    printf("All tests pass.\n");
}
