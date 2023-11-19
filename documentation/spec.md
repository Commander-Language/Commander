# Commander Specification
- [Commander Specification](#commander-specification)
  - [General Info](#general-info)
    - [Legal Characters](#legal-characters)
    - [Precedence](#precedence)
  - [Grammar Explanation](#grammar-explanation)
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
  - [Timeout Commands](#timeout-commands)
    - [Grammar](#grammar-3)
    - [Examples](#examples-10)
  - [Command Aliasing](#command-aliasing)
    - [Grammar](#grammar-4)
    - [Examples](#examples-11)
  - [Variables](#variables)
    - [Grammar](#grammar-5)
    - [Examples](#examples-12)
  - [While Loops](#while-loops)
    - [Grammar](#grammar-6)
    - [Examples](#examples-13)
  - [Do-While Loops](#do-while-loops)
    - [Grammar](#grammar-7)
    - [Examples](#examples-14)
  - [For Loops](#for-loops)
    - [Grammar](#grammar-8)
    - [Examples](#examples-15)
  - [If-Else Statements](#if-else-statements)
    - [Grammar](#grammar-9)
    - [Examples](#examples-16)
  - [Ternary Operator](#ternary-operator)
    - [Grammar](#grammar-10)
    - [Examples](#examples-17)
  - [Functions](#functions)
    - [Grammar](#grammar-11)
    - [Examples](#examples-18)
  - [Math Operations](#math-operations)
    - [Grammar (Unary)](#grammar-unary)
    - [Grammar (Binary)](#grammar-binary)
    - [Grammar (Comparison)](#grammar-comparison)
    - [Examples](#examples-19)
  - [Boolean Operations](#boolean-operations)
    - [Grammar](#grammar-12)
    - [Examples](#examples-20)
  - [String Concatenation](#string-concatenation)
    - [Grammar](#grammar-13)
    - [Examples](#examples-21)
  - [String Interpolation](#string-interpolation)
    - [Grammar](#grammar-14)
    - [Examples](#examples-22)
  - [Lambda Expressions](#lambda-expressions)
    - [Grammar](#grammar-15)
    - [Examples](#examples-23)
  - [User I/O](#user-io)
    - [Grammar](#grammar-16)
    - [Examples](#examples-24)
  - [File I/O](#file-io)
    - [Grammar](#grammar-17)
    - [Examples](#examples-25)
  - [User Defined Scopes](#user-defined-scopes)
    - [Grammar](#grammar-18)
    - [Examples](#examples-26)
  - [Comments](#comments)
    - [Examples](#examples-27)
  - [Code Separation and Libraries](#code-separation-and-libraries)
    - [Grammar](#grammar-19)
    - [Examples](#examples-28)
  - [Custom Types](#custom-types)
    - [Grammar](#grammar-20)
    - [Examples](#examples-29)
  - [API Functions](#api-functions)
    - [Grammar](#grammar-21)
  - [Errors](#errors)


## General Info
### Legal Characters
Commander scripts can only contain ascii characters 32 through 126, with a couple exceptions, which are ascii 9 (tab), 10 (new line), and 13 (carriage return). Any other character included in the script will result in an error being thrown.

### Precedence
Operator precedence is define as follows, from highest precedence to lowest precedence:
- Postfix ```[]``` for indexing arrays or tuples, or ```++``` and ```--``` for incrementing and decrementing.
- Prefix operations ```!```, ```-```, ```++```, or ```--```.
- Exponential operator ```**```.
- Multiplicative binary operators ```*```, ```/```, and ```%```
- Additive binary operators ```+``` and ```-```
- Binary comparisons ```<```, ```>```, ```<=```, ```>=```, ```==```, ```!=```
- Boolean binary operators ```&&``` and ```||```

Note: you may override precedence anytime using parentheses ```()```.

## Grammar Explanation
- Statements, expressed with ```<stmt>```, are either a line or multiple lines of code that ultimately end with a ```;```. They are simply executed at runtime, and don't return anything.
- Expresions, expressed with ```<expr>```, is code that evaluates to a value of a certain type, and returns that value at runtime.
- Variables, expressed with ```<variable>```, represent a variable name.
- Bindings, expressed with ```<binding>```, have the grammar 
    ```
    <binding> : <variable> : <type>
    ```
- Types are expressed as ```<int>``` for ints, ```<bool>``` for booleans, ```<string>``` for strings, etc. A general type (any type) would be expressed as ```<type>```. These are mostly used in API function definitions.
- Expressions that are of a particular type will be expressed as ```<int_expr>``` for int expressions, ```bool_expr``` for boolean expressions, etc.
- If, for some reason, multiple grammars apply, then they are separated using |. For example, commands are a sequence of strings, command strings, or variables, and so each command argument would be represented as ```<string | command_string | variable>```.
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
- Strings start and end with ```"``` or ```'```. The strings can be multiline as well, so any newlines present in them will be included in the final string (unless the line ends with a ```\``` character, in which case the new line that comes after it will not be included in the final string).
- Useful string operations such as [string concatenation](#string-concatenation) and [string interpolation](#string-interpolation) are supported.
- Strings that start and end with ```'``` are very literal strings. Anything inside of them will be considered a string, including escape characters, with the exception of:
  - Curly braces, if the string is an interpolated string (i.e. has ```$``` right before the starting ```'```). Curly braces can be escaped with ```\{``` or ```\}```
  - Single quotations, which require the escape character ```\'``` to be included in the string.
  - Backslashes, which can be escaped with ```\\```
- Strings that start and end with ```"``` are literal strings, but can have other special escape characters and can also have variable interpolation (see [string interpolation](#string-interpolation)). The following characaters can be escaped in double quotation strings:
  - Curly braces, if the string is an interpolated string (i.e. has ```$``` right before the starting ```'```). Curly braces can be escaped with ```\{``` or ```\}```
  - Double quotations, which require the escape character ```\"``` to be included in the string.
  - Backslashes, which can be escaped with ```\\```.
  - Newlines, with escape character ```\n```.
  - Tabs, with escape character ```\t```.
  - Carriage return, with escape character ```\r```.
  - Dollarsign, with escape character ```\$```.
- Additionally, comparisons are defined for strings, for example ```==``` to check if two strings are the same, or ```<``` to check if a string come before another string alphabetically.
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

'I have $0 in my wallet :('

"
This
is
a
multi-line
string
"

$'1 + 2 = {1 + 2}'

"3 + 4 = ${3 + 4}"

"My name is $name"
```
### array
Arrays contain multiple elements of a single type. To index an array, you use the following grammar: ```<expr> : <variable> [ <int> ]```, i.e. use postfix ```[]``` with a number inside (which is zero-based, so 0 gets you the first element). If an attempt to index a non-existent item, an error is thrown.
- Default: ```[]```
#### API
    1. toString() : <string>
#### Examples
```
myNumbers = [1, 2, 3]

one = myNumbers[0]
```
### tuple
Tuples contain multiple elements of different types. To index a tuple, you use the following grammar: ```<expr> : <variable> [ <int> ]```, i.e. use postfix ```[]``` with a number inside (which is zero-based, so 0 gets you the first element). If an attempt to index a non-existent item, an error is thrown.
- Default: ```()```
#### API
    1. toString() : <string>
#### Examples
```
myItems = ("hello", 2, 3.14, true)

pi = myItems[2]
```
Functions are types that represent a single function, and can be called. Functions can be initialized in two different ways (see [Functions](#functions) and [Lambda Expressions](#lambda-expressions)), but either way the function is stored into a variable.
### function
- Default: ```() => void```
#### API
    1. toString() : <string>
#### Examples
```
add(a, b) {
    return a + b;
};

subtract = (a, b) => a - b;
```

## Command Execution
Commands are simply a series of strings or variables followed one after another. For commands, quotations are not required for strings (which we call command strings) unless the command arguments involve the following special characters:
- backticks ``` ` ```,
- semicolons ```;```, 
- pipes ```|```, 
- or ampersands ```&```. 

For variables to be used as a command argument, they must be of type string, and must be preceded by a ```$```. 

If a command is surrounded in backticks ``` `` ```, the command will return a tuple of three where the first item is a string representing the standard output, the second item is a string representing the error output, and the third item is an int representing the exit code of the command. Backticks around commands are the only way to run commands as an expression as opposed to a statement.

### Grammar
```
<command> : <command_string | string | variable> <command_string | string | variable> ... <command_string | string | variable>
<stmt>    : <command>
<tuple_expr>   : `<command>`
```
### Examples
```
echo "Hello World!";

git clone $url;

ls -a | grep directoryName;

output = `cat myfile.txt`;
```

## Background (Asynchronous) Commands
Commands may be run in the background as opposed to the foreground if they end with an ```&```. In the background, they run asynchronously, so the command will be running concurrently with the rest of the script that follows after. Additionally, since it will be run in the background, any output the command normally prints out will not be printed out. 
### Grammar
```
<command> : <command> &
```
### Examples
```
git clone $url1 &;
```

## Piping Commands
The output from one command may be used as input to another through piping, using the ```|``` character. The output of the command on the left hand side of ```|``` is piped into the command on the right hand side as input to it. There can be many commands chained like this, and they get evaluated from left to right.
### Grammar
```
<command> : <command> | ... | <command>
```
### Examples
```
ls -a | grep myDirectory;
```

<!-- TODO -->
## Timeout Commands
### Grammar
```

```
### Examples
```

```

## Command Aliasing
You can alias a command, which is especially useful for frequently used commands that you wish to not have to type out all the time. The alias variable is in turn interpreted as a command, and follows all command rules as defined in [Command Execution](#command-execution)
### Grammar
```
<stmt>    : alias <command_variable> = <command>
<command> : <command_variable>
```
### Examples
```
alias listAll = ls -a;

listAll | grep myDirectory;
```

## Variables
Variables must be set with ```=```. Use keyword ```const``` if you want the value to be immutable. Each variable has a type, which may be optionally expressed.
### Grammar
```
<stmt> : <binding> = <expr>
       | const <binding> = <expr>
       | <variable> = <expr>
       
<expr> : <variable>
```
### Examples
```
const pi = 3.14159;
r = 5;
area = pi * r ** 2;
```

## While Loops
Execute lines of code multiple times, so long as the ```<bool_expr>``` is true before it starts executing the statements each iteration. You may break out of the loop early with the ```break;``` statement, or begin a new iteration of the loop with the ```continue;``` statement.
### Grammar
```
<stmt> : while ( <bool_expr> ) {
             <stmt>
             ...
         }
```
### Examples
```
x = 10;
while (x > 5) {
    print(x--);
};
```

## Do-While Loops
Like a while loop, but will execute all statements first before considering the ```<bool_expr>```. You may break out of the loop early with the ```break;``` statement, or begin a new iteration of the loop with the ```continue;``` statement.
### Grammar
```
<stmt> : do {
             <stmt>
             ...
         } while ( <bool_expr> );
```
### Examples
```
x = 10;
do {
    print(x--);
} while (x > 5);
```

## For Loops
Similar to a while loop in that it will only execute one iteration of the statements if the ```<bool_expr>``` is true. However, it also executes the first statement in the parentheses before running the loop, and after iteration will execute the last statement contained in the parentheses.
### Grammar
```
<stmt> : for ( <stmt> ; <bool_expr> ; <stmt> ) {
             <stmt>
             ...
         };
```
### Examples
```
for (i = 0; i < 10; i++) {
    print(i);
}
```

## If-Else Statements
Good for controlling code flow. Code within an ```if``` statement is only executed if the ```<bool_expr>``` is true. If it isn't, it will execute what's in the ```else``` statement. A chain of if and else if can occur as well, and which ever ```if``` statement is true first will be the one to execute.
### Grammar
```
<stmt> : if ( <booL_expr> ) <stmt>;
       | if ( <booL_expr> ) {
             <stmt>
             ...
         }

       | if ( <booL_expr> ) {
             <stmt>
             ...
         } else {
             <stmt>
             ...
         }

       | if ( <booL_expr> ) {
             <stmt>
             ...
         } else if ( <booL_expr> ) {
            <stmt>
            ...
         } ...

       | if ( <booL_expr> ) {
             <stmt>
             ...
         } else if ( <booL_expr> ) {
            <stmt>
            ...
         } ... else {
            <stmt>
            ...
         }
```
### Examples
```
if (true) {
    echo "true";
} else {
    echo "false";
}
```

## Ternary Operator
Similar to an ```if``` statement, but is evaluated as an expression. If ```<bool_expr>``` is true, then the first expression is evaluated and returned, otherwise the second expression is evaluated and returned.
### Grammar
```
<expr> : <booL_expr> ? <expr> : <expr>
```
### Examples
```
x = 1;
print(x == 1 ? "One" : "Not One");
```

## Functions
Functions contain statements that can be called anywhere after it is defined. You call a function by name followed by parentheses containing any parameter values that wish to be passed into it. Functions can have any number of parameters, and an optional return type. Exit from a function with the ```return;``` statement if the return type is ```void```, otherwise an expression must follow (i.e. the grammar ```return <expr>;```). Note that the grammar implies you may define functions inside of functions. Additionally, functions may be recursive, meaning you can call the same function from within itself (see examples below).
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

<stmt | expr> : <variable> ( <expr>, ... )
```
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

## Math Operations
Math operations are used for doing basic math, namely addition (e.g. ```+```, ```++```, ```+=```), subtraction (e.g. ```-```, ```--```, or ```-=```), multiplication (e.g. ```*``` or ```*=```), division (e.g. ```/``` or ```/=```), modulo (e.g. ```%``` or ```%=```), and powers (e.g. ```**``` or ```**=```). Usually, expressions involving only ints will evaulate to an int, and expressions involving ints and floats or only floats will evaluate to a float. Additionally, it is important to note that modulo is defined for floats.

There are also comparison operations for numbers (i.e. ints and floats) that will compare whether numbers are equal, greater than, or less than each other.
### Grammar (Unary)
```
<int_expr>   : -<int_expr>
             | --<int_expr>
             | <int_expr>--
             | ++<int_expr>
             | <int_expr>++

<float_expr> : -<float_expr>
             | --<float_expr>
             | <float_expr>--
             | ++<float_expr>
             | <float_expr>++
```
### Grammar (Binary)
```
<int_expr>   : <int_expr> + <int_expr>
             | <int_expr> - <int_expr>
             | <int_expr> * <int_expr>
             | <int_expr> / <int_expr>
             | <int_expr> % <int_expr>
             | <int_expr> ** <int_expr>

<float_expr> : <float_expr> + <float_expr>
             | <int_expr> + <float_expr>
             | <float_expr> + <int_expr>

             | <float_expr> - <float_expr>
             | <int_expr> - <float_expr>
             | <float_expr> - <int_expr>

             | <float_expr> * <float_expr>
             | <int_expr> * <float_expr>
             | <float_expr> * <int_expr>

             | <float_expr> / <float_expr>
             | <int_expr> / <float_expr>
             | <float_expr> / <int_expr>

             | <float_expr> % <float_expr>
             | <int_expr> % <float_expr>
             | <float_expr> % <int_expr>

             | <float_expr> ** <float_expr>
             | <int_expr> ** <float_expr>
             | <float_expr> ** <int_expr>

<stmt> : <variable> += <int_expr | float_expr>
       | <variable> -= <int_expr | float_expr>
       | <variable> *= <int_expr | float_expr>
       | <variable> /= <int_expr | float_expr>
       | <variable> %= <int_expr | float_expr>
       | <variable> **= <int_expr | float_expr>
```
### Grammar (Comparison)
```
<bool_expr> : <int_expr | float_expr> < <int_expr | float_expr>
            | <int_expr | float_expr> <= <int_expr | float_expr>
            | <int_expr | float_expr> > <int_expr | float_expr>
            | <int_expr | float_expr> >= <int_expr | float_expr>
            | <int_expr | float_expr> != <int_expr | float_expr>
            | <int_expr | float_expr> == <int_expr | float_expr>
```
### Examples
```
x = 2

y += x

z = (x <= y) ? 3 ** 2 : 2 % 4;
```

## Boolean Operations
Operations for boolean logic comparison: ```!``` for not, ```||``` for or, and ```&&``` for and. Additionally, ```==``` and ```!=``` can be used.
### Grammar
```
<bool_expr> : !<bool_expr>
            | <bool_expr> || <bool_expr>
            | <bool_expr> && <bool_expr>
            | <bool_expr> == <bool_expr>
            | <bool_expr> != <bool_expr>
            | true
            | false
```
### Examples
```
if (true || false) {
    x = true != false;
}
```

## String Concatenation
Strings may be combined into new strings this way.
### Grammar
```
<string_expr> : <string_expr> + ... + <string_expr>
```
### Examples
```
hello = "Hello " + "world!\n"
```

## String Interpolation
Way to evaulate expressions and insert them, or variables into strings. In double quotation strings, you can interpolate the value of a variable with the variable name preceded by ```$```. A string preceded by a ```$``` before the quotations will allow for expressions to be interpolated within curly braces. Additionally, in double quotation strings, you may interpolate as well, you just need to include a ```$``` before the curly braces in the string.
### Grammar
```
<string> : $"...{ <expr> }..."
         | "...${ <expr> }..."
         | $'...{ <expr> }'
         | "...$<variable>..."
```
### Examples
```
x = 4
y = 6
sum = $"The sum of {x} and {y} is {x + y}"
```

## Lambda Expressions
Similar to functions, but evaulate to an expression, allowing them to be stored in variables or passed as parameters on the fly.
### Grammar
```
<function_expr> : (<binding>, ...) : <type> -> <expr>
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
add = (a, b) -> a+b;

subtract = () -> {
    return 3 - 1;
}
```

## User I/O
You may either read user input using ```scan```, which takes in a string to prompt the user with. Additionally, you may output strings to the console using ```echo``` or ```print``` along with the string being printed.
### Grammar
```
<string_expr> : scan <string_expr>
              | scan( <string_expr> )

<stmt>   : echo <string_expr>
         | print( <string_expr> )
         | println( <string_expr> )
```
### Examples
```
name = read "Tell me your name: ";

echo $"Your name is {name}";
```

## File I/O
You may read in a file as a string using ```read``` followed by the path to the file, or write a string to a file using ```write``` followed by the path to write to first and then followed by the content being written to the file. 
### Grammar
```
<string> : read <string>
         | read( <string> )
         | write <string> <string>
         | write( <string>, <string> )
```
### Examples
```
userinfo = read "documents/userinfo.txt";

write "documents/name.txt" "My name is Bob";
```

## User Defined Scopes
You may defined a custom scope in your commander script. Any variables or functions initialize within the scope will no longer exist once the scope is left.
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
<stmt> : import <string_expr>
```
### Examples
```
import "add.cmdr"
// The add() function is defined in add.cmdr, which adds two numbers.
print(add(3, 4))
```

## Custom Types
Use the ```type``` keyword followed by a type to defined a custom type.
### Grammar
```
<stmt> : type <type>
```
### Examples
```
type AddFunction (a: int, b: int) -> int;

add: AddFunction = (a, b) -> a + b;
```

<!-- TODO: Separate into groups based on function (e.g. one group for file I/O, one for user I/O, one for math, etc.). Add others as needed -->
## API Functions
### Grammar
    1. parseInt(<string> | <int> | <bool>) : <int>
    2. parseFloat(<string> | <int> | <bool>) : <float>
    3. parseBool(<string> | <int> | <float>) : <bool>
    4. sqrt(<int> | <float>) : <float>
    5. ln(<int> | <float>) : <float>
    6.  log(<int> | <float>) : <float>
    7.  abs(<int>) : <int>
    8.  abs(<float>) : <float>
    9.  floor(<int> | <float>) : <int>
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
    25. csch(<int> | <float>) : <float>
    26. sech(<int> | <float>) : <float>
    27. coth(<int> | <float>) : <float>
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

<!-- TODO -->
## Errors
- Divide by zero is an error that will be thrown at runtime if an attempt is made to divide by zero.