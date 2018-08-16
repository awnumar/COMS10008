/*
 * upper.c - program to convert a file to uppercase
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char * argv[]) {
    // Print help message on incorrect input.
    if (argc != 3) {
        printf("Usage: ./upper /path/to/input /path/to/output\n");
        return 1;
    }

    // Open input and output files.
    FILE * input = fopen(argv[1], "rb"); if (!input) {
        printf("error: couldn't open %s for reading [%s]\n", argv[1], strerror(errno));
        return 1;
    }
    FILE * output = fopen(argv[2], "wb"); if (!output) {
        printf("error: couldn't open %s for writing [%s]\n", argv[2], strerror(errno));
        return 1;
    }

    // Read input in a single byte at a time.
    int b; // Store the character here.
    while ((b = getc(input)) != EOF) {
        // Convert alphabetic characters to their uppercase counterparts.
        if (b >= 'a' && b <= 'z') {
            b -= 32;
        }

        // Write the character to the output file.
        putc(b, output);
    }

    // Close the files.
    fclose(input);
    fclose(output);
}
