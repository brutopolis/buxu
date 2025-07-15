
# buxu

  buxu stands for (B)RUTER (U)ni(X) (U)tils, it is a set of tools used to build, run and debug buxu scripts and libraries, specifically on unix-like systems;

  buxu includes, but is not limited to:
  - buxu, a interpreter and repl for bruter representation;
  - bpm, stands for Buxu Package Manager;

*`UNDER HEAVY DEVELOPMENT`*

# Building instructions

  ```c
    gcc -o buxu buxu.c -ldl 
  ```
  does the trick;

# Usage
  
  "buxu" executable is the interpreter, and it also act as repl if no args are passed;
  
    # this will start in the repl;
    ./buxu 

    # run script;
    ./buxu path/to/script.br

    # print help message;
    ./buxu --help
    ./buxu -h

    # print version;
    ./buxu --version
    ./buxu -v

    # preload a library;
    ./buxu --load path/to/library.so
    ./buxu -l path/to/library.so

    # eval a string;
    ./buxu --eval "print 'hello world'"
    ./buxu -e "print 'hello world'"

  "bpm" executable is the package manager, it stands for Bruter Package Manager, and is expected to work only under unix-like system;

    # install a package;
    ./bpm install package_name

    # uninstall a package;
    ./bpm uninstall package_name

    # compile a program;
    ./bpm compile 

    # print help message;
    ./bpm --help
    ./bpm -h

# Dynamic libraries

  dynamic libraries functions are built in the cli.c file, while unload is a thing, all dynamic libs are auto-closed at exit;

  | Function    | Description                              | Function Signature                           |
  |-------------|------------------------------------------|----------------------------------------------|
  | `load`      | load a library                           | `void function(string path);`                |
  | `unload`    | unload a library                         | `void function(string path);`                |

  # DEPENDENCIES

  you might want to know how BRUTER works:  https://github.com/brutopolis/bruter
  
  you might also want to know how br(bruter representation) works: https://github.com/brutopolis/br
