
# bruter

## Description


bruter is a metaprogramable lightweight virtual machine/interpreted programming language;

*`UNDER HEAVY DEVELOPMENT`*


## Table of Contents


- [bruter](#bruter)
  - [bruter language](#bruter-language)
    - [description](#description)
    - [table of contents](#table-of-contents)
    - [arg types](#arg-types)
    - [reserved](#reserved)
    - [usage](#usage)
    - [building instructions](#building-instructions)



# bruter language

## Types


- `number` = starts with 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 or -;

- `string` = enclosed by double or single quotes, or by (@@ );

- `nil` = nil;

## Reserved

- `()` = expression

- `(@@ )` = string delimiter

- `""` = string delimiter

- `''` = string delimiter

- `...` = spread operator

- `;` = end of command separator

- `//` = comment

## Usage

    function;
    function ...;
    function (function (function ...));
    function variable_1 variable_2 ...;
    #set "name" value; 
    #set "name" (function ...);
    function name;
    do "code";
    repeat amount "code";
    while "condition" "code";
    ...a_list; // spread operator;
    // comment;
    function "string";
    function 'string';
    function (@@ string);
    if "condition" "code";
    ifelse "condition" "code" "code";
    function (:sub x y);

## Building instructions

  bruter include its own build script;

    # clean build for the current system;
    
    ./build.sh

    # debug build;
    # this can also be used with WEB flag(add some debug flags to the wasm file);
    # this mostly checks for memory leaks and add some debug flags;
    
    ./build.sh --debug

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

  note that, the clean build is always compiled, even with WASI, INO, EXEC, or WEB flags;