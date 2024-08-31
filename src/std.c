#include "bruter.h"


Int std_set(VirtualMachine *vm, VariableList *args)
{
    Variable varname = StackShift(*args);
    Variable value = StackShift(*args);


    if (varname.type == TYPE_STRING)
    {
        char * name = varname.value.string;
        Int index = hashfind(vm, name);

        if (index == -1)
        {
            index = newvar(vm);
            hashset(vm, name, index);
        }
        vm->stack->data[index] = valueDuplicate(value.value, value.type);
        vm->typestack->data[index] = value.type;
    }
    else if (varname.type == TYPE_NUMBER)
    {
        Int index = (Int)varname.value.number;
        if (index >= 0 && index < vm->stack->size)
        {
            unusevar(vm, index);
            vm->stack->data[index] = valueDuplicate(value.value, value.type);
            vm->typestack->data[index] = value.type;
        }
        else 
        {
            //create a new variable
            index = newvar(vm);
            vm->stack->data[index] = valueDuplicate(value.value, value.type);
            vm->typestack->data[index] = value.type;
        }
    }


    freerawvar(varname);
    freerawvar(value);
    return -1;
}

Int std_print(VirtualMachine *vm, VariableList *args)
{
    while (args->size > 0)
    {
        Variable var = StackShift(*args);
        Int _type = -1;

        Value temp = var.value;
        _type = var.type;

        if (var.type == TYPE_POINTER)
        {
            _type = vm->typestack->data[(Int)var.value.number];
            temp = vm->stack->data[(Int)var.value.number];
        }
        
        if (_type == TYPE_NUMBER)
        {
            printf("number: %f\n", temp.number);
        }
        else if (_type == TYPE_STRING)
        {
            printf("%s\n", temp.string);
        }
        else if (_type == TYPE_LIST)
        {
            printf("[");
            IntList *list = (IntList*)temp.pointer;
            for (Int i = 0; i < (list->size-1); i++)
            {
                printf("%d, ", list->data[i]);
            }
            printf("%d", list->data[list->size-1]);
            printf("]\n");
        }
        else if (_type == TYPE_ERROR)
        {
            printf("Error: %s\n", temp.string);
        }
        else if (_type == TYPE_FUNCTION)
        {
            printf("Function : %p\n", temp.pointer);
        }
        else
        {
            printf("Unknown type\n");
        }
        freerawvar(var);
    }
    return -1;
}

Int std_ls(VirtualMachine *vm, VariableList *args)
{
    for (Int i = 0; i < vm->stack->size; i++)
    {
        if (vm->typestack->data[i] == TYPE_FUNCTION)
        {
            printf("[%d] {function}: %p\n", i, vm->stack->data[i].pointer);
        }
        else if (vm->typestack->data[i] == TYPE_NUMBER)
        {
            printf("[%d] {number}: %f\n", i, vm->stack->data[i].number);
        }
        else if (vm->typestack->data[i] == TYPE_STRING)
        {
            char * temp = vm->stack->data[i].string;
            printf("[%d] {string}: %s\n", i, temp);
        }
        else if (vm->typestack->data[i] == TYPE_LIST)
        {
            printf("[%d] {list}: [", i);
            IntList *list = (IntList*)vm->stack->data[i].pointer;
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
        else if (vm->typestack->data[i] == TYPE_ERROR)
        {
            printf("[%d] {error}: %s\n", i, vm->stack->data[i].string);
        }
        else if (vm->typestack->data[i] == TYPE_POINTER)
        {
            printf("[%d] {pointer}: %d\n", i, (Int)vm->stack->data[i].number);
        }
        else if (vm->typestack->data[i] == TYPE_UNUSED)
        {
            printf("[%d] {free slot}\n", i);
        }
        else
        {
            printf("[%d] {unknown type}\n", i);
        }
    }
    return -1;
}

Int std_help(VirtualMachine *vm, VariableList *args)
{
    for (Int i = 0; i < vm->hashes->size; i++)
    {//[name] {type} @index: content
        Int index = vm->hashes->data[i].index;
        char *name = vm->hashes->data[i].key;
        if (vm->typestack->data[index] == TYPE_FUNCTION)
        {
            printf("[%s] {function} @%d: %p\n", name, index, vm->stack->data[index].pointer);
        }
        else if (vm->typestack->data[index] == TYPE_NUMBER)
        {
            printf("[%s] {number} @%d: %f\n", name, index, vm->stack->data[index].number);
        }
        else if (vm->typestack->data[index] == TYPE_STRING)
        {
            printf("[%s] {string} @%d: %s\n", name, index, vm->stack->data[index].string);
        }
        else if (vm->typestack->data[index] == TYPE_LIST)
        {
            printf("[%s] {list} @%d: [", name, index);
            IntList *list = (IntList*)vm->stack->data[index].pointer;
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
        else if (vm->typestack->data[index] == TYPE_ERROR)
        {
            printf("[%s] {error} @%d: %s\n", name, index, vm->stack->data[index].string);
        }
        else if (vm->typestack->data[index] == TYPE_POINTER)
        {
            printf("[%s] {pointer} @%d: %d\n", name, index, (Int)vm->stack->data[index].number);
        }
        else if (vm->typestack->data[index] == TYPE_UNUSED)
        {
            printf("[%s] {free slot}\n", name);
        }
        else
        {
            printf("[%s] {unknown type}\n", name);
        }
    }
    return -1;
}

Int std_eval(VirtualMachine *vm, VariableList *args)
{
    Variable str = StackShift(*args);
    char* _str = str.value.string;
    Int result = eval(vm, _str);
    freerawvar(str);
    return result;
}

Int std_delete(VirtualMachine *vm, VariableList *args)
{
    Variable index;
    while (args->size > 0)
    {
        index = StackShift(*args);
        if (index.type == TYPE_POINTER || index.type == TYPE_NUMBER)
        {
            unusevar(vm, index.value.number);
        }
        else if (index.type == TYPE_STRING)
        {
            Int _index = hashfind(vm, index.value.string);
            if (_index != -1)
            {
                unusevar(vm, _index);
            }
        }
        else
        {
            printf("invalid free!\n");
        }
        freerawvar(index);
    }
    return -1;
}

Int std_comment(VirtualMachine *vm, VariableList *args)
{
    while (args->size > 0)
    {
        Variable var = StackShift(*args);
        freerawvar(var);
    }
    
    return -1;
}

Int std_return(VirtualMachine *vm, VariableList *args)
{
    Variable var = StackShift(*args);
    return var.value.number;
}


// math functions
// math functions
// math functions
// math functions


Int std_math_add(VirtualMachine *vm, VariableList *args)
{
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
    Int result = newNumber(vm, a.value.number + b.value.number);
    freerawvar(a);
    freerawvar(b);
    return result;
}

Int std_math_sub(VirtualMachine *vm, VariableList *args)
{
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
    Int result = newNumber(vm, a.value.number - b.value.number);
    freerawvar(a);
    freerawvar(b);
    return result;
}

Int std_math_mul(VirtualMachine *vm, VariableList *args)
{
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
    Int result = newNumber(vm, a.value.number * b.value.number);
    freerawvar(a);
    freerawvar(b);
    return result;
}

Int std_math_div(VirtualMachine *vm, VariableList *args)
{
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
    Int result = newNumber(vm, a.value.number / b.value.number);
    freerawvar(a);
    freerawvar(b);
    return result;
}

Int std_math_mod(VirtualMachine *vm, VariableList *args)
{
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
    Int result = newNumber(vm, (Int)a.value.number % (Int)b.value.number);
    freerawvar(a);
    freerawvar(b);
    return result;
}

Int std_math_pow(VirtualMachine *vm, VariableList *args)
{
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
    Int result = newNumber(vm, pow(a.value.number, b.value.number));
    freerawvar(a);
    freerawvar(b);
    return result;
}


Int std_math_sqrt(VirtualMachine *vm, VariableList *args)
{
    Variable a = StackShift(*args);
    Int result = newNumber(vm, sqrt(a.value.number));
    freerawvar(a);
    return result;
}

Int std_math_abs(VirtualMachine *vm, VariableList *args)
{
    Variable a = StackShift(*args);
    Int result = newNumber(vm, fabs(a.value.number));
    freerawvar(a);
    return result;
}

Int std_math_random(VirtualMachine *vm, VariableList *args)
{
    Variable ___min = StackShift(*args);
    Variable ___max = StackShift(*args);
    Int result = newNumber(vm, rand() % (Int)___max.value.number + (Int)___min.value.number);
    freerawvar(___min);
    freerawvar(___max);
    return result;
}

Int std_math_seed(VirtualMachine *vm, VariableList *args)
{
    Variable seed = StackShift(*args);
    srand((Int)seed.value.number);
    freerawvar(seed);
    return -1;
}

Int std_math_floor(VirtualMachine *vm, VariableList *args)
{
    Variable a = StackShift(*args);
    Int result = newNumber(vm, floor(a.value.number));
    freerawvar(a);
    return result;;
}

Int std_math_ceil(VirtualMachine *vm, VariableList *args)
{
    Variable a = StackShift(*args);
    Int result = newNumber(vm, ceil(a.value.number));
    freerawvar(a);
    return result;
}

Int std_math_round(VirtualMachine *vm, VariableList *args)
{
    Variable a = StackShift(*args);
    Int result = newNumber(vm, round(a.value.number));
    freerawvar(a);
    return result;
}


// list functions
// list functions
// list functions
// list functions

Int std_list_new(VirtualMachine *vm, VariableList *args) 
{
    Int index = newList(vm);
    IntList *list = (IntList*)vm->stack->data[index].pointer;
    while (args->size > 0)
    {
        Variable var = StackShift(*args);
        if (var.type == TYPE_POINTER)
        {
            StackPush(*list, (Int)var.value.number);
        }
        else 
        {
            Int tmp = newvar(vm);
            vm->stack->data[tmp] = valueDuplicate(var.value, var.type);
            vm->typestack->data[tmp] = var.type;
            StackPush(*list, tmp);
        }
        freerawvar(var);
    }
    return index;
}

Int std_list_insert(VirtualMachine *vm, VariableList *args)
{
    Variable list = StackShift(*args);
    Variable index = StackShift(*args);
    Variable value = StackShift(*args);

    if (list.type == TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)list.value.number] == TYPE_LIST)
        {
            IntList *lst = (IntList*)vm->stack->data[(Int)list.value.number].pointer;
            StackInsert(*lst, (Int)index.value.number, (Int)value.value.number);
        }
    }
    return -1;
}

Int std_list_push(VirtualMachine *vm, VariableList *args)
{
    Variable list = StackShift(*args);
    Variable value = StackShift(*args);

    if (list.type == TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)list.value.number] == TYPE_LIST)
        {
            IntList *lst = (IntList*)vm->stack->data[(Int)list.value.number].pointer;
            StackPush(*lst, (Int)value.value.number);
        }
    }
    
    //freerawvar(list);
    freerawvar(value);
    return -1;
}

Int std_list_unshift(VirtualMachine *vm, VariableList *args) // returns the removed element
{
    Variable list = StackShift(*args);
    Variable value = StackShift(*args);
    if (list.type == TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)list.value.number] == TYPE_LIST)
        {
            IntList *lst = (IntList*)vm->stack->data[(Int)list.value.number].pointer;
            StackUnshift(*lst, (Int)value.value.number);
        }
    }
    freerawvar(value);
    return -1;   
}

Int std_list_remove(VirtualMachine *vm, VariableList *args)// returns the removed element
{
    Variable list = StackShift(*args);
    Variable index = StackShift(*args);
    if (list.type == TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)list.value.number] == TYPE_LIST)
        {
            IntList *lst = (IntList*)vm->stack->data[(Int)list.value.number].pointer;
            Int result = StackRemove(*lst, (Int)index.value.number);
            return result;
        }
    }
    return -1;
}

Int std_list_pop(VirtualMachine *vm, VariableList *args) // returns the removed element
{
    Variable list = StackShift(*args);
    if (list.type == TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)list.value.number] == TYPE_LIST)
        {
            IntList *lst = (IntList*)vm->stack->data[(Int)list.value.number].pointer;
            Int result = StackPop(*lst);
            return result;
        }
    }
    return -1;
}

Int std_list_shift(VirtualMachine *vm, VariableList *args)
{
    Variable list = StackShift(*args);
    if (list.type == TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)list.value.number] == TYPE_LIST)
        {
            IntList *lst = (IntList*)vm->stack->data[(Int)list.value.number].pointer;
            Int result = StackShift(*lst);
            return result;
        }
    }
    return -1;
}

Int std_list_concat(VirtualMachine *vm, VariableList *args)
{
    Variable list1 = StackShift(*args);
    Variable list2 = StackShift(*args);
    Int _newlist = newList(vm);
    IntList *newlist = (IntList*)vm->stack->data[_newlist].pointer;
    if (list1.type == TYPE_POINTER && list2.type == TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)list1.value.number] == TYPE_LIST && vm->typestack->data[(Int)list2.value.number] == TYPE_LIST)
        {
            IntList *lst1 = (IntList*)vm->stack->data[(Int)list1.value.number].pointer;
            IntList *lst2 = (IntList*)vm->stack->data[(Int)list2.value.number].pointer;
            for (Int i = 0; i < lst1->size; i++)
            {
                StackPush(*newlist, lst1->data[i]);
            }
            for (Int i = 0; i < lst2->size; i++)
            {
                StackPush(*newlist, lst2->data[i]);
            }
        }
    }
    return _newlist;
}

Int std_list_find(VirtualMachine *vm, VariableList *args)
{
    Variable list = StackShift(*args);
    Variable value = StackShift(*args);
    if (list.type == TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)list.value.number] == TYPE_LIST)
        {
            IntList *lst = (IntList*)vm->stack->data[(Int)list.value.number].pointer;
            for (Int i = 0; i < lst->size; i++)
            {
                if (lst->data[i] == (Int)value.value.number)
                {
                    return i;
                }
            }
        }
    }
    return -1;
}

void initStd(VirtualMachine *vm)
{
    spawnFunction(vm, "set", std_set);
    spawnFunction(vm, "print", std_print);
    spawnFunction(vm, "eval", std_eval);
    spawnFunction(vm, "ls", std_ls);
    spawnFunction(vm, "help", std_help);
    spawnFunction(vm, "delete", std_delete);
    spawnFunction(vm, "comment", std_comment);
    spawnFunction(vm, "return", std_return);
}

void initMath(VirtualMachine *vm)
{
    spawnFunction(vm, "add", std_math_add);
    spawnFunction(vm, "sub", std_math_sub);
    spawnFunction(vm, "mul", std_math_mul);
    spawnFunction(vm, "div", std_math_div);
    spawnFunction(vm, "mod", std_math_mod);
    spawnFunction(vm, "pow", std_math_pow);
    spawnFunction(vm, "abs", std_math_abs);
    spawnFunction(vm, "sqrt", std_math_sqrt);
    spawnFunction(vm, "ceil", std_math_ceil);
    spawnFunction(vm, "seed", std_math_seed);
    spawnFunction(vm, "floor", std_math_floor);
    spawnFunction(vm, "round", std_math_round);
    spawnFunction(vm, "random", std_math_random);
}

void initList(VirtualMachine *vm)
{
    spawnFunction(vm, "list.new", std_list_new);

    spawnFunction(vm, "list.insert", std_list_insert);
    spawnFunction(vm, "list.push", std_list_push);
    spawnFunction(vm, "list.unshift", std_list_unshift);
    
    spawnFunction(vm, "list.remove", std_list_remove);
    spawnFunction(vm, "list.pop", std_list_pop);
    spawnFunction(vm, "list.shift", std_list_shift);

    spawnFunction(vm, "list.concat", std_list_concat);
    spawnFunction(vm, "list.find", std_list_find);
}

void initAll(VirtualMachine *vm)
{
    initStd(vm);
    initMath(vm);
    initList(vm);
}