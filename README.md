
# bruter

## Description


bruter is a metaprogramable lightweight virtual machine;

*`UNDER HEAVY DEVELOPMENT`*


## Table of Contents


- [bruter](#bruter)
  - [bruter language](#bruter-language)
    - [description](#description)
    - [table of contents](#table-of-contents)
    - [arg types](#arg-types)
    - [usage](#usage)
    - [reserved](#reserved)



# bruter language

## Types


- `number` = starts with 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 or -;

- `string` = enclosed by double or single quotes, or by (@@ );

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
    repeat amount "code";
    while "condition" "code";
    each list "name" "code";
    
## Reserved

- `()` = expression
- `(@@ )` = string delimiter
- `""` = string delimiter
- `''` = string delimiter
- `...` = spread operator
- `;` = end of command separator
