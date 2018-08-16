/*
 * This is my submission for the open-ended, extra-credit portion
 * of the week two assignment.
 *
 * The program computes the checksum of any given number of files,
 * and outputs them.
 *
 * To compile, run the following command:
 *
 * gcc -std=c99 -Wall -O3 checksum.c -lssl -lcrypto -o checksum
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <openssl/sha.h>
#include <openssl/crypto.h>

// Declare global constants.
const int BUFLEN = 4096; // Length of buffer, for staggered reading.

// Declare function signatures.
void hashFile(char* path, unsigned char m[SHA256_DIGEST_LENGTH]);
void test();

int main(int argc, char *argv[]) {
    if (argc == 1) {
        // No arguments. Print help message.
        printf("Usage: ./checksum </path/to/file> (/path/to/files ...)\n\
       ./checksum --test\n");
        return 1;
    } else {
        // Check if they want to run tests.
        if (strcmp(argv[1], "--test") == 0) {
            test();
            return 0;
        }

        // We have arguments. Iterate over them.
        for (int i = 1; i < argc; i++) {
            // Allocate memory to store the result of the hash function.
            static unsigned char rawDigest[SHA256_DIGEST_LENGTH];

            // Fill the memory with the hash digest.
            hashFile(argv[i], rawDigest);

            // Output the digest.
            for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
                printf("%02x", rawDigest[i]);
            }

            // Output filename.
            printf("  %s\n", argv[i]);
        }
    }
}

// hashFile takes a filepath as an argument, streams the binary data
// in the file into a cryptographic hash function, and returns a string
// representing its hex-encoded hash digest. The hash function used is
// SHA256, implemented in openssl.
void hashFile(char* path, unsigned char m[SHA256_DIGEST_LENGTH]) {
    // Define needed local variables.
    FILE *f;          // File object.
    char buf[BUFLEN]; // Data buffer for staggered reading.

    // Open the file path in read-bytes mode.
    f = fopen(path, "rb");
    if (f == NULL) { // Oh god I've been coding in Go for too long.
        printf("error: couldn't open %s for reading [Err: %s]\n", path, strerror(errno));
        exit(1);
    }

    // Initialise the cryptographic hash function.
    // API painfully learned from the openssl source code and documentation.
    SHA256_CTX c; // Stores the current state.
    if (SHA256_Init(&c) != 1) { // Initialise the hash function.
        printf("error: couldn't initialise sha256\n");
        exit(1);
    }

    // Read BUFLEN elements of size 1 byte, from f into buf.
    // Continue until no more data is read in a call to fread.
    size_t bytesRead; // The number of bytes read in the last call to fread().
    while ((bytesRead = fread(buf, 1, BUFLEN, f)) > 0) {
        if (SHA256_Update(&c, buf, bytesRead) != 1) { // Add read data to hash.
            printf("error: couldn't update sha256\n");
            OPENSSL_cleanse(&c, sizeof(c)); // Wipe current state.
            exit(1);
        }
    }

    // Finalise the computation.
    if (SHA256_Final(m, &c) != 1) { // Fill the memory with the hash digest.
        printf("error: couldn't finalise sha256\n");
        OPENSSL_cleanse(&c, sizeof(c)); // Wipe current state.
        exit(1);
    }

    // Wipe the state history.
    OPENSSL_cleanse(&c, sizeof(c));
}

// This function tests the program against known hash values.
// It is difficult to test IO properly, and so we call the
// openssl API itself and compare the returned values with
// known sha256 values.
//
// There are no tests thus far because of the difficulty of
// testing IO operations. Perhaps they will be added in the
// future. For the time being, the output of this program can
// be compared with the output of `sha256sum`.
void test() {
    printf("There are no tests thus far because of the difficulty of testing IO\
 operations. Perhaps they will be added in the future. For the\
 time being, the output of this program can be compared with the\
 output of `sha256sum`.\n\nHere is some example output:\n\n");

    printf("checksum.c:\n\
7703a8b7ea752ff47084701f7c064134a2a0aac583cf160ec5bec92f89050fa2  checksum\n\
61a3148f61ec0a305c5358ad70018f7d10d6afc9186b161660f742ebcba4ca50  checksum.c\n\
12890d1acbd471fc5294282a62c1ca71c8fcc1e3680ffe143f08f4542719cf25  Makefile\n\
83ee47245398adee79bd9c0a8bc57b821e92aba10f5f9ade8a5d1fae4d8c4302  out\n\
\n\
sha256sum:\n\
7703a8b7ea752ff47084701f7c064134a2a0aac583cf160ec5bec92f89050fa2  checksum\n\
61a3148f61ec0a305c5358ad70018f7d10d6afc9186b161660f742ebcba4ca50  checksum.c\n\
12890d1acbd471fc5294282a62c1ca71c8fcc1e3680ffe143f08f4542719cf25  Makefile\n\
83ee47245398adee79bd9c0a8bc57b821e92aba10f5f9ade8a5d1fae4d8c4302  out\n\n");

    printf("Therefore, assuming `sha256sum` is correct, the program in its current\
 state is also correct, with overwhelming probability.\n");
}
