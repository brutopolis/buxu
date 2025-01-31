
# bruter

## Description


bruter is a metaprogramable lightweight programming language;

*`UNDER HEAVY DEVELOPMENT`*


## Table of Contents


- [bruter](#bruter)
  - [description](#description)
  - [table of contents](#table-of-contents)
  - [arg types](#arg-types)
  - [reserved](#reserved)
  - [usage](#usage)
  - [building instructions](#building-instructions)
  - [examples](#examples)
  - [language concept](#language-concept)
    - [foundation](#foundation)
    - [concepts](#concepts)
    - [types](#types)
    - [variables](#variables)
    - [functions](#functions)
    - [libraries](#libraries)
    - [control flow](#control-flow)
    - [operators](#operators)
    - [comments](#comments)
  - [vm concept](#vm-concept)
    - [foundation](#foundation-1)
    - [concepts](#concepts-1)
    - [types](#types-1)
    - [variables](#variables-1)
    - [functions](#functions-1)
    - [libraries](#libraries-1)


# Reserved

- `()` = expression

- `(@@ )` = string delimiter

- `""` = string delimiter

- `''` = string delimiter

- `...` = spread operator

- `;` = end of command separator

- `// ;` = comment

# Usage

    command;
    command ...;
    command (command (command ...));
    command variable_1 variable_2 ...;
    #new "name" value; 
    #new "name" (command ...);
    #new "name" (list: value value ...);
    command name;
    do "code";
    repeat amount "code";
    while "condition" "code";
    ...a_list; // spread operator;
    // comment;
    command "string";
    command 'string';
    command (@@ string);
    if "condition" "code";
    ifelse "condition" "code" "code";
    command (sub: @x @y);

# Building instructions

  bruter include its own build script;

    # clean build for the current system;
    
    ./build.sh

    # debug build;
    # this can also be used with WEB flag(add some debug flags to the wasm file);
    # this mostly checks for memory leaks and add some debug flags;
    
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

    ./build.sh --exclude path/to/lib.c
    ./build.sh --exclude path/to/lib1 --exclude path/to/lib2 
    ./build.sh --exclude "path/to/lib1.c path/to/lib2"
    
  note that, the clean build is always compiled, even with WASI, INO, EXEC, or WEB flags;

# Examples

    # print a string;
    print "Hello, World!";

    # print a number;
    print 42;

    # print a string and a number;
    print "Hello, World!" 42;

# Language Concept
  ## Foundation
  ## Concepts
  ## Types
  ## Variables
  ## Functions
  ## Libraries
  ## Control Flow
  ## Operators
  ## Comments


# VM Concept
  ## Foundation
  ## Concepts
  ## Types
  ## Variables
  ## Functions
  ## Libraries
