/*
 * This program takes three integers corresponding to the sides of a triangle,
 * and returns the type of the inputted triangle.
 *
 * compile: gcc -std=c99 -Wall -O3 -pedantic ./triangle.c -o ./triangle
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Define the triangle constants, and associated names, globally.
enum Triangle { Equilateral, Isosceles, Right, Scalene, Flat, Impossible, Illegal };
const char *TriangleNames[7] = {
    "Equilateral",
    "Isosceles",
    "Right",
    "Scalene",
    "Flat",
    "Impossible",
    "Illegal"
};

// Declare function signatures.
enum Triangle triangle(const char* a, const char* b, const char* c);
void test();

// Parse input and execute the appropriate code.
int main(int argc, char *argv[]) {
    if (argc == 1) {
        // No arguments. Run tests.
        test();
    } else if (argc == 4) {
        // Received an input! Output the corresponding triangle.
        printf("%s\n", TriangleNames[triangle(argv[1], argv[2], argv[3])]);
    } else {
        // Too many inputs. Print a help message to stderr.
        fprintf(stderr, "Usage: ./triangle [int side_1, side_2, side_3]\n");
        return 1;
    }

    // Everything went well.
    return 0;
}

// Checks if an argument is valid. Returns zero for valid
// lengths and one for invalid lengths.
int isValid(const char* length) {
    // Eliminate non-integers and negatives.
    for (unsigned int i = 0; i < strlen(length); i++) {
        if (isdigit(length[i]) == 0) {
            return 1;
        }
    }

    // Eliminate values with redundant zeroes.
    // Also eliminates zero values.
    if (length[0] == 48) {
        return 1;
    }

    // Valid triangle.
    return 0;
}

// triangle() takes three arguments corresponding to the three sides of a
// triangle, and it returns the type of that triangle after classification.
enum Triangle triangle(const char* a, const char* b, const char* c) {
    // Eliminate illegal lengths.
    if ((isValid(a) | isValid(b) | isValid(c)) == 1) {
        return Illegal;
    }

    // Convert the inputs to numeric types.
    unsigned long long an = strtoll(a, NULL, 10);
    unsigned long long bn = strtoll(b, NULL, 10);
    unsigned long long cn = strtoll(c, NULL, 10);

    // Eliminate numbers larger than the max allowed.
    if (an > 2147483647 ||
        bn > 2147483647 ||
        cn > 2147483647) {
        return Illegal;
    }

    // Classify the inputs.
    if (an + bn < cn ||
        bn + cn < an ||
        cn + an < bn) {
        return Impossible;
    } else if (an + bn == cn ||
               bn + cn == an ||
               an + cn == bn) {
        return Flat;
    } else if (an == bn &&
               bn == cn) {
        return Equilateral;
    } else if (an == bn ||
               bn == cn ||
               an == cn) {
        return Isosceles;
    } else if (an * an == (bn * bn) + (cn * cn) ||
               bn * bn == (an * an) + (cn * cn) ||
               cn * cn == (an * an) + (bn * bn)) {
        return Right;
    } else if (an != bn &&
               bn != cn &&
               an != cn) {
        return Scalene;
    } else {
        // This should never happen.
        // Included because I can't prove that it will never happen.
        return Impossible;
    }
}

// Run tests on the triangle function.
void test() {
    // Check that you haven't changed the triangle type constants.  (If you do, it
    // spoils automatic marking, when your program is linked with a test program.)
    assert(Equilateral==0 && Isosceles==1 && Right==2 && Scalene==3);
    assert(Flat==4 && Impossible==5 && Illegal==6);

    // Test the mapping of the TriangleNames to the Triangles.
    assert(strcmp(TriangleNames[Equilateral], "Equilateral") == 0);
    assert(strcmp(TriangleNames[Isosceles], "Isosceles") == 0);
    assert(strcmp(TriangleNames[Right], "Right") == 0);
    assert(strcmp(TriangleNames[Scalene], "Scalene") == 0);
    assert(strcmp(TriangleNames[Flat], "Flat") == 0);
    assert(strcmp(TriangleNames[Impossible], "Impossible") == 0);
    assert(strcmp(TriangleNames[Illegal], "Illegal") == 0);

    // Tests 1 to 2: check equilateral
    assert(triangle("8", "8", "8") == Equilateral);
    assert(triangle("1073", "1073", "1073") == Equilateral);

    // Tests 3 to 5: check isosceles
    assert(triangle("25", "25", "27") == Isosceles);
    assert(triangle("25", "27", "25") == Isosceles);
    assert(triangle("27", "25", "25") == Isosceles);

    // Tests 6 to 14: check right angled
    assert(triangle("3", "4", "5") == Right);
    assert(triangle("3", "5", "4") == Right);
    assert(triangle("5", "3", "4") == Right);
    assert(triangle("5", "12", "13") == Right);
    assert(triangle("5", "13", "12") == Right);
    assert(triangle("12", "5", "13") == Right);
    assert(triangle("12", "13", "5") == Right);
    assert(triangle("13", "5", "12") == Right);
    assert(triangle("13", "12", "5") == Right);

    // Tests 15 to 20: check scalene
    assert(triangle("12", "14", "15") == Scalene);
    assert(triangle("12", "15", "14") == Scalene);
    assert(triangle("14", "12", "15") == Scalene);
    assert(triangle("14", "15", "12") == Scalene);
    assert(triangle("15", "12", "14") == Scalene);
    assert(triangle("15", "14", "12") == Scalene);

    // Tests 21 to 25: check flat
    assert(triangle("7", "9", "16") == Flat);
    assert(triangle("7", "16", "9") == Flat);
    assert(triangle("9", "16", "7") == Flat);
    assert(triangle("16", "7", "9") == Flat);
    assert(triangle("16", "9", "7") == Flat);

    // Tests 26 to 31: check impossible
    assert(triangle("2", "3", "13") == Impossible);
    assert(triangle("2", "13", "3") == Impossible);
    assert(triangle("3", "2", "13") == Impossible);
    assert(triangle("3", "13", "2") == Impossible);
    assert(triangle("13", "2", "3") == Impossible);
    assert(triangle("13", "3", "2") == Impossible);

    // Tests 32 to 42: check illegal
    assert(triangle("0", "0", "0") == Illegal);
    assert(triangle("0", "10", "12") == Illegal);
    assert(triangle("10", "12", "0") == Illegal);
    assert(triangle("-1", "-1", "-1") == Illegal);
    assert(triangle("-1", "10", "12") == Illegal);
    assert(triangle("10", "-1", "12") == Illegal);
    assert(triangle("10", "12", "-1") == Illegal);
    assert(triangle("x", "y", "z") == Illegal);
    assert(triangle("3", "4y", "5") == Illegal);
    assert(triangle("10", "12", "13.4") == Illegal);
    assert(triangle("03", "4", "5") == Illegal);

    // Tests 43 to 47: check the upper limits on lengths
    assert(triangle("2147483647","2147483647","2147483647") == Equilateral);
    assert(triangle("2147483648","2147483647","2147483647") == Illegal);
    assert(triangle("2147483647","2147483648","2147483647") == Illegal);
    assert(triangle("2147483647","2147483647","2147483648") == Illegal);
    assert(triangle("2147483649","2147483649","2147483649") == Illegal);

    // Tests 48 to 50: check for correct handling of overflow
    assert(triangle("1100000000","1705032704","1805032704") == Scalene);
    assert(triangle("2000000001","2000000002","2000000003") == Scalene);
    assert(triangle("150000002","666666671","683333338") == Scalene);

    // All tests passed.
    printf("All tests passed\n");
}
