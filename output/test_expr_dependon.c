#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

int main() {
    int a;
    int b;
    int c;
    int result;
    int choice;
    bool isReady;

    a = 10;
    b = 5;
    c = 2;

    result = a + b * c;
    printf("Result of a + b * c: %d\n", result);

    result = (a + b) * c;
    printf("Result of (a + b) * c: %d\n", result);

    isReady = (a > b) && (c < b) || (a == 10);
    printf("isReady: %d\n", isReady);

    printf("Enter a choice (1, 2, or 3):\n");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            printf("You chose one.\n");
            break;
        case 2:
            printf("You chose two.\n");
            break;
        case 3:
            result = choice * 100;
            printf("You chose three, result is now: %d\n", result);
            break;
        default:
            printf("Invalid choice.\n");
            break;
    }
    return 0;
}
