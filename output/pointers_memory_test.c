#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>  // For malloc, realloc, free
#include <string.h>  // For string functions (though not strictly needed by this specific C output)

int main() {
    int num;
    int* pNum = NULL;
    int* pAllocatedNum = NULL;
    int* pGeneric = NULL;

    printf("--- Basic Pointer Test ---\n");
    num = 10;
    printf("num initial: %d", num);

    pNum = &num; // Corresponds to reference(num)
    printf(", Value via pNum: %d", (*pNum)); // Corresponds to dereference(pNum)

    (*pNum) = 20; // Corresponds to dereference(pNum) <- 20
    printf(", num after update: %d", num);
    printf(", pNum after update: %d\n", (*pNum));
    printf("---\n");

    printf("--- Dynamic Allocation Test (Integer) ---\n");
    // allocate(4) -> malloc(4)
    // C standard: malloc takes size_t. Integer literal 4 is fine.
    pAllocatedNum = (int*)malloc(4); // Explicit cast often used, though void* auto-converts in C.
                                     // Assuming generator adds cast for clarity or C++ compatibility.

    if (pAllocatedNum == NULL) { // Comparison with NULL
        printf("Integer allocation failed!\n");
    } else {
        (*(pAllocatedNum)) = 123; // dereference(pAllocatedNum) <- 123
        printf("Allocated value: %d\n", (*(pAllocatedNum)));

        // Reallocate example (C code if NOTAL part was uncommented):
        // int* temp_ptr_realloc = (int*)realloc(pAllocatedNum, 8);
        // if (temp_ptr_realloc == NULL && 8 > 0) {
        //     printf("Reallocation failed!\n");
        // } else {
        //     pAllocatedNum = temp_ptr_realloc;
        //     if (pAllocatedNum != NULL) {
        //        printf("Reallocated value (if successful): %d\n", (*(pAllocatedNum)));
        //     } else {
        //        printf("Reallocated to size 0, pointer is NULL.\n");
        //     }
        // }

        free(pAllocatedNum); // deallocate(pAllocatedNum)
        pAllocatedNum = NULL;
    }
    printf("---\n");

    printf("--- Dispose Test (as alias for deallocate) ---\n");
    pGeneric = (int*)malloc(4); // allocate(4)
    if (pGeneric == NULL) {
        printf("Generic allocation failed!\n");
    } else {
        (*(pGeneric)) = 456; // dereference(pGeneric) <- 456
        printf("Generic allocated value: %d\n", (*(pGeneric)));
        free(pGeneric); // dispose(pGeneric) maps to deallocate which maps to free
        pGeneric = NULL;
    }
    printf("---\n");

    return 0;
}
