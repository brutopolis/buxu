
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
    - [building instructions](#building-instructions)
      - [clean](#clean)
      - [debug](#debug)
      - [wasi](#wasi)
      - [web](#web)
      - [arduino](#arduino)
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
    
## Building instructions

  bruter include its own build script;

  ### CLEAN:

    # build for the current system;
    ./build.sh

  ### DEBUG:

    # this can also be used with WEB flag(add some debug flags to the wasm file);
    # this mostly checks for memory leaks and add some debug flags;
    DEBUG=1 ./build.sh

  ### WASI:

    # regular bruter interpreter but using WASI;
    # it also create build/run_bruter.sh which is just a wasmtime command;
    # you can find the WASI executable at build/bruter.wasm;
    WASICC='path/to/wasi/sdk/clang' ./build.sh

  ### WEB:

    # wasm + js module;
    # you can find bruter.wasm and bruter.js in build/web;
    EMCC='path/to/emcc' ./build.sh

  ### ARDUINO:

    # you can find the prepared arduino folder in build/arduino/bruter;
    INO=1 ./build.sh

  note that, the clean build is always compiled, even with WASI, INO, or WEB flags;


## Reserved

- `()` = expression
- `(@@ )` = string delimiter
- `""` = string delimiter
- `''` = string delimiter
- `...` = spread operator
- `;` = end of command separator
