// this file is the utility tool for bruter

#ifndef ARDUINO

// bruter header
#include "../include/bruter.h"
#include "../include/entrypoint.h"


int main(int argc, char **argv)
{
    // <embed code> 

    Int result = 0;
    VirtualMachine *vm = make_vm();
    
    // libraries init is not a merely comment
    // <libraries init>

    register_list(vm, "file.args");
    IntList *fileargs = (IntList*)data(hash_find(vm, "file.args")).pointer;
    for (int i = 1; i < argc; i++)
    {
        stack_push(*fileargs, new_string(vm, argv[i]));
    }
    
    // embed code tag is replaced by main_br and main_br_len during build
    result = eval(vm, src_main_br, NULL);

    free_vm(vm);
    return result;
}

#endif