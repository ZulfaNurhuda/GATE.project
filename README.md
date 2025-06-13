# Algorithmic Notation (AN) to C Transpiler (GATE - Ganesha Algorithmic Transpiler Engine)

**Version:** 1.0 (Based on initial brief v3.0)

## 1. Project Objective

To develop a production-quality, command-line compiler (transpiler) that correctly, efficiently, and reliably translates source code written in a pedagogical "Algorithmic Notation" (AN) into standard, well-formed, and compilable C source code. The AN grammar is derived from the Inggriani Liem dictations on Procedural Programming and Data Structures. The transpiler itself is implemented in modern C++ (C++17 or later).

## 2. Core Features of Algorithmic Notation Supported

*   **Program Structure:** `Program`, `KAMUS` (global declarations), `ALGORITMA` (main logic), subprogram implementations (functions/procedures) with `KAMUS LOKAL` and `ALGORITMA`.
*   **Data Types:**
    *   `integer` (maps to C `int`)
    *   `real` (maps to C `double`)
    *   `character` (maps to C `char`)
    *   `string` (maps to C `char*`. The `dispose()` keyword translates to `free()`. Input requires `malloc`, which should be handled by the AN programmer or by specific transpiler behavior for string inputs.)
    *   `boolean` (maps to C `bool` via `<stdbool.h>`)
    *   Record/Struct types: `type <Name> : <field1:type1, ...>`
    *   Array types: `array [min..max] of <ElementType>` (0-based index mapping in C)
*   **Operations:**
    *   Assignment: `var <- expr`
    *   Input: `input(var1, var2, ...)` (generates `scanf`)
    *   Output: `output(expr1, expr2, ...)` (generates `printf`, appends newline)
    *   `dispose(str_var)` (translates to `free(str_var)`)
    *   Arithmetic Operators: `+`, `-`, `*`, `/` (real division), `div` (integer division), `mod`, `^` (power, maps to `pow()`)
    *   Relational & Logical Operators: `=`, `<>`, `<`, `<=`, `>`, `>=`, `not`, `and`, `or`
*   **Control Flow:**
    *   `if (cond) then ... [else ...] endif`
    *   `while (cond) do ... endwhile`
    *   `repeat ... until (cond)`
    *   `for i <- start to end do ... endfor`
    *   `depend on (var) case <val1>: ... otherwise: ... enddependon` (maps to C `switch-case`)
*   **Subprograms:**
    *   `function <Name>(<params>) -> <return_type>`
    *   `procedure <Name>(<params>)`
    *   Parameter passing (default `input` mode).
    *   Return mechanism: `-> value` at end of function or `return` keyword within the body.
*   **Abstract Data Types (ADTs):**
    *   Support for `List`, `Stack`, `Tree` (and other user-defined ADTs if headers are provided).
    *   Generates `#include "adt_list.h"` etc., when ADT types are used.
    *   Translates ADT operations (e.g., `insertFirst(L,X)`) to C function calls (e.g., `list_append(&L,X)`).

## 3. Project Architecture (C++)

The transpiler is organized into the following main C++ components:
*   **Lexer (`lexer.cpp`, `lexer.hpp`):** Tokenizes the AN source code.
*   **Parser (`parser.cpp`, `parser.hpp`):** Builds an Abstract Syntax Tree (AST) from tokens, using polymorphic nodes and smart pointers. Populates a Symbol Table.
*   **SymbolTable (`symbol_table.cpp`, `symbol_table.hpp`):** Manages scopes and identifiers (variables, functions, types), including array and ADT metadata.
*   **CodeGenerator (`generator.cpp`, `generator.hpp`):** Traverses the AST (using the Visitor pattern) and generates C code, utilizing the Symbol Table for type information.
*   **ErrorHandler (`error_handler.cpp`, `error_handler.hpp`):** Provides structured error reporting with error codes, line/column numbers.
*   **Logger (`logger.cpp`, `logger.hpp`):** For diagnostic logging, configurable via CLI (`--log-level`).
*   **Main Driver (`main.cpp`):** Handles CLI arguments, orchestrates the components, and manages file I/O.
*   **ADT Implementations (`adt/*.c`, `adt/*.h`):** Pre-written C implementations for standard ADTs.

## 4. How to Build the Transpiler

The project uses a `Makefile` for compilation with `g++`.
1.  Ensure you have `g++` (supporting C++17) and `make` installed.
2.  Navigate to the project root directory.
3.  Run the command:
    ```bash
    make
    ```
4.  This will compile all C++ source files from the `src/` directory and create an executable named `gate` (Ganesha Algorithmic Transpiler Engine) in the project root.

## 5. How to Run the Transpiler

Use the following command-line interface:

```bash
./gate <source_file.notal> -o <target_c_file.c> [--log-level <level>]
```

*   `<source_file.notal>`: Path to the input Algorithmic Notation file.
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
(Adjust ADT source files in the `gcc` command as needed if other ADTs like `adt_tree.c` are used by your AN program).

## 6. Example AN to C Transpilation

This example demonstrates various features of the AN language and their C translation. (Uses the `CekKelulusan` example from the original project brief).

### Source File: `input/example.notal`
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
        output("Masukkan Nama Mahasiswa ke-", i, ": ")
        // AN programmer responsible for allocating string if needed before input
        // For this example, we assume input(Mhs1.Nama) expects Mhs1.Nama to be ready.
        // If Mhs1.Nama needed dynamic allocation, it would be:
        // Mhs1.Nama <- allocate(100) // Hypothetical AN allocate, or done via other means
        input(Mhs1.Nama)
        output("Masukkan Nilai ", Mhs1.Nama, ": ")
        input(Mhs1.Nilai)
        if IsLulus(Mhs1.Nilai) then
            output(Mhs1.Nama, " dinyatakan LULUS.")
        else
            output(Mhs1.Nama, " dinyatakan GAGAL.")
        endif
        output("")
        dispose(Mhs1.Nama) // Essential if Mhs1.Nama was dynamically allocated
    endfor
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
    endif
    -> hasil
```

### Transpiled Result: `output/example.c` (Reflecting Brief's String Handling)
```c
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h> // For malloc/free
#include <string.h>
#include <math.h>   // Typically included by the generator

// Forward declaration
bool IsLulus(int N);

typedef struct {
    char* Nama; // AN 'string' maps to 'char*'
    int Nilai;
} Mahasiswa;

int main() {
    Mahasiswa Mhs1;
    int i;

    for (i = 1; i <= 3; i++) {
        printf("Masukkan Nama Mahasiswa ke-%d: \n", i);

        // AN 'string' input requires memory allocation as per brief (Sec 3.1.2)
        Mhs1.Nama = (char*) malloc(100 * sizeof(char)); // Assuming a default size
        if (Mhs1.Nama == NULL) {
             fprintf(stderr, "Memory allocation failed for Mhs1.Nama\n");
             return 1;
        }
        scanf("%99s", Mhs1.Nama); // Read into allocated buffer, %99s to prevent overflow

        printf("Masukkan Nilai %s: \n", Mhs1.Nama);
        scanf("%d", &Mhs1.Nilai);

        if (IsLulus(Mhs1.Nilai)) {
            printf("%s dinyatakan LULUS.\n", Mhs1.Nama);
        } else {
            printf("%s dinyatakan GAGAL.\n", Mhs1.Nama);
        }
        printf("\n");

        free(Mhs1.Nama); // Corresponds to AN 'dispose(Mhs1.Nama)'
        Mhs1.Nama = NULL; // Good practice after free
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

## 7. Limitations / Future Work
*   **Error Recovery:** The parser uses basic error reporting and may not recover gracefully from all syntax errors.
*   **Semantic Analysis:** Type checking is primarily done during code generation for I/O. A dedicated semantic analysis phase could provide more comprehensive static checks (e.g., type compatibility in assignments/expressions, ensuring boolean conditions for loops/ifs).
*   **String Memory Management:** While `dispose` maps to `free`, the AN code must correctly manage string allocation (e.g. via a hypothetical `allocate` keyword or by ensuring fixed-size buffers if not using dynamic strings). The C `input` for strings assumes a buffer is ready. The `example.c` now shows explicit `malloc`/`free` to align with the brief's C output for string inputs.
*   **ADT Memory Management:** The C ADT implementations use `void*` and manual memory management. Users of ADTs in AN need to be mindful of memory for data stored within them.
*   **Optimization:** The generated C code is a direct translation and not optimized.

```