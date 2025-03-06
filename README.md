
# bruter


*`UNDER HEAVY DEVELOPMENT`*


# Table of Contents


- [bruter](#bruter)
- [table of contents](#table-of-contents)
- [Introduction](#introduction)
- [Syntax](#syntax)
- [Usage](#usage)
- [Types](#types)
- [Scope](#scope)
- [Building instructions](#building-instructions)
- [Standard Library](#standard-library)
  - [std.math](#stdmath)
  - [std.type](#stdtype)
  - [std.condition (index-based)](#stdcondition-index-based)
  - [std.hash](#stdhash)
  - [std.loop](#stdloop)
  - [std.mem](#stdmem)
  - [std.list](#stdlist)
  - [std.string](#stdstring)
  - [std.global](#stdglobal)
  - [std](#std)
  - [Dynamic libraries](#dynamic-libraries)



# Reserved

- `()` = expression

- `(@@ )` = string delimiter

- `""` = string delimiter

- `''` = string delimiter

- `;` = end of command separator

- `// ;` = comment deimiter

- `@` = memory operator

# Usage

    command;
    command ...;
    command (command (command ...));
    command variable_1 variable_2 ...;
    #new "name" value; 
    #new "name" (command ...);
    #new "name" (: value value ...);
    command name;
    "script whatever";
    repeat amount "code";
    while "condition" "code";
    // comment;
    command "string";
    command 'string';
    command (@@ string);
    if "condition" "code";
    command (sub: @x @y);

# Types
      
  bruter essentially has 4 types of data:
  - `any`, a integer, or a native function or anything;
  - `number`, a floating point number, float or double;
  - `string`, a string pointer;
  - `list`, a list;

# Scope
bruter has 2 types of scope, global scope and function scope;

- `global scope` its the default scope, bruter;

- `script scope` is created inside a script, if another script is created inside a script, it will have its own scope, there is no nested scope;


# Building instructions

  bruter include its own build script;

    # clean build for the current system;
    
    ./build.sh

    # debug build;
    
    ./build.sh --debug

    # you can also debug a specific file;

    ./build.sh --debug-file path/to/bruter_file.br

    # wasi build;
    # regular bruter interpreter but using WASI;
    # it also create build/run_bruter.sh which is just a wasmtime command;
    # you can find the WASI executable at build/bruter.wasm;
    
    ./build.sh --wasi path/to/wasi-sdk/clang

    # web build;
    # wasm + js module;
    # you can find bruter.wasm and bruter.js in build/web;

    ./build.sh --web

    # or you may want to define the emcc path direcly;

    ./build.sh --emcc path/to/emcc

    # arduino build;
    # you can find the prepared arduino folder in build/arduino/bruter;

    ./build.sh --ino

    # exec build;
    # this will create a executable file(interpreter + embedded code);
    # you can find the executable file in build/bruter_out;

    ./build.sh --exec path/to/bruter_file.br

    # cc option:
    # you can also define the path to a compiler;

    ./build.sh --cc path/to/compiler

    # you can exclude libs as well;

    ./build.sh --clean

    # you can compile the dynamic libraries from build.sh as well;
    
    ./build.sh --lib path/to/lib.so
    
    note that, dynamic libraries are only supported if not arduino or wasm;

# Standard Library

  ## std.math

  math functions are destructive.

  | Function  | Description                     | Function Signature                          |
  |-----------|---------------------------------|---------------------------------------------|
  | `+`       | Add `...` to origin             | `void function(Float origin, Float[] ...);` |
  | `-`       | Subtract `...` from origin      | `void function(Float origin, Float[] ...);` |
  | `*`       | Multiply origin by `...`        | `void function(Float origin, Float[] ...);` |
  | `/`       | Divide origin by `...`          | `void function(Float origin, Float[] ...);` |
  | `%`       | Modulo origin by `...`          | `void function(Float origin, Float ...);`   |
  | `random`  | Generate random number [0, 1)   | `void function(Float origin);`              |
  | `seed`    | Set seed for random function    | `void function(Float seed);`                |
  | `round`   | Round origin to nearest integer | `void function(Float origin);`              |
  | `floor`   | Floor origin                    | `void function(Float origin);`              |
  | `ceil`    | Ceil origin                     | `void function(Float origin);`              |
  | `sin`     | Sine of origin                  | `void function(Float origin);`              |
  | `cos`     | Cosine of origin                | `void function(Float origin);`              |
  | `tan`     | Tangent of origin               | `void function(Float origin);`              |
  | `min`     | Set origin to the minimum value | `void function(Float origin, Float[] ...);` |
  | `max`     | Set origin to the maximum value | `void function(Float origin, Float[] ...);` |

  ## std.type

  type functions are destructive.
  
  | Function  | Description                     | Function Signature                          |
  |-----------|---------------------------------|---------------------------------------------|
  | `type`    | Get type of origin              | `void function(Float origin);`              |
  | `cast`    | Cast origin to type             | `void function(Float origin, Float type);`  |
  | `pun`     | Pun origin to type              | `void function(Float origin, Float type);`  |

  | Variable       | Description   | Type  | Default    |
  |----------------|---------------|-------|------------|
  | `type.any`     | Any type      | Float | 0.0        | 
  | `type.number`  | Number type   | Float | 1.0        |
  | `type.string`  | String type   | Float | 2.0        |
  | `type.list`    | List type     | Float | 3.0        |
  | `type.size`    | List type     | Float | 4.0 or 8.0 |

  ## std.condition

  | Function  | Description                                                      | Function Signature                             |
  |-----------|------------------------------------------------------------------|------------------------------------------------|
  | `==`      | return @1 if a is equal to every element of ..., else return @0  | `Index function(any a, any ...);`              |
  | `!=`      | return @1 if a is not equal to a element of ..., else return @0  | `Index function(any a, any ...);`              |
  | `>`       | return @1 if ...[i] is greater than ...[i+1] and so on, else @0  | `Index function(any ...);`                     |
  | `<`       | return @1 if ...[i] is less than ...[i+1] and so on, else @0     | `Index function(any ...);`                     |
  | `>=`      | return @1 if ...[i] is greater or equal to ...[i+1] and so on    | `Index function(any ...);`                     |
  | `<=`      | return @1 if ...[i] is less or equal to ...[i+1] and so on       | `Index function(any ...);`                     |
  | `and`     | return @1 if ...[i] is true and ...[i+1] is true and so on       | `Index function(Int ...);`                     |
  | `or`      | return @1 if ...[i] is true or ...[i+1] is true or so on         | `Index function(Int ...);`                     |

  ## std.hash

  | Function    | Description                                                      | Function Signature                           |
  |-------------|------------------------------------------------------------------|----------------------------------------------|
  | `#new`      | Create a new(or replace) hash with the name args[1] and the value args[2]  | `void function(String name, any value);`     |
  | `#delete`   | Delete the hash with the name args[1]                                 | `void function(String name);`                |
  | `#priority` | Set the hash with the name args[1] to the priority args[2], if passed only one arg re-set the base priority | `void function(String name, Float priority);`|

  ## std.loop

  | Function  | Description                                                      | Function Signature                              |
  |-----------|------------------------------------------------------------------|-------------------------------------------------|
  | `while`   | Execute args[2] while args[1] is true                                      | `void function(String condition, String code);` |
  | `loop`    | Execute args[2] args[1] times                                              | `void function(Float amount, String code);`     |


  ## std.mem

  | Function    | Description                                                      | Function Signature                           |
  |-------------|------------------------------------------------------------------|----------------------------------------------|
  | `mem.copy`  | Copy the value of args[1] and return it                               | `any function(any value);`                   |
  | `mem.delete`| Delete the value of args[1]                                           | `void function(any value);`                  |


  ## std.list

  | Function    | Description                                                      | Function Signature                                        |
  |-------------|------------------------------------------------------------------|-----------------------------------------------------------|
  | `list:`     | Create a new list with the values of args[1] to args[n] and return it | `List function(any ...);`                                 |
  | `get:`      | Get the value of args[2] from args[1] and return it                        | `any function(List list, Float index);`                   |
  | `set:`      | Set the value of args[2] from args[1] to args[3]                                | `void function(List list, Float index, any value);`       |
  | `len:`      | Get the length of args[1] and return it                               | `Float function(List list);`                              |
  | `pop:`      | Pop the last element of args[1] and return it                         | `any function(List list);`                                |
  | `push:`     | Push args[2] to the end of args[1]                                         | `void function(List list, any value);`                    |
  | `shift:`    | Shift the first element of args[1] and return it                      | `any function(List list);`                                |
  | `unshift:`  | Unshift args[2] to the start of args[1]                                    | `void function(List list, any value);`                    |
  | `find:`     | Find args[2] in args[1] and return its index                               | `Float function(List list, any value);`                   |
  | `concat:`   | Concatenate args[1] and args[2] and return it                              | `List function(List list1, List list2);`                  |
  | `split:`    | Split args[1] at args[2] and return a list with the lists                  | `List function(List list, any value);`                    |
  | `reverse:`  | Reverse args[1] and                                                   | `void function(List list);`                               |
  | `insert:`   | Insert args[3] in args[1] at args[2]                                            | `void function(List list, Float index, any value);`       |
  | `remove:`   | Remove args[2] from args[1] and return it                                  | `List function(List list, any value);`                    |
  | `swap:`     | Swap args[2] and args[3] in args[1]                                             | `void function(List list, Float index1, Float index2);`   |
  | `replace:`  | Replace args[2] with args[3] in args[1]                                         | `void function(List list, any value1, any value2);`       |
  | `sub:`      | Get a sub list of args[1] from args[2] to args[3]                               | `List function(List list, Float start, Float end);`       |

  ## std.string

  | Function    | Description                                                      | Function Signature                                            |
  |-------------|------------------------------------------------------------------|---------------------------------------------------------------|
  | `get:`      | get the character [2] from string and return it as index         | `Index function(String string, Float index);`                 |
  | `set:`      | set the character arg[2] from string to arg[3]                   | `void function(String string, Float index, any value);`       |
  | `len:`      | get the length of string and return it                           | `Float function(String string);`                              |
  | `pop:`      | pop the last character from string and return it                 | `Index function(String string);`                              |
  | `push:`     | push arg[2] to the end of string                                 | `void function(String string, any value);`                    |
  | `shift:`    | shift the first character from string and return it              | `Index function(String string);`                              |
  | `unshift:`  | unshift arg[2] to the start of string                            | `void function(String string, any value);`                    |
  | `find:`     | find arg[2] in string and return its index                       | `Float function(String string, any value);`                   |
  | `concat:`   | concatenate string and arg[2] and return it                      | `String function(String string1, String string2);`            |
  | `split:`    | split string at arg[2] and return a list with the strings        | `List function(String string, String separator);`             |
  | `reverse:`  | reverse string and return it                                     | `String function(String string);`                             |
  | `insert:`   | insert arg[3] in string at arg[2]                                | `void function(String string, Float index, Index char);`      |
  | `remove:`   | remove arg[2] from string and return it                          | `String function(String string, Index char);`                 |
  | `swap:`     | swap arg[2] and arg[3] in string                                 | `void function(String string, Float index1, Float index2);`   |
  | `replace:`  | replace arg[2] with arg[3] in string                             | `void function(String string, String value1, String value2);` |
  | `sub:`      | get a sub string of string from arg[2] to arg[3]                 | `String function(String string, Float start, Float end);`     |

  ## std.global

  | Function    | Description                                                      | Function Signature                           |
  |-------------|------------------------------------------------------------------|----------------------------------------------|
  | `get:`      | get the value of arg[2] from the global scope and return it      | `any function(Float index);`                 |
  | `set:`      | set the value of arg[2] from the global scope to arg[3]          | `void function(Float index, any value);`     |
  | `len:`      | get the length of the global scope and return it                 | `Float function();`                          |
  | `pop:`      | pop the last value from the global scope                         | `void function();`                           |
  | `push:`     | push arg[2] to the end of the global scope                       | `void function(any value);`                  |
  | `shift:`    | shift the first value from the global scope                      | `void function();`                           |
  | `unshift:`  | unshift arg[2] to the start of the global scope                  | `void function(any value);`                  |
  | `find:`     | find arg[2] in the global scope and return its index             | `Float function(any value);`                 |
  | `concat:`   | concatenate the global scope and arg[2] and return it            | `List function(List list);`                  |
  | `split:`    | split the global scope at arg[2] and return a list with the lists| `List function(any value);`                  |
  | `reverse:`  | reverse the global scope and return it                           | `void function();`                           |
  | `insert:`   | insert arg[3] in the global scope at arg[2]                      | `void function(Float index, any value);`     |
  | `remove:`   | remove arg[2] from the global scope and return it                | `List function(any value);`                  |
  | `swap:`     | swap arg[2] and arg[3] in the global scope                       | `void function(Float index1, Float index2);` |
  | `replace:`  | replace arg[2] with arg[3] in the global scope                   | `void function(any value1, any value2);`     |
  | `sub:`      | get a sub list of the global scope from arg[2] to arg[3]         | `List function(Float start, Float end);`     |

  ## std

  | Function    | Description                                                      | Function Signature                           |
  |-------------|------------------------------------------------------------------|----------------------------------------------|
  | `ls`        | List all entries in the global scope                             | `void function();`                           |
  | `ls.hash`   | List all hashes                                                  | `void function();`                           |
  | `#`         | ignore return values                                             | `void function(any ...);`                    |
  | `$`         | duplicate the second value and return it (for destructive funcs) | `any function(any ...);`                     |
  | `print`     | Print the values based on the type                               | `void function(any ...);`                    |
  | `scan`      | get the input from the user and return it                        | `String function();`                         |
  | `return`    | return the value                                                 | `void function(any value);`                  |

  | Variable       | Description    | Type  | Default |
  |----------------|----------------|-------|---------|
  | `VERSION`      | Version number | Float | depends | 

  ## Dynamic libraries

  dynamic libraries functions are actually not part of stdlib itself, it comes from the idea of clean bruter, a bruter withou std that can load dynamic libs, like the std itself, while dl.close is a thing, all dynamic libs are auto-closed at exit(except if you delete the dl.list);

  | Function    | Description                              | Function Signature                           |
  |-------------|------------------------------------------|----------------------------------------------|
  | `dl.open`   | load a library                           | `void function(string path);`                |
  | `dl.close`  | close a library                          | `void function(any libpointer);`             |

  | Variable       | Description              | Type  | Default |
  |----------------|--------------------------|-------|---------|
  | `dl.list`      | List of loaded libraries | List  |         |
