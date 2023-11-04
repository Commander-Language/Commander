# Commander Specification
- [Commander Specification](#commander-specification)
  - [Installation, Setup, and Execution](#installation-setup-and-execution)
  - [Types](#types)
    - [int](#int)
      - [API](#api)
      - [Examples](#examples)
    - [float](#float)
      - [API](#api-1)
      - [Examples](#examples-1)
    - [bool](#bool)
      - [API](#api-2)
      - [Examples](#examples-2)
    - [string](#string)
      - [API](#api-3)
      - [Examples](#examples-3)
    - [array](#array)
      - [API](#api-4)
      - [Examples](#examples-4)
    - [tuple](#tuple)
      - [API](#api-5)
      - [Examples](#examples-5)
    - [function](#function)
      - [API](#api-6)
      - [Examples](#examples-6)
  - [Command Execution](#command-execution)
    - [Grammar](#grammar)
    - [Examples](#examples-7)
  - [Background (Asynchronous) Commands](#background-asynchronous-commands)
    - [Grammar](#grammar-1)
    - [Examples](#examples-8)
  - [Piping Commands](#piping-commands)
    - [Grammar](#grammar-2)
    - [Examples](#examples-9)
  - [Command Return Value](#command-return-value)
    - [Grammar](#grammar-3)
    - [Examples](#examples-10)
  - [Timeout Commands](#timeout-commands)
    - [Grammar](#grammar-4)
    - [Examples](#examples-11)
  - [Command Aliasing](#command-aliasing)
    - [Grammar](#grammar-5)
    - [Examples](#examples-12)
  - [Variables](#variables)
    - [Grammar](#grammar-6)
    - [Examples](#examples-13)
  - [While Loops](#while-loops)
    - [Grammar](#grammar-7)
    - [Examples](#examples-14)
  - [Do-While Loops](#do-while-loops)
    - [Grammar](#grammar-8)
    - [Examples](#examples-15)
  - [For Loops](#for-loops)
    - [Grammar](#grammar-9)
    - [Examples](#examples-16)
  - [If-Else Statements](#if-else-statements)
    - [Grammar](#grammar-10)
    - [Examples](#examples-17)
  - [Ternary Operator](#ternary-operator)
    - [Grammar](#grammar-11)
    - [Examples](#examples-18)
  - [Functions](#functions)
    - [Grammar](#grammar-12)
    - [Examples](#examples-19)
  - [Math Operations](#math-operations)
    - [Grammar (Unary)](#grammar-unary)
    - [Grammar (Binary)](#grammar-binary)
    - [Grammar (Comparison)](#grammar-comparison)
    - [Examples](#examples-20)
  - [Boolean Operations](#boolean-operations)
    - [Grammar](#grammar-13)
    - [Examples](#examples-21)
  - [String Concatenation](#string-concatenation)
    - [Grammar](#grammar-14)
    - [Examples](#examples-22)
  - [String Interpolation](#string-interpolation)
    - [Grammar](#grammar-15)
    - [Examples](#examples-23)
  - [Lambda Expressions](#lambda-expressions)
    - [Grammar](#grammar-16)
    - [Examples](#examples-24)
  - [User Defined Scopes](#user-defined-scopes)
    - [Grammar](#grammar-17)
    - [Examples](#examples-25)
  - [Comments](#comments)
    - [Examples](#examples-26)
  - [Code Separation and Libraries](#code-separation-and-libraries)
    - [Grammar](#grammar-18)
    - [Examples](#examples-27)
  - [Expressions](#expressions)
  - [API Functions](#api-functions)
    - [Grammar](#grammar-19)
    - [Examples](#examples-28)
  - [Errors](#errors)

<!-- TODO -->
## Installation, Setup, and Execution

<!-- TODO: List all types, with details. What is initialization syntax? What are the type specific functions/operations you can perform on them (toString() will be required for all of them) -->
## Types
### int
- 64 bit two's complement
- Max value: ```9223372036854775807```
- Min value: ```-9223372036854775808```
- Integer overflow is well-defined, so once overflow is reached, integers will wrap around.
- Default: ```0```
#### API
    1. toString() : <string>
#### Examples
```
42

-35

0
```
### float
- 64 bit double precision IEEE 754 Standard
- Max value: ```1.79769313486231570e308```
- Includes special cases: ```NaN```, ```Inf```, ```-Inf```
- Default: ```0.0```
#### API
    1. toString() : <string>
#### Examples
```
3.1415926

-7.0

6.022e23
```
### bool
- 8 bit
- Can be either ```true``` or ```false```
- Default: ```false```
#### API
    1. toString() : <string>
#### Examples
```
true

false
```
### string
- Strings start and end with ```"``` or ```'``` for single line strings, or ```"""``` for multi-line strings.
- Strings can include any ASCII character from 32 to 126
- Special characters like newlines and tabs can be included in strings via escape characters, which include:
  - ```\t``` for tabs
  - ```\n``` for new lines
  - ```\'``` for single quotations (only necessary if string is initialized using ```'```)
  - ```\"``` for double quotations (only necessary if string is initialized using ```"```)
  - ```\\``` for backslash
- Useful string operations such as [string concatenation](#string-concatenation) and [string interpolation](#string-interpolation) are also supported
- Default: ```""```
#### API
    1. charAt(<int>) : <string>
    2. startsWith(<string>) : <bool>
    3. endsWith(<string>) : <bool>
    4. includes(<string>) : <bool>
    5. indexOf(<string>) : <int>
    6. length() : <int>
    7. replace(<string>, <string>) : <string>
    8. replaceAll(<string>, <string>) : <string>
    9. substring(<int>) : <string>
    10. substring(<int>, <int>) : <string>
    11. trim() : <string>
    12. lower() : <string>
    13. upper() : <string>
    14. split(<string>) : <array:string>
#### Examples
```
"Hello World!"

'My favorite number is 3.\n\tIt is super cool!'

"""
This
is
a
multi-line
string
"""
```
### array
- Default: ```[]```
#### API
    1. toString() : <string>
#### Examples
```

```
### tuple
- Default: ```()```
#### API
    1. toString() : <string>
#### Examples
```

```
### function
- Default: ```() => void```
#### API
    1. toString() : <string>
#### Examples
```

```

<!-- TODO -->
## Command Execution
### Grammar
```

```
### Examples
```

```

<!-- TODO -->
## Background (Asynchronous) Commands
### Grammar
```

```
### Examples
```

```

<!-- TODO -->
## Piping Commands
### Grammar
```

```
### Examples
```

```

<!-- TODO -->
## Command Return Value
### Grammar
```

```
Commands return a tuple that contains: the standard output, the error output, and the exit code.
### Examples
```

```

<!-- TODO -->
## Timeout Commands
### Grammar
```

```
### Examples
```

```

<!-- TODO -->
## Command Aliasing
### Grammar
```

```
### Examples
```

```

## Variables
Set variables with ```=```. Use keyword ```const``` if you want the value to be immutable.
### Grammar
```
<stmt> : <variable> = <expr>
       : const <variable> = <expr>
```
### Examples
```
const pi = 3.14159
r = 5
area = pi * r ** 2
```

## While Loops
### Grammar
```
<stmt> : while ( <bool> ) {
             <stmt>
             ...
         }
```
### Examples
```
x = 10
while (x > 5) {
    print(x--)
}
```

## Do-While Loops
### Grammar
```
<stmt> : do {
             <stmt>
             ...
         } while ( <bool> )
```
### Examples
```
x = 10
do {
    print(x--)
} while (x > 5)
```

## For Loops
### Grammar
```
<stmt> : for ( <stmt> ; <bool> ; <stmt> ) {
             <stmt>
             ...
         }
```
### Examples
```
for (i = 0; i < 10; i++) {
    print(i)
}
```

<!-- TODO -->
## If-Else Statements
### Grammar
```
<stmt> : if ( <bool> ) {
             <stmt>
             ...
         }

       | if ( <bool> ) {
             <stmt>
             ...
         } else {
             <stmt>
             ...
         }

       | if ( <bool> ) {
             <stmt>
             ...
         } else if ( <bool> ) {
            <stmt>
            ...
         } ...

       | if ( <bool> ) {
             <stmt>
             ...
         } else if ( <bool> ) {
            <stmt>
            ...
         } ... else {
            <stmt>
            ...
         }
```
### Examples
```

```

## Ternary Operator
### Grammar
```
<expr> : <bool> ? <expr> : <expr>
```
### Examples
```
x = 1
print(x == 1 ? "One" : "Not One")
```

## Functions
### Grammar
```
<stmt> : <variable> ( <binding> , ... ) : <type> {
             <stmt>
             ...
         }
       | <variable> ( <binding> , ... ) {
             <stmt>
             ...
         }
```
Note that the grammar implies you may define functions inside of functions. Additionally, functions may be recursive (see examples below).

### Examples
```
hello() {
    echo “Hello World!”;
}
```
```
goodbye(): void {
    echo “Goodbye!”;
}
```
```
add(a, b) {
    echo f"Performing {a} + {b} ...";
    return a + b;
}
```
```
factorial(n: int): int {
    return n == 1 ? 1 : factorial(n - 1) * n;
}
```

<!-- TODO -->
## Math Operations
### Grammar (Unary)
```
<int>   : -<int>
        | --<int>
        | <int>--
        | ++<int>
        | <int>++

<float> : -<float>
        | --<float>
        | <float>--
        | ++<float>
        | <float>++
```
### Grammar (Binary)
```
<int>   : <int> + <int>
        | <int> - <int>
        | <int> * <int>
        | <int> / <int>
        | <int> % <int>
        | <int> ** <int>

<float> : <float> + <float>
        | <int> + <float>
        | <float> + <int>

        | <float> - <float>
        | <int> - <float>
        | <float> - <int>

        | <float> * <float>
        | <int> * <float>
        | <float> * <int>

        | <float> / <float>
        | <int> / <float>
        | <float> / <int>

        | <float> % <float>
        | <int> % <float>
        | <float> % <int>

        | <float> ** <float>
        | <int> ** <float>
        | <float> ** <int>
```
### Grammar (Comparison)
```
<bool> : <int | float> < <int | float>
       | <int | float> <= <int | float>
       | <int | float> > <int | float>
       | <int | float> >= <int | float>
       | <int | float> != <int | float>
       | <int | float> == <int | float>
```
### Examples
```

```

<!-- TODO -->
## Boolean Operations
### Grammar
```
<bool> : !<bool>
       : <bool> || <bool>
       : <bool> && <bool>
       : <bool> == <bool>
       : <bool> != <bool>
```
### Examples
```

```

## String Concatenation
### Grammar
```
<string> : <string> + ... + <string>
```
### Examples
```
hello = "Hello " + "world!\n"
```

## String Interpolation
### Grammar
```
<string> : $"...{ <expr> }..."
```
### Examples
```
x = 4
y = 6
sum = $"The sum of {x} and {y} is {x + y}"
```

<!-- TODO -->
## Lambda Expressions
### Grammar
```
<expr> : (<binding>, ...) : <type> -> <expr>
       | (<binding>, ...) -> <expr>
       | (<binding>, ...) : <type> -> {
             <stmt>
             ...
         }
       | (<binding>, ...) -> {
             <stmt>
             ...
         }
```
### Examples
```

```

## User Defined Scopes
### Grammar
```
<stmt> : {
             <stmt> 
             ...
         }
```
### Examples
```
{
    x = 5
    print(x) // Prints 5
}
print(x) // Throws error that x is not initialized
```

## Comments
You can use ```// <comment>``` to define a single line comment, or ```/* <comment> */``` to define a multi-line comment.
### Examples
```
// This is a single line comment

/*
This
is
a
multi-line
comment
*/
```

## Code Separation and Libraries
Use the ```import``` keyword followed by the path to the commander script file with code you would like to use, as defined below.
### Grammar
```
<stmt> : import <string>
```
### Examples
```
import "add.cmdr"
// The add() function is defined in add.cmdr, which adds two numbers.
print(add(3, 4))
```

<!-- TODO: Precedence, parentheses, statements as expressions, etc. -->
## Expressions


<!-- TODO: Separate into groups based on function (e.g. one group for file I/O, one for user I/O, one for math, etc.). Add others as needed -->
## API Functions
### Grammar
    1. read(<string>) : <string>
    2. write(<string>) : <string>
    3. scan(<string>) : <string>
    4. print(<string>) : void
    5. parseInt(<string> | <int> | <bool>) : <int>
    6. parseFloat(<string> | <int> | <bool>) : <float>
    7. parseBool(<string> | <int> | <float>) : <bool>
    8. sqrt(<int> | <float>) : <float>
    9. ln(<int> | <float>) : <float>
    10. log(<int> | <float>) : <float>
    11. abs(<int>) : <int>
    12. abs(<float>) : <float>
    13. floor(<int> | <float>) : <int>
    14. ceil(<int> | <float>) : <int>
    15. round(<int> | <float>) : <int>
    16. random() : <float>
    17. time() : <int>
    18. date() : <tuple>
    19. sleep(<int>) : void
    20. sin(<int> | <float>) : <float>
    21. cos(<int> | <float>) : <float>
    22. tan(<int> | <float>) : <float>
    23. csc(<int> | <float>) : <float>
    24. sec(<int> | <float>) : <float>
    25. cot(<int> | <float>) : <float>
    26. sinh(<int> | <float>) : <float>
    27. cosh(<int> | <float>) : <float>
    28. tanh(<int> | <float>) : <float>
    29. csch(<int> | <float>) : <float>
    30. sech(<int> | <float>) : <float>
    31. coth(<int> | <float>) : <float>
    32. arcsin(<int> | <float>) : <float>
    33. arccos(<int> | <float>) : <float>
    34. arctan(<int> | <float>) : <float>
    35. arccsc(<int> | <float>) : <float>
    36. arcsec(<int> | <float>) : <float>
    37. arccot(<int> | <float>) : <float>
    38. arcsinh(<int> | <float>) : <float>
    39. arccosh(<int> | <float>) : <float>
    40. arctanh(<int> | <float>) : <float>
    41. arccsch(<int> | <float>) : <float>
    42. arcsech(<int> | <float>) : <float>
    43. arccoth(<int> | <float>) : <float>
### Examples
```

```


<!-- TODO -->
## Errors
