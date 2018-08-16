/*
 * hex.c - output the hex dump of arbitrary binary files.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[]) {
    // Print help message on incorrect input.
    if (argc != 2) {
        printf("Usage: ./hex /path/to/input\n");
        return 1;
    }

    // Open input and output files.
    FILE * input = fopen(argv[1], "rb"); if (!input) {
        printf("error: couldn't open %s for reading [%s]\n", argv[1], strerror(errno));
        return 1;
    }

    // Read the file 16 bytes at a time.
    size_t bytesRead = 0, offset = 0;
    unsigned char * buf = (unsigned char *) malloc(16);
    while ((bytesRead = fread(buf, 1, 16, input)) > 0) {
        // Print the offset that we are at, in octal.
        printf("%07o", (unsigned int) offset); putchar(' ');
        offset += bytesRead;

        // Print the bytes that we read, on the same line.
        for (size_t i = 0; i < bytesRead; i++) {
            printf("%02x", buf[i]);
            if (i != 15) { putchar(' '); }
        } putchar('\n');
    }

    // Output the total length, right at the end.
    printf("%07o\n", (unsigned int) offset);

    // Free memory and close files.
    free(buf);
    fclose(input);
}
