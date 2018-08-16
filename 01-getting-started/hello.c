/*
 * This is a basic FizzBuzz solution.
 */

#include <stdio.h>
#include <stdbool.h>

int main() {
  for (int i = 0; true; i++) {
    // Handle the case where none are factors.
    if (i % 3 != 0 && i % 5 != 0) {
      printf("%d\n", i); // Include newline.
      continue; // No need to carry on.
    }

    // Handle the case where 3 is a factor.
    if (i % 3 == 0) {
      printf("%s", "Fizz"); // No newline.
    }

    // Handle the case where 5 is a factor.
    if (i % 5 == 0) {
      printf("%s", "Buzz"); // No newline.
    }

    // Print the missing newline.
    printf("%s", "\n");
  }

  return 0;
}
