
# brutelang

## Description

Specifications and reference implementation of brutelang.

Brutelang is meant to be a transfer language that can be used to write code in a way that is easy to parse and understand, and then be translated to other languages, it can be both dynamically and statically typed, and depends completely on each implementation to define the types and the behavior of the language. The reference is only meant to define the syntax and the behavior of the language. There is no standard library, the language is meant to be used with the standard library of the target languages, there is a plan to make a reference standard library, with the bare-basics functions that almost every language has, but that is not a priority.


THIS implementation relies completely on terralang and lua.

## Table of Contents

- [Types](#types)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Operators and such


- `$` = indicates a variable

- `:` = indicates assignment, can be used only in the first argument like: `variable:function;` this assigns the result of function to $variable

- `;` = indicates the end of a command

## Types


- `$variable` = args that starts with $, can be anything, everything is a variable

- `number` = args that starts with 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 or -

- `string` = any other args

## Usage

- argless functions:

`function;`


- returnless function:

`function arg1 arg2 ...;`


- function which return goes to $target_variable (":" denotes a return, the target variable must be on the left):

`target_variable:function ...;`


- argless function which return goes to $target_variable:

`target_variable:function;`


- variables always starts with $:

`function $variable_1 $variable_2 ...;`


## Contributing

[WORK IN PROGRESS]

## License

[WORK IN PROGRESS]