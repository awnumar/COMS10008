My open-ended submission is a C implementation of common constant-time algorithms used in cryptographic implementations.

The Makefile covers most things:

    make build  - Compile the source code into a C object file.
    make test   - Compile the library and run the provided tests.
    make clean  - Remove leftover files from previous compilations.

The algorithms are taken from the Go crypto/subtle library [0], which I have partially contributed to myself [1]. Below is a short analysis of how each algorithm works.

The time taken by these functions is designed to be completely independent of the contents of the arguments. The implementations are also designed to avoid branch-misdirections, and to allow the CPU only a single, constant-time, execution-path.

- int compare(size_t length, char* x, char* y);

    We iterate over every element in the equal length arrays. Common timing attacks occur because most comparison functions iterate over inputs only until a difference is found.

    So, if we instead continue the iteration for the entire length of the array, and then evaluate and "discover" the result at the end, timing-attacks are mitigated against.

    On each iteration of the loop, the XOR of the values at index `i` is computed and then the OR of this value and the value from the previous iterations is computed. The result is stored for later iterations to compare against.

    The way XOR works is that if the two operands are equal, the result is zero. So if any of the iterations return a non-zero value, this will propagate across all of the iterations, as the OR computation will never again return a zero.

    To return a `1` to indicate equality and a `0` to indicate the opposite, we need to "flip" the result of the earlier computation. This can be achieved with the following expression:

        1 & ((v - 1) >> 8)

    where `v` is the stored result of the successive OR operations.

- int equbyte(unsigned char x, unsigned char y);

    Strictly speaking, this one is relatively easy. Comparing a single byte is a simple case of computing the XOR of the two arguments. If they were equal, the result would be zero.

    However, we want to return a `1` for equality, and a `0` for inequality, so we must map the result as such.

    The first step is to compute the bitwise-NOT of the XOR operation. If the result was zero, this operation would return `255`, or `11111111` in binary representation (since we are working with uint8 values).

    What follows is a series of bitwise operations that reduce this value to either `1` or `0`.

    In the case of equality:

        11111111 >> 4         => 00001111
        11111111 AND 00001111 => 00001111
        00001111 >> 2         => 00000011
        00001111 AND 00000011 => 00000011
        00000011 >> 1         => 00000001
        00000011 AND 00000001 => 00000001

    In the case of inequality, the actual values may differ, but here is an example where x, y = 235, 29.

        11101011 XOR 00011011 => 11110000
        NOT 11110000          => 00001111

        00001111 >> 4         => 00000000
        00001111 AND 00000000 => 00000000
        00000000 >> 2         => 00000000
        00000000 AND 00000000 => 00000000
        00000000 >> 1         => 00000000
        00000000 AND 00000000 => 00000000

    I wrote a simple example script [2] to demonstrate this mapping. It can be played with in the browser.

- void copy(size_t length, char* x, char* y);

    The process for the copying is the same for each index in the arrays, so we will just define it for the first byte. For the sake of the example, we will set:

        x[0] = 195 => 11000011
        y[0] = 233 => 11101001

    We define two constants:

        xmask = 0     => 00000000
        ymask = NOT 0 => 11111111

    We then compute some partial values:

        xmasked = x[0] AND xmask => 00000000
        ymasked = y[0] AND ymask => 11101001

    And then set the value of x[0] to the corresponding value, y[0]:

        x[0] = xmasked OR ymasked => 11101001

    Therefore, at the end of this process, we have that:

        x[0] = y[0] = 11101001

[0] https://golang.org/pkg/crypto/subtle/
[1] https://go-review.googlesource.com/c/go/+/62770
[2] https://play.golang.org/p/Y1TfGJh9ao
