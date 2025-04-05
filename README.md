
# buxu


*`UNDER HEAVY DEVELOPMENT`*

# Building instructions

  buxu include its own build script;

    # clean build for the current system;
    
    ./build.sh

    # debug build;
    
    ./build.sh --debug

    # you can also debug a specific file;

    ./build.sh --debug-file path/to/buxu_file.bu

    # cc option:
    # you can also define the path to a compiler;

    ./build.sh --cc path/to/compiler

    # you can compile the dynamic libraries from build.sh as well;
    
    ./build.sh --lib path/to/lib.so

  ## Dynamic libraries

  dynamic libraries functions are built in the cli.c file, while unload is a thing, all dynamic libs are auto-closed at exit;

  | Function    | Description                              | Function Signature                           |
  |-------------|------------------------------------------|----------------------------------------------|
  | `load`   | load a library                           | `void function(string path);`                |
  | `unload` | unload a library                         | `void function(string path);`                |

  # BRUTER

  you may want to know how the BRUTER vm works: https://github.com/jardimdanificado/bruter
