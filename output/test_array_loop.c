#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

int main() {
    int myNumbers[5]; /* AN: array [1..5] of integer */
    int i;
    int temp;

    // Initialize array
    for (i = 1; i <= 5; i++) {
        myNumbers[i - 1] = i * 10;
    }

    // Output array elements
    printf("Array elements:\n");
    for (i = 1; i <= 5; i++) {
        printf("Element %d: %d\n", i, myNumbers[i - 1]);
    }

    // Test input to an array element
    printf("Enter a new value for element 3:");
    scanf("%d", &myNumbers[3 - 1]);
    printf("New element 3 is: %d\n", myNumbers[3 - 1]);

    return 0;
}
