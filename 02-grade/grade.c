/*
 * A simple program to calculate a discreet grade value from on a numeric input.
 *
 * This program takes an optional, numerical command line argument.
 * It will output a discreet grade value for the given input.
 * If there are no arguments passed to the program, it will run tests
 * instead.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

// Define the grade constants, and associated names, globally.
enum Grade { First, UpperSecond, LowerSecond, Third, Fail, Invalid };
const char *GradeNames[6] = {
    "First",
    "Upper second",
    "Lower second",
    "Third",
    "Fail",
    "Invalid"
};

// Declare function signatures for any functions used below.
enum Grade getGrade(const char *mark); // Computes grade value.
void test(); // Tests program.

int main(int argc, char *argv[]) {
    if (argc == 1) {
        // No arguments. Run tests.
        test();
    } else if (argc == 2) {
        // Received an input! Output the corresponding grade.
        printf("%s\n", GradeNames[getGrade(argv[1])]);
    } else {
        // Too many inputs. Print a help message.
        printf("Usage: ./grade [mark]\n");
        return 1;
    }

    // Everything went well.
    return 0;
}

// getGrade takes a string representing a mark
// and returns the corresponding grade value.
enum Grade getGrade(const char *mark) {
    // Eliminate non-integers and negatives.
    for (unsigned int i = 0; i < strlen(mark); i++) {
        if (isdigit(mark[i]) == 0) {
            return Invalid;
        }
    }

    // Eliminate values with redundant zeroes.
    if (mark[0] == 48 && strlen(mark) != 1) {
        return Invalid;
    }

    // Convert the string to an integer. This is guaranteed to succeed.
    int markVal = strtol(mark, NULL, 10);

    // Compute the grade value.
    if (markVal >= 70 && markVal <= 100) {
        return First;
    } else if (markVal >= 60 && markVal <= 69) {
        return UpperSecond;
    } else if (markVal >= 50 && markVal <= 59) {
        return LowerSecond;
    } else if (markVal >= 40 && markVal <= 49) {
        return Third;
    } else if (markVal >= 0 && markVal <= 39) {
        return Fail;
    } else {
        return Invalid;
    }
}

// This function checks the implementation against the specification.
// If there are any invalid results, it will return an error and quit.
void test() {
    // Test the mapping of the GradeNames to the Grades.
    assert(strcmp(GradeNames[First], "First") == 0);
    assert(strcmp(GradeNames[UpperSecond], "Upper second") == 0);
    assert(strcmp(GradeNames[LowerSecond], "Lower second") == 0);
    assert(strcmp(GradeNames[Third], "Third") == 0);
    assert(strcmp(GradeNames[Fail], "Fail") == 0);
    assert(strcmp(GradeNames[Invalid], "Invalid") == 0);

    // Test each grade (tests 1 to 5)
    assert(getGrade("35") == Fail);
    assert(getGrade("45") == Third);
    assert(getGrade("55") == LowerSecond);
    assert(getGrade("65") == UpperSecond);
    assert(getGrade("75") == First);

    // Test the boundary points of the grades (tests 6 to 15)
    assert(getGrade("0") == Fail);
    assert(getGrade("39") == Fail);
    assert(getGrade("40") == Third);
    assert(getGrade("49") == Third);
    assert(getGrade("50") == LowerSecond);
    assert(getGrade("59") == LowerSecond);
    assert(getGrade("60") == UpperSecond);
    assert(getGrade("69") == UpperSecond);
    assert(getGrade("70") == First);
    assert(getGrade("100") == First);

    // Test that grade only accepts valid marks, and rejects non-digits, numbers
    // outside the range 0..100, and unnecessary leading zeros (tests 16 to 25)
    assert(getGrade("40.5") == Invalid);
    assert(getGrade("x") == Invalid);
    assert(getGrade("-1") == Invalid);
    assert(getGrade("40x5") == Invalid);
    assert(getGrade(" 40") == Invalid);
    assert(getGrade("40 ") == Invalid);
    assert(getGrade("101") == Invalid);
    assert(getGrade("547") == Invalid);
    assert(getGrade("01") == Invalid);
    assert(getGrade("040") == Invalid);

    // No errors, everything went well.
    printf("All tests pass\n");
}
