# Notasi Algoritmik (NOTAL) to C Transpiler (GATE - Ganesha Algorithmic Transpiler Engine)

**Version:** 1.4 (Based on initial brief v3.0)

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
    *   `procedure <Name>(<params>)` (parameterless procedures can be called with or without `()`).
    *   Parameter passing (default `input` mode).
    *   **Procedure Parameter Modifiers:** NOTAL procedures support `input`, `output`, and `input/output` modifiers for parameters to control how data is passed and modified.
        *   `input`: (Default) Parameter is pass-by-value. C equivalent: `T param_name`.
        *   `output`: Parameter is pass-by-reference. C equivalent: `T* param_name` for most types; `char** param_name` for `string` type (procedure allocates).
        *   `input/output`: Parameter is pass-by-reference. C equivalent: `T* param_name` for most types; `char** param_name` for `string` type (procedure can reallocate/repoint). Caller's string should be heap-allocated.
        *(See `input/procedure_params_test.notal` for detailed examples)*
    *   Return mechanism: `-> value` at end of function or `return` keyword within the body.
*   **Abstract Data Types (ADTs):**
    *   Support for `List`, `Stack`, `Tree` etc. (if headers are provided).
    *   Generates `#include "adt_list.h"` etc., when ADT types are used.
    *   Translates ADT operations to C function calls.

## 9. Pointers, Constants, Enums, and Other Features

This section details additional language features, including those for more advanced data manipulation and structuring.

*   **Pointer Declaration & Operations:**
    *   Declaration: `var_name : pointer to <base_type>` (Translates to `base_type* var_name;` or `char**` for `pointer to string`).
    *   Reference: `reference(variable)` (Translates to `&variable`).
    *   Dereference: `dereference(pointer_variable)` (Translates to `(*pointer_variable)`). Used for both read/write.
    *   Pointer Member Access: `pointer_expr^.member_name` (Translates to `(pointer_expr_code)->member_name`).
*   **Dynamic Memory Management:**
    *   `allocate(size_expression)`: Translates to `malloc(size_expression)`.
    *   `reallocate(pointer_variable, new_size_expression)`: Translates to `realloc(ptr, new_size)`.
    *   `deallocate(pointer_variable)` / `dispose(pointer_variable)`: Both translate to `free(ptr)`.
*   **Null Pointer (`NULL`):**
    *   Represents a null pointer value. Translates to `NULL` in C.
    *   Example: `if p = NULL then ...`
*   **Enum Type Declarations:**
    *   Syntax: `type MyEnum: (VAL1, VAL2, VAL3)`
    *   Translates to C: `typedef enum { VAL1, VAL2, VAL3 } MyEnum;`
    *   Enum values (`VAL1`, etc.) are added to the symbol table as constants of type `MyEnum`.
*   **Constant Declarations:**
    *   Syntax: `constant MY_CONST = <literal_value>`
    *   Translates to C: `const <type> MY_CONST = <value>;`
    *   Type is inferred from the literal (e.g., `10` is integer, `"hi"` is string, `true` is boolean, `NULL` is `void*`).
*   **Multi-Assignment Lines:**
    *   Multiple assignment statements can be written on a single line at the beginning of an `ALGORITMA` block, separated by semicolons.
    *   Example: `ALGORITMA \n    a <- 1; b <- 2; c <- "test"`

*   **Defining Linked Data Structures (e.g., Linked Lists):**
    - Users can define recursive data structures like linked lists using records that contain pointers to their own type.
    - The `pointer to <RecordName>` syntax within a field definition allows for self-referential structures.
    - The code generator ensures that C `struct`s are defined with appropriate forward `typedef`s to handle such self-references.

    **Example: Linked List Node (`input/list_adt_test.notal`)**
    ```notal
    KAMUS
        type List < value: integer,
                    next: pointer to List >;
        myListHead : pointer to List
    ```

    **Generated C (`output/list_adt_test.c`):**
    ```c
    typedef struct List List; // Forward declaration for typedef name
    struct List {
        int value;
        List* next; // Self-referential pointer
    };

    List* myListHead = NULL;
    ```
    - This allows manual creation and manipulation of linked lists, as demonstrated in `input/list_adt_test.notal` using `allocate`, `dereference`, and `^.` for node operations.

### Example: Advanced Features (`input/advanced_features_test.notal`)
```notal
Program TestAdvancedFeatures
KAMUS
    type Days: (MONDAY, TUESDAY, WEDNESDAY) // Abbreviated
    constant PI = 3.14159
    constant DEFAULT_DAY = MONDAY
    today : Days
    type UserRecord: <id: integer, name: string>
    pUser : pointer to UserRecord
    procedure InitGlobals // Parameterless
ALGORITMA
    InitGlobals
    counter <- 0; message <- "default_msg"
    today <- WEDNESDAY
    if today = DEFAULT_DAY then output("Default day!")
    pUser <- allocate(100) // Simplified size, real size via sizeof(UserRecord)
    if pUser = NULL then output("Allocation failed")
    else
        pUser^.id <- 101
        output("User ID: ", pUser^.id)
        deallocate(pUser)
END.
```
### Generated C Snippet (Illustrative from `output/advanced_features_test.c`):
```c
// Enum
typedef enum { MONDAY, TUESDAY, WEDNESDAY } Days;
// Constant
const double PI = 3.14159;
const Days DEFAULT_DAY = MONDAY;
// Record
typedef struct { int id; char* name; } UserRecord;
// Global
Days today;
UserRecord* pUser = NULL;
// Procedure
void InitGlobals(void);

int main() {
    InitGlobals();
    counter = 0; message = "default_msg";
    today = WEDNESDAY;
    if (today == DEFAULT_DAY) { printf("Default day!\n"); }
    pUser = (UserRecord*)malloc(100);
    if (pUser == NULL) { printf("Allocation failed\n"); }
    else {
        (pUser)->id = 101;
        printf("User ID: %d\n", (pUser)->id);
        free(pUser);
    }
    return 0;
}
// ... procedure implementations ...
```

## 10. Limitations / Future Work (Renumbered)
*   **Error Recovery:** The parser uses basic error reporting and may not recover gracefully from all syntax errors. Indentation errors might be particularly sensitive.
*   **Semantic Analysis:** Type checking is primarily done during code generation for I/O. A dedicated semantic analysis phase could provide more comprehensive static checks.
*   **String Memory Management:** While `dispose` maps to `free`, the NOTAL code must correctly manage string allocation. The C `input` for strings assumes a buffer is ready (example shows `malloc`). For `pointer to string`, careful management of `char**` is needed.
*   **ADT Memory Management:** The C ADT implementations use `void*` and manual memory management.
*   **Optimization:** The generated C code is a direct translation and not optimized.
*   **Indentation Style:** The current indentation parser assumes consistent use of spaces per level. Handling mixed tabs/spaces or variable spaces per level robustly could be enhanced.
*   **Pointer Arithmetic:** Not explicitly supported in NOTAL syntax (e.g. `p + 1` on a pointer to step by type size).
*   **Type Casting for `allocate`:** `allocate` returns a generic pointer; explicit casting if needed would be a NOTAL language feature or a more advanced generator task. Current C output relies on implicit `void*` conversion or adds a basic cast.

## 11. Contributors (Renumbered)

This project has benefited from the contributions and insights of:

*   [Zulfa Nurhuda](https://github.com/ZulfaNurhuda)
*   [Anisa Alhaqi](https://github.com/anisaalhaqi)