
# bruter


*`UNDER HEAVY DEVELOPMENT`*


# Reserved

- `()` = expression

- `(@ )` = string delimiter

- `""` = string delimiter

- `''` = string delimiter

- `;` = end of command separator

# Usage

    command;
    command ...;
    command (command (command ...));
    command variable_1 variable_2 ...;
    command name;
    "script whatever";

# Types
      
  bruter essentially has 4 types of data:
  - `any`, a integer, or a native function or anything;
  - `number`, a floating point number, float or double;
  - `string`, a string pointer;
  - `list`, a list;

# Building instructions

  bruter include its own build script;

    # clean build for the current system;
    
    ./build.sh

    # debug build;
    
    ./build.sh --debug

    # you can also debug a specific file;

    ./build.sh --debug-file path/to/bruter_file.br

    # cc option:
    # you can also define the path to a compiler;

    ./build.sh --cc path/to/compiler

    # you can compile the dynamic libraries from build.sh as well;
    
    ./build.sh --lib path/to/lib.so

  ## Dynamic libraries

  dynamic libraries functions are built in the main.c file, while unload is a thing, all dynamic libs are auto-closed at exit;

  | Function    | Description                              | Function Signature                           |
  |-------------|------------------------------------------|----------------------------------------------|
  | `load`   | load a library                           | `void function(string path);`                |
  | `unload` | unload a library                         | `void function(string path);`                |
