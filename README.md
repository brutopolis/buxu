
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
    - [standard library](#standard-library)
      - [std.math ](#stdmath)
      - [std.type ](#stdtype)
      - [std.condition (index-based)](#stdcondition-index-based)
      - [std.hash](#stdhash)
      - [std.loop](#stdloop)
      - [std.mem](#stdmem)
      - [std.list](#stdlist)
      - [std.string](#stdstring)
      - [std.global](#stdglobal)
      - [std](#std)
    - [dynamic libraries](#dynamic-libraries)
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
    #new "name" (: value value ...);
    command name;
    "script whatever";
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

    # you can compile the dynamic libraries from build.sh as well;
    
    ./build.sh --lib path/to/lib.so
    
  note that, dynamic libraries are only supported if not arduino or wasm;

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
  - `lightweight`, bruter is a very small language, and the stdlib is also tiny, e.g. in 0.7.6 bruter stdlib has 62 functions and 6 variables at total, maybe seems a lot, but this also includes the basic math functions, conditions, loops and such things whose are usually implemented in the language itself;
  - `no OOP`, bruter has no OOP;
  - `no GC`, bruter memory is completely manual, you can delete the variables at anytime, manually;
  - `metaprogramable`, everything said above can be changed or implemented in another way, you can create a new interpreter function and replace the old one, you can even build bruter withou any value at all, with a clean stack;

  ## Types
      
  bruter essentially has 4 types of data:
  - `any`, the default type, its a integer, but can store pointers as integers so it can be anything, any is also treated as a built-in C function if passed as first argument;
  - `number`, a floating point number, float or double;
  - `string`, a string pointer;
  - `list`, the list the the most complex type

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
  - `(: )` list delimiter;

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
    set: twelve ($ + twelve 12);
    // update: alternatively you wouldnt need to re-set, because std math functions are now destructive, so you could just do:;
    // + twelve 12;
    print twelve;

  this code will create a new hash with the name "zero" and the value 0, then it will create values from 1 to 16, then it will create a new hash with the name "twelve" and the value of the hash "zero", then it will set the value of the hash "twelve" to the sum of the value of the hash "twelve" and 12, then it will print the value of the hash "twelve" which will be 12;


  ## Functions

  functions work just like functions in other languages, nothing new here, beside its scope cant access the upper scope if not created from global, only two scopes can be used at time, the global scope and the *current* function scope;

  C functions doesnt have a own scope, but  you can easily create the context at the start of the function and delete it at the end;

  ## Libraries
  
  *bruter language* does support(if not arduino or wasm) dynamic libraries, which are bruter C libraries, you can load and unload them at anytime, and if you dont want to manually unload them, they will be automatically unloaded at exit;

  ## Standard Library

  bruter has a small standard library, some concepts are only part of the stdlib and not a language or vm concept itself.
  all functions detailed:

  ### std.math

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

  ### std.type

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

  ### std.condition (index-based)

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

  ### std.hash

  | Function    | Description                                                      | Function Signature                           |
  |-------------|------------------------------------------------------------------|----------------------------------------------|
  | `#new`      | Create a new(or replace) hash with the name args[1] and the value args[2]  | `void function(String name, any value);`     |
  | `#delete`   | Delete the hash with the name args[1]                                 | `void function(String name);`                |
  | `#priority` | Set the hash with the name args[1] to the priority args[2], if passed only one arg re-set the base priority | `void function(String name, Float priority);`|

  ### std.loop

  | Function  | Description                                                      | Function Signature                              |
  |-----------|------------------------------------------------------------------|-------------------------------------------------|
  | `while`   | Execute args[2] while args[1] is true                                      | `void function(String condition, String code);` |
  | `loop`    | Execute args[2] args[1] times                                              | `void function(Float amount, String code);`     |


  ### std.mem

  | Function    | Description                                                      | Function Signature                           |
  |-------------|------------------------------------------------------------------|----------------------------------------------|
  | `mem.copy`  | Copy the value of args[1] and return it                               | `any function(any value);`                   |
  | `mem.delete`| Delete the value of args[1]                                           | `void function(any value);`                  |


  ### std.list

  | Function    | Description                                                      | Function Signature                                        |
  |-------------|------------------------------------------------------------------|-----------------------------------------------------------|
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

  ### std.string

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

  ### std.global

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

  ### std

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


# VM Concept
  ## Concepts
  bruter interpreting function can be replaced anytime during execution, but the new interpreting function must be built-in in some library;

  bruter has a default type size, 4bytes on 32bits systems and 8bytes on 64bits systems, plus 1byte for the type;

  bruter is essentially a special list of char[(4 or 8)] along with a list of char;

  those 4 or 8 bytes are punned to anything the interpreter needs;

  bruter is meant to be a compile-yourself language, the developer should compile its own interpreter based on his needs, C built-in function are very easy to implement and may be used where bruter is not opmitized;

  but, it is extremlly easy to build, a simple ./build.sh will build the interpreter for you, there are other advanced options as well;

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
  
    typedef List(Hash) HashList;

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
  
  `C file libraries` are the default library type, it can be loaded as dynamic lib, it must have a function called `init_$name` where $name is the name of the file, all C files inside lib folder will be added to header and the interpreter initializer, example for a file lib/lib_name.c:

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

  `folder libs` are a bit more complex type of library, those cant be loaded as dynamic lib by its nature, it is expected that you understand all the build.sh file, the entire bruter.c and bruter.h and know every folder of the bruter source file structure(which are not really hard, bruter is really simple), knowing that, you can create a folder lib/lib_name with the following structure:

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
  - when using a hash of something, there is a overhead associated to the time it takes to find the hash, it you have few hashes its unoticable, but might be a problem if you have a lot of hashes, in this case you can use the #priority function;

  - usually takes longer to delete/unuse a variable than to create one, especially if there are unused variables, but the delete overhead is just for strings, lists and functions, functions are the slower, but not that slow tho;

  - you can replace the interpreter function by another one with the optimizations you need;

  - you can easy write functions in C like explained before, and even more easy to load them as dynamic libs(if not arduino or wasm);

  - you can delete the unused variables at anytime;

  - most of the bruter stdlib is meant for endless arguments, so:

      `+ 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16;`

    peforms faster than:

      `+ 1 ($ + 2 ($ + 3 ($ + 4 ($ + 5 ($ + 6 ($ + 7 ($ + 8 ($ + 9 ($ + 10 ($ + 11 ($ + 12 ($ + 13 ($ + 14 ($ + 15 16))))))))))))));`
  
  - you can exclude all the stdlib or part of it if you dont need it, see the build instructions;

  - loops contexts are optimized if it does not contain any # or ()(except for string delimitation);

  - while loops are usually faster if inside a function;