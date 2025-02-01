
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
    - [concepts](#concepts)
    - [paradigms](#paradigms)
    - [types](#types)
    - [scope](#scope)
    - [operators and delimiters](#operators-and-delimiters)
    - [variables](#variables)
    - [syntax](#syntax)
    - [functions](#functions)
    - [libraries](#libraries)
  - [vm concept](#vm-concept)
    - [concepts](#concepts-1)
    - [the vm](#the-vm)
    - [types](#types-1)
    - [variables](#variables-1)
    - [functions](#functions-1)
    - [libraries](#libraries-1)
    - [optimizations advices](#optimizations-advices)


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
  ## Concepts
  bruter is meant to be a lang extremly easy to implement and parse;

  bruter is not a stack based language, but it can be implemented as one;

  bruter has hashes by default;

  ## Paradigms

  - `procedural`, bruter is a very imperative language;
  - `semi-functional`, bruter has some functional features, but it is not a complete functional language;
  - `interpreted`, bruter is an interpreted language, but can be compiled to a executable file, which just embeds the code in the interpreter;
  - `lightweight`, bruter is a very small language, and the stdlib is also tiny, e.g. in 0.7.5 bruter stdlib has 80functions at total and 9 variables, maybe seems a lot, but this also includes the basic math functions, conditions, loops and such things whose are usually implemented in the language itself;
  - `weakly typed`, bruter is a weakly typed language, but it has types, the types are not really enforced, but they are there, also the functions can check the types if needed, but that is not really common, at least in the stdlib;
  - `non-modular`, bruter is not a modular language, you can create a library, but it will not be modular, the library will be included in the interpreter, the interpreter is meant to be compiled with the libraries you need, you can exclude the libraries you dont need during compiling, but you cant include a library at runtime;
  - `no OOP`, bruter has no OOP;
  - `no GC`, bruter memory is completely manual, you can delete the variables at anytime, but you need to do it manually;
  - `metaprogramable`, everything said above can be changed or implemented in another way, you can create a new interpreter function and replace the old one, you can even build bruter withou any value at all, with a clean stack;

  ## Types
      
  bruter essentially has 6 types of data:
  - `any`, the default type, its a integer, but can store pointers as integers so it can be anything, any is not suitable for arithmetic operations;
  - `number`, a float, can be used for arithmetic operations;
  - `string`, a string;
  - `list`, a list;
  - `function`, a function;
  - `builtin`, a builtin function, not really essential, very implementation dependent;

  ## Scope
  bruter has 2 types of scope, global scope and function scope;

  - `global scope` its the default scope, bruter;

  - `function scope` is created inside a function, if another function is created inside a function, it will have its own scope, there is no nested scope;

  ## Operators and Delimiters
  bruter has 3 operators:
  - `//` comment operator, usually this is not a operator, but, here it works as a operator, it will not parse anything after it to the end of the command;
  - `...` spread operator, it will spread a list;
  - `@` memory operator, if @n are used, they will be the n-th element of the global stack, else if @name then it will return the index of the name as a number value;

  bruter has 5 delimiters:
  - `;` end of command delimiter;
  - `()` expression delimiter;
  - `""` string delimiter;
  - `''` string delimiter;
  - `(@@ )` string delimiter;

  ## Variables
  bruter consider everything a variable;

  the first variable of a commmand will be executed if a function or a string;

  ## Syntax
  first lets understand the following command:

    command "string" 42 3.14;

  this command will execute the variable 'command' with the string "string" and the numbers 42 and 3.14 as arguments;
  note that "string", 42 and 3.14 are created permanently on the global stack until the user manually delete or reuse it;
  
  now lets understand the following command:
  
    @0 @1 @2

  this command will try to execute the slot 0 of the the memory with the slots 1 and 2 as arguments;
  now lets understand the following command:

    command a b c;
  
  this command will try to execute the slot which a hash with the name "command" points to, with the slots which the hashes with the names "a" and "b" points to as arguments;
  now lets use a more real example:

    print (sub: @2 @4);

  first this command will exec the variable 'sub:' with the slots 2 and 4 as arguments, then it will exec the variable 'print' with the result of the 'sub:' as argument, as result will be printed a list;
  now lets use a more complex example:

    #new "group" (sub: @2 @16);
    print ...group;

  this command will create a new hash with the name "group" and the result of the 'sub:' as value, then it will print the spread of the hash "group", this would be equivalent to:

    print @2 @3 @4 @5 @6 @7 @8 @9 @10 @11 @12 @13 @14 @15 @16;

  take a look a this code now:

    #new "zero" 0;
    # 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16;
    #new "twelve" @zero;
    set: twelve (+ twelve 12);
    print twelve;

  this code will create a new hash with the name "zero" and the value 0, then it will create values from 1 to 16, then it will create a new hash with the name "twelve" and the value of the hash "zero", then it will set the value of the hash "twelve" to the sum of the value of the hash "twelve" and 12, then it will print the value of the hash "twelve" which will be 12;


  ## Functions

  functions work just like functions in other languages, nothing new here, beside it has its scope cant access the upper scope if not created from global, only two scopes can be used at time, the global scope and the *current* function scope, they can recurse but will not share the same scope;

  built-in function doesnt have a own scope, but they are written in C, so you can easily create the context at the start of the function and delete it at the end;

  ## Libraries
  
  in bruter language doesnt really exists the concept of libraries, you can call scripts with declaring the functions and call that a library, the bruter vm has a similar concept of libraries, which are also not modular;

# VM Concept
  ## Concepts
  bruter interpreting function can be replaced anytime during execution, but the new interpreting function must be built-in in some library;

  bruter has a default type size, 4bytes on 32bits systems and 8bytes on 64bits systems, plus 1byte for the type;

  bruter is essentially a special list of char[(4 or 8)] along with a list of char;

  those 4 or 8 bytes are punned to anything the interpreter needs;

  bruter is meant to be a compile-yourself language, the developer should compile its own interpreter based on his needs, C built-in function are very easy to implement and may be used where bruter is not opmitized;

  you can find the list/stack macros in the include/bruter.h file;
  they are the essentially the core of the bruter, not everything is a list, but everything is inside one;
  
  this is the list struct:
  
    T struct
    {
        T *data;
        Int size;
        Int capacity;
    };

  another important struct is the hash struct:
  
    typedef struct
    {
        char *key;
        Int index;
    } Hash;

  so a type called HashList, for example, is a:
  
    typedef struct
    {
        Hash *data;
        Int size;
        Int capacity;
    } HashList;

  which can be simplified with a macro:
  
    typedef Stack(Hash) HashList;

  ## The VM

  this is the vm struct:

    typedef struct
    {
        ValueList *stack;
        CharList *typestack;
        HashList *hashes;
        IntList *unused;
        Int (*interpret)(void*, char*, HashList*);
    } VirtualMachine;

  ## Types
  bruter types are stored in the typestack, the typestack is a list of char, it used to be a list of unsigned char, but it was changed to char to make things easier;

  ## Variables
  before we talk about the variables, you need to understand the Value type:
    
    typedef union 
    {
        Float number;
        Int integer;
        char* string;
        void* pointer;
        char byte[sizeof(Float)];
    } Value;

  as we said before every variable has 4 or 8 bytes, this is because of the pointer size, the pointer size is 4bytes on 32bits systems and 8bytes on 64bits systems;

  on arduino the pointer size is usually 2bytes, but the Value is still 4bytes because i didn't want to implement a specific float type with 2bytes would be too much work;

  ## Functions
  the functions are always in the following format:

    Int name(VirtualMachine *vm, IntList *args, HashList *context);
  
  which of course can be simplified with a macro:

    function(name)
    {
        // code
        return -1;
    }
  
  you might have noticed that we dont move Values around, we move indexes to them around, this is because its easier to move indexes around, we dont need to worry about memory leaking because the Value is always in the stack, and any other headache related to duplicating, losing, or freeing memory;

  the downside of this is that we dont have "temporary" variables, there is a list of indexes that are not being used called vm.unused, but the variables need to be manually sent to the unused list, using mem.delete which also frees the pointer if its a string, a function or a list;

  ## Libraries

  bruter have 2 types of libraries, which work mostly the same way.
  
  `C file libraries` are the default library type, it must have a function called `init_$name` where $name is the name of the file, all C files inside lib folder will be added to header and the interpreter initializer, example for a file lib/lib_name.c:

    // including bruter.h is mandatory, it has everything you need to create a library;
    #include "bruter.h"

    // you can use the function macro to create a functions;
    function(print)
    {
        // you can use the print function to print a string;
        print(arg(0).string);
        return -1;
    }

    function(next)
    {
        // simple function to return the next variable;
        return arg_i(0) + 1;
    }

    init(lib_name) // this is the initializer, it will always be called when the interpreter is initialized;
    {
        register_function(vm, "print", print);
        register_function(vm, "next", next);
        register_number(vm, "pi", 3.14159265359);
        register_string(vm, "hello", "Hello, World!");
    }

  `folder libs` are a bit more complex type of library, it is expected that you understand all the build.sh file, the entire bruter.c and bruter.h and know every folder of the bruter source file structure(which are not really hard, bruter is really simple), knowing that, you can create a folder lib/lib_name with the following structure:

    lib_name/
    ├── include
    │   ├── headers_here_are_auto_included_but_need_to_be_included_in_the_c_files.c
    │   └── whatever.whatever
    ├── src
    │   ├── files_here_arent_auto_included_or_processed.txt
    │   └── whatever.any
    ├── lib
    │   ├── c_files_here_are_auto_included.c
    │   ├── you_cant_have_a_lib_folder_here_to_avoid_recursivity.c
    │   ├── lib1.c
    │   ├── lib2.c
    │   └── libn.c
    └── anything_outside_those_3_folders_are_ignored.txt

  these are the macros that will make your life easier when creating libs:

    // macros
    #define data(index) (vm->stack->data[index])
    #define data_t(index) (vm->typestack->data[index])
    #define data_unused(index) (vm->unused->data[index])
    #define data_temp(index) (vm->temp->data[index])

    #define hash(index) (vm->hashes->data[index])

    #define arg(index) (vm->stack->data[args->data[index]])
    #define arg_i(index) (args->data[index])
    #define arg_t(index) (vm->typestack->data[args->data[index]])

    #define function(name) Int name(VirtualMachine *vm, IntList *args, HashList *context)
    #define init(name) void init_##name(VirtualMachine *vm)
    

  ## Optimizations Advices
  - when using a hash of something, there is a overhead associated to the time it takes to find the hash, it you have few hashes its unoticable, but might be a problem if you have a lot of hashes, in this case you can use the index directly or move the most used hashes to the start of the hashlist;

  - usually takes longer to delete/unuse a variable than to create one, especially if there are unused variables, but the delete overhead is just for strings, lists and functions, functions are the slower, but not that slow tho;

  - you can replace the interpreter function by another one with the optimizations you need, e.g. less types or such;

  - you can easy write functions in C like explained before;

  - you can delete the unused variables at anytime;

  - most of the bruter stdlib is meant for endless arguments, so:

      `+ 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16;`

    peforms faster than:

      `+ 1 (+ 2 (+ 3 (+ 4 (+ 5 (+ 6 (+ 7 (+ 8 (+ 9 (+ 10 (+ 11 (+ 12 (+ 13 (+ 14 (+ 15 16))))))))))))));`
  
  - you can exclude all the stdlib or part of it if you dont need it, see the build instructions;

    