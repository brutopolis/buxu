
# bruter

## Description


Bruter is a metaprogramable lightweight language written in C;

*`UNDER HEAVY DEVELOPMENT`*

## Table of Contents

- [Operators](#operators)
- [Types](#types)
- [Usage](#usage)
- [Libraries](#libraries)
- [Reserved](#reserved)

## Operators


- `$` = variable.

- `;` = end.

- `.` = recursion.

## Arg Types


- `$variable` = starts with $

- `number` = starts with 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 or -

- `string` = enclosed by $() or untyped.

- `nil` = nil;

## Usage

    function;
    function ...;
    function (function (function ...));
    set target_variable (function ...);
    set target_variable value;
    function $variable_1 $variable_2 ...;

## Libraries -- not implemented yet -- desconsider

bruter libraries are loaded with `using`:

    using library_name;

library search path:

    libr_path/library_name/library_name.br
    libr_path/library_name.br

## Reserved

- `()` = expression
- `($())` = multi-expression
- `$()` = string delimiter
- `$` = variable indicator
- `.` = recursion
- `;` = end of command