#include <stdio.h>
#include <stdbool.h> // For bool, though not directly used by user vars here
#include <string.h>
#include <stdlib.h>
#include <math.h>   // Included by default by current generator

int main() {
    int n;
    int i; // Loop counter

    printf("Enter a number (n):");
    scanf("%d", &n);

    if (n > 0) {
        for (i = 1; i <= n; i++) {
            printf("Hello\n");
        }
    } else {
        printf("N is not positive, no Hellos for you!\n");
    }

    return 0;
}
