# **NOTAL Transpiler to Pascal**

# **1. Introduction**

## **1.1 Project Objective**

The primary aim of this project is to make Notasi Algoritmik (NOTAL) executable through the Pascal compiler. 

## **1.2. Vision and Benefit**

-  ***`// Vision //`***

   Establish a link between comprehensive algorithmic concepts and practical implementation within a programming language.

-  ***`// Benefit //`***
  
   Students could develop a solid understanding of data structure, flow control, and procedural programming paradigms

## **1.3. Transpiler Scope**

In consideration of the limitation of NOTAL's operation, this project can only run an independent file. Consequently, header files cannot be handled.

## **1.4. Document Audience**

The audience for this project consists of undergraduate students at Institut Teknologi Bandung who have taken courses that utilize NOTAL to represent their basic programming logic, such as Data Structures and Algorithms, Object-Oriented Programming courses.

# **2. NOTAL Language Overview**

## **2.1. NOTAL Design Philosophy**

Notasi Algoritmik is a method developed by lecturers at ITB. It is primarily derived from Inggriani Liem's 'Data Structures' dictat. We've also observed that NOTAL shares significant syntactic similarities with Pascal.

## **2.2. NOTAL Program Structure**

### **2.2.1. Program Header Block**

```
PROGRAM ProgramName 
{ Program's specification }
```

... or with this syntax for multiline comments:

```
PROGRAM ProgramName 
{ Program's specification
  I.S. Condition 1
  F.S. Condition 2 }
```

### **2.2.2. DICTIONARY Block**

```
KAMUS
   constant PI: real = 3.14159
   constant MAX_SIZE: integer = 100
   constant GREETING: string = 'Selamat Datang'
   
   type Point: < x: real,
                 y: real >
   type Status: (active, inactive, pending) 	 	
   
   coordinates: Point
   numbers: array[1..10][1..5] of integer
   dynamicData: array of integer
   counter: integer
   isValid: boolean
```

### **2.2.3. ALGORITHM Block**

```
ALGORITMA
   counter <- 0
   coordinates.x <- 10.5
   coordinates.y <- 20.3

   allocate(dynamicData, 10)
   
   i traversal [1..10]
      j traversal [1..5]
         numbers[i][j] <- i * j
         
   i traversal [1..10]
      dynamicData[i] <- i * 2
   
   output('Hasil perhitungan: ', numbers[2][3])
   output('Data dinamis: ', dynamicData[5])
   
   deallocate[](dynamicData)
```

# **3. NOTAL Syntax Specification and Pascal Equivalents**

## **3.1. Data Types and Structures**

### **3.1.1. Fundamental Data Types**

#### **3.1.1.1. Integer**


|   NOTAL   |  Pascal   |
| :-------: | :-------: |
| `integer` | `integer` |

#### **3.1.1.2. Real**

| NOTAL  | Pascal |
| :----: | :----: |
| `real` | `real` |

#### **3.1.1.3. Boolean**

|   NOTAL   |  Pascal   |
| :-------: | :-------: |
| `boolean` | `boolean` |

#### **3.1.1.4. Character**

|    NOTAL    | Pascal |
| :---------: | :----: |
| `character` | `char` |

#### **3.1.1.5. String**

|  NOTAL   |  Pascal  |
| :------: | :------: |
| `string` | `string` |

### **3.1.2. Structured Types (Record)**

**NOTAL**

```
type Student: <
   name: string,
   age: integer,
   gpa: real,
   status: (active, graduated, dropped),
>

type Course: < 
   code: string,
   title: string,
   credits: integer[1..4],
   instructor: Student,
>
```

**Pascal**

```pascal
type
  	Student = record
  		name: string;
  		age: integer;
  		gpa: real;
  		status: (active, graduated, dropped);
  	end;

  	Course = record
  		code: string;
  		title: string;
  		credits: 1..4;
  		instructor: Student;
  	end;
```



### **3.1.3. Static and Multidimensional Arrays**

**NOTAL**

```
matrix: array[1..5][1..3] of real
cube: array[1..10][1..10][1..10] of integer
```

**Pascal**

```pascal
var
   matrix: array[1..5, 1..3] of real;
   cube: array[1..10, 1..10, 1..10] of integer;
```

### **3.1.4. Dynamic Arrays (Declaration, Allocation, Access, Deallocation)**

**NOTAL**

-  **Declaration**

   ```
   KAMUS
      data1D: array of integer
      data2D: array of array of integer
      data3D: array of array of array of integer
   ```

-  **Allocation**

   ```
   allocate(data1D, 10)
   allocate(data2D, 10, 5)
   allocate(data3D, 10, 5, 3)
   ```

-  **Access**

   ```
   data1D[1] <- 100
   data2D[1][1] <- 200
   data3D[1][1][1] <- 300
   ```

-  **Deallocation**

   ```
   deallocate[1](data1D)
   deallocate[2](data2D)
   deallocate[3](data3D)
   ```

   > [!TIP]
   > ... or just `deallocate[](data1D)` for 1D dynamic array.

**Pascal**

-  **Declaration**

   ```pascal
   var
      data1D: array of integer;
      data2D: array of array of integer;
      data3D: array of array of array of integer;
   ```

-  **Allocation**

   ```pascal
   SetLength(data1D, 10);
   SetLength(data2D, 10, 5);
   SetLength(data3D, 10, 5, 3);
   ```

-  **Access**

   ```pascal
   data1D[1] := 100;
   data2D[1, 1] := 200;
   data3D[1, 1, 1] := 300;
   ```

-  **Deallocation**

   ```pascal
   SetLength(data1D, 0);
   SetLength(data2D, 0, 0);
   SetLength(data3D, 0, 0, 0);
   ```

### **3.1.5. Enumerated Types and Constraints**

**Enumerated**

-  **NOTAL**

   ```
   type Day: (monday, tuesday, wednesday, thursday, friday, saturday, sunday)
   type Grade: (A, B, C, D, E)
   ```

-  **Pascal**

   ```pascal
   type
      Day = (monday, tuesday, wednesday, thursday, friday, saturday, sunday);
      Grade = (A, B, C, D, E);
   ```

**Constraints**

> [!IMPORTANT]
> Constraint types are not implicitly supported by the Pascal language. However, to adhere to NOTAL syntax requirements, the transpiler generates a setter procedure for each constraint type. This setter procedure processes all assignment operations and includes validation to ensure that the assigned value meets the specified constraints. The validation is performed using Pascal's `Assert` statement, which checks the constraints and halts the program with an error message if the validation fails, thereby enforcing the constraints at the procedural level.
> **Don't forget to add flag `-Sa` to your compiler command to enable assertions. If you don't, the assertions will be ignored.**

-  **NOTAL**

   ```
   PROGRAM ConstraintsExample

   KAMUS
      age: integer | age >= 0 and age <= 150
      score: real | score >= 0.0 and score <= 100.0

   ALGORITMA
      age <- 200 { Error! }
      Score <- 98.0 { Passed }
   ```

-  **Pascal**

   ```pascal
   program ConstraintsExample;
   
   var
      age: integer;
      score: real;
   
   procedure SetAge(var age: integer; value: integer); forward;
   procedure SetScore(var score: real; value: real); forward;

   procedure SetAge(var age: integer; value: integer);
   begin
      Assert((value < 0) or (value > 150), 'Error: Age must be between 0 and 150!');
      age := value;
   end;

   procedure SetScore(var score: real; value: real);
   begin
      Assert((value < 0.0) or (value > 100.0), 'Error: Score must be between 0.0 and 100.0!');
      score := value;
   end;
   
   begin
      SetAge(age, 200);
      SetScore(score, 98.0);
   end.
   ```

## **3.2. Operators and Expressions** 

### **3.2.1. Arithmetic Operators**

**NOTAL**

| **Arithmetic Operators**         | **Representation**  |
| :------------------------------- | :------------------ |
| Basic Operations                 | `+`, `-`, `*`, `/`  |
| Integer Division (*rounding*)    | `div (ex. 5 div 2)` |
| Modulo (*remainder of division*) | `mod (ex. 5 mod 2)` |
| Power                            | `^`                 |

**Pascal**

| **Arithmetic Operators**         | **Representation**                |
| :------------------------------- | :-------------------------------- |
| Basic Operations                 | `+`, `-`, `*`, `/`                |
| Integer Division (*rounding*)    | `div`                             |
| Modulo (*remainder of division*) | `mod`                             |
| Power                            | `trunc(exp(exponent * ln(base)))` |

         

### **3.2.2. Relational Operators**

**Both NOTAL and Pascal**

| **Relational Operators** | **Representation** |
| :----------------------- | :----------------- |
| Equal to                 | `=`                |
| Not equal to             | `<>`               |
| Greater than             | `>`                |
| Greater than or equal to | `>=`               |
| Less than                | `<`                |
| Less than or equal to    | `<=`               |

### **3.2.3. Logical Operators**

**Both NOTAL and Pascal**

| **Logical Operators**  | **Representation** |
| :--------------------- | :----------------- |
| Binary Logic Operation | `and`, `or`        |
| Logical Negation       | `not`              |

### **3.2.4. String Operator**

**NOTAL**

| **String Operator** | **Representation** |
| :------------------ | :----------------- |
| Concatenation       | `&`                |

**Pascal**

| **String Operator** | **Representation** |
| :------------------ | :----------------- |
| Concatenation       | `+`                |

### **3.2.5. Assignment and Initialization**

**NOTAL**

```
KAMUS
   constant MAX: integer = 100

   counter: integer
   name: string
   isReady: boolean

ALGORITMA
   counter <- 0
   name <- 'John Doe'
   isReady <- true
```

**Pascal**

```pascal
const
   MAX = 100;

var
   counter: integer;
   name: string;
   isReady: boolean;

begin
   counter := 0;
   name := 'John Doe';
   isReady := true;
end.
```

## **3.3. Program Flow Control Structures**

### **3.3.1. Conditional Statements** 

**NOTAL**

-  **Basic**

   ```
   if score >= 90 then
      grade <- 'A'
   ```

-  **Alternative**

   ```
   if temperature > 30 then
      status <- 'Hot'
   else
      status <- 'Cool'
   ```

-  **Nested**

   ```
   if score >= 90 then
      grade <- 'A'
   elif score >= 80 then
      grade <- 'B'
   elif score >= 70 then
      grade <- 'C'
   else
      grade <- 'D'
   ```

**Pascal**

-  **Basic**

   ```pascal
   if score >= 90 then
      begin
         grade := 'A';
      end;
   ```

-  **Alternative**

   ```pascal
   if temperature > 30 then
      begin
         status := 'Hot'
      end
   else
      begin
         status := 'Cool';
      end;
   ```

-  **Nested**

   ```pascal
   if score >= 90 then
      begin
         grade := 'A';
      end
   else if score >= 80 then
      begin
         grade := 'B';
      end
   else if score >= 70 then
      begin
         grade := 'C';
      end
   else
      begin
         grade := 'D';
      end;
   ```

### **3.3.2. Depend On Structure**

**NOTAL**

-  **Literal**

   ```
   depend on (dayofWeek)
      monday: output('Start of work week')
      friday: output('End of work week')
      saturday, sunday: output('Weekend')
      otherwise: output('Regular day')
   ```

-  **Complex**

   ```
   depend on (score, attendance)
      score >= 90 and attendance >= 80: output('Excellent')
      score >= 70 and attendance >= 60: output('Good')
      otherwise: output('Needs improvement')
   ```

**Pascal**

-  **Literal**

   ```pascal
   case dayofWeek of
      monday: writeln('Start of work week');
      friday: writeln('End of work week');
      saturday, sunday: writeln('Weekend');
      else
         begin
            writeln('Regular day');
         end;
   end;
   ```

-  **Complex**

   ```pascal
   if (score >= 90) and (attendance >= 80) then
      begin
         writeln('Excellent');
      end
   else if (score >= 70) and (attendance >= 60) then
      begin
         writeln('Good');
      end
   else
      begin
         writeln('Needs improvement');
      end;
   ```

### **3.3.3. Looping Structures**

#### **3.3.3.1. While Loop**

**NOTAL**

```
counter <- 1
while counter <= 10 do
   output(counter)
   counter <- counter + 1
```

**Pascal**

```pascal
counter := 1;
while counter <= 10 do
begin
   writeln(counter);
   counter := counter + 1;
end;
```

#### **3.3.3.2. Repeat-Until Loop**

**NOTAL**

```
repeat
   output('Enter positive number: ')
   input(num)
until num > 0
```

**Pascal**

```pascal
repeat
   begin
      writeln('Enter positive number: ');
      readln(num);
   end;
until num > 0;
```

#### **3.3.3.3. Traversal Loop**

**NOTAL**

-  **Normal (1-Step)**

   ```
   i traversal [1..10]
      output('Number: ', i)
   ```

-  **With 2-Step**

   ```
   i traversal [1..20 step 2]
      output('Odd number: ', i)
   ```

-  **With 3-Step**

   ```
   i traversal [1..50 step 5]
      output('A multiple of 5 (+1): ', i)
   ```

**Pascal**

-  **Normal (1-Step)**

```pascal
i := 1;
while (i <= 10) do
begin
   writeln('Number: ', i);
   Inc(i, 1);
end;
```

-  **With 2-Step**

```pascal
i := 1;
while (i <= 20) do
begin
   writeln('Odd number: ', i);
   Inc(i, 2);
end;
```

-  **With 3-Step**

```pascal
i := 1;
while (i <= 50) do
begin
   writeln('A multiple of 5 (+1): ', i);
   Inc(i, 5);
end;
```

#### **3.3.3.4. Iterate-Stop Loop**

**NOTAL**

```
iterate
   output('Enter command (quit to exit): ')
   input(command)
stop (command = 'quit')
   processCommand(command)
```

**Pascal**

```pascal
while true do
begin
   writeln('Enter command (quit to exit): ');
   readln(command);
if command = 'quit' then break;
   processCommand(command);
end;
```

#### **3.3.3.5. Repeat N Times**

**NOTAL**

```
repeat 5 times
   output('Hello World')
```

**Pascal**

```pascal
for i := 1 to 5 do
begin
   writeln('Hello World');
end;
```

### **3.3.4. Loop Control (stop, skip)**

**NOTAL**

-  **Break**

   ```
   i traversal [1..100]
      if numbers[i] = target then stop
      counter <- counter + 1
   ```

-  **Continue**

   ```
   i traversal [1..10]
      if numbers[i] < 0 then skip
      output(numbers[i])
   ```

**Pascal**

-  **Break**

   ```pascal
   i := 1;
   while (i <= 100) do
   begin
      if number[i] = target then break;
      counter := counter + 1;
      Inc(i, 1);
   end;
   ```

-  **Continue**

   ```pascal
   i := 1;
   while (i <= 100) do
   begin
      if number[i] < 0 then continue;
      writeln(numbers[i]);
   end;
   ```

## **3.4. Procedures and Functions**

### **3.4.1. Procedure Declaration and Definition**

**NOTAL**

-  **Tanpa Parameter**

   ```
   procedure displayMenu

   KAMUS
      { Scope lokal prosedur }
   
   ALGORITMA
      output('=== MENU UTAMA ===')
      output('1. Input Data')
      output('2. Process Data')
   ```

-  **Dengan Parameter**

   ```
   procedure calculateGrade(input score: real, output grade: character)
   
   KAMUS
      { Scope lokal prosedur }
   
   ALGORITMA
      if score >= 90 then
         grade <- 'A'
      elif score >= 80 then
         grade <- 'B'
      else
         grade <- 'C'
   ```

**Pascal**

-  **Tanpa Parameter**

   ```pascal
   procedure displayMenu;
   begin
      writeln('=== MENU UTAMA ===');
      writeln('1. Input Data');
      writeln('2. Process Data');
   end;
   ```

-  **Dengan Parameter**

   ```pascal
   procedure calculateGrade(score: real, var grade: character);
   begin
      if score >= 90 then
         begin
            grade := 'A';
         end
      elif score >= 80 then
         begin
            grade := 'B';
         end
      else
         begin
            grade := 'C';
         end;
   end;
   ```

### **3.4.2. Function Declaration and Definition**

**NOTAL**

-  **Dengan Parameter**

   ```
   function factorial (n: integer) -> integer
   
   KAMUS
      result: integer
   
   ALGORITMA
      if n <= 1 then
         result <- 1
      else
         result <- n * factorial(n-1)
      -> result
   ```

**Pascal**

-  **Dengan Parameter**

   ```pascal
   function factorial(n: integer): integer;
   var
      result: integer;
   begin
      if n <= 1 then
         begin
            result := 1;
         end
      else
         begin
            result := n * factorial(n-1);
         end;
      factorial := result;
   end;
   ```

### **3.4.3. Parameter Modes (input, output, input/output)**
> [!NOTE]
> In NOTAL syntax, the formal procedure parameters contain the prefixes `input`, `output`, and `input/output`. Here is the difference in the context of conversion to Pascal programming language.
> | NOTAL                                                                                            | Pascal | Description           |
> | :----------------------------------------------------------------------------------------------- | :----- | :-------------------- |
> | `input` (Only to retrieve the value of a variable without changing its value)                    | `-`    | *`pass-by-value`*     |
> | `output` (Variables not yet initialized, will be initiated and assigned values in the procedure) | `var`  | *`pass-by-reference`* |
> | `input/output` (The variable already has a value and will be changed in the procedure)           | `var`  | *`pass-by-reference`* |

**NOTAL**

-  **Example 1**

   ```
   procedure setName (input some_input: string, output fix_name: string)
   ALGORITMA
      fix_name <- some_input
   ```

-  **Example 2**

   ```
   procedure formatName (input/output name: string)
   KAMUS
      formatted_name: string
   ALGORITMA
      formatted_name <- name & 'XX'
      name <- formatted_name
   ```

**Pascal**

-  **Example 1**

   ```pascal
   procedure setName(some_input: string; var fix_name: string);
   begin
      fix_name := some_input;
   end;
   ```

-  **Example 2**

   ```pascal
   procedure formatName(var name: string; nim: string);
   var
      formatted_name: string;
   begin
      formatted_name := name + '_' + nim;
      name := formatted_name;
   end;
   ```   

### **3.4.4. Scope and Visibility**

**NOTAL**

```
PROGRAM ScopeExample

KAMUS
   globalVar: integer
   procedure testScope

ALGORITMA
   globalVar <- 10
   testScope
   output(globalVar)

procedure testScope
KAMUS
   localVar: integer
ALGORITMA
   localVar <- 20
   globalVar <- globalVar + localVar
   output('Local: ', localVar)
```

**Pascal**

```pascal
program ScopeExample;

var
   globalVar: integer;

procedure testScope; forward;

procedure testScope;
var
   localVar: integer;
begin
   localVar := 20;
   globalVar := globalVar + localVar;
   writeln('Local: ', localVar)
end;

begin
   globalVar := 10;
   testScope;
   writeln('Global: ', globalVar);
end.
```

## **3.5. Pointer Operations and Memory Management**

### **3.5.1. Simple Pointer Declaration and Operations**

**NOTAL**

```
KAMUS
   ptr: pointer to integer
   name: pointer to string
   value: integer

ALGORITMA
   value <- 42
   ptr <- @value { Reference }
   output(^ptr) { Dereference }
   
   allocate(ptr)
   ptr^ <- 100
   output(ptr^)
   deallocate(ptr)

   allocate(name)
   name^ <- 'John'
   output(name^)
   deallocate(name)
```

**Pascal**

```pascal
var
   ptr: ^integer;
   name: ^string;
   value: integer;

begin
   value := 42;
   ptr := @value;
   writeln(ptr^);
   
   New(ptr);
   ptr^ := 100;
   writeln(ptr^);
   Dispose(ptr);
   
   New(name);
   name^ := 'John';
   writeln(name^);
   Dispose(name);
end.
```

### **3.5.2. Combination of Pointers and Dynamic Arrays**

**NOTAL**

```
KAMUS
   type TIntArray: array of integer
   ptrToArray: pointer to TIntArray

ALGORITMA
   allocate(ptrToArray) { Pointer allocation }
   allocate(ptrToArray^, 10) { Array allocation }
   
   ptrToArray^[1] <- 100
   ptrToArray^[2] <- 200
   
   output('Elemen 1: ', ptrToArray^[1])
   output('Elemen 2: ', ptrToArray^[2])
   
   deallocate[](ptrToArray^) { Array deallocation }
   deallocate(ptrToArray) { Pointer deallocation }
```

**Pascal**

```pascal
type
   TIntArray = array of integer;

var
   ptrToArray: ^TIntArray;

begin
   New(ptrToArray);
   SetLength(ptrToArray^, 10);
   
   ptrToArray^[1] := 100;
   ptrToArray^[2] := 200;
   
   writeln('Elemen 1: ', ptrToArray^[1]);
   writeln('Elemen 2: ', ptrToArray^[2]);
   
   SetLength(ptrToArray^, 0);
   Dispose(ptrToArray);
end.
```

### **3.5.3. Pointer Parameters in Procedures/Functions**

**NOTAL**

-  **Function**

```
function processData (ptr: pInt) -> boolean
{ Assume, pInt = ^integer }
KAMUS
   { Local scope }
ALGORITMA
   if ptr^ > 0 then
      -> true
   else
      -> false
```

-  **Procedure**

```
procedure initializeArray (input/output ptrArray: TIntArray, input size: integer)
{ Assume, TIntArray = array of integer }
KAMUS
   i: integer
ALGORITMA
   allocate(ptrArray, size)
   i traversal [1..size]
      ptrArray[i] <- i * 10
```

**Pascal**

-  **Function**

```pascal
function processData (ptr: pInt): boolean;
{ Assume, pInt = ^integer }
begin
   if ptr^ > 0 then
      begin
         processData := true;
      end;
   else
      begin
         processData := false;
      end;
end;
```

-  **Procedure**

```pascal
procedure initializeArray (var ptrArray: TIntArray; size: integer);
{ Assume, TIntArray = array of integer }
var
   i: integer;
begin
   SetLength(ptrArray, size);
   for i := 1 to size do
   begin
      ptrArray[i] := i * 10;
   end;
end;
```      

## **3.6. Input/Output and Casting**

### **3.6.1. Input/Output Operations**

**NOTAL**

| **Operations**         | **Syntax**                                                                                   |
| :--------------------- | :------------------------------------------------------------------------------------------- |
| **Input**              | `input(variableName)`                                                                        |
| **Basic Output**       | `output('Hello World')` or `output(variableName)`                                            |
| **Output with Format** | `output('The value of x is: ', x)` or `output('Result: ', result, ' with status: ', status)` |

**Pascal**

| **Operations**         | **Syntax**                                                                                      |
| :--------------------- | :---------------------------------------------------------------------------------------------- |
| **Input**              | `readln(variableName);`                                                                         |
| **Basic Output**       | `writeln('Hello World');` or `writeln(variableName);`                                           |
| **Output with Format** | `writeln('The value of x is ', x);` or `writeln('Result: ', result, ' with status: ', status);` |

### **3.6.2. Casting and Conversion Functions**

> [!NOTE]
> In NOTAL, all type conversion functions are considered **built-in**. This means you can use them directly in your algorithms without needing to write or include their implementations. The Pascal code provided in this document serves as a reference for how these functions are realized when the NOTAL code is transpiled.

#### **3.6.2.1. From String to Other Types**

**NOTAL**

-   **String to Integer**

    Returns a Boolean (true or false) indicating the success of the data type conversion process.

    ```
    program StringToIntegerDemo

    KAMUS
        success: boolean
        intResult: integer
        int64Result: integer64

    ALGORITMA
        output('--- Testing StringToInteger ---')

        { Test case 1: Valid integer string }
        success <- StringToInteger(' 123 ', intResult)
        output('StringToInteger('' 123 '') -> Success: ', success, ', Value: ', intResult)

        { Test case 2: Valid negative integer }
        success <- StringToInteger('-45', intResult)
        output('StringToInteger(''-45'') -> Success: ', success, ', Value: ', intResult)

        { Test case 3: Large number for Int64 }
        success <- StringToInteger('9876543210987654321', int64Result)
        output('StringToInteger(''9876543210987654321'') -> Success: ', success, ', Value: ', int64Result)

        { Test case 4: Invalid integer string }
        success <- StringToInteger('abc', intResult)
        output('StringToInteger(''abc'') -> Success: ', success, ', Value: ', intResult)

        { Test case 5: String that is out of range for Integer but valid for Int64 }
        success <- StringToInteger('3000000000', intResult)
        output('StringToInteger(''3000000000'') -> Success: ', success, ', Value: ', intResult)

        output('--- Test Complete ---')
    ```

-   **String to Real**

    Returns a Boolean (true or false) indicating the success of the data type conversion process.

    ```
    program StringToRealDemo

    KAMUS
        success: boolean
        realResult: real
        doubleResult: double
    
    ALGORITMA
        output('--- Testing StringToReal ---')

        { Test case 1: Valid real number string }
        success <- StringToReal(' 123.45 ', realResult)
        output('StringToReal('' 123.45 '') -> Success: ', success, ', Value: ', realResult:0:2)

        { Test case 2: Valid negative real number }
        success <- StringToReal('-98.76', doubleResult)
        output('StringToReal(''-98.76'') -> Success: ', success, ', Value: ', doubleResult:0:2)

        { Test case 3: Scientific notation }
        success <- StringToReal('1.2345E-2', doubleResult)
        output('StringToReal(''1.2345E-2'') -> Success: ', success, ', Value: ', doubleResult:0:5)

        { Test case 4: Invalid real number string }
        success <- StringToReal('12.34.56', realResult)
        output('StringToReal(''12.34.56'') -> Success: ', success, ', Value: ', realResult:0:2)

        { Test case 5: Empty string }
        success <- StringToReal('', doubleResult)
        output('StringToReal('''') -> Success: ', success, ', Value: ', doubleResult:0:2)

        output('--- Test Complete ---')
    ```

-   **String Hex to Integer**

    Returns a Boolean (true or false) indicating the success of the data type conversion process.
    
    ```
    program StringHexToIntegerDemo

    KAMUS
        success: boolean
        intResult: integer
        int64Result: integer64

    ALGORITMA
        output('--- Testing StringHexToInteger ---')

        { Test case 1: Valid hex string }
        success <- StringHexToInteger('FF', intResult)
        output('StringHexToInteger(''FF'') -> Success: ', success, ', Value: ', intResult)

        { Test case 2: Valid hex string with different case }
        success <- StringHexToInteger('aBcDeF', intResult)
        output('StringHexToInteger(''aBcDeF'') -> Success: ', success, ', Value: ', intResult)

        { Test case 3: Large hex for Int64 }
        success <- StringHexToInteger('7FFFFFFFFFFFFFFF', int64Result)
        output('StringHexToInteger(''7FFFFFFFFFFFFFFF'') -> Success: ', success, ', Value: ', int64Result)

        { Test case 4: Invalid hex string }
        success <- StringHexToInteger('XYZ', intResult)
        output('StringHexToInteger(''XYZ'') -> Success: ', success, ', Value: ', intResult)

        { Test case 5: Out of range for Integer }
        success <- StringHexToInteger('FFFFFFFF', intResult)
        output('StringHexToInteger(''FFFFFFFF'') -> Success: ', success, ', Value: ', intResult)

        output('--- Test Complete ---')
    ```

-   **String To Boolean**

    Returns a Boolean (true or false) indicating the success of the data type conversion process.

    ```
    program StringToBooleanDemo

    KAMUS
        success: boolean
        boolResult: boolean

    ALGORITMA
        output('--- Testing StringToBoolean ---')

        { Test case 1: 'true' string }
        success <- StringToBoolean(' TRUE ', boolResult)
        output('StringToBoolean('' TRUE '') -> Success: ', success, ', Value: ', boolResult)

        { Test case 2: 'false' string }
        success <- StringToBoolean('false', boolResult)
        output('StringToBoolean(''false'') -> Success: ', success, ', Value: ', boolResult)

        { Test case 3: '1' string }
        success <- StringToBoolean('1', boolResult)
        output('StringToBoolean(''1'') -> Success: ', success, ', Value: ', boolResult)

        { Test case 4: '0' string }
        success <- StringToBoolean(' 0 ', boolResult)
        output('StringToBoolean('' 0 '') -> Success: ', success, ', Value: ', boolResult)

        { Test case 5: Invalid string }
        success <- StringToBoolean('yes', boolResult)
        output('StringToBoolean(''yes'') -> Success: ', success, ', Value: ', boolResult)

        { Test case 6: Strings of numbers other than 0 are considered true  }
        success <- StringToBoolean(' 123 ', boolResult)
        output('StringToBoolean('' 123 '') -> Success: ', success, ', Value: ', boolResult)

        { Test case 7: Strings of numbers other than 0 are considered true, including negative numbers true }
        success <- StringToBoolean('-5', boolResult)
        output('StringToBoolean(''-5'') -> Success: ', success, ', Value: ', boolResult)

        output('--- Test Complete ---')
    ```

-   **String To Char**

    Returns a Boolean (true or false) indicating the success of the data type conversion process.

    ```
    program StringToCharDemo

    KAMUS
        success: boolean
        charResult: char

    ALGORITMA
        output('--- Testing StringToChar ---')

        { Test case 1: Valid single character string }
        success <- StringToChar(' A ', charResult)
        output('StringToChar('' A '') -> Success: ', success, ', Value: ', charResult)

        { Test case 2: Another valid character }
        success <- StringToChar('%', charResult)
        output('StringToChar(''%'') -> Success: ', success, ', Value: ', charResult)

        { Test case 3: Empty string }
        success <- StringToChar('', charResult)
        output('StringToChar('''') -> Success: ', success, ', Value: ', charResult)

        { Test case 4: Multi-character string }
        success <- StringToChar('Hello', charResult)
        output('StringToChar(''Hello'') -> Success: ', success, ', Value: ', charResult)

        output('--- Test Complete ---')
    ```

**Pascal**

-   **String to Integer**

    Returns a Boolean (true or false) indicating the success of the data type conversion process.

    ```pascal
    program StringToIntegerDemo;

    uses SysUtils;

    var
        success: Boolean;
        intResult: Integer;
        int64Result: Int64;

    function StringToInteger(const inStr: string; var outInt: Int64): Boolean; forward;
    function StringToInteger(const inStr: string; var outInt: Integer): Boolean; forward;

    { Function to safely convert a string to an Int64 }
    function StringToInteger(const inStr: string; var outInt: Int64): Boolean;
    var
        code: Integer;
        trimmedS: string;
    begin
        trimmedS := Trim(inStr);
        if trimmedS = '' then
            begin
                outInt := 0;
                StringToInteger := False;
                Exit;
            end;
        Val(trimmedS, outInt, code);
        StringToInteger := (code = 0);
    end;

    { Overloaded function for standard Integer }
    function StringToInteger(const inStr: string; var outInt: Integer): Boolean;
    var
        tempInt64: Int64;
    begin
        if StringToInteger(inStr, tempInt64) then
            begin
                if (tempInt64 >= Low(Integer)) and (tempInt64 <= High(Integer)) then
                    begin
                        outInt := Integer(tempInt64);
                        StringToInteger := True;
                    end
                else
                    begin
                        outInt := 0;
                        StringToInteger := False; // Out of range for Integer
                    end;
            end
        else
            begin
                outInt := 0;
                StringToInteger := False;
            end;
    end;

    begin
        WriteLn('--- Testing StringToInteger ---');

        { Test case 1: Valid integer string }
        success := StringToInteger(' 123 ', intResult);
        WriteLn('StringToInteger('' 123 '') -> Success: ', success, ', Value: ', intResult);

        { Test case 2: Valid negative integer }
        success := StringToInteger('-45', intResult);
        WriteLn('StringToInteger(''-45'') -> Success: ', success, ', Value: ', intResult);

        { Test case 3: Large number for Int64 }
        success := StringToInteger('9876543210987654321', int64Result);
        WriteLn('StringToInteger(''9876543210987654321'') -> Success: ', success, ', Value: ', int64Result);

        { Test case 4: Invalid integer string }
        success := StringToInteger('abc', intResult);
        WriteLn('StringToInteger(''abc'') -> Success: ', success, ', Value: ', intResult);

        { Test case 5: String that is out of range for Integer but valid for Int64 }
        success := StringToInteger('3000000000', intResult);
        WriteLn('StringToInteger(''3000000000'') -> Success: ', success, ', Value: ', intResult);

        WriteLn('--- Test Complete ---');
    end.
    ```

-   **String to Real**

    Returns a Boolean (true or false) indicating the success of the data type conversion process.

    ```pascal
    program StringToRealDemo;

    uses SysUtils;

    var
        success: Boolean;
        realResult: Real;
        doubleResult: Double;

    function StringToReal(const inStr: string; var outReal: Double): Boolean; forward;
    function StringToReal(const inStr: string; var outReal: Real): Boolean; forward;

    { Function to safely convert a string to a Double }
    function StringToReal(const inStr: string; var outReal: Double): Boolean;
    var
        code: Integer;
        trimmedS: string;
    begin
        trimmedS := Trim(inStr);
        if trimmedS = '' then
            begin
                outReal := 0.0;
                StringToReal := False;
                Exit;
            end;
        Val(trimmedS, outReal, code);
        StringToReal := (code = 0);
    end;

    { Overloaded function for standard Real }
    function StringToReal(const inStr: string; var outReal: Real): Boolean;
    var
        tempDouble: Double;
    begin
        if StringToReal(inStr, tempDouble) then
            begin
                { Free Pascal uses Double for Real, so this is a safe assignment }
                outReal := tempDouble;
                StringToReal := True;
            end
        else
            begin
                outReal := 0.0;
                StringToReal := False;
            end;
    end;

    begin
        WriteLn('--- Testing StringToReal ---');

        { Test case 1: Valid real number string }
        success := StringToReal(' 123.45 ', realResult);
        WriteLn('StringToReal('' 123.45 '') -> Success: ', success, ', Value: ', realResult:0:2);

        { Test case 2: Valid negative real number }
        success := StringToReal('-98.76', doubleResult);
        WriteLn('StringToReal(''-98.76'') -> Success: ', success, ', Value: ', doubleResult:0:2);

        { Test case 3: Scientific notation }
        success := StringToReal('1.2345E-2', doubleResult);
        WriteLn('StringToReal(''1.2345E-2'') -> Success: ', success, ', Value: ', doubleResult:0:5);

        { Test case 4: Invalid real number string }
        success := StringToReal('12.34.56', realResult);
        WriteLn('StringToReal(''12.34.56'') -> Success: ', success, ', Value: ', realResult:0:2);
        { Test case 5: Empty string }
        success := StringToReal('', doubleResult);
        WriteLn('StringToReal('''') -> Success: ', success, ', Value: ', doubleResult:0:2);

        WriteLn('--- Test Complete ---');
    end.
    ```

-   **String Hex to Integer**

    Returns a Boolean (true or false) indicating the success of the data type conversion process.
    
    ```pascal
    program StringHexToIntegerDemo;

    uses SysUtils;

    var
        success: Boolean;
        intResult: Integer;
        int64Result: Int64;   

    function StringHexToInteger(const inStr: string; var outInt: Int64): Boolean; forward;
    function StringHexToInteger(const inStr: string; var outInt: Integer): Boolean; forward;

    { Function to safely convert a hex string to an Int64 }
    function StringHexToInteger(const inStr: string; var outInt: Int64): Boolean;
    var
        code: Integer;
        trimmedS: string;
    begin
        trimmedS := Trim(inStr);
        if trimmedS = '' then
            begin
                outInt := 0;
                StringHexToInteger := False;
                Exit;
            end;
        { Free Pascal's Val function can handle '$' prefix for hex }
        Val('$' + trimmedS, outInt, code);
        StringHexToInteger := (code = 0);
    end;

    { Overloaded function for standard Integer }
    function StringHexToInteger(const inStr: string; var outInt: Integer): Boolean;
    var
        tempInt64: Int64;
    begin
        if StringHexToInteger(inStr, tempInt64) then
            begin
                if (tempInt64 >= Low(Integer)) and (tempInt64 <= High(Integer)) then
                    begin
                        outInt := Integer(tempInt64);
                        StringHexToInteger := True;
                    end
                else
                    begin
                        outInt := 0;
                        StringHexToInteger := False; { Out of range }
                    end;
            end
        else
            begin
                outInt := 0;
                StringHexToInteger := False;
            end;
    end;

    begin
        WriteLn('--- Testing StringHexToInteger ---');

        { Test case 1: Valid hex string }
        success := StringHexToInteger('FF', intResult);
        WriteLn('StringHexToInteger(''FF'') -> Success: ', success, ', Value: ', intResult);

        { Test case 2: Valid hex string with different case }
        success := StringHexToInteger('aBcDeF', intResult);
        WriteLn('StringHexToInteger(''aBcDeF'') -> Success: ', success, ', Value: ', intResult);

        { Test case 3: Large hex for Int64 }
        success := StringHexToInteger('7FFFFFFFFFFFFFFF', int64Result);
        WriteLn('StringHexToInteger(''7FFFFFFFFFFFFFFF'') -> Success: ', success, ', Value: ', int64Result);

        { Test case 4: Invalid hex string }
        success := StringHexToInteger('XYZ', intResult);
        WriteLn('StringHexToInteger(''XYZ'') -> Success: ', success, ', Value: ', intResult);

        { Test case 5: Out of range for Integer }
        success := StringHexToInteger('FFFFFFFF', intResult);
        WriteLn('StringHexToInteger(''FFFFFFFF'') -> Success: ', success, ', Value: ', intResult);

        WriteLn('--- Test Complete ---');
    end.
    ```

-   **String To Boolean**

    Returns a Boolean (true or false) indicating the success of the data type conversion process.

    ```pascal
    program StringToBooleanDemo;

    uses SysUtils;

    var
        success: Boolean;
        boolResult: Boolean;

    function StringToBoolean(const inStr: string; var outBool: Boolean): Boolean; forward;

    { Function to convert a string ('true', 'false', '1', '0') to a boolean }
    function StringToBoolean(const inStr: string; var outBool: Boolean): Boolean;
    var
        lowerS: string;
        numValue: Int64;
        code: Integer;
    begin
        lowerS := LowerCase(Trim(inStr));

        { First, try to convert the string to an integer }
        Val(lowerS, numValue, code);

        if code = 0 then { Numeric conversion was successful }
            begin
                outBool := (numValue <> 0); { Number other than 0 is considered true }
                StringToBoolean := True;
            end
        else if (lowerS = 'true') then { If not a number, check 'true' }
            begin
                outBool := True;
                StringToBoolean := True;
            end
        else if (lowerS = 'false') then { Check 'false' }
            begin
                outBool := False;
                StringToBoolean := True;
            end
        else 
            begin { not a number, not ‘true’, and not 'false' }
                outBool := False;
                StringToBoolean := False;
            end;
    end;

    begin
        WriteLn('--- Testing StringToBoolean ---');

        { Test case 1: 'true' string }
        success := StringToBoolean(' TRUE ', boolResult);
        WriteLn('StringToBoolean('' TRUE '') -> Success: ', success, ', Value: ', boolResult);

        { Test case 2: 'false' string }
        success := StringToBoolean('false', boolResult);
        WriteLn('StringToBoolean(''false'') -> Success: ', success, ', Value: ', boolResult);

        { Test case 3: '1' string }
        success := StringToBoolean('1', boolResult);
        WriteLn('StringToBoolean(''1'') -> Success: ', success, ', Value: ', boolResult);

        { Test case 4: '0' string }
        success := StringToBoolean(' 0 ', boolResult);
        WriteLn('StringToBoolean('' 0 '') -> Success: ', success, ', Value: ', boolResult);

        { Test case 5: Invalid string }
        success := StringToBoolean('yes', boolResult);
        WriteLn('StringToBoolean(''yes'') -> Success: ', success, ', Value: ', boolResult);

        { Test case 6: Strings of numbers other than 0 are considered true  }
        success := StringToBoolean(' 123 ', boolResult);
        WriteLn('StringToBoolean('' 123 '') -> Success: ', success, ', Value: ', boolResult);

        { Test case 7: Strings of numbers other than 0 are considered true, including negative numbers true }
        success := StringToBoolean('-5', boolResult);
        WriteLn('StringToBoolean(''-5'') -> Success: ', success, ', Value: ', boolResult);

        WriteLn('--- Test Complete ---');
    end.
    ```

-   **String To Char**

    Returns a Boolean (true or false) indicating the success of the data type conversion process.

    ```pascal
    program StringToCharDemo;

    uses SysUtils;

    var
        success: Boolean;
        charResult: Char;

    function StringToChar(const inStr: string; var outChar: Char): Boolean; forward;

    { Function to convert a single-character string to a Char }
    function StringToChar(const inStr: string; var outChar: Char): Boolean;
    var
        trimmedS: string;
    begin
        trimmedS := Trim(inStr);
        if Length(trimmedS) = 1 then
            begin
                outChar := trimmedS[1];
                StringToChar := True;
            end
        else
            begin
                outChar := #0; { Null character for failure }
                StringToChar := False;
            end;
    end;

    begin
        WriteLn('--- Testing StringToChar ---');

        { Test case 1: Valid single character string }
        success := StringToChar(' A ', charResult);
        WriteLn('StringToChar('' A '') -> Success: ', success, ', Value: ', charResult);

        { Test case 2: Another valid character }
        success := StringToChar('%', charResult);
        WriteLn('StringToChar(''%'') -> Success: ', success, ', Value: ', charResult);

        { Test case 3: Empty string }
        success := StringToChar('', charResult);
        WriteLn('StringToChar('''') -> Success: ', success, ', Value: ', charResult);

        { Test case 4: Multi-character string }
        success := StringToChar('Hello', charResult);
        WriteLn('StringToChar(''Hello'') -> Success: ', success, ', Value: ', charResult);

        WriteLn('--- Test Complete ---');
    end.
    ```

#### **3.6.2.2. From Boolean to Other Types**

**NOTAL**

-   **Boolean to String**

    Converts a boolean value to its string representation ("True" or "False"). This conversion cannot fail.
    
    ```
    program BooleanToStringDemo

    KAMUS
        strResult: string

    ALGORITMA
        output('--- Testing BooleanToString ---')

        { Test case 1: True value }
        BooleanToString(true, strResult)
        output('BooleanToString(true) -> Value: ', strResult)

        { Test case 2: False value }
        BooleanToString(false, strResult)
        output('BooleanToString(false) -> Value: ', strResult)

        output('--- Test Complete ---')
    ```

-   **Boolean to Integer**

    Converts a boolean value to an integer (True becomes 1, False becomes 0). This conversion cannot fail.

    ```
    program BooleanToIntegerDemo

    KAMUS
        intResult: integer

    ALGORITMA
        output('--- Testing BooleanToInteger ---')

        { Test case 1: True value }
        BooleanToInteger(true, intResult)
        output('BooleanToInteger(true) -> Value: ', intResult)

        { Test case 2: False value }
        BooleanToInteger(false, intResult)
        output('BooleanToInteger(false) -> Value: ', intResult)

        output('--- Test Complete ---')
    ```

-   **Boolean to Real**

    Converts a boolean value to a real number (True becomes 1.0, False becomes 0.0). This conversion cannot fail.

    ```
    program BooleanToRealDemo

    KAMUS
        realResult: real

    ALGORITMA
        output('--- Testing BooleanToReal ---')

        { Test case 1: True value }
        BooleanToReal(true, realResult)
        output('BooleanToReal(true) -> Value: ', realResult:0:1)

        { Test case 2: False value }
        BooleanToReal(false, realResult)
        output('BooleanToReal(false) -> Value: ', realResult:0:1)

        output('--- Test Complete ---')
    ```

-   **Boolean to Char**

    Converts a boolean value to a character ('T' for True, 'F' for False). This conversion always succeeds.

    ```
    program BooleanToCharDemo

    KAMUS
        charResult: char
        success: boolean

    ALGORITMA
        output('--- Testing BooleanToChar ---')

        { Test case 1: True value }
        success <- BooleanToChar(true, charResult)
        output('BooleanToChar(true) -> Success: ', success, ', Value: ', charResult)

        { Test case 2: False value }
        success <- BooleanToChar(false, charResult)
        output('BooleanToChar(false) -> Success: ', success, ', Value: ', charResult)

        output('--- Test Complete ---')
    ```

**Pascal**

-   **Boolean to String**

    Converts a boolean value to its string representation ("True" or "False"). This conversion cannot fail.

    ```pascal
    program BooleanToStringDemo;

    uses SysUtils;

    var
        strResult: string;

    procedure BooleanToString(inBool: Boolean; var outStr: string); forward;

    procedure BooleanToString(inBool: Boolean; var outStr: string);
    begin
        if inBool then
            outStr := 'True'
        else
            outStr := 'False';
    end;

    begin
        WriteLn('--- Testing BooleanToString ---');

        { Test case 1: True value }
        BooleanToString(True, strResult);
        WriteLn('BooleanToString(True) -> Value: ', strResult);

        { Test case 2: False value }
        BooleanToString(False, strResult);
        WriteLn('BooleanToString(False) -> Value: ', strResult);

        WriteLn('--- Test Complete ---');
    end.
    ```

-   **Boolean to Integer**

    Converts a boolean value to an integer (True becomes 1, False becomes 0). This conversion cannot fail.

    ```pascal
    program BooleanToIntegerDemo;

    uses SysUtils;

    var
        intResult: Integer;

    procedure BooleanToInteger(inBool: Boolean; var outInt: Integer); forward;

    procedure BooleanToInteger(inBool: Boolean; var outInt: Integer);
    begin
        if inBool then
            outInt := 1
        else
            outInt := 0;
    end;

    begin
        WriteLn('--- Testing BooleanToInteger ---');

        { Test case 1: True value }
        BooleanToInteger(True, intResult);
        WriteLn('BooleanToInteger(True) -> Value: ', intResult);

        { Test case 2: False value }
        BooleanToInteger(False, intResult);
        WriteLn('BooleanToInteger(False) -> Value: ', intResult);

        WriteLn('--- Test Complete ---');
    end.
    ```

-   **Boolean to Real**

    Converts a boolean value to a real number (True becomes 1.0, False becomes 0.0). This conversion cannot fail.

    ```pascal
    program BooleanToRealDemo;

    uses SysUtils;

    var
        realResult: Real;

    procedure BooleanToReal(inBool: Boolean; var outReal: Real); forward;

    procedure BooleanToReal(inBool: Boolean; var outReal: Real);
    begin
        if inBool then
            outReal := 1.0
        else
            outReal := 0.0;
    end;

    begin
        WriteLn('--- Testing BooleanToReal ---');

        { Test case 1: True value }
        BooleanToReal(True, realResult);
        WriteLn('BooleanToReal(True) -> Value: ', realResult:0:1);

        { Test case 2: False value }
        BooleanToReal(False, realResult);
        WriteLn('BooleanToReal(False) -> Value: ', realResult:0:1);

        WriteLn('--- Test Complete ---');
    end.
    ```

-   **Boolean to Char**

    Converts a boolean value to a character ('T' for True, 'F' for False). This conversion always succeeds.

    ```pascal
    program BooleanToCharDemo;

    uses SysUtils;

    var
        charResult: Char;
        success: Boolean;

    function BooleanToChar(inBool: Boolean; var outChar: Char): Boolean; forward;

    function BooleanToChar(inBool: Boolean; var outChar: Char): Boolean;
    begin
        if inBool then
            outChar := 'T'
        else
            outChar := 'F';
        Result := True;
    end;

    begin
        WriteLn('--- Testing BooleanToChar ---');

        { Test case 1: True value }
        success := BooleanToChar(True, charResult);
        WriteLn('BooleanToChar(True) -> Success: ', success, ', Value: ', charResult);

        { Test case 2: False value }
        success := BooleanToChar(False, charResult);
        WriteLn('BooleanToChar(False) -> Success: ', success, ', Value: ', charResult);

        WriteLn('--- Test Complete ---');
    end.
    ```

#### **3.6.2.3. From Char to Other Types**

**NOTAL**

-   **Char to String**

    Converts a single character to a string. This conversion cannot fail.

    ```
    program CharToStringDemo

    KAMUS
        strResult: string

    ALGORITMA
        output('--- Testing CharToString ---')

        { Test case 1: Uppercase letter }
        CharToString('Z', strResult)
        output('CharToString(''Z'') -> Value: ', strResult)

        { Test case 2: Lowercase letter }
        CharToString('a', strResult)
        output('CharToString(''a'') -> Value: ', strResult)

        { Test case 3: Digit }
        CharToString('5', strResult)
        output('CharToString(''5'') -> Value: ', strResult)

        { Test case 4: Symbol }
        CharToString('#', strResult)
        output('CharToString(''#'') -> Value: ', strResult)

        output('--- Test Complete ---')
    ```

-   **Char to Integer**

    Returns a Boolean (true or false) indicating the success of the data type conversion process. Succeeds only if the character is a digit from '0' to '9'.

    ```
    program CharToIntegerDemo

    KAMUS
        success: boolean
        intResult: integer

    ALGORITMA
        output('--- Testing CharToInteger ---')

        { Test case 1: Valid numeric character '9' }
        success <- CharToInteger('9', intResult)
        output('CharToInteger(''9'') -> Success: ', success, ', Value: ', intResult)

        { Test case 2: Valid numeric character '0' }
        success <- CharToInteger('0', intResult)
        output('CharToInteger(''0'') -> Success: ', success, ', Value: ', intResult)

        { Test case 3: Non-numeric character }
        success <- CharToInteger('A', intResult)
        output('CharToInteger(''A'') -> Success: ', success, ', Value: ', intResult)

        { Test case 4: Symbol }
        success <- CharToInteger('*', intResult)
        output('CharToInteger(''*'') -> Success: ', success, ', Value: ', intResult)

        output('--- Test Complete ---')
    ```

-   **Char to Real**

    Returns a Boolean (true or false) indicating the success of the data type conversion process. Succeeds only if the character is a digit from '0' to '9'.

    ```
    program CharToRealDemo

    KAMUS
        success: boolean
        realResult: real

    ALGORITMA
        output('--- Testing CharToReal ---')

        { Test case 1: Valid numeric character '5' }
        success <- CharToReal('5', realResult)
        output('CharToReal(''5'') -> Success: ', success, ', Value: ', realResult:0:1)

        { Test case 2: Valid numeric character '0' }
        success <- CharToReal('0', realResult)
        output('CharToReal(''0'') -> Success: ', success, ', Value: ', realResult:0:1)

        { Test case 3: Non-numeric character }
        success <- CharToReal('X', realResult)
        output('CharToReal(''X'') -> Success: ', success, ', Value: ', realResult:0:1)

        output('--- Test Complete ---')
    ```

-   **Char to Boolean**

    Returns a Boolean (true or false) indicating the success of the data type conversion process. Succeeds if the character is 'T', 't', 'F', 'f', or any digit.

    ```
    program CharToBooleanDemo

    KAMUS
        success: boolean
        boolResult: boolean

    ALGORITMA
        output('--- Testing CharToBoolean ---')

        { Test case 1: 'T' character }
        success <- CharToBoolean('T', boolResult)
        output('CharToBoolean(''T'') -> Success: ', success, ', Value: ', boolResult)

        { Test case 2: 'f' character }
        success <- CharToBoolean('f', boolResult)
        output('CharToBoolean(''f'') -> Success: ', success, ', Value: ', boolResult)

        { Test case 3: '1' character }
        success <- CharToBoolean('1', boolResult)
        output('CharToBoolean(''1'') -> Success: ', success, ', Value: ', boolResult)

        { Test case 4: '0' character }
        success <- CharToBoolean('0', boolResult)
        output('CharToBoolean(''0'') -> Success: ', success, ', Value: ', boolResult)

        { Test case 5: Any other digit (e.g., '5') }
        success <- CharToBoolean('5', boolResult)
        output('CharToBoolean(''5'') -> Success: ', success, ', Value: ', boolResult)

        { Test case 6: Invalid character }
        success <- CharToBoolean('Q', boolResult)
        output('CharToBoolean(''Q'') -> Success: ', success, ', Value: ', boolResult)

        output('--- Test Complete ---')
    ```

**Pascal**

-   **Char to String**

    Converts a single character to a string. This conversion cannot fail.

    ```pascal
    program CharToStringDemo;

    uses SysUtils;

    var
        strResult: string;

    procedure CharToString(inChar: Char; var outStr: string); forward;

    procedure CharToString(inChar: Char; var outStr: string);
    begin
        outStr := inChar;
    end;

    begin
        WriteLn('--- Testing CharToString ---');

        { Test case 1: Uppercase letter }
        CharToString('Z', strResult);
        WriteLn('CharToString(''Z'') -> Value: ', strResult);

        { Test case 2: Lowercase letter }
        CharToString('a', strResult);
        WriteLn('CharToString(''a'') -> Value: ', strResult);

        { Test case 3: Digit }
        CharToString('5', strResult);
        WriteLn('CharToString(''5'') -> Value: ', strResult);

        { Test case 4: Symbol }
        CharToString('#', strResult);
        WriteLn('CharToString(''#'') -> Value: ', strResult);

        WriteLn('--- Test Complete ---');
    end.
    ```

-   **Char to Integer**

    Returns a Boolean (true or false) indicating the success of the data type conversion process. Succeeds only if the character is a digit from '0' to '9'.

    ```pascal
    program CharToIntegerDemo;

    uses SysUtils;

    var
        success: Boolean;
        intResult: Integer;

    function CharToInteger(inChar: Char; var outInt: Integer): Boolean; forward;

    function CharToInteger(inChar: Char; var outInt: Integer): Boolean;
    begin
        if (inChar >= '0') and (inChar <= '9') then
        begin
            outInt := Ord(inChar) - Ord('0');
            Result := True;
        end
        else
        begin
            outInt := 0;
            Result := False;
        end;
    end;

    begin
        WriteLn('--- Testing CharToInteger ---');

        { Test case 1: Valid numeric character '9' }
        success := CharToInteger('9', intResult);
        WriteLn('CharToInteger(''9'') -> Success: ', success, ', Value: ', intResult);

        { Test case 2: Valid numeric character '0' }
        success := CharToInteger('0', intResult);
        WriteLn('CharToInteger(''0'') -> Success: ', success, ', Value: ', intResult);

        { Test case 3: Non-numeric character }
        success := CharToInteger('A', intResult);
        WriteLn('CharToInteger(''A'') -> Success: ', success, ', Value: ', intResult);

        { Test case 4: Symbol }
        success := CharToInteger('*', intResult);
        WriteLn('CharToInteger(''*'') -> Success: ', success, ', Value: ', intResult);

        WriteLn('--- Test Complete ---');
    end.
    ```

-   **Char to Real**

    Returns a Boolean (true or false) indicating the success of the data type conversion process. Succeeds only if the character is a digit from '0' to '9'.

    ```pascal
    program CharToRealDemo;

    uses SysUtils;

    var
        success: Boolean;
        realResult: Real;

    function CharToReal(inChar: Char; var outReal: Real): Boolean; forward;

    function CharToReal(inChar: Char; var outReal: Real): Boolean;
    var
        intResult: Integer;
    begin
        if (inChar >= '0') and (inChar <= '9') then
        begin
            intResult := Ord(inChar) - Ord('0');
            outReal := intResult;
            Result := True;
        end
        else
        begin
            outReal := 0.0;
            Result := False;
        end;
    end;

    begin
        WriteLn('--- Testing CharToReal ---');

        { Test case 1: Valid numeric character '5' }
        success := CharToReal('5', realResult);
        WriteLn('CharToReal(''5'') -> Success: ', success, ', Value: ', realResult:0:1);

        { Test case 2: Valid numeric character '0' }
        success := CharToReal('0', realResult);
        WriteLn('CharToReal(''0'') -> Success: ', success, ', Value: ', realResult:0:1);

        { Test case 3: Non-numeric character }
        success := CharToReal('X', realResult);
        WriteLn('CharToReal(''X'') -> Success: ', success, ', Value: ', realResult:0:1);

        WriteLn('--- Test Complete ---');
    end.
    ```

-   **Char to Boolean**

    Returns a Boolean (true or false) indicating the success of the data type conversion process. Succeeds if the character is 'T', 't', 'F', 'f', or any digit.

    ```pascal
    program CharToBooleanDemo;

    uses SysUtils;

    var
        success: Boolean;
        boolResult: Boolean;

    function CharToBoolean(inChar: Char; var outBool: Boolean): Boolean; forward;

    function CharToBoolean(inChar: Char; var outBool: Boolean): Boolean;
    var
        upC: Char;
    begin
        upC := UpCase(inChar);
        if (upC = 'T') or ((inChar >= '1') and (inChar <= '9')) then
        begin
            outBool := True;
            Result := True;
        end
        else if (upC = 'F') or (inChar = '0') then
        begin
            outBool := False;
            Result := True;
        end
        else
        begin
            outBool := False;
            Result := False;
        end;
    end;

    begin
        WriteLn('--- Testing CharToBoolean ---');

        { Test case 1: 'T' character }
        success := CharToBoolean('T', boolResult);
        WriteLn('CharToBoolean(''T'') -> Success: ', success, ', Value: ', boolResult);

        { Test case 2: 'f' character }
        success := CharToBoolean('f', boolResult);
        WriteLn('CharToBoolean(''f'') -> Success: ', success, ', Value: ', boolResult);

        { Test case 3: '1' character }
        success := CharToBoolean('1', boolResult);
        WriteLn('CharToBoolean(''1'') -> Success: ', success, ', Value: ', boolResult);

        { Test case 4: '0' character }
        success := CharToBoolean('0', boolResult);
        WriteLn('CharToBoolean(''0'') -> Success: ', success, ', Value: ', boolResult);

        { Test case 5: Any other digit (e.g., '5') }
        success := CharToBoolean('5', boolResult);
        WriteLn('CharToBoolean(''5'') -> Success: ', success, ', Value: ', boolResult);

        { Test case 6: Invalid character }
        success := CharToBoolean('Q', boolResult);
        WriteLn('CharToBoolean(''Q'') -> Success: ', success, ', Value: ', boolResult);

        WriteLn('--- Test Complete ---');
    end.
    ```

#### **3.6.2.4. From Integer to Other Types**

**NOTAL**

-   **Integer to String**

    Converts an integer to its string representation. This conversion cannot fail.

    ```
    program IntegerToStringDemo

    KAMUS
        strResult: string

    ALGORITMA
        output('--- Testing IntegerToString ---')

        { Test case 1: Positive integer }
        IntegerToString(123, strResult)
        output('IntegerToString(123) -> Value: ', strResult)

        { Test case 2: Negative integer }
        IntegerToString(-987, strResult)
        output('IntegerToString(-987) -> Value: ', strResult)

        { Test case 3: Zero }
        IntegerToString(0, strResult)
        output('IntegerToString(0) -> Value: ', strResult)

        { Test case 4: Large Int64 value }
        IntegerToString(9876543210987654321, strResult)
        output('IntegerToString(9876543210987654321) -> Value: ', strResult)
        
        { Test case 5: MaxInt }
        IntegerToString(2147483647, strResult)
        output('IntegerToString(MaxInt) -> Value: ', strResult)

        output('--- Test Complete ---')
    ```

-   **Integer to Hex String**

    Converts an integer to its hexadecimal string representation. This conversion cannot fail.

    ```
    program IntegerToHexStringDemo

    KAMUS
        strResult: string

    ALGORITMA
        output('--- Testing IntegerToHexString ---')

        { Test case 1: Common value }
        IntegerToHexString(255, strResult)
        output('IntegerToHexString(255) -> Value: ', strResult)

        { Test case 2: Another value }
        IntegerToHexString(4096, strResult)
        output('IntegerToHexString(4096) -> Value: ', strResult)

        { Test case 3: Zero }
        IntegerToHexString(0, strResult)
        output('IntegerToHexString(0) -> Value: ', strResult)

        { Test case 4: High(Integer) }
        IntegerToHexString(2147483647, strResult)
        output('IntegerToHexString(High(Integer)) -> Value: ', strResult)

        { Test case 5: High(Int64) }
        IntegerToHexString(9223372036854775807, strResult)
        output('IntegerToHexString(High(Int64)) -> Value: ', strResult)

        output('--- Test Complete ---')
    ```

-   **Integer to Real**

    Converts an integer to a real number. This conversion cannot fail.

    ```
    program IntegerToRealDemo

    KAMUS
        realResult: real
        doubleResult: double

    ALGORITMA
        output('--- Testing IntegerToReal ---')

        { Test case 1: Positive integer }
        IntegerToReal(42, realResult)
        output('IntegerToReal(42) -> Value: ', realResult:0:1)

        { Test case 2: Negative integer }
        IntegerToReal(-150, doubleResult)
        output('IntegerToReal(-150) -> Value: ', doubleResult:0:1)

        { Test case 3: Zero }
        IntegerToReal(0, realResult)
        output('IntegerToReal(0) -> Value: ', realResult:0:1)

        { Test case 4: Large integer to Double }
        IntegerToReal(1234567890, doubleResult)
        output('IntegerToReal(1234567890) -> Value: ', doubleResult:0:1)

        output('--- Test Complete ---')
    ```

-   **Integer to Char**

    Returns a Boolean (true or false) indicating the success of the data type conversion process. Succeeds only if the integer is a single digit (0-9).

    ```
    program IntegerToCharDemo

    KAMUS
        success: boolean
        charResult: char

    ALGORITMA
        output('--- Testing IntegerToChar ---')

        { Test case 1: Valid integer 0-9 }
        success <- IntegerToChar(7, charResult)
        output('IntegerToChar(7) -> Success: ', success, ', Value: ', charResult)

        { Test case 2: Another valid integer }
        success <- IntegerToChar(0, charResult)
        output('IntegerToChar(0) -> Success: ', success, ', Value: ', charResult)

        { Test case 3: Integer out of range (negative) }
        success <- IntegerToChar(-5, charResult)
        output('IntegerToChar(-5) -> Success: ', success, ', Value: ', charResult)

        { Test case 4: Integer out of range (positive) }
        success <- IntegerToChar(10, charResult)
        output('IntegerToChar(10) -> Success: ', success, ', Value: ', charResult)

        output('--- Test Complete ---')
    ```

-   **Integer to Boolean**

    Converts an integer to a boolean (0 becomes False, any other number becomes True). This conversion cannot fail.

    ```
    program IntegerToBooleanDemo

    KAMUS
        boolResult: boolean

    ALGORITMA
        output('--- Testing IntegerToBoolean ---')

        { Test case 1: Non-zero integer (should be True) }
        IntegerToBoolean(-1, boolResult)
        output('IntegerToBoolean(-1) -> Value: ', boolResult)

        { Test case 2: Another non-zero integer }
        IntegerToBoolean(123, boolResult)
        output('IntegerToBoolean(123) -> Value: ', boolResult)

        { Test case 3: Zero (should be False) }
        IntegerToBoolean(0, boolResult)
        output('IntegerToBoolean(0) -> Value: ', boolResult)

        { Test case 4: Large non-zero Int64 }
        IntegerToBoolean(9876543210, boolResult)
        output('IntegerToBoolean(9876543210) -> Value: ', boolResult)

        output('--- Test Complete ---')
    ```

**Pascal**

-   **Integer to String**

    Converts an integer to its string representation. This conversion cannot fail.

    ```pascal
    program IntegerToStringDemo;

    uses SysUtils;

    var
        strResult: string;

    procedure IntegerToString(inInt: Int64; var outStr: string); forward;
    procedure IntegerToString(inInt: Integer; var outStr: string); forward;

    procedure IntegerToString(inInt: Int64; var outStr: string);
    begin
        outStr := IntToStr(inInt);
    end;

    procedure IntegerToString(inInt: Integer; var outStr: string);
    begin
        IntegerToString(Int64(inInt), outStr);
    end;

    begin
        WriteLn('--- Testing IntegerToString ---');

        { Test case 1: Positive integer }
        IntegerToString(123, strResult);
        WriteLn('IntegerToString(123) -> Value: ', strResult);

        { Test case 2: Negative integer }
        IntegerToString(-987, strResult);
        WriteLn('IntegerToString(-987) -> Value: ', strResult);

        { Test case 3: Zero }
        IntegerToString(0, strResult);
        WriteLn('IntegerToString(0) -> Value: ', strResult);

        { Test case 4: Large Int64 value }
        IntegerToString(9876543210987654321, strResult);
        WriteLn('IntegerToString(9876543210987654321) -> Value: ', strResult);
        
        { Test case 5: MaxInt }
        IntegerToString(High(Integer), strResult);
        WriteLn('IntegerToString(High(Integer)) -> Value: ', strResult);

        WriteLn('--- Test Complete ---');
    end.
    ```

-   **Integer to Hex String**

    Converts an integer to its hexadecimal string representation. This conversion cannot fail.

    ```pascal
    program IntegerToHexStringDemo;

    uses SysUtils;

    var
        strResult: string;

    procedure IntegerToHexString(inInt: Int64; var outStr: string); forward;
    procedure IntegerToHexString(inInt: Integer; var outStr: string); forward;

    procedure IntegerToHexString(inInt: Int64; var outStr: string);
    begin
        outStr := IntToHex(inInt, 0);
    end;

    procedure IntegerToHexString(inInt: Integer; var outStr: string);
    begin
        IntegerToString(Int64(inInt), outStr);
    end;

    begin
        WriteLn('--- Testing IntegerToHexString ---');

        { Test case 1: Common value }
        IntegerToHexString(255, strResult);
        WriteLn('IntegerToHexString(255) -> Value: ', strResult);

        { Test case 2: Another value }
        IntegerToHexString(4096, strResult);
        WriteLn('IntegerToHexString(4096) -> Value: ', strResult);

        { Test case 3: Zero }
        IntegerToHexString(0, strResult);
        WriteLn('IntegerToHexString(0) -> Value: ', strResult);

        { Test case 4: High(Integer) }
        IntegerToHexString(High(Integer), strResult);
        WriteLn('IntegerToHexString(High(Integer)) -> Value: ', strResult);

        { Test case 5: High(Int64) }
        IntegerToHexString(High(Int64), strResult);
        WriteLn('IntegerToHexString(High(Int64)) -> Value: ', strResult);

        WriteLn('--- Test Complete ---');
    end.
    ```

-   **Integer to Real**

    Converts an integer to a real number. This conversion cannot fail.

    ```pascal
    program IntegerToRealDemo;

    uses SysUtils;

    var
        realResult: Real;
        doubleResult: Double;

    procedure IntegerToReal(inInt: Integer; var outReal: Real); forward;
    procedure IntegerToReal(inInt: Int64; var outReal: Double); forward;
    procedure IntegerToReal(inInt: Integer; var outReal: Double); forward;

    procedure IntegerToReal(inInt: Integer; var outReal: Real);
    begin
        outReal := inInt;
    end;

    procedure IntegerToReal(inInt: Int64; var outReal: Double);
    begin
        outReal := inInt;
    end;

    procedure IntegerToReal(inInt: Integer; var outReal: Double);
    begin
        outReal := inInt;
    end;

    begin
        WriteLn('--- Testing IntegerToReal ---');

        { Test case 1: Positive integer }
        IntegerToReal(42, realResult);
        WriteLn('IntegerToReal(42) -> Value: ', realResult:0:1);

        { Test case 2: Negative integer }
        IntegerToReal(-150, doubleResult);
        WriteLn('IntegerToReal(-150) -> Value: ', doubleResult:0:1);

        { Test case 3: Zero }
        IntegerToReal(0, realResult);
        WriteLn('IntegerToReal(0) -> Value: ', realResult:0:1);

        { Test case 4: Large integer to Double }
        IntegerToReal(1234567890, doubleResult);
        WriteLn('IntegerToReal(1234567890) -> Value: ', doubleResult:0:1);

        WriteLn('--- Test Complete ---');
    end.
    ```

-   **Integer to Char**

    Returns a Boolean (true or false) indicating the success of the data type conversion process. Succeeds only if the integer is a single digit (0-9).

    ```pascal
    program IntegerToCharDemo;

    uses SysUtils;

    var
        success: Boolean;
        charResult: Char;

    function IntegerToChar(inInt: Int64; var outChar: Char): Boolean; forward;
    function IntegerToChar(inInt: Integer; var outChar: Char): Boolean; forward;

    function IntegerToChar(inInt: Int64; var outChar: Char): Boolean;
    begin
        if (inInt >= 0) and (inInt <= 9) then
        begin
            outChar := Chr(Ord('0') + Trunc(inInt));
            Result := True;
        end
        else
        begin
            outChar := #0;
            Result := False;
        end;
    end;

    function IntegerToChar(inInt: Integer; var outChar: Char): Boolean;
    begin
        Result := IntegerToChar(Int64(inInt), outChar);
    end;

    begin
        WriteLn('--- Testing IntegerToChar ---');

        { Test case 1: Valid integer 0-9 }
        success := IntegerToChar(7, charResult);
        WriteLn('IntegerToChar(7) -> Success: ', success, ', Value: ', charResult);

        { Test case 2: Another valid integer }
        success := IntegerToChar(0, charResult);
        WriteLn('IntegerToChar(0) -> Success: ', success, ', Value: ', charResult);

        { Test case 3: Integer out of range (negative) }
        success := IntegerToChar(-5, charResult);
        WriteLn('IntegerToChar(-5) -> Success: ', success, ', Value: ', charResult);

        { Test case 4: Integer out of range (positive) }
        success := IntegerToChar(10, charResult);
        WriteLn('IntegerToChar(10) -> Success: ', success, ', Value: ', charResult);

        WriteLn('--- Test Complete ---');
    end.
    ```

-   **Integer to Boolean**

    Converts an integer to a boolean (0 becomes False, any other number becomes True). This conversion cannot fail.

    ```pascal
    program IntegerToBooleanDemo;

    uses SysUtils;

    var
        boolResult: Boolean;

    procedure IntegerToBoolean(inInt: Int64; var outBool: Boolean); forward;
    procedure IntegerToBoolean(inInt: Integer; var outBool: Boolean); forward;

    procedure IntegerToBoolean(inInt: Int64; var outBool: Boolean);
    begin
        outBool := (inInt <> 0);
    end;

    procedure IntegerToBoolean(inInt: Integer; var outBool: Boolean);
    begin
        IntegerToBoolean(Int64(inInt), outBool);
    end;

    begin
        WriteLn('--- Testing IntegerToBoolean ---');

        { Test case 1: Non-zero integer (should be True) }
        IntegerToBoolean(-1, boolResult);
        WriteLn('IntegerToBoolean(-1) -> Value: ', boolResult);

        { Test case 2: Another non-zero integer }
        IntegerToBoolean(123, boolResult);
        WriteLn('IntegerToBoolean(123) -> Value: ', boolResult);

        { Test case 3: Zero (should be False) }
        IntegerToBoolean(0, boolResult);
        WriteLn('IntegerToBoolean(0) -> Value: ', boolResult);

        { Test case 4: Large non-zero Int64 }
        IntegerToBoolean(9876543210, boolResult);
        WriteLn('IntegerToBoolean(9876543210) -> Value: ', boolResult);

        WriteLn('--- Test Complete ---');
    end.
    ```

#### **3.6.2.5. From Real to Other Types**

**NOTAL**

-   **Real to String**

    Converts a real number to its string representation. This conversion cannot fail.

    ```
    program RealToStringDemo

    KAMUS
        strResult: string

    ALGORITMA
        output('--- Testing RealToString ---')

        { Test case 1: Positive real number }
        RealToString(123.456, strResult)
        output('RealToString(123.456) -> Value: ', strResult)

        { Test case 2: Negative real number }
        RealToString(-987.654, strResult)
        output('RealToString(-987.654) -> Value: ', strResult)

        { Test case 3: Zero }
        RealToString(0.0, strResult)
        output('RealToString(0.0) -> Value: ', strResult)

        { Test case 4: Large double value }
        RealToString(1234567890.123456789, strResult)
        output('RealToString(1234567890.123456789) -> Value: ', strResult)

        output('--- Test Complete ---')
    ```

-   **Real to Integer**

    Converts a real number to an integer by rounding to the nearest whole number. This conversion cannot fail, but precision may be lost.

    ```
    program RealToIntegerDemo

    KAMUS
        intResult: integer
        int64Result: integer64

    ALGORITMA
        output('--- Testing RealToInteger ---')

        { Test case 1: Rounding up }
        RealToInteger(9.81, intResult)
        output('RealToInteger(9.81) -> Value: ', intResult)

        { Test case 2: Rounding down }
        RealToInteger(10.2, intResult)
        output('RealToInteger(10.2) -> Value: ', intResult)

        { Test case 3: Negative rounding }
        RealToInteger(-5.6, intResult)
        output('RealToInteger(-5.6) -> Value: ', intResult)

        { Test case 4: Large double to Int64 }
        RealToInteger(9876543210.123, int64Result)
        output('RealToInteger(9876543210.123) -> Value: ', int64Result)
        
        { Test case 5: Double that is out of range for Integer }
        RealToInteger(3000000000.0, intResult)
        output('RealToInteger(3000000000.0) -> Value: ', intResult)

        output('--- Test Complete ---')
    ```

-   **Real to Char**

    Returns a Boolean (true or false) indicating the success of the data type conversion process. Succeeds only if the real number has no fractional part and is a single digit (0-9).

    ```
    program RealToCharDemo

    KAMUS
        success: boolean
        charResult: char

    ALGORITMA
        output('--- Testing RealToChar ---')

        { Test case 1: Valid real number (8.0) }
        success <- RealToChar(8.0, charResult)
        output('RealToChar(8.0) -> Success: ', success, ', Value: ', charResult)

        { Test case 2: Valid real number (0.0) }
        success <- RealToChar(0.0, charResult)
        output('RealToChar(0.0) -> Success: ', success, ', Value: ', charResult)

        { Test case 3: Real number with fractional part }
        success <- RealToChar(8.1, charResult)
        output('RealToChar(8.1) -> Success: ', success, ', Value: ', charResult)

        { Test case 4: Real number out of range }
        success <- RealToChar(15.0, charResult)
        output('RealToChar(15.0) -> Success: ', success, ', Value: ', charResult)

        { Test case 5: Negative real number }
        success <- RealToChar(-3.0, charResult)
        output('RealToChar(-3.0) -> Success: ', success, ', Value: ', charResult)

        output('--- Test Complete ---')
    ```

-   **Real to Boolean**

    Converts a real number to a boolean (0.0 becomes False, any other number becomes True). This conversion cannot fail.

    ```
    program RealToBooleanDemo

    KAMUS
        boolResult: boolean

    ALGORITMA
        output('--- Testing RealToBoolean ---')

        { Test case 1: Non-zero real (should be True) }
        RealToBoolean(1.0, boolResult)
        output('RealToBoolean(1.0) -> Value: ', boolResult)

        { Test case 2: Another non-zero real }
        RealToBoolean(-0.0001, boolResult)
        output('RealToBoolean(-0.0001) -> Value: ', boolResult)

        { Test case 3: Zero (should be False) }
        RealToBoolean(0.0, boolResult)
        output('RealToBoolean(0.0) -> Value: ', boolResult)

        output('--- Test Complete ---')
    ```

**Pascal**

-   **Real to String**

    Converts a real number to its string representation. This conversion cannot fail.

    ```pascal
    program RealToStringDemo;

    uses SysUtils;

    var
        strResult: string;

    procedure RealToString(inReal: Double; var outStr: string); forward;
    procedure RealToString(inReal: Real; var outStr: string); forward;

    procedure RealToString(inReal: Double; var outStr: string);
    begin
        outStr := FloatToStr(inReal);
    end;

    procedure RealToString(inReal: Real; var outStr: string);
    begin
        RealToString(Double(inReal), outStr);
    end;

    begin
        WriteLn('--- Testing RealToString ---');

        { Test case 1: Positive real number }
        RealToString(123.456, strResult);
        WriteLn('RealToString(123.456) -> Value: ', strResult);

        { Test case 2: Negative real number }
        RealToString(-987.654, strResult);
        WriteLn('RealToString(-987.654) -> Value: ', strResult);

        { Test case 3: Zero }
        RealToString(0.0, strResult);
        WriteLn('RealToString(0.0) -> Value: ', strResult);

        { Test case 4: Large double value }
        RealToString(1234567890.123456789, strResult);
        WriteLn('RealToString(1234567890.123456789) -> Value: ', strResult);

        WriteLn('--- Test Complete ---');
    end.
    ```

-   **Real to Integer**

    Converts a real number to an integer by rounding to the nearest whole number. This conversion cannot fail, but precision may be lost.

    ```pascal
    program RealToIntegerDemo;

    uses SysUtils;

    var
        intResult: Integer;
        int64Result: Int64;

    procedure RealToInteger(inReal: Double; var outInt: Int64); forward;
    procedure RealToInteger(inReal: Real; var outInt: Integer); forward;
    procedure RealToInteger(inReal: Real; var outInt: Int64); forward;
    procedure RealToInteger(inReal: Double; var outInt: Integer); forward;

    procedure RealToInteger(inReal: Double; var outInt: Int64);
    begin
        outInt := Round(inReal);
    end;

    procedure RealToInteger(inReal: Real; var outInt: Integer);
    begin
        outInt := Round(inReal);
    end;

    procedure RealToInteger(inReal: Real; var outInt: Int64);
    begin
        outInt := Round(inReal);
    end;

    procedure RealToInteger(inReal: Double; var outInt: Integer);
    var
        tempInt64: Int64;
    begin
        tempInt64 := Round(inReal);
        if (tempInt64 >= Low(Integer)) and (tempInt64 <= High(Integer)) then
            outInt := Integer(tempInt64)
        else
            outInt := 0;
    end;

    begin
        WriteLn('--- Testing RealToInteger ---');

        { Test case 1: Rounding up }
        RealToInteger(9.81, intResult);
        WriteLn('RealToInteger(9.81) -> Value: ', intResult);

        { Test case 2: Rounding down }
        RealToInteger(10.2, intResult);
        WriteLn('RealToInteger(10.2) -> Value: ', intResult);

        { Test case 3: Negative rounding }
        RealToInteger(-5.6, intResult);
        WriteLn('RealToInteger(-5.6) -> Value: ', intResult);

        { Test case 4: Large double to Int64 }
        RealToInteger(9876543210.123, int64Result);
        WriteLn('RealToInteger(9876543210.123) -> Value: ', int64Result);
        
        { Test case 5: Double that is out of range for Integer }
        RealToInteger(3000000000.0, intResult);
        WriteLn('RealToInteger(3000000000.0) -> Value: ', intResult);

        WriteLn('--- Test Complete ---');
    end.
    ```

-   **Real to Char**

    Returns a Boolean (true or false) indicating the success of the data type conversion process. Succeeds only if the real number has no fractional part and is a single digit (0-9).

    ```pascal
    program RealToCharDemo;

    uses SysUtils;

    var
        success: Boolean;
        charResult: Char;

    function RealToChar(inReal: Double; var outChar: Char): Boolean; forward;
    function RealToChar(inReal: Real; var outChar: Char): Boolean; forward;

    function RealToChar(inReal: Double; var outChar: Char): Boolean;
    var
        intValue: Int64;
    begin
        if (inReal = Trunc(inReal)) and (inReal >= 0) and (inReal <= 9) then
        begin
            intValue := Trunc(inReal);
            outChar := Chr(Ord('0') + intValue);
            Result := True;
        end
        else
        begin
            outChar := #0;
            Result := False;
        end;
    end;

    function RealToChar(inReal: Real; var outChar: Char): Boolean;
    begin
        Result := RealToChar(Double(inReal), outChar);
    end;

    begin
        WriteLn('--- Testing RealToChar ---');

        { Test case 1: Valid real number (8.0) }
        success := RealToChar(8.0, charResult);
        WriteLn('RealToChar(8.0) -> Success: ', success, ', Value: ', charResult);

        { Test case 2: Valid real number (0.0) }
        success := RealToChar(0.0, charResult);
        WriteLn('RealToChar(0.0) -> Success: ', success, ', Value: ', charResult);

        { Test case 3: Real number with fractional part }
        success := RealToChar(8.1, charResult);
        WriteLn('RealToChar(8.1) -> Success: ', success, ', Value: ', charResult);

        { Test case 4: Real number out of range }
        success := RealToChar(15.0, charResult);
        WriteLn('RealToChar(15.0) -> Success: ', success, ', Value: ', charResult);

        { Test case 5: Negative real number }
        success := RealToChar(-3.0, charResult);
        WriteLn('RealToChar(-3.0) -> Success: ', success, ', Value: ', charResult);

        WriteLn('--- Test Complete ---');
    end.
    ```

-   **Real to Boolean**

    Converts a real number to a boolean (0.0 becomes False, any other number becomes True). This conversion cannot fail.

    ```pascal
    program RealToBooleanDemo;

    uses SysUtils;

    var
        boolResult: Boolean;

    procedure RealToBoolean(inReal: Double; var outBool: Boolean); forward;
    procedure RealToBoolean(inReal: Real; var outBool: Boolean); forward;

    procedure RealToBoolean(inReal: Double; var outBool: Boolean);
    begin
        outBool := (inReal <> 0.0);
    end;

    procedure RealToBoolean(inReal: Real; var outBool: Boolean);
    begin
        RealToBoolean(Double(inReal), outBool);
    end;

    begin
        WriteLn('--- Testing RealToBoolean ---');

        { Test case 1: Non-zero real (should be True) }
        RealToBoolean(1.0, boolResult);
        WriteLn('RealToBoolean(1.0) -> Value: ', boolResult);

        { Test case 2: Another non-zero real }
        RealToBoolean(-0.0001, boolResult);
        WriteLn('RealToBoolean(-0.0001) -> Value: ', boolResult);

        { Test case 3: Zero (should be False) }
        RealToBoolean(0.0, boolResult);
        WriteLn('RealToBoolean(0.0) -> Value: ', boolResult);

        WriteLn('--- Test Complete ---');
    end.
    ```

# **4. Transpiler Architecture and Implementation**

## **4.1. Core Transpiler Components**

### **4.1.1. Lexical Analyzer (Lexer)**

Lexer separates NOTAL's code into tokens. Token contains keyword, identifier, literal, operator, and delimiter. The Lexer's design should incorporate case sensitivity and adhere to valid identifier formation rules, including specific syntactic requirements for dynamic arrays. 

### **4.1.2. Syntax Parser**

To build an Abstract Syntax Tree (AST) from a token stream, a Recursive Descent Parser is employed. This top-down parsing strategy involves a set of mutually recursive procedures, each responsible for processing one of the grammar's non-terminals. The parser validates the sequence of tokens against the NOTAL grammar rules.

### **4.1.3. Semantic Analyzer**

The Semantic Analyzer traverses the Abstract Syntax Tree (AST) to verify the program's semantic correctness. Its key responsibilities include:

* **Type Checking:** Ensures that operators and functions are applied to compatible data types.  
* **Scope Resolution:** Verifies that all variables are declared within the correct scope before being used.  
* **Parameter Matching:** Checks if the number and types of arguments in a function or procedure call match its declaration.  
* **Constraint Validation:** Confirms that assignments to variables with constraints adhere to the defined rules.

### **4.1.4. Code Generator**

The Code Generator traverses the validated and annotated AST to produce the final Pascal source code. It maps each AST node to its corresponding Pascal syntax. For instance, a `traversal` loop node is converted into a `for..to..do` loop, and an assignment node (`<-`) is converted into a Pascal assignment statement (`:=`). This component is also responsible for generating helper procedures, such as setters for variables with constraints.

## **4.2. Technologies and Support Components**

### **4.2.1. Implementation Language (C++)**

C++ is selected for its high performance, strong control over memory management, and extensive standard libraries, which are crucial for building an efficient and robust transpiler.

### **4.2.2. Build System (Makefile)**

A Makefile is used as the build system to automate the compilation process. It contains a set of rules and dependencies that explicitly define how C++ source code files are compiled and linked to produce the transpiler's executable file. While not as portable as CMake for non-Unix environments, a Makefile provides precise control and is a *de facto* standard for many C++ projects in Linux/macOS environments.

### **4.2.3. Testing Framework (Google Test)**

Google Test (`gtest`) is utilized for creating comprehensive unit tests. This allows for the isolated testing of each transpiler component (Lexer, Parser, etc.) to ensure its correctness and reliability.

### **4.2.4. Command Line Interface (`cxxopts`)**

The *cxxopts* library is used to parse command-line options. This provides a user-friendly interface for users to specify the input NOTAL file and other potential flags for the transpiler.

### **4.2.5. Comprehensive and Strict Error Handler**

This component is crucial for providing meaningful feedback to the user. It is designed to detect, manage, and report syntax or semantic errors found during the transpilation process. When an error is encountered, the handler will stop the process and provide a clear message that includes the error type, line number, and a description to help the user easily locate and fix the issue in the NOTAL source code.

### **4.2.6. Logging System**

To ensure transparency of the transpilation process and assist in debugging, a dedicated logging system (e.g., using libraries like `spdlog` or `fmt`) is integrated. This system generates clear and structured log messages that detail each stage of the transpiler's operation, from tokenization and parsing to code generation. These logs are invaluable for both end-users who wish to understand the process and for developers during maintenance and further development.

# **5. Testing and Validation Strategy**

## **5.1. Comprehensive Test Cases**

### **5.1.1. Unit Testing**

Each function and class of the transpiler is tested in isolation. For example, testing the Lexer to ensure it correctly tokenizes various inputs, or testing specific parser functions to verify they build the correct AST sub-trees.

### **5.1.2. Integration Testing**

Tests are designed to verify that different components of the transpiler work together correctly. For instance, ensuring that the token stream from the Lexer is correctly parsed by the Parser, and the resulting AST is correctly processed by the Semantic Analyzer and Code Generator.

### **5.1.3. Dynamic Array Test Cases**

Specific tests are created to validate all operations on dynamic arrays, including declaration, allocation (`allocate`), multi-dimensional access, and deallocation (`deallocate[]`), ensuring they are transpiled to the correct `SetLength` calls in Pascal.

### **5.1.4. Regression Testing**

A full suite of existing tests is run automatically whenever a change is made to the codebase. This ensures that new features or bug fixes do not inadvertently break existing functionality.

## **5.2. Semantic Validation**

### **5.2.1. Type Safety**

   Test cases include code with deliberate type mismatches (e.g., assigning a string to an integer variable) to verify that the Semantic Analyzer correctly identifies and reports these errors.

### **5.2.2. Memory Management Validation**

   Tests focus on pointer and dynamic array operations to ensure that every `allocate` has a corresponding `deallocate`. This helps validate that the transpiled code does not introduce memory leaks through `New`/`Dispose` or `SetLength`.

### **5.2.3. Scope Resolution**

   Tests are designed to confirm the correct handling of global and local variables. This includes cases like accessing global variables from within procedures and ensuring local variables do not leak outside their intended scope.

### **5.2.4. Constraint Validation**

   Specific test cases are written to validate the constraint mechanism. This involves attempting to assign values that violate the defined constraints and verifying that the transpiled code correctly triggers the runtime error-handling logic (e.g., *Halt*).

# **6. Example Programs and Transpilation Result**

## **6.1. Example 1: Simple Calculator with Conditional Structure**

**NOTAL Code**

```
PROGRAM SimpleCalculator

KAMUS
   a, b, hasil: real
   op: character

ALGORITMA
   input(a)
   input(op)
   input(b)
   
   depend on (op)
      '+': hasil ← a + b
      '-': hasil ← a - b
      '*': hasil ← a * b
      '/':  if b <> 0 then 
               hasil ← a / b              
            else
               output("Error: Division by zero")

   output("Hasil: ", hasil)
```

**Transpiled Pascal Code**

```pascal
PROGRAM SimpleCalculator;

var
   a, b, hasil: real;
   op: char;

begin
   readln(a);
   readln(op);
   readln(b);
   
   case op of
      '+': hasil := a + b;
      '-': hasil := a - b;
      '*': hasil := a * b;
      '/':  if b <> 0 then
               hasil := a / b
            else
               writeln('Error: Division by zero');
   end;
   
   writeln('Hasil: ', hasil);   
end.
```

## **6.2. Example 2: Dynamic Array Operations and Procedures**

**NOTAL Code**

```
PROGRAM DynamicArrayExample

KAMUS
   data: array of integer
   procedure fillArray (input/output arr: array of integer, input size: integer)
   
ALGORITMA
   fillArray(data, 5)
   output(data[3])
   deallocate[](data)

procedure fillArray (input/output arr: array of integer, input size: integer)
KAMUS
   i: integer 
ALGORITMA
   allocate(arr, size)
   i traversal [1..size]
      arr[i] ← i * 10
```

**Transpiled Pascal Code**

```
program DynamicArrayExample;

type
  TIntArray = array of integer;

var
   data: TIntArray;

procedure fillArray (var arr: TIntArray; size: integer); forward;

procedure fillArray (var arr: TIntArray; size: integer);
var
   i: integer;
begin
   SetLength(arr, size);
   for i := 1 to size do
   begin
      arr[i] := i * 10;
   end;
end;

begin
   fillArray(data, 5);
   writeln(data[3]);
   SetLength(data, 0);
end.
```

## **6.3. Example 3: Pointer Operations and Data Structures**

**NOTAL Code**

```
PROGRAM PointerExample

KAMUS
   type Node: < 
      info: integer,
      next: pointer to Node
   >
   p, head: pointer to Node

ALGORITMA
   allocate(p)
   p^.info ← 100
   p^.next ← nil
   head ← p
   output(head^.info)
   deallocate(p)
```

**Transpiled Pascal Code**

```
program PointerExample;

type
   PNode = ^Node;
   Node = record
      info: integer;
      next: PNode;
   end;
   
var
   p, head: PNode;

begin 
   New(p);
   p^.info := 100;
   p^.next := nil;
   head := p;
   writeln(head^.info);
   Dispose(p);
end.
```

# **7. Conclusion and Future Plans**

## **7.1. Summary of Achievements**

This project successfully developed a transpiler capable of converting NOTAL source code into valid, executable Pascal programs. It effectively bridges the gap between algorithmic pseudocode and practical programming, providing an educational tool that reinforces fundamental concepts like data structures, control flow, and memory management. The modular architecture ensures the system is maintainable and robust.

## **7.2. Potential Future Development**

The foundation laid by this project opens up several avenues for future enhancement:

* **Multi-Language Support**: Extending the code generator to target other programming languages such as C++, Python, or Java.  
* **Modular Programming**: Introducing support for header files or modules to allow for larger, more complex projects.  
* **Enhanced Error Reporting**: Improving error messages to provide more context and suggestions for fixes, further enhancing its educational value.  
* **IDE Integration**: Developing a plugin for popular IDEs like VS Code to provide syntax highlighting and real-time transpilation for NOTAL.

# **8. Appendix**

## **8.1. Lexical Rules**

The Lexer recognizes the following token categories:

* Keywords: `PROGRAM`, `KAMUS`, `ALGORITMA`, `if`, `then`, `else`, `elif`, `depend`, `on`, `while`, `do`, `repeat`, `until`, `traversal`, `iterate`, `stop`, `times`, `procedure`, `function`, `type`, `constant`, `array`, `of`, `pointer`, `to`, `input`, `output`, `and`, `or`, `not`, `div`, `mod`.  
* Identifiers: A sequence of letters, digits, and underscores, not starting with a digit.  
* Literals: Integer (`123`), Real (`3.14`), String (`"Hello"`), Character (`'A'`), Boolean (`true`, `false`).  
* Operators: `<-`, `+`, `-`, `*`, `/`, `^`, `&`, `=`, `<>`, `<`, `>`, `<=`, `>=`.  
* Delimiters: `(`, `)`, `[`, `]`, `<`, `>`, `:`, `,`.  
* Comments: Content enclosed in `{` and `}`.

## **8.2. NOTAL Grammar (BNF/EBNF)**

A simplified EBNF representation of the NOTAL grammar:

```
program                   ::= 'PROGRAM' IDENTIFIER '{' (TEXT)* '}'
program_body program_body ::= [ dictionary_block ] algorithm_block
dictionary_block          ::= 'KAMUS' { declaration }
declaration               ::= constant_decl | type_decl | variable_decl
algorithm_block           ::= 'ALGORITMA' { statement }
statement                 ::= assignment | if_statement | loop_statement | io_statement | proc_call
assignment                ::= variable '<-' expression
if_statement              ::= 'if' condition 'then' { statement } [ 'else' { statement } ]
loop_statement            ::= while_loop | repeat_until_loop | traversal_loop
variable                  ::= IDENTIFIER { '[' expression ']' | '.' IDENTIFIER }` 
```

## **8.3. AST Specification**

The Abstract Syntax Tree is a hierarchical data structure representing the code.

* **ProgramNode**: The root of the tree, with children for `DictionaryNode` and `AlgorithmNode`.  
* **DictionaryNode**: Contains a list of declaration nodes (`ConstantDeclNode`, `TypeDeclNode`, `VariableDeclNode`).  
* **AlgorithmNode**: Contains a list of statement nodes.  
* **Statement Nodes**: Include `AssignmentNode`, `IfNode`, `WhileNode`, `TraversalNode`, `ProcedureCallNode`, `InputNode`, and `OutputNode`. Each node stores relevant information, such as the variable and expression for an assignment, or the condition and body for a loop.  
* **Expression Nodes**: Represent expressions, such as `BinaryOpNode` (for `a + b`), `LiteralNod` (for `10`), and `VariableNode` (for `x`).