/*
 * sketch.c - simple graphics engine
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "display.h"

// Define global opcode constants.
const int DX = 0; // Change x position.
const int DY = 1; // Change y position.
const int DT = 2; // Pause instruction.
const int PN = 3; // Toggle pen states. // Also used as extension opcode.
const int CL = 4; // Clear the display.
const int KY = 5; // Wait for key down.
const int HU = 6; // Change draw color.

// Define a structure for the current state.
struct state {
    int sx; // Stored x-coordinate of cursor.
    int sy; // Stored y-coordinate of cursor.

    int cx; // Current x-coordinate of cursor.
    int cy; // Current y-coordinate of cursor.

    bool PD; // Is the pen down?
};
typedef struct state state;

// Define global objects.
FILE * input;
display * D;
state * S;

// Declare function signatures.
void dx(int operand);
void dy(int operand);
void dt(int operand);
void pn(); void cl(); void ky();
void hu(unsigned char operands[4]);
int bytes_to_int(int n, unsigned char bytes[4]);

int main(int argc, char * argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./sketch [/path/to/file.sketch]\n");
        return 1;
    }

    // Open the file for reading.
    FILE * input = fopen(argv[1], "rb"); if (!input) {
        fprintf(stderr, "error: %s\n", strerror(errno));
        return 1;
    }

    // Initialise a state instance.
    S = (state *) malloc(sizeof(state));
    S->sx = 0; S->sy = 0; S->cx = 0; S->cy = 0; S->PD = false;

    // Initialise a new graphical display.
    D = newDisplay(argv[1], 200, 200);

    // Read the file a single byte at a time.
    int byte; while ((byte = getc(input)) != EOF) {
        // Check validity of input.
        int opcode = byte >> 6;
        int operand = byte & ~(0x3 << 6);

        // Handle each opcode case.
        if (opcode == PN) {
            // Extension parsing triggered; compute new opcode.
            opcode = operand & ~(0xF << 4);

            // Compute new operand bytes.
            unsigned char operand_bytes[4] = {0};
            int n = (operand >> 4) & 0x3;
            if (n == 3) { n = 4; }
            if (n != 0) {
                for (int i = 0; i < n; i++) {
                    operand_bytes[i] = getc(input);
                }
            }
            operand = bytes_to_int(n, operand_bytes);

            // Handle each embedded opcode case.
            if (opcode == DX) {
                // If negative, compute the two's complement.
                dx(operand);
            } else if (opcode == DY) {
                dy(operand);
            } else if (opcode == DT) {
                dt(operand);
            } else if (opcode == PN) {
                pn();
            } else if (opcode == CL) {
                cl();
            } else if (opcode == KY) {
                ky();
            } else if (opcode == HU) {
                hu(operand_bytes);
            } else {
                fprintf(stderr, "error: unknown opcode\n");
                return 1;
            }
        } else if (opcode == DX) {
            if (operand >> 5 == 1) {
                dx((int)(~operand ^ 0x3F));
            } else {
                dx(operand);
            }
        } else if (opcode == DY) {
            if (operand >> 5 == 1) {
                dy((int)(~operand ^ 0x3F));
            } else {
                dy(operand);
            }
        } else if (opcode == DT) {
            dt(operand);
        } else {
            fprintf(stderr, "error: unknown opcode\n");
            return 1;
        }
    }

    // Close the file descriptor.
    fclose(input);

    // End it all.
    end(D);
}

/*
 * Functions that directly execute given instructions.
 *****************************************************/

void dx(int operand) {
    S->cx += operand;
}

void dy(int operand) {
    S->cy += operand;
    if (S->PD) {
        line(D, S->sx, S->sy, S->cx, S->cy);
    }
    S->sx = S->cx; S->sy = S->cy;
}

void dt(int operand) {
    pause(D, operand * 10);
}

void pn() {
    S->PD = !S->PD;
}

void cl() {
    clear(D);
}

void ky() {
    key(D);
}

void hu(unsigned char operands[4]) {
    // Pack the colour into an integer.
    int rgba = 0;
    rgba |= ((int) operands[0]) << 24;
    rgba |= ((int) operands[1]) << 16;
    rgba |= ((int) operands[2]) << 8;
    rgba |= (int) operands[3];

    // Execute the instruction.
    colour(D, rgba);
}

/*****************************************************/

int bytes_to_int(int n, unsigned char bytes[4]) {
    if (n == 0) {
        return 0;
    } else if (n == 1) {
        return (int) bytes[0];
    } else if (n == 2) {
        uint32_t operand = 0;
        operand |= ((uint32_t) bytes[0]) << 8;
        operand |= (uint32_t) bytes[1];
        if (operand >> 15 == 1) {
            operand = ~operand ^ 0xFFFF;
        }
        return operand;
    } else if (n == 3 || n == 4) {
        uint32_t operand = 0;
        operand |= ((uint32_t) bytes[0]) << 24;
        operand |= ((uint32_t) bytes[1]) << 16;
        operand |= ((uint32_t) bytes[2]) << 8;
        operand |= (uint32_t) bytes[3];
        if (operand >> 31 == 1) {
            operand = ~operand ^ 0xFFFFFFFF;
        }
        return operand;
    } else {
        fprintf(stderr, "error: impossible n value [%d]\n", n);
        exit(1);
    }
}
