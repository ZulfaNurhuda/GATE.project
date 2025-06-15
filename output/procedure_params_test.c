#include <stdio.h>
#include <stdbool.h> // For bool, if used
#include <string.h>  // For strdup, strcpy if used by generator
#include <stdlib.h>  // For malloc, free

// Forward declarations for procedures
void ProcessNumbers(int a, int* b, int* c);
void ProcessStrings(char* s1, char** s2, char** s3); // s3 as char** for IN_OUT string

int main() {
    int num_in;
    int num_out;
    int num_io;
    char* str_in = NULL;
    char* str_out = NULL;
    char* str_io = NULL;

    num_in = 10;
    num_io = 20;

    // String literals assigned to char*
    // In a real scenario with dynamic strings, strdup might be used for str_in too.
    // For this example, str_in is a literal.
    str_in = "hello_in";
    // str_io is initialized then its content is effectively replaced by ProcessStrings.
    // The generator for `s3 <- "new_string_literal"` for a `char** s3` param will do `*s3 = strdup("new_string_literal")`
    // after potentially freeing `*s3`. So, initial value should be on heap to be safely freeable.
    str_io = strdup("world_io_initial");
    if (str_io == NULL) { fprintf(stderr, "Failed to allocate str_io\n"); return 1; }


    printf("Initial values: num_in: %d, num_io: %d str_in: %s, str_io: %s\n", num_in, num_io, str_in, str_io);

    ProcessNumbers(num_in, &num_out, &num_io);

    printf("Values after ProcessNumbers: num_out: %d, num_io: %d\n", num_out, num_io);

    ProcessStrings(str_in, &str_out, &str_io);

    printf("Values after ProcessStrings: str_out: %s, str_io: %s\n", str_out, str_io);

    // Corresponds to dispose(str_out)
    if (str_out) {
        free(str_out);
        str_out = NULL;
    }
    // Corresponds to dispose(str_io)
    if (str_io) {
        free(str_io);
        str_io = NULL;
    }
    // str_in is a string literal, not allocated on heap in this example, so no free.

    return 0;
}

void ProcessNumbers(int a, int* b, int* c) {
    // b is 'output integer' (int* b)
    *b = a * 2;
    // c is 'input/output integer' (int* c)
    *c = *c * 2;
}

void ProcessStrings(char* s1, char** s2, char** s3) {
    // s1 is 'input string' (char* s1)

    // s2 is 'output string' (char** s2)
    // Callee allocates memory for *s2. Free old *s2 if it's not NULL.
    if (*s2 != NULL) {
        free(*s2);
        *s2 = NULL;
    }
    *s2 = strdup("processed_output");
    if (*s2 == NULL) {
        fprintf(stderr, "Failed to allocate for s2 in ProcessStrings\n");
        // Potentially handle error, e.g. by setting *s2 to NULL or exiting
    }

    // s3 is 'input/output string' (char** s3)
    // Callee can change where caller's pointer *s3 points.
    // Free old *s3 if it's not NULL before reassigning.
    if (*s3 != NULL) {
        free(*s3);
        *s3 = NULL;
    }
    *s3 = strdup("modified_io");
    if (*s3 == NULL) {
        fprintf(stderr, "Failed to allocate for s3 in ProcessStrings\n");
        // Potentially handle error
    }
}
