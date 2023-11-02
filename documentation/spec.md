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
    - [Grammar](#grammar-13)
    - [Examples](#examples-20)
  - [Boolean Operations](#boolean-operations)
    - [Grammar](#grammar-14)
    - [Examples](#examples-21)
  - [String Concatenation](#string-concatenation)
    - [Grammar](#grammar-15)
    - [Examples](#examples-22)
  - [String Interpolation](#string-interpolation)
    - [Grammar](#grammar-16)
    - [Examples](#examples-23)
  - [Lambda Expressions](#lambda-expressions)
    - [Grammar](#grammar-17)
    - [Examples](#examples-24)
  - [User Defined Scopes](#user-defined-scopes)
    - [Grammar](#grammar-18)
    - [Examples](#examples-25)
  - [File I/O](#file-io)
    - [Grammar](#grammar-19)
    - [Examples](#examples-26)
  - [User I/O](#user-io)
    - [Grammar](#grammar-20)
    - [Examples](#examples-27)
  - [Comments](#comments)
    - [Grammar](#grammar-21)
    - [Examples](#examples-28)
  - [Code Separation and Libraries](#code-separation-and-libraries)
    - [Grammar](#grammar-22)
    - [Examples](#examples-29)
  - [](#)
    - [Grammar](#grammar-23)
    - [Examples](#examples-30)
  - [API Functions](#api-functions)
    - [Grammar](#grammar-24)
    - [Examples](#examples-31)
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
  - ```\'``` for single quotations (only nessecary if string is initialized using ```'```)
  - ```\"``` for double quotations (only nessecary if string is initialized using ```"```)
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

<!-- TODO -->
## Variables
### Grammar
```

```
### Examples
```

```

<!-- TODO -->
## While Loops
### Grammar
```

```
### Examples
```

```

<!-- TODO -->
## Do-While Loops
### Grammar
```

```
### Examples
```

```

<!-- TODO -->
## For Loops
### Grammar
```

```
### Examples
```

```

<!-- TODO -->
## If-Else Statements
### Grammar
```

```
### Examples
```

```

<!-- TODO -->
## Ternary Operator
### Grammar
```

```
### Examples
```

```

## Functions
### Grammar
```
stmt  : <variable> ( <binding> , ... ) : <type> {
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
### Grammar
```

```
### Examples
```

```

<!-- TODO -->
## Boolean Operations
### Grammar
```

```
### Examples
```

```

<!-- TODO -->
## String Concatenation
### Grammar
```

```
### Examples
```

```

<!-- TODO -->
## String Interpolation
### Grammar
```

```
### Examples
```

```

<!-- TODO -->
## Lambda Expressions
### Grammar
```

```
### Examples
```

```

<!-- TODO -->
## User Defined Scopes
### Grammar
```

```
### Examples
```

```

<!-- TODO -->
## File I/O
### Grammar
```

```
### Examples
```

```

<!-- TODO -->
## User I/O
### Grammar
```

```
### Examples
```

```

<!-- TODO -->
## Comments
### Grammar
```

```
### Examples
```

```

<!-- TODO -->
## Code Separation and Libraries
### Grammar
```

```
### Examples
```

```

<!-- TODO -->
## 
### Grammar
```

```
### Examples
```

```

<!-- TODO: Separate into groups based on function. Add more -->
## API Functions
### Grammar
    1. parseInt(<string> | <int> | <bool>) : <int>
    2. parseFloat(<string> | <int> | <bool>) : <float>
    3. parseBool(<string> | <int> | <float>) : <bool>
    4. sqrt(<int> | <float>) : <float>
    5. ln(<int> | <float>) : <float>
    6. log(<int> | <float>) : <float>
    7. abs(<int>) : <int>
    8. abs(<float>) : <float>
    9. floor(<int> | <float>) : <int>
    10. ceil(<int> | <float>) : <int>
    11. round(<int> | <float>) : <int>
    12. random() : <float>
    13. time() : <int>
    14. date() : <tuple>
    15. sleep(<int>) : void
    16. sin(<int> | <float>) : <float>
    17. cos(<int> | <float>) : <float>
    18. tan(<int> | <float>) : <float>
    19. csc(<int> | <float>) : <float>
    20. sec(<int> | <float>) : <float>
    21. cot(<int> | <float>) : <float>
    22. sinh(<int> | <float>) : <float>
    23. cosh(<int> | <float>) : <float>
    24. tanh(<int> | <float>) : <float>
    24. csch(<int> | <float>) : <float>
    25. sech(<int> | <float>) : <float>
    26. coth(<int> | <float>) : <float>
    27. arcsin(<int> | <float>) : <float>
    28. arccos(<int> | <float>) : <float>
    29. arctan(<int> | <float>) : <float>
    30. arccsc(<int> | <float>) : <float>
    31. arcsec(<int> | <float>) : <float>
    32. arccot(<int> | <float>) : <float>
    33. arcsinh(<int> | <float>) : <float>
    34. arccosh(<int> | <float>) : <float>
    35. arctanh(<int> | <float>) : <float>
    36. arccsch(<int> | <float>) : <float>
    37. arcsech(<int> | <float>) : <float>
    38. arccoth(<int> | <float>) : <float>
### Examples
```

```


<!-- TODO -->
## Errors
