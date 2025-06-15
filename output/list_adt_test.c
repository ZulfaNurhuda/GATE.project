#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>  // For malloc, free, NULL
#include <string.h>  // For string functions if any were used

// User-defined List ADT structure
typedef struct List List; // Forward declaration for typedef name
struct List {
    int value;
    List* next; // Self-referential pointer using the typedef'd name
};

// Global variables (assuming KAMUS translates to global scope for main program)
List* myListHead = NULL;
List* newNode = NULL;
List* tempPtr = NULL;
int i;
List* nodeToFree = NULL; // Corresponds to hoisted 'declare nodeToFree'

int main(void) {
    printf("--- User-Defined List ADT Test ---\n");

    myListHead = NULL;

    for (i = 1; i <= 2; i++) {
        // Assuming allocate(16) -> (List*)malloc(sizeof(List)) if generator is smart,
        // or (List*)malloc(16) if it takes the size literally.
        // The prompt implies literal size, but sizeof(List) is better C.
        // For this manual C generation, let's use sizeof(List).
        newNode = (List*)malloc(sizeof(List));
        if (newNode == NULL) {
            printf("Memory allocation failed for newNode!\n");
        } else {
            if (i == 1) {
                (*(newNode)).value = 10; // dereference(newNode).value <- 10
            } else {
                (*(newNode)).value = 20; // dereference(newNode).value <- 20
            }
            (*(newNode)).next = myListHead; // dereference(newNode).next <- myListHead
            myListHead = newNode;
        }
    }

    printf("List content (should be 20 -> 10):\n");
    tempPtr = myListHead;
    while (tempPtr != NULL) { // tempPtr <> NULL
        printf("Value: %d\n", (tempPtr)->value); // tempPtr^.value
        tempPtr = (tempPtr)->next;               // tempPtr <- tempPtr^.next
    }

    printf("Deallocating list...\n");
    tempPtr = myListHead;
    while (tempPtr != NULL) {
        // nodeToFree is already declared at the top of main's scope
        nodeToFree = tempPtr;
        tempPtr = (tempPtr)->next; // tempPtr <- tempPtr^.next
        free(nodeToFree);          // deallocate(nodeToFree)
    }
    myListHead = NULL;

    printf("List deallocated.\n");
    printf("--- Test Complete ---\n");
    return 0;
}
