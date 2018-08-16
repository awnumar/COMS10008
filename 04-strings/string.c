/* Custom versions of standard string functions. */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

// Find the length of a string, assuming it has a null terminator (like strlen).
int length(const char s[]) {
    for (int i = 0; true; i++) {
        if (s[i] == 0x00) { return i; }
    }
}

// Copy string s into the character array t (like strcpy).
void copy(char t[], const char s[]) {
    for (int i = 0; i < length(s) + 1; i++) {
        t[i] = s[i];
    }
}

// Compare two strings, returning negative, zero or positive (like strcmp).
int compare(const char s[], const char t[]) {
    for (int i = 0; i < length(s) + 1; i++) {
        if (s[i] == 0x00) {
            if (t[i] != 0x00) {
                return -1;
            }
            return 0;
        }
        if (s[i] > t[i]) { return 1; }
        if (s[i] < t[i]) { return -1; }
    }
    return 0; // Unreachable code.
}

// Join string s to the end of string t (like strcat).
void append(char t[], const char s[]) {
    int lt = length(t); int ls = length(s);
    copy(&t[lt], s); t[lt+ls] = 0x00;
}

// Find the (first) position of s in t, or return -1 (like strstr).
int find(const char t[], const char s[]) {
    for (int i = 0; i < length(t); i++) {
        unsigned char result = 0;
        for (int j = 0; j < length(s); j++) {
            result |= t[i+j] ^ s[j];
        }
        if (result == 0) { return i; }
    }
    return -1;
}

// -----------------------------------------------------------------------------
// Tests and user interface

// Tests 1 to 5
void testLength() {
    assert(length("") == 0);
    assert(length("c") == 1);
    assert(length("ca") == 2);
    assert(length("cat") == 3);
    char s[] = "dog";
    assert(length(s) == 3);
}

// Tests 6 to 9
void testCopy() {
    char t[10];
    copy(t, "cat");
    assert(t[0] == 'c' && t[1] =='a' && t[2] == 't' && t[3] =='\0');
    copy(t, "at");
    assert(t[0] == 'a' && t[1] =='t' && t[2] =='\0');
    copy(t, "t");
    assert(t[0] == 't' && t[1] =='\0');
    copy(t, "");
    assert(t[0] == '\0');
}

// Tests 10 to 17
void testCompare() {
    assert(compare("cat", "dog") < 0);
    assert(compare("dog", "cat") > 0);
    assert(compare("cat", "cat") == 0);
    assert(compare("an", "ant") < 0);
    assert(compare("ant", "an") > 0);
    assert(compare("", "a") < 0);
    assert(compare("a", "") > 0);
    assert(compare("", "") == 0);
}

// Tests 18 to 20
void testAppend() {
    char t[10] = "cat";
    append(t, "");
    assert(t[0] == 'c' && t[1] =='a' && t[2] == 't' && t[3] =='\0');
    t[2] = '\0';
    append(t, "t");
    assert(t[0] == 'c' && t[1] =='a' && t[2] == 't' && t[3] =='\0');
    t[0] = '\0';
    append(t, "cat");
    assert(t[0] == 'c' && t[1] =='a' && t[2] == 't' && t[3] =='\0');
}

// Tests 21 to 25
void testFind() {
    assert(find("cat", "cat") == 0);
    assert(find("cat", "c") == 0);
    assert(find("cat", "t") == 2);
    assert(find("cat", "x") == -1);
    assert(find("banana", "an") == 1);
}

// Test the functions.
int main() {
    testLength();
    testCopy();
    testCompare();
    testAppend();
    testFind();
    printf("All tests passed.\n");
    return 0;
}
