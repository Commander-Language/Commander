# Commander Specification
- [Commander Specification](#commander-specification)
    - [Functions](#functions)
      - [Grammar](#grammar)
      - [Examples](#examples)

### Functions
#### Grammar
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

#### Examples
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
