#include "bruter.h"

/*
    this lib is an example of how to create a library for the bruter vm
*/

//#ifndef ARDUINO

Int example_function(VirtualMachine *vm, IntList *args) // example function
{
    printf("Hello, World!\n");
    return -1;
}

void init_example(VirtualMachine *vm) // example lib initializer
{
    //registerBuiltin(vm, "example", example_function); // register example function
    //registerNumber(vm, "example.number", 42); // register example number
    //registerString(vm, "example.string", "Hello, World!"); // register example string   
}

//#endif