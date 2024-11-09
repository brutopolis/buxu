
# bruter

## Description


bruter is a metaprogramable lightweight language written in C;

*`UNDER HEAVY DEVELOPMENT`*

## Table of Contents

- [bruter](#bruter)
  - [Description](#description)
  - [Table of Contents](#table-of-contents)
  - [Arg Types](#arg-types)
  - [Usage](#usage)
  - [Reserved](#reserved)
  
## Arg Types

- `number` = starts with 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 or -;

- `string` = enclosed by double quotes;

- `nil` = nil;

## Usage

    function;
    function ...;
    function (function (function ...));
    function variable_1 variable_2 ...;
    hash.set "name" value; 
    hash.set "name" (function ...);
    function name;
    do "code";
    do "code" repeat number;
    do "code" while (function ...);
    do "code" each variable;
    # (.p 1);
    # (.r 5000) (.= 1 7);
    
## Reserved

- `()` = expression
- `(. )` = direct command delimiter
- `""` = string delimiter
- `''` = string delimiter
- `;` = end of command separator
