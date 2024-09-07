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

Int std_new(VirtualMachine *vm, VariableList *args)
{
    Variable var = StackShift(*args);
    Int result = newvar(vm);
    vm->stack->data[result] = valueDuplicate(var.value, var.type);
    vm->typestack->data[result] = var.type;
    freerawvar(var);
    return result;
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
        else 
        {
            freerawvar(var);
            
            return -1;
        }
        
        if (_type == TYPE_FUNCTION)
        {
            printf("%p", temp.pointer);
        }
        else if (_type == TYPE_NUMBER)
        {
            if (round(temp.number) == temp.number)
            {
                printf("%d", (Int)temp.number);
            }
            else
            {
                printf("%f", temp.number);
            }
        }
        else if (_type == TYPE_STRING)
        {
            printf("%s", temp.string);
        }
        else if (_type == TYPE_LIST)
        {
            printf("[");
            IntList *list = (IntList*)temp.pointer;
            for (Int i = 0; i < (list->size-1); i++)
            {
                printf("%d, ", list->data[i]);
            }
            if (list->size > 0)
            {
                printf("%d]", list->data[list->size-1]);
            }
            else
            {
                printf("]");
            }
        }
        else if (_type == TYPE_ERROR)
        {
            printf("%s", temp.string);
        }
        else if (_type == TYPE_POINTER)
        {
            printf("%d", (Int)temp.number);
        }
        else if (_type == TYPE_UNUSED)
        {
            printf("{free}");
        }
        else
        {
            printf("{unknown}");
        }
        freerawvar(var);
        
        if (args->size > 0)
        {
            printf(" ");
        }
    }
    printf("\n");
    return -1;
}

Int std_ls(VirtualMachine *vm, VariableList *args)
{
    if (args->size == 0)
    {
        for (Int i = 0; i < vm->stack->size; i++)
        {
            if (vm->typestack->data[i] == TYPE_FUNCTION)
            {
                printf("[%d] {function}: %p\n", i, vm->stack->data[i].pointer);
            }
            else if (vm->typestack->data[i] == TYPE_NUMBER)
            {
                if (round(vm->stack->data[i].number) == vm->stack->data[i].number)
                {
                    printf("[%d] {number}: %d\n", i, (Int)vm->stack->data[i].number);
                }
                else
                {
                    printf("[%d] {number}: %f\n", i, vm->stack->data[i].number);
                }
            }
            else if (vm->typestack->data[i] == TYPE_STRING)
            {
                printf("[%d] {string}: %s\n", i, vm->stack->data[i].string);
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
                printf("[%d] {free}\n", i);
            }
            else
            {
                printf("[%d] {unknown}\n", i);
            }
        }
    }
    else
    {
        Variable _var = StackShift(*args);
        if (_var.type == TYPE_STRING)
        {
            if(strcmp("hashes", _var.value.string) == 0 ||
               strcmp("hash", _var.value.string) == 0) 
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
                        if (round(vm->stack->data[index].number) == vm->stack->data[index].number)
                        {
                            printf("[%s] {number} @%d: %d\n", name, index, (Int)vm->stack->data[index].number);
                        }
                        else
                        {
                            printf("[%s] {number} @%d: %f\n", name, index, vm->stack->data[index].number);
                        }
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
                        printf("[%s] {free}\n", name);
                    }
                    else
                    {
                        printf("[%s] {unknown}\n", name);
                    }
                }
            }
        }
        else if ((_var.type == TYPE_POINTER && vm->typestack->data[(Int)_var.value.number] == TYPE_LIST)
        ||       _var.type == TYPE_LIST)
        {
            IntList *list = (IntList*)vm->stack->data[(Int)_var.value.number].pointer;
            for (Int i = 0; i < list->size; i++)
            {
                if (vm->typestack->data[list->data[i]] == TYPE_FUNCTION)
                {
                    printf("[%d] (%d) {function}: %p\n", i, list->data[i], vm->stack->data[list->data[i]].pointer);
                }
                else if (vm->typestack->data[list->data[i]] == TYPE_NUMBER)
                {
                    if (round(vm->stack->data[list->data[i]].number) == vm->stack->data[list->data[i]].number)
                    {
                        printf("[%d] (%d) {number}: %d\n", i, list->data[i], (Int)vm->stack->data[list->data[i]].number);
                    }
                    else
                    {
                        printf("[%d] (%d) {number}: %f\n", i, list->data[i], vm->stack->data[list->data[i]].number);
                    }
                }
                else if (vm->typestack->data[list->data[i]] == TYPE_STRING)
                {
                    printf("[%d] (%d) {string}: %s\n", i, list->data[i], vm->stack->data[list->data[i]].string);
                }
                else if (vm->typestack->data[list->data[i]] == TYPE_LIST)
                {
                    printf("[%d] (%d) {list}: [", i, list->data[i]);
                    IntList *list2 = (IntList*)vm->stack->data[list->data[i]].pointer;
                    for (Int j = 0; j < (list2->size-1); j++)
                    {
                        printf("%d, ", list2->data[j]);
                    }
                    if (list2->size > 0)
                    {
                        printf("%d]\n", list2->data[list2->size-1]);
                    }
                    else
                    {
                        printf("]\n");
                    }
                }
                else if (vm->typestack->data[list->data[i]] == TYPE_ERROR)
                {
                    printf("[%d] (%d) {error}: %s\n", i, list->data[i], vm->stack->data[list->data[i]].string);
                }
                else if (vm->typestack->data[list->data[i]] == TYPE_POINTER)
                {
                    printf("[%d] (%d) {pointer}: %d\n", i, list->data[i], (Int)vm->stack->data[list->data[i]].number);
                }
                else if (vm->typestack->data[list->data[i]] == TYPE_UNUSED)
                {
                    printf("[%d] (%d) {free}\n", i, list->data[i]);
                }
                else
                {
                    printf("[%d] (%d) {unknown}\n", list->data[i]);
                }
            }
        }
        freerawvar(_var);
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
        else if (index.type == TYPE_STRING || index.type == TYPE_ERROR)
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
    Int result = newvar(vm);
    vm->stack->data[result] = var.value;
    vm->typestack->data[result] = var.type;
    return result;
}

Int std_type(VirtualMachine *vm, VariableList *args)
{
    Variable var = StackShift(*args);
    Int result = newNumber(vm, var.type);
    freerawvar(var);
    return result;
}

Int std_get(VirtualMachine *vm, VariableList *args)
{
    Variable var = StackShift(*args);
    Int result = -1;
    if (var.type == TYPE_STRING)
    {
        Int index = hashfind(vm, var.value.string);
        if (index == -1)
        {
            char *error = strf("Variable %s not found", var.value.string);
            result = newError(vm, error);
            free(error);
        }
        else
        {
            result = index;
        }
    }
    else if (var.type == TYPE_NUMBER || var.type == TYPE_POINTER)
    {
        Int index = (Int)var.value.number;
        if (index >= 0 && index < vm->stack->size)
        {
            result = index;
        }
        else 
        {
            char *error = strf("Variable %d not found", index);
            result = newError(vm, error);
            free(error);
        }
    }
    freerawvar(var);
    return result;
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

Int std_list_get(VirtualMachine *vm, VariableList *args)
{
    Variable list = StackShift(*args);
    Variable index = StackShift(*args);
    if (list.type == TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)list.value.number] == TYPE_LIST)
        {
            IntList *lst = (IntList*)vm->stack->data[(Int)list.value.number].pointer;
            if (index.type == TYPE_NUMBER)
            {
                if (index.value.number >= 0 && index.value.number < lst->size)
                {
                    return lst->data[(Int)index.value.number];
                }
            }
        }
    }
    return -1;
}

// std string

Int std_string_concat(VirtualMachine *vm, VariableList *args)
{
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
    Int result = -1;
    if (a.type == TYPE_NUMBER)
    {
        if (round(a.value.number) == a.value.number)
        {
            char _a[(int)log10(a.value.number)+2];
            sprintf(_a, "%d", (Int)a.value.number);
            a.value.string = strduplicate(_a);
            a.type = TYPE_STRING;
        }
        else
        {
            char _a[(int)log10(a.value.number)+8];
            sprintf(_a, "%f", a.value.number);
            a.value.string = strduplicate(_a);
            a.type = TYPE_STRING;
        }
    }
    else if (a.type == TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)a.value.number] == TYPE_STRING)
        {
            a.value.string = strduplicate(vm->stack->data[(Int)a.value.number].string);
            a.type = TYPE_STRING;
        }
        else if (vm->typestack->data[(Int)a.value.number] == TYPE_NUMBER)
        {
            if (round(vm->stack->data[(Int)a.value.number].number) == vm->stack->data[(Int)a.value.number].number)
            {
                char _a[(int)log10(vm->stack->data[(Int)a.value.number].number)+2];
                sprintf(_a, "%d", (Int)vm->stack->data[(Int)a.value.number].number);
                a.value.string = strduplicate(_a);
                a.type = TYPE_STRING;
            }
            else
            {
                char _a[(int)log10(vm->stack->data[(Int)a.value.number].number)+8];
                sprintf(_a, "%f", vm->stack->data[(Int)a.value.number].number);
                a.value.string = strduplicate(_a);
                a.type = TYPE_STRING;
            }
        }
    }
    else if (a.type == TYPE_ERROR)
    {
        a.value.string = strduplicate(a.value.string);
        a.type = TYPE_STRING;
    }
    else if (a.type == TYPE_FUNCTION)
    {
        char _a[20];
        sprintf(_a, "%p", a.value.pointer);
        a.value.string = strduplicate(_a);
        a.type = TYPE_STRING;
    }
    
    if (b.type == TYPE_NUMBER)
    {
        if (round(b.value.number) == b.value.number)
        {
            char _b[(int)log10(b.value.number)+2];
            sprintf(_b, "%d", (Int)b.value.number);
            b.value.string = strduplicate(_b);
            b.type = TYPE_STRING;
        }
        else
        {
            char _b[(int)log10(b.value.number)+8];
            sprintf(_b, "%f", b.value.number);
            b.value.string = strduplicate(_b);
            b.type = TYPE_STRING;
        }
    }
    else if (b.type == TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)b.value.number] == TYPE_STRING)
        {
            b.value.string = strduplicate(vm->stack->data[(Int)b.value.number].string);
            b.type = TYPE_STRING;
        }
        else if (vm->typestack->data[(Int)b.value.number] == TYPE_NUMBER)
        {
            if (round(vm->stack->data[(Int)b.value.number].number) == vm->stack->data[(Int)b.value.number].number)
            {
                char _b[(int)log10(vm->stack->data[(Int)b.value.number].number)+2];
                sprintf(_b, "%d", (Int)vm->stack->data[(Int)b.value.number].number);
                b.value.string = strduplicate(_b);
                b.type = TYPE_STRING;
            }
            else
            {
                char _b[(int)log10(vm->stack->data[(Int)b.value.number].number)+8];
                sprintf(_b, "%f", vm->stack->data[(Int)b.value.number].number);
                b.value.string = strduplicate(_b);
                b.type = TYPE_STRING;
            }
        }
    }
    else if (b.type == TYPE_ERROR)
    {
        b.value.string = strduplicate(b.value.string);
        b.type = TYPE_STRING;
    }
    else if (b.type == TYPE_FUNCTION)
    {
        char _b[20];
        sprintf(_b, "%p", b.value.pointer);
        b.value.string = strduplicate(_b);
        b.type = TYPE_STRING;
    }

    char* _newstr = strconcat(a.value.string, b.value.string);
    result = newString(vm, _newstr);
    free(_newstr);


    freerawvar(a);
    freerawvar(b);
    return result;
}

Int std_string_find(VirtualMachine *vm, VariableList *args)
{
    Variable str = StackShift(*args);
    Variable substr = StackShift(*args);
    Int result = -1;
    if ((str.type == TYPE_STRING || str.type == TYPE_ERROR) && (substr.type == TYPE_STRING || substr.type == TYPE_ERROR))
    {
        char* _str = str.value.string;
        char* _substr = substr.value.string;
        char* _result = strstr(_str, _substr);
        if (_result != NULL)
        {
            result = newNumber(vm, _result - _str);
        }
    }
    freerawvar(str);
    freerawvar(substr);
    return result;
}

Int std_string_ndup(VirtualMachine *vm, VariableList *args)
{
    Variable str = StackShift(*args);
    Variable start = StackShift(*args);
    Variable end = StackShift(*args);
    Int result = -1;
    if ((str.type == TYPE_STRING || str.type == TYPE_ERROR) && start.type == TYPE_NUMBER && end.type == TYPE_NUMBER)
    {
        char* _str = strndup(str.value.string + (Int)start.value.number, (Int)end.value.number - (Int)start.value.number);
        printf("%s\n", _str);
        result = newString(vm, _str);
        printf("STRING:%s\n", _str);
        free(_str);
    }
    freerawvar(str);
    freerawvar(start);
    freerawvar(end);
    return result;
}

Int std_string_split(VirtualMachine *vm, VariableList *args)
{
    Variable str = StackShift(*args);
    Variable delim = StackShift(*args);
    Int result = -1;
    if ((str.type == TYPE_STRING || str.type == TYPE_ERROR) && (delim.type == TYPE_STRING || delim.type == TYPE_ERROR))
    {
        StringList *list = splitString(str.value.string, delim.value.string);
        result = newList(vm);
        IntList *newlist = (IntList*)vm->stack->data[result].pointer;
        while (list->size > 0)
        {
            char* _str = StackShift(*list);
            printf("STRING:%s\n", _str);
            char* cmd = strf("list.push @%d @%d", result, newString(vm, _str));
            eval(vm, cmd);
            free(cmd);
            free(_str);
        }
        StackFree(*list);
    }
    freerawvar(str);
    freerawvar(delim);
    return result;
}

Int std_string_replace(VirtualMachine *vm, VariableList *args)
{
    Variable str = StackShift(*args);
    Variable substr = StackShift(*args);
    Variable replacement = StackShift(*args);
    Int result = -1;
    if (str.type == TYPE_STRING && substr.type == TYPE_STRING && replacement.type == TYPE_STRING)
    {
        char* _str = str.value.string;
        char* _substr = substr.value.string;
        char* _replacement = replacement.value.string;
        char* _newstr = strreplace(_str, _substr, _replacement);
        result = newString(vm, _newstr);
        free(_newstr);
    }
    freerawvar(str);
    freerawvar(substr);
    freerawvar(replacement);
    return result;
}

Int std_string_length(VirtualMachine *vm, VariableList *args)
{
    Variable str = StackShift(*args);
    Int result = -1;
    if (str.type == TYPE_STRING)
    {
        result = newNumber(vm, strlen(str.value.string));
    }
    freerawvar(str);
    return result;
}

// std conditions
// std conditions
// std conditions

Int std_condition_equals(VirtualMachine *vm, VariableList *args)
{
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
    Int result = -1;
    if (a.type == b.type)
    {
        if (a.type == TYPE_NUMBER)
        {
            result = newNumber(vm, a.value.number == b.value.number);
        }
        else if (a.type == TYPE_STRING)
        {
            result = newNumber(vm, strcmp(a.value.string, b.value.string) == 0);
        }
        else if (a.type == TYPE_POINTER)
        {
            result = newNumber(vm, a.value.number == b.value.number);
        }
        else if (a.type == TYPE_ERROR)
        {
            result = newNumber(vm, strcmp(a.value.string, b.value.string) == 0);
        }
    }
    freerawvar(a);
    freerawvar(b);
    return result;
}

Int std_condition_not_equals(VirtualMachine *vm, VariableList *args)
{
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
    Int result = -1;
    if (a.type == b.type)
    {
        if (a.type == TYPE_NUMBER)
        {
            result = newNumber(vm, a.value.number != b.value.number);
        }
        else if (a.type == TYPE_STRING)
        {
            result = newNumber(vm, strcmp(a.value.string, b.value.string) != 0);
        }
        else if (a.type == TYPE_POINTER)
        {
            result = newNumber(vm, a.value.number != b.value.number);
        }
        else if (a.type == TYPE_ERROR)
        {
            result = newNumber(vm, strcmp(a.value.string, b.value.string) != 0);
        }
    }
    freerawvar(a);
    freerawvar(b);
    return result;
}

Int std_condition_greater(VirtualMachine *vm, VariableList *args)
{
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
    Int result = -1;
    if (a.type == b.type)
    {
        if (a.type == TYPE_NUMBER)
        {
            result = newNumber(vm, a.value.number > b.value.number);
        }
        else if (a.type == TYPE_STRING)
        {
            result = newNumber(vm, strcmp(a.value.string, b.value.string) > 0);
        }
        else if (a.type == TYPE_POINTER)
        {
            result = newNumber(vm, a.value.number > b.value.number);
        }
        else if (a.type == TYPE_ERROR)
        {
            result = newNumber(vm, strcmp(a.value.string, b.value.string) > 0);
        }
    }
    freerawvar(a);
    freerawvar(b);
    return result;
}

Int std_condition_less(VirtualMachine *vm, VariableList *args)
{
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
    Int result = -1;
    if (a.type == b.type)
    {
        if (a.type == TYPE_NUMBER)
        {
            result = newNumber(vm, a.value.number < b.value.number);
        }
        else if (a.type == TYPE_STRING)
        {
            result = newNumber(vm, strcmp(a.value.string, b.value.string) < 0);
        }
        else if (a.type == TYPE_POINTER)
        {
            result = newNumber(vm, a.value.number < b.value.number);
        }
        else if (a.type == TYPE_ERROR)
        {
            result = newNumber(vm, strcmp(a.value.string, b.value.string) < 0);
        }
    }
    freerawvar(a);
    freerawvar(b);
    return result;
}

Int std_condition_greater_equals(VirtualMachine *vm, VariableList *args)
{
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
    Int result = -1;
    if (a.type == b.type)
    {
        if (a.type == TYPE_NUMBER)
        {
            result = newNumber(vm, a.value.number >= b.value.number);
        }
        else if (a.type == TYPE_STRING)
        {
            result = newNumber(vm, strcmp(a.value.string, b.value.string) >= 0);
        }
        else if (a.type == TYPE_POINTER)
        {
            result = newNumber(vm, a.value.number >= b.value.number);
        }
        else if (a.type == TYPE_ERROR)
        {
            result = newNumber(vm, strcmp(a.value.string, b.value.string) >= 0);
        }
    }
    freerawvar(a);
    freerawvar(b);
    return result;
}

Int std_condition_less_equals(VirtualMachine *vm, VariableList *args)
{
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
    Int result = -1;
    if (a.type == b.type)
    {
        if (a.type == TYPE_NUMBER)
        {
            result = newNumber(vm, a.value.number <= b.value.number);
        }
        else if (a.type == TYPE_STRING)
        {
            result = newNumber(vm, strcmp(a.value.string, b.value.string) <= 0);
        }
        else if (a.type == TYPE_POINTER)
        {
            result = newNumber(vm, a.value.number <= b.value.number);
        }
        else if (a.type == TYPE_ERROR)
        {
            result = newNumber(vm, strcmp(a.value.string, b.value.string) <= 0);
        }
    }
    freerawvar(a);
    freerawvar(b);
    return result;
}

Int std_condition_and(VirtualMachine *vm, VariableList *args)
{
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
    Int result = -1;
    result = newNumber(vm, (isTrue(a) && isTrue(b)));
    freerawvar(a);
    freerawvar(b);
    return result;
}

Int std_condition_or(VirtualMachine *vm, VariableList *args)
{
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
    Int result = -1;
    if (isTrue(a))
    {
        Int index = newvar(vm);
        if (a.type == TYPE_LIST || a.type == TYPE_STRING)
        {
            vm->stack->data[index] = a.value;
            freerawvar(b);
        }
        else
        {
            vm->stack->data[index] = valueDuplicate(a.value, a.type);
            freerawvar(a);
            freerawvar(b);
        }
        vm->typestack->data[index] = a.type;
        result = index;
    }
    else
    {
        Int index = newvar(vm);
        if (b.type == TYPE_LIST || b.type == TYPE_STRING)
        {
            vm->stack->data[index] = b.value;
            freerawvar(a);
        }
        else
        {
            vm->stack->data[index] = valueDuplicate(b.value, b.type);
            freerawvar(a);
            freerawvar(b);
        }
    }
    return result;
}

Int std_condition_not(VirtualMachine *vm, VariableList *args)
{
    Variable a = StackShift(*args);
    Int result = -1;
    if (a.type == TYPE_NUMBER)
    {
        result = newNumber(vm, !a.value.number);
    }
    else if (isTrue(a))
    {
        result = newNumber(vm, 0);
    }
    else
    {
        result = newNumber(vm, 1);
    }
    
    if (a.type != TYPE_LIST || a.type != TYPE_STRING)
    {
        freerawvar(a);
    }

    return result;
}

Int std_condition_if(VirtualMachine *vm, VariableList *args)
{
    Variable condition = StackShift(*args);
    Variable _then = StackShift(*args);
    Int result = -1;
    if (isTrue(condition))
    {
        result = eval(vm, _then.value.string);
    }
    freerawvar(condition);
    freerawvar(_then);
    return result;
}

Int std_condition_ifelse(VirtualMachine *vm, VariableList *args)
{
    Variable condition = StackShift(*args);
    Variable _then = StackShift(*args);
    Variable _else = StackShift(*args);
    Int result = -1;
    if (isTrue(condition))
    {
        result = eval(vm, _then.value.string);
    }
    else
    {
        result = eval(vm, _else.value.string);
    }
    freerawvar(condition);
    freerawvar(_then);
    freerawvar(_else);
    return result;
}


void initStd(VirtualMachine *vm)
{
    spawnFunction(vm, "set", std_set);
    spawnFunction(vm, "new", std_new);
    spawnFunction(vm, "print", std_print);
    spawnFunction(vm, "eval", std_eval);
    spawnFunction(vm, "ls", std_ls);
    spawnFunction(vm, "delete", std_delete);
    spawnFunction(vm, "//", std_comment);
    spawnFunction(vm, "return", std_return);
    spawnFunction(vm, "type", std_type);
    spawnFunction(vm, "get", std_get);
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

    spawnFunction(vm, "list.get", std_list_get);
}

void initString(VirtualMachine *vm)
{
    //spawnFunction(vm, "string.format", std_string_new);
    spawnFunction(vm, "string.concat", std_string_concat);
    spawnFunction(vm, "string.find", std_string_find);
    spawnFunction(vm, "string.sub", std_string_ndup);
    spawnFunction(vm, "string.split", std_string_split);
    spawnFunction(vm, "string.replace", std_string_replace);
    spawnFunction(vm, "string.len", std_string_length);
}

void initConditions(VirtualMachine *vm)
{
    spawnFunction(vm, "==", std_condition_equals);
    spawnFunction(vm, "~=", std_condition_not_equals);
    spawnFunction(vm, ">", std_condition_greater);
    spawnFunction(vm, "<", std_condition_less);
    spawnFunction(vm, ">=", std_condition_greater_equals);
    spawnFunction(vm, "<=", std_condition_less_equals);
    spawnFunction(vm, "and", std_condition_and);
    spawnFunction(vm, "or", std_condition_or);
    spawnFunction(vm, "not", std_condition_not);
    spawnFunction(vm, "if", std_condition_if);
    spawnFunction(vm, "ifelse", std_condition_ifelse);
}

void initDefaultVars(VirtualMachine *vm)
{
    spawnNumber(vm, "PI", 3.14159265358979323846);// PI number
    spawnNumber(vm, "ERROR", TYPE_ERROR);// error type
    spawnNumber(vm, "NIL", TYPE_NIL);// nil type
    spawnNumber(vm, "UNUSED", TYPE_UNUSED);// unused type
    spawnNumber(vm, "POINTER", TYPE_POINTER);// pointer type
    spawnNumber(vm, "NUMBER", TYPE_NUMBER);// number type
    spawnNumber(vm, "STRING", TYPE_STRING);// string type
    spawnNumber(vm, "FUNCTION", TYPE_FUNCTION);// function type
    spawnNumber(vm, "LIST", TYPE_LIST);// list type

    spawnString(vm, "VERSION", VERSION);// version
}

void initAll(VirtualMachine *vm)
{
    initStd(vm);
    initMath(vm);
    initList(vm);
    initString(vm);
    initConditions(vm);
    initDefaultVars(vm);
}