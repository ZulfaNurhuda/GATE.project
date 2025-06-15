#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Enum Definition
typedef enum {
    MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY, SUNDAY
} Days;

// Constant Declarations
const double PI = 3.14159;
const char* SITE_NAME = "NotalTranspiler";
const bool IS_FEATURE_ENABLED = true;
const Days DEFAULT_DAY = MONDAY;

// Record Definition
typedef struct {
    int id;
    char* name;
} UserRecord;

// Global Variables
Days today;
int counter;
char* message = NULL;
UserRecord user1;
UserRecord* pUser = NULL;

// Procedure Prototypes
void PrintDay(Days d);
void InitGlobals(void);
void ShowUser(void);

int main() {
    InitGlobals();
    counter = 0; message = "default_msg"; // Multi-assignment

    today = WEDNESDAY;
    PrintDay(today);
    PrintDay(DEFAULT_DAY);

    if (today == DEFAULT_DAY) {
        printf("It is indeed the default day!\n");
    }

    printf("Value of PI: %f\n", PI);
    printf("Website Name: %s\n", SITE_NAME);
    printf("Is Feature Enabled (true=1, false=0): %d\n", IS_FEATURE_ENABLED);

    user1.id = 1;
    // For user1.name, direct assignment of a string literal to a char* is acceptable in C.
    // If it were from another variable, strdup would be appropriate for a mutable string.
    user1.name = "Alice";
    pUser = &user1; // reference(user1)

    ShowUser();

    // Note: `message` points to a string literal, so it should not be freed.
    // `user1.name` also points to a string literal.
    // No dynamic allocations for these strings in this simplified main example.

    return 0;
}

void InitGlobals(void) {
    printf("--- InitGlobals called (parameterless procedure) ---\n");
}

void PrintDay(Days d) {
    printf("PrintDay received: ");
    switch (d) {
        case MONDAY: printf("Monday\n"); break;
        case TUESDAY: printf("Tuesday\n"); break;
        case WEDNESDAY: printf("Wednesday\n"); break;
        case THURSDAY: printf("Thursday\n"); break;
        case FRIDAY: printf("Friday\n"); break;
        case SATURDAY: printf("Saturday\n"); break;
        case SUNDAY: printf("Sunday\n"); break;
        default: printf("Unknown Day!\n"); break;
    }
}

void ShowUser(void) {
    printf("--- ShowUser called ---\n");
    if (pUser == NULL) {
        printf("pUser is NULL.\n");
    } else {
        // Read access using -> (from ^.)
        printf("User ID (via ^.): %d", (pUser)->id);
        printf(", User Name (via ^.): %s\n", (pUser)->name);
        // Read access using (*ptr).member (from dereference().)
        // Note: NOTAL's `dereference(pUser).id` might translate to `(*(pUser)).id`
        // or `pUser->id` if the generator optimizes/normalizes.
        // Here, we reflect the AST structure more directly for `dereference()`.
        printf("User ID (via dereference().): %d", (*(pUser)).id);
        printf(", User Name (via dereference().): %s\n", (*(pUser)).name);
    }
}
