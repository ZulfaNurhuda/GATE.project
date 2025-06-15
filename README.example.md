# Notasi Algoritmik (NOTAL) to C Transpiler (GATE - Ganesha Algorithmic Transpiler Engine)

**Version:** 1.0 (Based on initial brief v3.0)

## 1. Project Objective

To develop a production-quality, command-line compiler (transpiler) that correctly, efficiently, and reliably translates source code written in Notasi Algoritmik (NOTAL) into standard, well-formed, and compilable C source code. The NOTAL grammar is derived from the Inggriani Liem dictations on Procedural Programming and Data Structures. The transpiler itself is implemented in modern C++ (C++17 or later).

## 2. About Notasi Algoritmik (NOTAL)

Notasi Algoritmik (NOTAL) is a pedagogical programming language designed to teach fundamental programming concepts. It uses a syntax that is intended to be clear and easy to understand, bridging the gap between pseudocode and formal programming languages. While NOTAL aims for simplicity, it supports common procedural constructs, data types, and control flow mechanisms. A key aspect of its design philosophy is the use of indentation to define block structures for control flow statements like IF, WHILE, FOR, and DEPEND ON, minimizing the need for explicit block termination keywords in these contexts.

## 3. How It Works & Project Architecture (C++)

The transpilation process involves several stages: first, the **Lexer** breaks down the NOTAL source code into a stream of tokens. The **Parser** then consumes these tokens to build an Abstract Syntax Tree (AST), which represents the program's structure, while also populating a **Symbol Table** with information about identifiers. Finally, the **Code Generator** traverses the AST and uses the Symbol Table to produce equivalent C code.

The transpiler is organized into the following main C++ components:
*   **Lexer (`lexer.cpp`, `lexer.hpp`):** Tokenizes the NOTAL source code, now including indentation detection.
*   **Parser (`parser.cpp`, `parser.hpp`):** Builds an Abstract Syntax Tree (AST) from tokens, using indentation for block structuring. Populates a Symbol Table.
*   **SymbolTable (`symbol_table.cpp`, `symbol_table.hpp`):** Manages scopes and identifiers (variables, functions, types), including array and ADT metadata.
*   **CodeGenerator (`generator.cpp`, `generator.hpp`):** Traverses the AST (using the Visitor pattern) and generates C code, utilizing the Symbol Table for type information.
*   **ErrorHandler (`error_handler.cpp`, `error_handler.hpp`):** Provides structured error reporting with error codes, line/column numbers.
*   **Logger (`logger.cpp`, `logger.hpp`):** For diagnostic logging, configurable via CLI (`--log-level`).
*   **Main Driver (`main.cpp`):** Handles CLI arguments, orchestrates the components, and manages file I/O.
*   **ADT Implementations (`adt/*.c`, `adt/*.h`):** Pre-written C implementations for standard ADTs.

## 4. Getting the Project

You can obtain the project files in one of the following ways:

### Forking (Recommended for Contributors)
1. Go to the project's GitHub repository page.
2. Click the "Fork" button in the top-right corner to create a copy of the repository under your GitHub account.
3. Clone your forked repository: `git clone https://github.com/YOUR_USERNAME/REPOSITORY_NAME.git` (Replace `YOUR_USERNAME` and `REPOSITORY_NAME` accordingly).

### Cloning (For Direct Use)
If you have direct access or just want to clone the main repository:
`git clone https://github.com/ORIGINAL_OWNER/REPOSITORY_NAME.git` (Replace `ORIGINAL_OWNER` and `REPOSITORY_NAME`).

### Downloading ZIP
1. Go to the project's GitHub repository page.
2. Click the green "Code" button.
3. Select "Download ZIP".
4. Extract the downloaded ZIP file to your desired location.

## 5. How to Build the Transpiler

The project uses a `Makefile` for compilation with `g++`.
1.  Ensure you have a C++17 compliant GNU GCC (specifically, the `g++` compiler) and `make` installed. For Windows users, it is recommended to use the [MSYS2 project](https://www.msys2.org/) to install a suitable GNU GCC toolchain.
2.  Navigate to the project root directory.
3.  Run the command:
    ```bash
    make
    ```
4.  This will compile all C++ source files from the `src/` directory and create an executable named `gate` (Ganesha Algorithmic Transpiler Engine) in the project root.

## 6. How to Run the Transpiler

Use the following command-line interface:

```bash
./gate <source_file.notal> -o <target_c_file.c> [--log-level <level>]
```

*   `<source_file.notal>`: Path to the input Notasi Algoritmik (NOTAL) file.
*   `<target_c_file.c>`: Path where the generated C code will be saved.
*   `[--log-level <level>]` (Optional): Sets the diagnostic log level. `<level>` can be one of: `DEBUG`, `INFO`, `WARNING`, `ERROR`, `NONE`. Defaults to `INFO`.

**Example Usage:**
```bash
./gate input/example.notal -o output/example.c --log-level DEBUG
```

To compile the generated C code (which might depend on the provided ADTs):
```bash
gcc -Wall -Wextra -pedantic -o my_program output/example.c adt/adt_list.c adt/adt_stack.c -lm
./my_program
```
(Adjust ADT source files in the `gcc` command as needed if other ADTs like `adt_tree.c` are used by your NOTAL program).

## 7. Example NOTAL to C Transpilation

This example demonstrates various features of NOTAL and their C translation, now reflecting indentation-based syntax and the `INLINE` feature.

### Source File: `input/example.notal` (Illustrative - check repository for actual file)
```algorithmn
Program CekKelulusan {
    Membaca nama dan nilai mahasiswa, kemudian menentukan status kelulusan.
    Program ini akan berjalan sebanyak 3 kali.
}

KAMUS
    type Mahasiswa: < Nama: string, Nilai: integer >
    Mhs1: Mahasiswa
    i: integer
    function IsLulus(N: integer) -> boolean

ALGORITMA
    for i <- 1 to 3 do
        output("Masukkan Nama Mahasiswa ke-", i, ": ", INLINE)
        input(Mhs1.Nama)
        output("Masukkan Nilai ", Mhs1.Nama, ": ", INLINE)
        input(Mhs1.Nilai)
        if IsLulus(Mhs1.Nilai) then
            output(Mhs1.Nama, " dinyatakan LULUS.")
        else
            output(Mhs1.Nama, " dinyatakan GAGAL.")
        // No ENDIF here, block ends with dedent
        output("") // For a blank line
        dispose(Mhs1.Nama)
    // No ENDFOR here, block ends with dedent
    output("Proses selesai.")

function IsLulus(N: integer) -> boolean
{ Mengembalikan true jika nilai N >= 65, selain itu false }
KAMUS LOKAL
    hasil: boolean
ALGORITMA
    if (N >= 65) then
        hasil <- true
    else
        hasil <- false
    // No ENDIF here
    -> hasil
```

### Transpiled Result: `output/example.c` (Illustrative - check repository for actual file)
```c
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Forward declaration
bool IsLulus(int N);

typedef struct {
    char* Nama;
    int Nilai;
} Mahasiswa;

int main() {
    Mahasiswa Mhs1;
    int i;

    for (i = 1; i <= 3; i++) {
        printf("Masukkan Nama Mahasiswa ke-%d: ", i); // INLINE effect
        Mhs1.Nama = (char*) malloc(100 * sizeof(char));
        if (Mhs1.Nama == NULL) {
             fprintf(stderr, "Memory allocation failed for Mhs1.Nama\n");
             return 1;
        }
        scanf("%99s", Mhs1.Nama);

        printf("Masukkan Nilai %s: ", Mhs1.Nama); // INLINE effect
        scanf("%d", &Mhs1.Nilai);

        if (IsLulus(Mhs1.Nilai)) {
            printf("%s dinyatakan LULUS.\n", Mhs1.Nama);
        } else {
            printf("%s dinyatakan GAGAL.\n", Mhs1.Nama);
        }
        printf("\n"); // Corresponds to output("")

        free(Mhs1.Nama);
        Mhs1.Nama = NULL;
    }
    printf("Proses selesai.\n");
    return 0;
}

bool IsLulus(int N) {
    bool hasil;
    if (N >= 65) {
        hasil = true;
    } else {
        hasil = false;
    }
    return hasil;
}
```

## 8. Core Features of Notasi Algoritmik (NOTAL) Supported

*   **Program Structure:** `Program`, `KAMUS` (global declarations), `ALGORITMA` (main logic), subprogram implementations (functions/procedures) with `KAMUS LOKAL` and `ALGORITMA`. Blocks are defined by indentation.
*   **Data Types:**
    *   `integer` (maps to C `int`)
    *   `real` (maps to C `double`)
    *   `character` (maps to C `char`)
    *   `string` (maps to C `char*`. The `dispose()` keyword translates to `free()`. Input requires `malloc`.)
    *   `boolean` (maps to C `bool` via `<stdbool.h>`)
    *   Record/Struct types: `type <Name> : <field1:type1, ...>`
    *   Array types: `array [min..max] of <ElementType>` (0-based index mapping in C by default)
*   **Operations:**
    *   Assignment: `var <- expr`
    *   Input: `input(var1, var2, ...)` (generates `scanf`)
    *   Output: `output(expr1, expr2, ..., [INLINE])` (generates `printf`. Appends newline by default; `INLINE` suppresses it.)
    *   `dispose(str_var)` (translates to `free(str_var)`)
    *   Arithmetic Operators: `+`, `-`, `*`, `/` (real division), `div` (integer division), `mod`, `^` (power, maps to `pow()`)
    *   Relational & Logical Operators: `=`, `<>`, `<`, `<=`, `>`, `>=`, `not`, `and`, `or`
*   **Control Flow (Indentation-based):**
    *   `if (cond) then ... [else ...]`
    *   `while (cond) do ...`
    *   `repeat ... until (cond)`
    *   `for i <- start to end do ...`
    *   `depend on (var) case <val1>: ... otherwise: ...`
*   **Subprograms:**
    *   `function <Name>(<params>) -> <return_type>`
    *   `procedure <Name>(<params>)`
    *   Parameter passing (default `input` mode).
    *   Return mechanism: `-> value` at end of function or `return` keyword within the body.
    *   **Procedure Parameter Modifiers:** NOTAL procedures support `input`, `output`, and `input/output` modifiers for parameters to control how data is passed and modified.
        *   `input`: (Default) Parameter is pass-by-value. The procedure receives a copy of the argument. Changes inside the procedure do not affect the caller's variable. C equivalent: `T param_name`.
        *   `output`: Parameter is pass-by-reference. The procedure is expected to provide a value for this parameter. The caller's variable will be updated. C equivalent: `T* param_name` for most types; `char** param_name` for `string` type, where the procedure is responsible for allocating memory for the string (e.g., using `strdup`).
        *   `input/output`: Parameter is pass-by-reference. The caller provides an initial value, and the procedure can modify it, with changes reflected back to the caller. C equivalent: `T* param_name` for most types; `char** param_name` for `string` type, allowing the procedure to reallocate or repoint the string. The caller's string variable should typically be heap-allocated (e.g. via `strdup` or other means) to allow the procedure to safely `free` and reassign it.

        **Example (`input/procedure_params_test.notal`):**
        ```notal
        procedure ProcessNumbers(input a: integer, output b: integer, input/output c: integer)
        ALGORITMA
            b <- a * 2;
            c <- c * 2;

        procedure ProcessStrings(input s1: string, output s2: string, input/output s3: string)
        ALGORITMA
            s2 <- "processed_output";
            s3 <- "modified_io";
        ```

        **Generated C Snippet (from `output/procedure_params_test.c`):**
        ```c
        // Forward declarations
        void ProcessNumbers(int a, int* b, int* c);
        void ProcessStrings(char* s1, char** s2, char** s3);

        // In main:
        // ProcessNumbers(num_in, &num_out, &num_io);
        // ProcessStrings(str_in, &str_out, &str_io); // Assuming str_io was strdup'd

        void ProcessNumbers(int a, int* b, int* c) {
            *b = a * 2;
            *c = *c * 2;
        }

        void ProcessStrings(char* s1, char** s2, char** s3) {
            if (*s2) free(*s2);
            *s2 = strdup("processed_output");

            if (*s3) free(*s3);
            *s3 = strdup("modified_io");
        }
        ```
*   **Abstract Data Types (ADTs):**
    *   Support for `List`, `Stack`, `Tree` etc. (if headers are provided).
    *   Generates `#include "adt_list.h"` etc., when ADT types are used.
    *   Translates ADT operations to C function calls.

## 9. Limitations / Future Work
*   **Error Recovery:** The parser uses basic error reporting and may not recover gracefully from all syntax errors. Indentation errors might be particularly sensitive.
*   **Semantic Analysis:** Type checking is primarily done during code generation for I/O. A dedicated semantic analysis phase could provide more comprehensive static checks.
*   **String Memory Management:** While `dispose` maps to `free`, the NOTAL code must correctly manage string allocation. The C `input` for strings assumes a buffer is ready (example shows `malloc`).
*   **ADT Memory Management:** The C ADT implementations use `void*` and manual memory management.
*   **Optimization:** The generated C code is a direct translation and not optimized.
*   **Indentation Style:** The current indentation parser assumes consistent use of spaces per level. Handling mixed tabs/spaces or variable spaces per level robustly could be enhanced.

## 10. Contributors

This project has benefited from the contributions and insights of:

*   [Zulfa Nurhuda](https://github.com/ZulfaNurhuda)
*   [Anisa Alhaqi](https://github.com/anisaalhaqi)

---
## Pointers and Memory Management

NOTAL now supports basic pointer types and memory management operations, translating to their C equivalents.

*   **Pointer Declaration:** Variables can be declared as pointers to other types using the syntax:
    `var_name : pointer to <base_type>`
    This translates to `base_type* var_name;` in C. For `pointer to string`, it becomes `char** var_name;` in C, allowing modification of the string pointer itself.

*   **Reference Operator (`reference`):**
    - Syntax: `reference(variable)`
    - Returns the memory address of `variable`.
    - Translates to `&variable` in C.
    - Example: `pNum <- reference(num)` (where `num` is `integer`, `pNum` is `pointer to integer`) becomes `pNum = &num;` in C.

*   **Dereference Operator (`dereference`):**
    - Syntax: `dereference(pointer_variable)`
    - Accesses the value pointed to by `pointer_variable`.
    - Can be used on both the left-hand side (LHS) of an assignment to change the pointed-to value, and on the right-hand side (RHS) or in expressions to read the value.
    - Translates to `(*pointer_variable)` in C.
    - Example (RHS): `val <- dereference(pNum)` becomes `val = (*pNum);`
    - Example (LHS): `dereference(pNum) <- 20` becomes `(*pNum) = 20;`

*   **Dynamic Memory Allocation:**
    - `allocate(size_expression)`:
        - Dynamically allocates a block of memory of `size_expression` bytes.
        - Translates to `malloc(size_expression)` in C. The result is typically assigned to a pointer variable.
        - Example: `pData <- allocate(100)` becomes `pData = malloc(100);` (casting to specific pointer type like `(int*)malloc(100)` is often done by the C code generator for type safety, though C allows direct assignment from `void*`).
    - `reallocate(pointer_variable, new_size_expression)`:
        - Changes the size of the memory block pointed to by `pointer_variable` to `new_size_expression` bytes.
        - Translates to `realloc(pointer_variable, new_size_expression)` in C.
        - (Full example usage in `input/pointers_memory_test.notal` is commented out for this iteration but feature is parsed).
    - `deallocate(pointer_variable)` and `dispose(pointer_variable)`:
        - Frees the dynamically allocated memory block pointed to by `pointer_variable`.
        - Both keywords translate to `free(pointer_variable)` in C.
        - It's good practice to set the pointer to `NULL` after deallocation to prevent dangling pointers.

*   **Null Pointer (`NULL`):**
    - NOTAL uses `NULL` to represent a null pointer value.
    - This is typically used to check if an allocation was successful or to initialize/reset pointers.
    - Translates directly to `NULL` in C (from `<stdlib.h>`).
    - Example: `if pAllocated = NULL then ...` becomes `if (pAllocated == NULL) { ... }`.

### Example: Pointers and Memory (`input/pointers_memory_test.notal`)
```notal
Program TestPointersMemory
KAMUS
    num : integer
    pNum : pointer to integer
    pAllocatedNum : pointer to integer
ALGORITMA
    output("--- Basic Pointer Test ---")
    num <- 10
    pNum <- reference(num)
    output("Value via pNum: ", dereference(pNum))
    dereference(pNum) <- 20
    output("num after update: ", num)

    output("--- Dynamic Allocation Test ---")
    pAllocatedNum <- allocate(4) // Assuming size of integer
    if pAllocatedNum = NULL then
        output("Allocation failed!")
    else
        dereference(pAllocatedNum) <- 123
        output("Allocated value: ", dereference(pAllocatedNum))
        deallocate(pAllocatedNum)
        pAllocatedNum <- NULL
    output("---")
END.
```

### Generated C Snippet (from `output/pointers_memory_test.c`):
```c
#include <stdio.h>
#include <stdlib.h> // For malloc, free, NULL

int main() {
    int num;
    int* pNum = NULL;
    int* pAllocatedNum = NULL;

    printf("--- Basic Pointer Test ---\n");
    num = 10;
    pNum = &num;
    printf("Value via pNum: %d\n", (*pNum));
    (*pNum) = 20;
    printf("num after update: %d\n", num);

    printf("--- Dynamic Allocation Test ---\n");
    pAllocatedNum = (int*)malloc(4);
    if (pAllocatedNum == NULL) {
        printf("Allocation failed!\n");
    } else {
        (*(pAllocatedNum)) = 123;
        printf("Allocated value: %d\n", (*(pAllocatedNum)));
        free(pAllocatedNum);
        pAllocatedNum = NULL;
    }
    printf("---\n");
    return 0;
}
```

## 11. Limitations / Future Work (Renumbered)
*   **Error Recovery:** The parser uses basic error reporting and may not recover gracefully from all syntax errors. Indentation errors might be particularly sensitive.
*   **Semantic Analysis:** Type checking is primarily done during code generation for I/O. A dedicated semantic analysis phase could provide more comprehensive static checks.
*   **String Memory Management:** While `dispose` maps to `free`, the NOTAL code must correctly manage string allocation. The C `input` for strings assumes a buffer is ready (example shows `malloc`). For `pointer to string`, careful management of `char**` is needed.
*   **ADT Memory Management:** The C ADT implementations use `void*` and manual memory management.
*   **Optimization:** The generated C code is a direct translation and not optimized.
*   **Indentation Style:** The current indentation parser assumes consistent use of spaces per level. Handling mixed tabs/spaces or variable spaces per level robustly could be enhanced.
*   **Pointer Arithmetic:** Not explicitly supported in NOTAL syntax (e.g. `p + 1` on a pointer to step by type size).
*   **Type Casting for `allocate`:** `allocate` returns a generic pointer; explicit casting if needed would be a NOTAL language feature or a more advanced generator task. Current C output relies on implicit `void*` conversion or adds a basic cast.

## 12. Contributors (Renumbered)

This project has benefited from the contributions and insights of:

*   [Zulfa Nurhuda](https://github.com/ZulfaNurhuda)
*   [Anisa Alhaqi](https://github.com/anisaalhaqi)
