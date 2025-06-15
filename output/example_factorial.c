#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// Forward declaration for the recursive function
int FactorialRecursive(int n);

int main() {
    int num;
    int i;
    int fact;

    printf("Enter a non-negative integer:");
    scanf("%d", &num);

    if (num < 0) {
        printf("Factorial is not defined for negative numbers.\n");
    } else {
        // Using the recursive function
        fact = FactorialRecursive(num);
        printf("Factorial of %d (recursive) is: %d\n", num, fact);

        // Iterative calculation
        if (num == 0) { // AN '=' for comparison becomes C '=='
            fact = 1;
        } else {
            fact = 1;
            for (i = 1; i <= num; i++) {
                fact = fact * i;
            }
        }
        printf("Factorial of %d (iterative) is: %d\n", num, fact);
    }
    return 0;
}

// Implementation of the recursive function
int FactorialRecursive(int n) {
    // No KAMUS LOKAL needed
    if (n == 0) { // AN '=' for comparison becomes C '=='
        return 1;
    } else {
        return n * FactorialRecursive(n - 1);
    }
}
