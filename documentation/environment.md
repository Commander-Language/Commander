# Coding Environment

## Building

Requirements:

* CMake 3.10+
* A generator (GNU make, ninja, ...) (usually included with CMake installation)
* A C++ compiler (GCC, Clang, MSVC, MinGW, ...)

Building is simple: invoke CMake and let it take care of the rest.

```bash
mkdir build && cd build
cmake ..
make
```

## CMake

[CMake](https://cmake.org/) will be used as the build system.
It has a bit of a learning curve, but once it's set up,
there will be little need for modification outside of copying other build
configurations to make new ones.
Cayden will be in charge of maintaining the `CMakeLists.txt` file,
which defines the build configurations.

One nice thing about CMake is that it works for virtually any operating system
and compiler.
One person can build with gcc and GNU Make on Linux, and another can compile
with MSVC and ninja on Windows, without changing the build file at all.

## Clang-Tidy

[Clang-Tidy](https://clang.llvm.org/extra/clang-tidy/index.html)
will be used for code linting (i.e., diagnosing and fixing typical programming
errors).

All checks are on by default.
Some unimportant or irrelevant checks are turned off.
This means that your code needs to be **very clean**.
Most warnings will give you instructions on what needs to change;
often, it will have automatic fixes that you can apply.
If a warning is confusing, search it on Google;
you'll be able to find a lot of good advice on Stack Overflow and elsewhere.

**All code must pass Clang-Tidy's checks before committing.**
This will make a **huge difference** when it comes to code correctness and
robustness.
It won't save you from *every* segmentation fault,
but it will save you from most.

It doesn't take long to get the hang of it and naturally start writing
code that passes the checks.

## ClangFormat

Notice that *Clang-Tidy* has a hyphen and *ClangFormat* does not.
This is not a typo.

[ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) is a handy tool
that will help us to keep our code clean, readable, and consistent.
It works in-place. It's incredibly easy to use.

The target style is described in detail later in this document.

**All code must be formatted with ClangFormat before committing.**

## CLion

[CLion](https://www.jetbrains.com/clion/) is an IDE from JetBrains
that has a rich feature set and powerful integration with `clangd`.

When you open the Commander project in CLion for the first time,
it will automatically load the build configurations from the `CMakeLists.txt`
file.
When you go to run the project, you can select any of the build configurations
from a drop-down menu.
You can set breakpoints and run a debugger.
It has a built-in compiler, which is especially handy on Windows,
and it can integrate with WSL to build Linux binaries as well.

This includes integration with Clang-Tidy.
Problematic code will be highlighted,
and hovering over that code will show a warning with a detailed description
of what needs to change.
Often, you can automatically apply the needed changes, with no intervention
outside of clicking on the button on the drop-down menu.

Furthermore, it integrates nicely with ClangFormat.
A single keyboard shortcut will run ClangFormat and apply the needed
style changes.

## Code Style

### Naming Conventions

The following case definitions come from
[Clang-Tidy documentation](https://clang.llvm.org/extra/clang-tidy/checks/readability/identifier-naming.html):

* `lower_case`
* `UPPER_CASE`
* `camelBack`
* `CamelCase`
* `camel_Snake_Back`
* `Camel_Snake_Case`
* `aNy_CasE`
* `Leading_upper_snake_case`

Here are the naming conventions for each symbol type:

* `class`es and `struct`s:
  * `CamelCase`
  * At least three letters.
  * Should be named after `WhatItRepresents`.
  * Up to three words are allowed,
      not including its super class if it's a derived type.

      For example, `WordWordWordExpr` would be allowed
      if it's a derived type of a class named `Expr`.
* Members of `class`es and `struct`s:
  * `camelBack`
  * At least three letters.
  * Includes both member functions and member variables.
  * Also includes static functions of a `class` or `struct`.
  * Should be named after `WhatItRepresents`.
  * Up to three words are allowed.
  * Should be `private` **unless** one of the following is true:
    * It's constant.
    * It makes sense for outsiders to directly modify it.
    * It needs to be used by a derived class
      (in which case it should be `protected`).
  * If the member is private or protected, it **must have the prefix** `_`.
  * Example:

    ```c++
    class Vehicle {
    private:
        //  Private: nothing external to the class needs access.
        Vin _vin;

        //  Private: even though it needs to be read outside
        //           of the class, it doesn't make sense for
        //           external environments to modify it.
        //           Therefore, a getter is provided instead.
        unsigned int odometerMileage;

    protected:
        //  Protected: subclasses `Car` and `Plane`
        //             need to know the fuel type.
        FuelType _fuelType;

    public:      
        //  Public: anyone can construct the vehicle.
        Vehicle(Vin vin);

        //  Public: anyone can get the mileage.
        //          It doesn't make sense for anyone to modify it,
        //          so it has a getter method instead of being
        //          a public variable.
        unsigned int getMileage();

        //  Public: anyone can rename their vehicle
        //          whenever they want to.
        std::string name;
    };
    ```

* `union`s:
  * Don't use `union`s.
* Variables, functions, and parameters:
  * `camelBack` (just like `class`/`struct` members)
  * At least three letters.
* `enum`s:
  * `CamelCase` (just like `class`es)
  * At least three letters.
  * Items in an `enum` are in `UPPER_CASE`.
  * Example:

    ```c++
    enum FuelType {
        GASOLINE,
        DIESEL,
        JET_FUEL,
        ELECTRIC
    };
    ```

* `namespace`s:
  * `CamelCase` (just like `class`es)
  * At least three letters.
  * Example:

    ```c++
    namespace MiscFunctions {
      void doFoo() {}

      void doBar() {}
    }
    ```

* Macros:
  * Don't use macros.

* Template parameters:
  * `CamelCase`
  * At least three letters.
  * Types must have the suffix `Type`.
  * Values must have the suffix `Val`.
  * Example:

    ```c++
    template <typename DataType, int SizeVal>
    struct ArrayWrapper {
        std::array<DataType, SizeVal> _backingArray;
    };
    ```

* Type Aliases:
  * `CamelCase`
  * A type alias is when you use the `using` keyword
    to define a new alias to a type.
  * **Never** use type aliases outside of a class or function.
  * **Never** use the `typedef` keyword.
  * Example:

    ```c++
    using TokenVec = std::vector<Token>;
    ```

### Other Code Style Rules

It's far too lengthy to write out in Markdown.
The highlights are below.
For details, see the `.clang-format` file.

* Indentation is 4 spaces.
* `public`/`private`/`protected` keywords are lined up
  evenly with the `class`/`struct` that contains them.
* Short `if`/loop statements are allowed on a single line.
* The maximum length of a line is 120 characters.
* Pointer asterisks are aligned to the left, next to the data type
  (that is, `int* variable` instead of `int *variable`).
