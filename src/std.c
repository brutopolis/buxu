#include "bruter.h"

//functions

Int _set(VirtualMachine *vm, IntList *args)
{
    Reference varname = argshift(vm, args);
    Reference value = argshift(vm, args);

    //printf("Set %d to %d\n", varname.index, value.index);


    if (varname.variable->type != TYPE_STRING)
    {
        //return newError(vm, 2);//"First argument must be a string"
    }

    if (value.variable->type == TYPE_ERROR)
    {
        //return newError(vm, 1);//"Second argument must be a value"
    }

    char * name = toString(varname.variable->value.pointer);
    Int index = hashfind(vm, name);
    
    if (index == -1)
    {
        index = newVar(vm);
        hashset(vm, name, index);
    }
    setVar(vm, index, value.variable->type, value.variable->value, Nil);


    freeref(vm, varname);
    freeref(vm, value);
    free(name);
    return -1;
}

Int _eval(VirtualMachine *vm, IntList *args)
{
    Reference str = argshift(vm, args);
    char* _str = toString(str.variable->value.pointer);
    Int result = eval(vm, _str);
    freeref(vm, str);
    free(_str);
    return result;
}

Int _print(VirtualMachine *vm, IntList *args)
{
    Reference _ref = argshift(vm, args);
    //print(vm, _ref.index);
    if (vm->stack->data[_ref.index]->type == TYPE_NUMBER)
    {
        printf("%f\n", vm->stack->data[_ref.index]->value.number);
    }
    else if (vm->stack->data[_ref.index]->type == TYPE_STRING)
    {
        char * temp = toString(vm->stack->data[_ref.index]->value.pointer);
        printf("%s\n", temp);
        free(temp);
    }
    else if (vm->stack->data[_ref.index]->type == TYPE_LIST)
    {
        printf("[");
        IntList *list = (IntList*)vm->stack->data[_ref.index]->value.pointer;
        for (Int i = 0; i < (list->size-1); i++)
        {
            printf("%d, ", list->data[i]);
        }
        printf("%d", list->data[list->size-1]);
        printf("]\n");
    }
    /*else if (vm->stack->data[_ref.index]->type == TYPE_ERROR)
    {
        printf("Error: %s\n", toString(vm->stack->data[_ref.index]->value.pointer));
    }*/
    else if (vm->stack->data[_ref.index]->type == TYPE_FUNCTION)
    {
        printf("Function : [%d] %p\n", _ref.index, vm->stack->data[_ref.index]->value.pointer);
    }
    else
    {
        printf("Unknown type\n");
    }
    
    freeref(vm, _ref);
    return -1;
}

Int _help(VirtualMachine *vm, IntList *args)
{
    for (Int i = 0; i < vm->hashes->size; i++)
    {
        if (vm->hashes->data[i] != NULL)
        {
            if (vm->stack->data[vm->hashes->data[i]->index]->type == TYPE_FUNCTION)
            {
                printf("[%d] {function} @%s: %p\n", i, vm->hashes->data[i]->key, vm->stack->data[vm->hashes->data[i]->index]->value.pointer);
            }
            else if (vm->stack->data[vm->hashes->data[i]->index]->type == TYPE_NUMBER)
            {
                printf("[%d] {number} @%s: %f\n", i, vm->hashes->data[i]->key, vm->stack->data[vm->hashes->data[i]->index]->value.number);
            }
            else if (vm->stack->data[vm->hashes->data[i]->index]->type == TYPE_STRING)
            {
                char * temp = toString(vm->stack->data[vm->hashes->data[i]->index]->value.pointer);
                printf("[%d] {string} @%s: %s\n", i, vm->hashes->data[i]->key, temp);
                free(temp);
            }
            else if (vm->stack->data[vm->hashes->data[i]->index]->type == TYPE_LIST)
            {
                printf("[%d] {list} @%s: [", i, vm->hashes->data[i]->key);
                IntList *list = (IntList*)vm->stack->data[vm->hashes->data[i]->index]->value.pointer;
                for (Int j = 0; j < (list->size-1); j++)
                {
                    printf("%d, ", list->data[j]);
                }
                if (list->size > 0)
                {
                    printf("%d]\n", list->data[list->size-1]);
                }
                else
                {
                    printf("]\n");
                }
            }
            else if (vm->stack->data[vm->hashes->data[i]->index]->type == TYPE_ERROR)
            {
                printf("[%d] {error} @%s: %s\n", i, vm->hashes->data[i]->key, toString(vm->stack->data[vm->hashes->data[i]->index]->value.pointer));
            }
            else
            {
                printf("[%d] {unknown type} @%s\n", i, vm->hashes->data[i]->key);
            }
        }
    }
    return -1;
}

Int _ls(VirtualMachine *vm, IntList *args)
{
    for (Int i = 0; i < vm->stack->size; i++)
    {
        if (vm->stack->data[i] != NULL)
        {
            if (vm->stack->data[i]->type == TYPE_FUNCTION)
            {
                printf("[%d] {function}: %p\n", i, vm->stack->data[i]->value.pointer);
            }
            else if (vm->stack->data[i]->type == TYPE_NUMBER)
            {
                printf("[%d] {number}: %f\n", i, vm->stack->data[i]->value.number);
            }
            else if (vm->stack->data[i]->type == TYPE_STRING)
            {
                
            }
            else if (vm->stack->data[i]->type == TYPE_LIST)
            {
                printf("[%d] {list}: [", i);
                IntList *list = (IntList*)vm->stack->data[i]->value.pointer;
                for (Int j = 0; j < (list->size-1); j++)
                {
                    printf("%d, ", list->data[j]);
                }
                if (list->size > 0)
                {
                    printf("%d]\n", list->data[list->size-1]);
                }
                else
                {
                    printf("]\n");
                }
            }
            /*else if (vm->stack->data[i]->type == TYPE_ERROR)
            {
                printf("[%d] {error}: %s\n", i, toString(vm->stack->data[i]->value.pointer));
            }*/
            else
            {
                printf("[%d] {unknown type}\n", i);
            }
        }
    }
    return -1;
}

Int ___exit(VirtualMachine *vm, IntList *args)
{
    freeVM(vm);
    exit(0);
}

//math

Int _add(VirtualMachine *vm, IntList *args)
{
    Reference a = argshift(vm, args);
    Reference b = argshift(vm, args);

    if (a.variable->type == TYPE_NUMBER && b.variable->type == TYPE_NUMBER)
    {
        Int index = newVar(vm);
        vm->stack->data[index]->type = TYPE_NUMBER;
        vm->stack->data[index]->value.number = a.variable->value.number + b.variable->value.number;
        freeref(vm, a);
        freeref(vm, b);
        return index;
    }
    else
    {
        freeref(vm, a);
        freeref(vm, b);
        //return newError(vm, "Both arguments must be numbers");
    }
}

//list

Int _list(VirtualMachine *vm, IntList *args)
{
    Int index = newVar(vm);
    vm->stack->data[index]->type = TYPE_LIST;
    vm->stack->data[index]->value.pointer = malloc(sizeof(IntList));
    StackInit(*(IntList*)vm->stack->data[index]->value.pointer);
    return index;
}

void initStd(VirtualMachine *vm)
{
    spawnFunction(vm, "eval", _eval);
    spawnFunction(vm, "print", _print);
    spawnFunction(vm, "set", _set);
    spawnFunction(vm, "help", _help);
    spawnFunction(vm, "ls", _ls);
    spawnFunction(vm, "exit", ___exit);

    spawnFunction(vm, "add", _add);

    spawnFunction(vm, "list", _list);
}