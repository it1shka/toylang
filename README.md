# The Toy Programming language

An interpreted language implemented in C++

## Table of Contents

- [Installation](#Installation)
- [Short Guide](#ShortGuide)
- [Real Application Example](#AppExample)
- [Builtin Functions](#BuiltinFunctions)
- [Contacts](#Contacts)

## Installation

In order to work with the language,
you need to build it on your local machine.
You have to clone the project from this repo
and build it using __CMake__

> Attention! You need to exclude __'tests'__ from 
> directories listed in root __CMakeLists.txt__
> because the testing library (Google Test) is 
> gitignored

```shell
git clone https://github.com/it1shka/toylang
cd toylang
codium CMakeLists.txt # open CMakeLists.txt in any text editor
```

After doing if everything is done successfully,
you will see __CMakeLists.txt__ file (it will look similar to 
the one listed below):
```cmake
project(toy_lang)
# Exclude "tests" folder from the array below -- it's important!
set(DIRECTORIES "utils" "lexer" "parser" "interpreter" "app" "tests") # Exclude the last

foreach(directory ${DIRECTORIES})
    message("Adding a subdirectory: ${directory}")
    add_subdirectory(${directory})
endforeach()
```

Finally, you are ready to build:
```shell
cmake .
cmake --build app
```

And you can find an executable following the path:
**"toylang/app/toy_lang_app"**:

```shell
cd app
./toy_lang_app help
```

## Short Guide

1. Variable declaration 
```toy
let a = 1;
let b = a + 2;
let c;
```
2. For loop
```toy
for (i from 1 to 10) {
    echo "Hello, " + i;
}

let sum = 0;
for (i from 1 to 100 step 2) {
    sum += i;
}
```
3. While loop
```toy
while (true) {
    println("Infinite cycle");
}

let a = 0;
while (a < 10) a += 1;
```
4. Flow operators (break, continue, return)
```toy
let running = true;
while (running) {
    if (input() == 'exit') break;
    else continue; # Just for illustration purposes,
                   # makes no sense
}

fun add(a, b) {
    return a + b;
}

fun doSomething() {
    echo "Doing something";
    return; # For illustration purposes
            # makes no sense
}
```
5. If-Else statement
```toy
let info = [input("Name: "), input("Surname: ")];
if (info == ["Tikhon", "Belousov"]) echo "you are author";
else {
    echo "you are user";
}
```
6. Functions
```toy
fun map(arr, fn) {
    let output = array();
    for (i from 0 to size(arr)) {
        let elem = arr[i];
        output += fn(elem);
    }
    return output;
}

fun main() {
    let numbers = [1, 2, 3, 4, 5];
    let doubled = map(numbers, lambda(x) { return x * 2; });
    echo doubled;
}
```
7. Import/Export
```toy
# library.toy
exports.add = lambda(a, b) {
    return a + b;
};

# lib2.toy
fun subtract(a, b) {
    return a - b;
}
exports = subtract;

# main.toy
import library as lib;
import lib2;
echo lib.add(1, 2);
echo lib2(2, 1);
```
8. Output to the console
```toy
echo "Hello world!"; # Prints to the output and hits new line
println("Hello, ", "world!"); # The same as echo, but accepts multiple args
print("Hello, world!"); # Prints without new line
```
9. Creating objects and values
```toy
let a = 1.123123123;
let b = "Hello";
let c = 'world';
let d = true and false;
let nothing = nil;
let names = ['Tikhon', 'Sasha', "Mateusz", "Jan"];
let add = lambda(a, b) { return a + b; };
let me = obj {
    "name": "Tikhon",
    "surname": "Belousov",
    "age": 19,
};
```
10. Math operators:
```
=, or, and, ==, !=, <, >, <=, >=, -, +, *, /, div, mod, ^, not
```
11. OOP
```toy
fun Cat(name) {
    
    let foodEaten = 0;

    fun meow() {
        echo "Cat " + name + "said meow!";
    }
    
    fun purr() {
        echo "Cat " + name + "said purrrrr!";
    }
    
    fun feed(food) {
        foodEaten += food;
    }
    
    fun hungry() {
        return foodEaten < 100;
    }
    
    return obj {
        "meow": meow,
        "purr": purr,
        "feed": feed,
        "hungry": hungry,
    };
}
```

## Real Application Example

I built a minesweeper console app in my language.
You can find it [here](https://github.com/it1shka/minesweeper-toy)

## Builtin Functions
Below there is a list of functions included in __language prelude__

1. **size(array)**
   Returns the size (number of elements) of the given array.

2. **chars(string)**
   Converts a string into an array of individual characters.

3. **abs(number)**
   Returns the absolute value of the given number.

4. **all(array)**
   Returns true if all elements in the array evaluate to true; false otherwise.

5. **any(array)**
   Returns true if any element in the array evaluates to true; false otherwise.

6. **print(...args)**
   Prints the string representations of the provided arguments to the standard output.

7. **println(...args)**
   Prints the string representations of the provided arguments to the standard output, followed by a newline.

8. **array(...args)**
   Creates an array containing the provided arguments.

9. **input(...args)**
   Prints the string representations of the provided arguments, then reads a line of input from the user.

10. **bool(value)**
    Converts the given value to a boolean.

11. **number(value)**
    Converts the given value to a number.

12. **max(array)**
    Returns the maximum value in the array.

13. **min(array)**
    Returns the minimum value in the array.

14. **range(start, end, step)**
    Generates an array of numbers from start to end with the specified step size.

15. **typeof(value)**
    Returns the type of the given value as a string.

16. **str(value)**
    Converts the given value to its string representation.

17. **sum(array)**
    Computes the sum of all elements in the array.

18. **slice(array, start, end)**
    Returns a sub-array from the specified start index to the end index.

19. **reversed(array)**
    Returns a new array with the elements in reverse order.

20. **read(filename)**
    Reads the contents of a file specified by the filename.

21. **write(filename, content)**
    Writes the content to a file specified by the filename.

22. **round(number)**
    Rounds the given number to the nearest integer.

23. **trunc(number)**
    Truncates the given number towards zero.

24. **keys(object)**
    Returns an array of keys from the given object.

25. **values(object)**
    Returns an array of values from the given object.

26. **wait(milliseconds)**
    Pauses the execution for the specified number of milliseconds.

27. **cls()**
    Clears the console screen.

28. **rand(lower, upper)**
    Generates a random number between the lower and upper bounds.

29. **randint(lower, upper)**
    Generates a random integer between the lower and upper bounds.

## Contacts

In case you have some suggestions / bugs to share with me, 
feel free to contact me via email [2tishbel@gmail.com](mailto:2tishbel@gmail.com)