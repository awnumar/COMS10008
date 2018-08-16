/*
 * reverse.c - interactive string reversal program
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[]) {
    for (;;) {
        // Prompt the user.
        printf("> ");

        // Create a buffer for storing the input, and the length of the input.
        size_t n = 1024; char * buf = (char *) malloc(n);

        // Get a single line from standard input.
        ssize_t read = getline(&buf, &n, stdin);
        if (read == -1) {
            // Probably an EOF. Alternative is a malloc err, which is unlikely.
            return 0;
        }

        // Did the user want to exit?
        if (strcmp(buf, ".\n") == 0) {
            return 0;
        }

        // Print the reversal of the input.
        for (int i = read-2; i >= 0; --i) {
            putchar(buf[i]);
        }; putchar('\n');

        // Free the buffer that we created.
        free(buf);
    }
}
