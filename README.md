
# bruter

## Description


Bruter is a metaprogramable template language written in lua;

*`UNDER HEAVY DEVELOPMENT`*

Lua(5.1+) compatible;


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

## Types


- `$variable` = starts with $

- `number` = starts with 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 or -

- `boolean` = true or false.

- `string` = enclosed by (``) or ({}).

- `nil` = nil;

anything untyped is string.

## Usage

    function;
    function ...;
    function (function (function ...));
    set target_variable from function ...;
    set target_variable (function ...);
    set target_variable value;
    function $variable_1 $variable_2 ...;

## Libraries

bruter libraries are loaded with `using`:

    using library_name;

library search path:

    libr_path/library_name/library_name.br
    libr_path/library_name/library_name.lua
    libr_path/library_name.br
    libr_path/library_name.lua

## Reserved

- ` `` ` = string delimiter
- `{}` = string delimiter
- `$` = variable indicator
- `.` = recursion
- `;` = end of command