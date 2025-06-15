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
