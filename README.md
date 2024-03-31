
# bruter

## Description

Bruter is meant to be a language template, all it can parse are simple commands, but has built-in preprocessing so you can create your own language that transpile to bruter.


Thanks to Terralang, bruter does fully support Lua and Terra, and also supports most of C features.

(make sure to install terra and add it to your PATH before using bruter)


*`UNDER HEAVY DEVELOPMENT` - api should change a lot*


## Table of Contents

- [Types](#types)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Operators and such


- `$` = indicates a variable, used to get the value of a variables, but not to set.

- `;` = indicates the end of a command.

- `.` = indicates recursion.

## Types


- `$variable` = args that starts with $, can be anything, everything in bruter is stored as a variable and can be modified.

- `number` = args that starts with 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 or -

- `boolean` = args that are true or false

- `string` = args delimited by backticks(`) and anything else that doesnt match the other types

- `{}` = special type to create empty objects

- `nil` = nil;

## Usage


- argless functions:

`function;`


- returnless function:

`function arg1 arg2 ...;`


- function which return goes to $target_variable:

`set target_variable from function ...;`


- argless function which return goes to $target_variable:

`set target_variable from function;`


- variables always starts with $:

`function $variable_1 $variable_2 ...;`

## Libraries

bruter by default does not work with modules, bruter libraries are just scripts that are interpreted real-time, so you can create your own libraries by creating a script that defines functions and variables, and then include it in your main script, bruter libraries have access to all variables and functions in the main script and vice-versa, use with caution, as this does not use any cache at all, it will re-interpret the library every time it is called.

bruter libraries can be loaded by the `using` command like so:

    using library_name;

it will look for the library in the following paths:

    libr_path/library_name/library_name.br
    libr_path/library_name/library_name.lua
    libr_path/library_name/library_name.t
    libr_path/library_name.br
    libr_path/library_name.lua
    libr_path/library_name.t
if none of the paths are found it will throw an error.    

if you really want a library that work as a module you can create a "fake module" see `examples/fake_module/main.br` for an example in vanilla bruter. 

you can also use the `require` function:

    set module from require `module_name`;
    
lua and terra packages are handled with loadfile not require, so they are not cached and can be reloaded, this is done to behave like brute packages, but you can use require to load lua and terra packages.

## Contributing

[WORK IN PROGRESS]

## License

[WORK IN PROGRESS]