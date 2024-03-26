
# bruter

## Description

Specifications and reference implementation of bruter.


Bruter is meant to be a language template, all it can parse are simple commands, but has built-in preprocessing and lineprocessing so you can create your own language that transpile to bruter.


Thanks to Terralang, bruter does fully support Lua and Terra, and also supports most of C features.

(make sure to install terra and add it to your PATH before using bruter)


*`UNDER HEAVY DEVELOPMENT` - api should change a lot, while the raw language itself should be stable*


## Table of Contents

- [Types](#types)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Operators and such


- `$` = indicates a variable, used to get the value of a variables, but not to set.

- `;` = indicates the end of a command.

- `.` = indicates recursion.

- `:` = indicates assignment, can be used only in the first argument like: `variable:function;` this assigns the result of function to $variable (note: this operator is just a syntactic sugar, this operator can be simply removed from the lineprocessors list).

## Types


- `$variable` = args that starts with $, can be anything, everything in bruter is stored as a variable and can be modified.

- `number` = args that starts with 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 or -

- `boolean` = args that are true or false

- `string` = args delimited by backticks(`) and anything else that doesnt match the other types

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