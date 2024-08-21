#include "bruter.h"

Int hashfind(VirtualMachine *vm, char *varname)
{
    for (Int i = 0; i < vm->hashes->size; i++)
    {
        if (strcmp(vm->hashes->data[i]->key, varname) == 0)
        {
            return vm->hashes->data[i]->index;
        }
    }
    return -1;
}

void hashset(VirtualMachine *vm, char* varname, Int index)
{
    if (hashfind(vm, varname) != -1)
    {
        vm->hashes->data[hashfind(vm, varname)]->index = index;
    }
    else 
    {
        Hash *hash = (Hash*)malloc(sizeof(Hash));
        hash->key = strduplicate(varname);
        hash->index = index;
        StackPush(*vm->hashes, hash);
    }
}

void hashunset(VirtualMachine *vm, char* varname)
{
    Int index = hashfind(vm, varname);
    if (index != -1)
    {
        free(vm->hashes->data[index]->key);
        vm->hashes->data[index]->key = NULL;
        free(vm->hashes->data[index]);
        vm->hashes->data[index] = NULL;
        StackRemove(*vm->hashes, index);
    }
}

Variable* hashget(VirtualMachine *vm, char* varname)
{
    Int index = hashfind(vm, varname);
    if (index == -1)
    {
        return makeVariable(TYPE_ERROR, (Value){number: 3});//3 is the error code for variable not found
    }
    return vm->stack->data[index];
}