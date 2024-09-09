#include "bruter.h"

Int std_temp(VirtualMachine *vm, IntList *args)
{
    Int result = StackShift(*args);
    StackPush(*vm->temp, result);
    return result;
}

Int std_clear(VirtualMachine *vm, IntList *args)
{
    while (vm->temp->size > 0)
    {
        unusevar(vm, StackShift(*vm->temp));
    }
    return -1;
}

Int std_set(VirtualMachine *vm, IntList *args)
{
    Int varname = StackShift(*args);
    Int value = StackShift(*args);

    if (vm->typestack->data[varname] == TYPE_STRING)
    {
        char * name = vm->stack->data[varname].string;
        Int index = hashfind(vm, name);

        if (index >= 0)
        {
            unusevar(vm, index);
            index = newvar(vm);
            vm->stack->data[index] = valueDuplicate(vm->stack->data[value], vm->typestack->data[value]);
            vm->typestack->data[index] = vm->typestack->data[value];
        }
        else 
        {
            //create a new variable
            index = newvar(vm);
            vm->stack->data[index] = valueDuplicate(vm->stack->data[value], vm->typestack->data[value]);
            vm->typestack->data[index] = vm->typestack->data[value];
            hashset(vm, name, index);
        }
    }
    else if (vm->typestack->data[varname] == TYPE_NUMBER)
    {
        Int index = (Int)vm->stack->data[varname].number;
        if (index >= 0)
        {
            unusevar(vm, index);
            index = newvar(vm);
            vm->stack->data[index] = valueDuplicate(vm->stack->data[value], vm->typestack->data[value]);
            vm->typestack->data[index] = vm->typestack->data[value];
        }
        else 
        {
            //create a new variable
            index = newvar(vm);
            vm->stack->data[index] = valueDuplicate(vm->stack->data[value], vm->typestack->data[value]);
            vm->typestack->data[index] = vm->typestack->data[value];
        }
    }
    //unusevar(vm, varname);
    return -1;
}

Int std_new(VirtualMachine *vm, IntList *args)
{
    Int var = StackShift(*args);
    return var;
}

Int std_print(VirtualMachine *vm, IntList *args)
{
    while (args->size > 0)
    {
        Int var = StackShift(*args);
        Int _type = -1;

        Value temp = vm->stack->data[var];
        _type = vm->typestack->data[var];

        if (vm->typestack->data[var] == TYPE_POINTER)
        {
            _type = vm->typestack->data[(Int)vm->stack->data[var].number];
            temp = vm->stack->data[(Int)vm->stack->data[var].number];
        }
        else 
        {
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
        
        if (args->size > 0)
        {
            printf(" ");
        }
    }
    printf("\n");
    return -1;
}

Int std_ls(VirtualMachine *vm, IntList *args)
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
        Int _var = StackShift(*args);
        if (vm->typestack->data[_var] == TYPE_STRING)
        {
            if(strcmp("hashes", vm->stack->data[_var].string) == 0 ||
               strcmp("hash", vm->stack->data[_var].string) == 0) 
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
        else if ((vm->typestack->data[_var] == TYPE_POINTER && vm->typestack->data[(Int)vm->stack->data[_var].number] == TYPE_LIST)
        ||       vm->typestack->data[_var] == TYPE_LIST)
        {
            IntList *list = (IntList*)vm->stack->data[(Int)vm->stack->data[_var].number].pointer;
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
    }
    return -1;
}

Int std_eval(VirtualMachine *vm, IntList *args)
{
    Int str = StackShift(*args);
    char* _str = vm->stack->data[str].string;
    Int result = eval(vm, _str);
    return result;
}

Int std_delete(VirtualMachine *vm, IntList *args)
{
    Int index;
    while (args->size > 0)
    {
        index = StackShift(*args);
        if (vm->typestack->data[index] == TYPE_POINTER || vm->typestack->data[index] == TYPE_NUMBER)
        {
            unusevar(vm, vm->stack->data[index].number);
        }
        else if (vm->typestack->data[index] == TYPE_STRING || vm->typestack->data[index] == TYPE_ERROR)
        {
            Int _index = hashfind(vm, vm->stack->data[index].string);
            if (_index != -1)
            {
                unusevar(vm, _index);
            }
        }
        else
        {
            printf("invalid free!\n");
        }
    }
    return -1;
}

Int std_comment(VirtualMachine *vm, IntList *args)
{
    return -1;
}

Int std_return(VirtualMachine *vm, IntList *args)
{
    Int var = StackShift(*args);
    return var;
}

Int std_type(VirtualMachine *vm, IntList *args)
{
    Int var = StackShift(*args);
    Int result = newNumber(vm, vm->typestack->data[var]);
    return result;
}

Int std_get(VirtualMachine *vm, IntList *args)
{
    Int var = StackShift(*args);
    Int result = -1;
    if (vm->typestack->data[var] == TYPE_STRING)
    {
        Int index = hashfind(vm, vm->stack->data[var].string);
        if (index == -1)
        {
            char *error = strf("Variable %s not found", vm->stack->data[var].string);
            result = newError(vm, error);
            free(error);
        }
        else
        {
            result = index;
        }
    }
    else if (vm->typestack->data[var] == TYPE_NUMBER || vm->typestack->data[var] == TYPE_POINTER)
    {
        Int index = (Int)vm->stack->data[var].number;
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
    return result;
}

// math functions
// math functions
// math functions
// math functions


Int std_math_add(VirtualMachine *vm, IntList *args)
{
    Int a = StackShift(*args);
    Int b = StackShift(*args);
    Int result = newNumber(vm, vm->stack->data[a].number + vm->stack->data[b].number);
    return result;
}

Int std_math_sub(VirtualMachine *vm, IntList *args)
{
    Int a = StackShift(*args);
    Int b = StackShift(*args);
    Int result = newNumber(vm, vm->stack->data[a].number - vm->stack->data[b].number);
    return result;
}

Int std_math_mul(VirtualMachine *vm, IntList *args)
{
    Int a = StackShift(*args);
    Int b = StackShift(*args);
    Int result = newNumber(vm, vm->stack->data[a].number * vm->stack->data[b].number);
    
    
    return result;
}

Int std_math_div(VirtualMachine *vm, IntList *args)
{
    Int a = StackShift(*args);
    Int b = StackShift(*args);
    Int result = newNumber(vm, vm->stack->data[a].number / vm->stack->data[b].number);
    
    
    return result;
}

Int std_math_mod(VirtualMachine *vm, IntList *args)
{
    Int a = StackShift(*args);
    Int b = StackShift(*args);
    Int result = newNumber(vm, (Int)vm->stack->data[a].number % (Int)vm->stack->data[b].number);
    
    
    return result;
}

Int std_math_pow(VirtualMachine *vm, IntList *args)
{
    Int a = StackShift(*args);
    Int b = StackShift(*args);
    Int result = newNumber(vm, pow(vm->stack->data[a].number, vm->stack->data[b].number));
    
    
    return result;
}


Int std_math_sqrt(VirtualMachine *vm, IntList *args)
{
    Int a = StackShift(*args);
    Int result = newNumber(vm, sqrt(vm->stack->data[a].number));
    
    return result;
}

Int std_math_abs(VirtualMachine *vm, IntList *args)
{
    Int a = StackShift(*args);
    Int result = newNumber(vm, fabs(vm->stack->data[a].number));
    
    return result;
}

Int std_math_random(VirtualMachine *vm, IntList *args)
{
    Int ___min = StackShift(*args);
    Int ___max = StackShift(*args);
    Int result = newNumber(vm, rand() % (Int)vm->stack->data[___max].number + (Int)vm->stack->data[___min].number);
    return result;
}

Int std_math_seed(VirtualMachine *vm, IntList *args)
{
    Int seed = StackShift(*args);
    srand((Int)vm->stack->data[seed].number);
    return -1;
}

Int std_math_floor(VirtualMachine *vm, IntList *args)
{
    Int a = StackShift(*args);
    Int result = newNumber(vm, floor(vm->stack->data[a].number));
    return result;;
}

Int std_math_ceil(VirtualMachine *vm, IntList *args)
{
    Int a = StackShift(*args);
    Int result = newNumber(vm, ceil(vm->stack->data[a].number));
    return result;
}

Int std_math_round(VirtualMachine *vm, IntList *args)
{
    Int a = StackShift(*args);
    Int result = newNumber(vm, round(vm->stack->data[a].number));
    return result;
}


// list functions
// list functions
// list functions
// list functions

Int std_list_new(VirtualMachine *vm, IntList *args) 
{
    Int index = newList(vm);
    IntList *list = (IntList*)vm->stack->data[index].pointer;
    while (args->size > 0)
    {
        Int var = StackShift(*args);
        if (vm->typestack->data[var] == TYPE_POINTER)
        {
            StackPush(*list, (Int)vm->stack->data[var].number);
        }
        else 
        {
            Int tmp = newvar(vm);
            vm->stack->data[tmp] = valueDuplicate(vm->stack->data[var], vm->typestack->data[var]);
            vm->typestack->data[tmp] = vm->typestack->data[var];
            StackPush(*list, tmp);
        }
    }
    return index;
}

Int std_list_insert(VirtualMachine *vm, IntList *args)
{
    Int list = StackShift(*args);
    Int index = StackShift(*args);
    Int value = StackShift(*args);

    if (vm->typestack->data[list] == TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)vm->stack->data[list].number] == TYPE_LIST)
        {
            IntList *lst = (IntList*)vm->stack->data[(Int)vm->stack->data[list].number].pointer;
            StackInsert(*lst, (Int)vm->stack->data[index].number, (Int)vm->stack->data[value].number);
        }
    }
    return -1;
}

Int std_list_push(VirtualMachine *vm, IntList *args)
{
    Int list = StackShift(*args);
    Int value = StackShift(*args);

    if (vm->typestack->data[list] == TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)vm->stack->data[list].number] == TYPE_LIST)
        {
            IntList *lst = (IntList*)vm->stack->data[(Int)vm->stack->data[list].number].pointer;
            StackPush(*lst, (Int)vm->stack->data[value].number);
        }
    }
    return -1;
}

Int std_list_unshift(VirtualMachine *vm, IntList *args) // returns the removed element
{
    Int list = StackShift(*args);
    Int value = StackShift(*args);
    if (vm->typestack->data[list] == TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)vm->stack->data[list].number] == TYPE_LIST)
        {
            IntList *lst = (IntList*)vm->stack->data[(Int)vm->stack->data[list].number].pointer;
            StackUnshift(*lst, (Int)vm->stack->data[value].number);
        }
    }
    return -1;   
}

Int std_list_remove(VirtualMachine *vm, IntList *args)// returns the removed element
{
    Int list = StackShift(*args);
    Int index = StackShift(*args);
    if (vm->typestack->data[list] == TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)vm->stack->data[list].number] == TYPE_LIST)
        {
            IntList *lst = (IntList*)vm->stack->data[(Int)vm->stack->data[list].number].pointer;
            Int result = StackRemove(*lst, (Int)vm->stack->data[list].number);
            return result;
        }
    }
    return -1;
}

Int std_list_pop(VirtualMachine *vm, IntList *args) // returns the removed element
{
    Int list = StackShift(*args);
    if (vm->typestack->data[list] == TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)vm->stack->data[list].number] == TYPE_LIST)
        {
            IntList *lst = (IntList*)vm->stack->data[(Int)vm->stack->data[list].number].pointer;
            Int result = StackPop(*lst);
            return result;
        }
    }
    return -1;
}

Int std_list_shift(VirtualMachine *vm, IntList *args)
{
    Int list = StackShift(*args);
    if (vm->typestack->data[list]== TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)vm->stack->data[list].number] == TYPE_LIST)
        {
            IntList *lst = (IntList*)vm->stack->data[(Int)vm->stack->data[list].number].pointer;
            Int result = StackShift(*lst);
            return result;
        }
    }
    return -1;
}

Int std_list_concat(VirtualMachine *vm, IntList *args)
{
    Int list1 = StackShift(*args);
    Int list2 = StackShift(*args);
    Int _newlist = newList(vm);
    IntList *newlist = (IntList*)vm->stack->data[_newlist].pointer;
    if (vm->typestack->data[list1] == TYPE_POINTER && vm->typestack->data[list2] == TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)vm->stack->data[list1].number] == TYPE_LIST && vm->typestack->data[(Int)vm->stack->data[list2].number] == TYPE_LIST)
        {
            IntList *lst1 = (IntList*)vm->stack->data[(Int)vm->stack->data[list1].number].pointer;
            IntList *lst2 = (IntList*)vm->stack->data[(Int)vm->stack->data[list2].number].pointer;
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

Int std_list_find(VirtualMachine *vm, IntList *args)
{
    Int list = StackShift(*args);
    Int value = StackShift(*args);
    if (vm->typestack->data[list] == TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)vm->stack->data[list].number] == TYPE_LIST)
        {
            IntList *lst = (IntList*)vm->stack->data[(Int)vm->stack->data[list].number].pointer;
            for (Int i = 0; i < lst->size; i++)
            {
                if (lst->data[i] == (Int)vm->stack->data[list].number)
                {
                    return i;
                }
            }
        }
    }
    return -1;
}

Int std_list_get(VirtualMachine *vm, IntList *args)
{
    Int list = StackShift(*args);
    Int index = StackShift(*args);
    if (vm->typestack->data[list] == TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)vm->stack->data[list].number] == TYPE_LIST)
        {
            IntList *lst = (IntList*)vm->stack->data[(Int)vm->stack->data[list].number].pointer;
            if (vm->typestack->data[index] == TYPE_NUMBER)
            {
                if (vm->stack->data[index].number >= 0 && vm->stack->data[index].number < lst->size)
                {
                    return lst->data[(Int)vm->stack->data[index].number];
                }
            }
        }
    }
    return -1;
}

// std string

Int std_string_concat(VirtualMachine *vm, IntList *args)
{
    Int a = StackShift(*args);
    Int b = StackShift(*args);
    Int result = -1;
    if (vm->typestack->data[a] == TYPE_NUMBER)
    {
        if (round(vm->stack->data[a].number) == vm->stack->data[a].number)
        {
            char _a[(int)log10(vm->stack->data[a].number)+2];
            sprintf(_a, "%d", (Int)vm->stack->data[a].number);
            vm->stack->data[a].string = strduplicate(_a);
            vm->typestack->data[a] = TYPE_STRING;
        }
        else
        {
            char _a[(int)log10(vm->stack->data[a].number)+8];
            sprintf(_a, "%f", vm->stack->data[a].number);
            vm->stack->data[a].string = strduplicate(_a);
            vm->typestack->data[a] = TYPE_STRING;
        }
    }
    else if (vm->typestack->data[a] == TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)vm->stack->data[a].number] == TYPE_STRING)
        {
            vm->stack->data[a].string = strduplicate(vm->stack->data[(Int)vm->stack->data[a].number].string);
            vm->typestack->data[a] = TYPE_STRING;
        }
        else if (vm->typestack->data[(Int)vm->stack->data[a].number] == TYPE_NUMBER)
        {
            if (round(vm->stack->data[(Int)vm->stack->data[a].number].number) == vm->stack->data[(Int)vm->stack->data[a].number].number)
            {
                char _a[(int)log10(vm->stack->data[(Int)vm->stack->data[a].number].number)+2];
                sprintf(_a, "%d", (Int)vm->stack->data[(Int)vm->stack->data[a].number].number);
                vm->stack->data[a].string = strduplicate(_a);
                vm->typestack->data[a] = TYPE_STRING;
            }
            else
            {
                char _a[(int)log10(vm->stack->data[(Int)vm->stack->data[a].number].number)+8];
                sprintf(_a, "%f", vm->stack->data[(Int)vm->stack->data[a].number].number);
                vm->stack->data[a].string = strduplicate(_a);
                vm->typestack->data[a] = TYPE_STRING;
            }
        }
    }
    else if (vm->typestack->data[a] == TYPE_ERROR)
    {
        vm->stack->data[a].string = strduplicate(vm->stack->data[a].string);
        vm->typestack->data[a] = TYPE_STRING;
    }
    else if (vm->typestack->data[a] == TYPE_FUNCTION)
    {
        char _a[20];
        sprintf(_a, "%p", vm->stack->data[a].pointer);
        vm->stack->data[a].string = strduplicate(_a);
        vm->typestack->data[a] = TYPE_STRING;
    }
    
    if (vm->typestack->data[b] == TYPE_NUMBER)
    {
        if (round(vm->stack->data[b].number) == vm->stack->data[b].number)
        {
            char _b[(int)log10(vm->stack->data[b].number)+2];
            sprintf(_b, "%d", (Int)vm->stack->data[b].number);
            vm->stack->data[b].string = strduplicate(_b);
            vm->typestack->data[b] = TYPE_STRING;
        }
        else
        {
            char _b[(int)log10(vm->stack->data[b].number)+8];
            sprintf(_b, "%f", vm->stack->data[b].number);
            vm->stack->data[b].string = strduplicate(_b);
            vm->typestack->data[b] = TYPE_STRING;
        }
    }
    else if (vm->typestack->data[b] == TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)vm->stack->data[b].number] == TYPE_STRING)
        {
            vm->stack->data[b].string = strduplicate(vm->stack->data[(Int)vm->stack->data[b].number].string);
            vm->typestack->data[b] = TYPE_STRING;
        }
        else if (vm->typestack->data[(Int)vm->stack->data[b].number] == TYPE_NUMBER)
        {
            if (round(vm->stack->data[(Int)vm->stack->data[b].number].number) == vm->stack->data[(Int)vm->stack->data[b].number].number)
            {
                char _b[(int)log10(vm->stack->data[(Int)vm->stack->data[b].number].number)+2];
                sprintf(_b, "%d", (Int)vm->stack->data[(Int)vm->stack->data[b].number].number);
                vm->stack->data[b].string = strduplicate(_b);
                vm->typestack->data[b] = TYPE_STRING;
            }
            else
            {
                char _b[(int)log10(vm->stack->data[(Int)vm->stack->data[b].number].number)+8];
                sprintf(_b, "%f", vm->stack->data[(Int)vm->stack->data[b].number].number);
                vm->stack->data[b].string = strduplicate(_b);
                vm->typestack->data[b] = TYPE_STRING;
            }
        }
    }
    else if (vm->typestack->data[b] == TYPE_ERROR)
    {
        vm->stack->data[b].string = strduplicate(vm->stack->data[b].string);
        vm->typestack->data[b] = TYPE_STRING;
    }
    else if (vm->typestack->data[b] == TYPE_FUNCTION)
    {
        char _b[20];
        sprintf(_b, "%p", vm->stack->data[b].pointer);
        vm->stack->data[b].string = strduplicate(_b);
        vm->typestack->data[b] = TYPE_STRING;
    }

    char* _newstr = strconcat(vm->stack->data[a].string, vm->stack->data[b].string);
    result = newString(vm, _newstr);
    free(_newstr);
    return result;
}

Int std_string_find(VirtualMachine *vm, IntList *args)
{
    Int str = StackShift(*args);
    Int substr = StackShift(*args);
    Int result = -1;
    if ((vm->typestack->data[str] == TYPE_STRING || vm->typestack->data[str] == TYPE_ERROR) && (vm->typestack->data[substr] == TYPE_STRING || vm->typestack->data[substr] == TYPE_ERROR))
    {
        char* _str = vm->stack->data[str].string;
        char* _substr = vm->stack->data[substr].string;
        char* _result = strstr(_str, _substr);
        if (_result != NULL)
        {
            result = newNumber(vm, _result - _str);
        }
    }
    return result;
}

Int std_string_ndup(VirtualMachine *vm, IntList *args)
{
    Int str = StackShift(*args);
    Int start = StackShift(*args);
    Int end = StackShift(*args);
    Int result = -1;
    if ((vm->typestack->data[str] == TYPE_STRING || vm->typestack->data[str] == TYPE_ERROR) && vm->typestack->data[start] == TYPE_NUMBER && vm->typestack->data[end] == TYPE_NUMBER)
    {
        char* _str = strndup(vm->stack->data[str].string + (Int)vm->stack->data[start].number, (Int)vm->stack->data[end].number - (Int)vm->stack->data[start].number);
        printf("%s\n", _str);
        result = newString(vm, _str);
        printf("STRING:%s\n", _str);
        free(_str);
    }
    return result;
}

Int std_string_split(VirtualMachine *vm, IntList *args)
{
    Int str = StackShift(*args);
    Int delim = StackShift(*args);
    Int result = -1;
    if ((vm->typestack->data[str] == TYPE_STRING || vm->typestack->data[str] == TYPE_ERROR) && (vm->typestack->data[delim] == TYPE_STRING || vm->typestack->data[delim] == TYPE_ERROR))
    {
        StringList *list = splitString(vm->stack->data[str].string, vm->stack->data[delim].string);
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
    return result;
}

Int std_string_replace(VirtualMachine *vm, IntList *args)
{
    Int str = StackShift(*args);
    Int substr = StackShift(*args);
    Int replacement = StackShift(*args);
    Int result = -1;
    if (vm->typestack->data[str] == TYPE_STRING && vm->typestack->data[substr] == TYPE_STRING && vm->typestack->data[replacement] == TYPE_STRING)
    {
        char* _str = vm->stack->data[str].string;
        char* _substr = vm->stack->data[substr].string;
        char* _replacement = vm->stack->data[replacement].string;
        char* _newstr = strreplace(_str, _substr, _replacement);
        result = newString(vm, _newstr);
        free(_newstr);
    }
    return result;
}

Int std_string_length(VirtualMachine *vm, IntList *args)
{
    Int str = StackShift(*args);
    Int result = -1;
    if (vm->typestack->data[str] == TYPE_STRING)
    {
        result = newNumber(vm, strlen(vm->stack->data[str].string));
    }
    return result;
}

// std conditions
// std conditions
// std conditions

Int std_condition_equals(VirtualMachine *vm, IntList *args)
{
    Int a = StackShift(*args);
    Int b = StackShift(*args);
    Int result = -1;
    if (vm->typestack->data[a] == vm->typestack->data[b])
    {
        if (vm->typestack->data[a] == TYPE_NUMBER)
        {
            result = newNumber(vm, vm->stack->data[a].number == vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_STRING)
        {
            result = newNumber(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) == 0);
        }
        else if (vm->typestack->data[a] == TYPE_POINTER)
        {
            result = newNumber(vm, vm->stack->data[a].number == vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_ERROR)
        {
            result = newNumber(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) == 0);
        }
    }
    if (vm->typestack->data[a] != TYPE_STRING && vm->typestack->data[a] != TYPE_ERROR && vm->typestack->data[a] != TYPE_LIST)
    {
        
    }
    if (vm->typestack->data[b] != TYPE_STRING && vm->typestack->data[b] != TYPE_ERROR && vm->typestack->data[b] != TYPE_LIST)
    {
        
    }
    return result;
}

Int std_condition_not_equals(VirtualMachine *vm, IntList *args)
{
    Int a = StackShift(*args);
    Int b = StackShift(*args);
    Int result = -1;
    if (vm->typestack->data[a] == vm->typestack->data[b])
    {
        if (vm->typestack->data[a] == TYPE_NUMBER)
        {
            result = newNumber(vm, vm->stack->data[a].number != vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_STRING)
        {
            result = newNumber(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) != 0);
        }
        else if (vm->typestack->data[a] == TYPE_POINTER)
        {
            result = newNumber(vm, vm->stack->data[a].number != vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_ERROR)
        {
            result = newNumber(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) != 0);
        }
    }
    
    if (vm->typestack->data[a] != TYPE_STRING && vm->typestack->data[a] != TYPE_ERROR && vm->typestack->data[a] != TYPE_LIST)
    {
        
    }
    if (vm->typestack->data[b] != TYPE_STRING && vm->typestack->data[b] != TYPE_ERROR && vm->typestack->data[b] != TYPE_LIST)
    {
        
    }
    return result;
}

Int std_condition_greater(VirtualMachine *vm, IntList *args)
{
    Int a = StackShift(*args);
    Int b = StackShift(*args);
    Int result = -1;
    if (vm->typestack->data[a] == vm->typestack->data[b])
    {
        if (vm->typestack->data[a] == TYPE_NUMBER)
        {
            result = newNumber(vm, vm->stack->data[a].number > vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_STRING)
        {
            result = newNumber(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) > 0);
        }
        else if (vm->typestack->data[a] == TYPE_POINTER)
        {
            result = newNumber(vm, vm->stack->data[a].number > vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_ERROR)
        {
            result = newNumber(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) > 0);
        }
    }
    if (vm->typestack->data[a] != TYPE_STRING && vm->typestack->data[a] != TYPE_ERROR && vm->typestack->data[a] != TYPE_LIST)
    {
        
    }
    if (vm->typestack->data[b] != TYPE_STRING && vm->typestack->data[b] != TYPE_ERROR && vm->typestack->data[b] != TYPE_LIST)
    {
        
    }
    return result;
}

Int std_condition_less(VirtualMachine *vm, IntList *args)
{
    Int a = StackShift(*args);
    Int b = StackShift(*args);
    Int result = -1;
    if (vm->typestack->data[a] == vm->typestack->data[b])
    {
        if (vm->typestack->data[a] == TYPE_NUMBER)
        {
            result = newNumber(vm, vm->stack->data[a].number < vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_STRING)
        {
            result = newNumber(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) < 0);
        }
        else if (vm->typestack->data[a] == TYPE_POINTER)
        {
            result = newNumber(vm, vm->stack->data[a].number < vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_ERROR)
        {
            result = newNumber(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) < 0);
        }
    }
    if (vm->typestack->data[a] != TYPE_STRING && vm->typestack->data[a] != TYPE_ERROR && vm->typestack->data[a] != TYPE_LIST)
    {
        
    }
    if (vm->typestack->data[b] != TYPE_STRING && vm->typestack->data[b] != TYPE_ERROR && vm->typestack->data[b] != TYPE_LIST)
    {
        
    }
    return result;
}

Int std_condition_greater_equals(VirtualMachine *vm, IntList *args)
{
    Int a = StackShift(*args);
    Int b = StackShift(*args);
    Int result = -1;
    if (vm->typestack->data[a] == vm->typestack->data[b])
    {
        if (vm->typestack->data[a] == TYPE_NUMBER)
        {
            result = newNumber(vm, vm->stack->data[a].number >= vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_STRING)
        {
            result = newNumber(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) >= 0);
        }
        else if (vm->typestack->data[a] == TYPE_POINTER)
        {
            result = newNumber(vm, vm->stack->data[a].number >= vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_ERROR)
        {
            result = newNumber(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) >= 0);
        }
    }
    if (vm->typestack->data[a] != TYPE_STRING && vm->typestack->data[a] != TYPE_ERROR && vm->typestack->data[a] != TYPE_LIST)
    {
        
    }
    if (vm->typestack->data[b] != TYPE_STRING && vm->typestack->data[b] != TYPE_ERROR && vm->typestack->data[b] != TYPE_LIST)
    {
        
    }
    return result;
}

Int std_condition_less_equals(VirtualMachine *vm, IntList *args)
{
    Int a = StackShift(*args);
    Int b = StackShift(*args);
    Int result = -1;
    if (vm->typestack->data[a] == vm->typestack->data[b])
    {
        if (vm->typestack->data[a] == TYPE_NUMBER)
        {
            result = newNumber(vm, vm->stack->data[a].number <= vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_STRING)
        {
            result = newNumber(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) <= 0);
        }
        else if (vm->typestack->data[a] == TYPE_POINTER)
        {
            result = newNumber(vm, vm->stack->data[a].number <= vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_ERROR)
        {
            result = newNumber(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) <= 0);
        }
    }
    if (vm->typestack->data[a] != TYPE_STRING && vm->typestack->data[a] != TYPE_ERROR && vm->typestack->data[a] != TYPE_LIST)
    {
        
    }
    if (vm->typestack->data[b] != TYPE_STRING && vm->typestack->data[b] != TYPE_ERROR && vm->typestack->data[b] != TYPE_LIST)
    {
        
    }
    return result;
}

Int std_condition_and(VirtualMachine *vm, IntList *args)
{
    Int a = StackShift(*args);
    Int b = StackShift(*args);
    Int result = -1;
    result = newNumber(vm, (isTrue(vm->stack->data[a], vm->typestack->data[a]) && isTrue(vm->stack->data[b], vm->typestack->data[b])));
    if (vm->typestack->data[a] != TYPE_STRING && vm->typestack->data[a] != TYPE_ERROR && vm->typestack->data[a] != TYPE_LIST)
    {
        
    }
    if (vm->typestack->data[b] != TYPE_STRING && vm->typestack->data[b] != TYPE_ERROR && vm->typestack->data[b] != TYPE_LIST)
    {
        
    }
    return result;
}

Int std_condition_or(VirtualMachine *vm, IntList *args)
{
    Int a = StackShift(*args);
    Int b = StackShift(*args);
    Int result = -1;
    if (isTrue(vm->stack->data[a], vm->typestack->data[a]))
    {
        Int index = newvar(vm);
        if (vm->typestack->data[a] == TYPE_LIST || vm->typestack->data[a] == TYPE_STRING)
        {
            vm->stack->data[index] = vm->stack->data[a];
            
        }
        else
        {
            vm->stack->data[index] = valueDuplicate(vm->stack->data[a], vm->typestack->data[a]);
            
            
        }
        vm->typestack->data[index] = vm->typestack->data[a];
        result = index;
    }
    else
    {
        Int index = newvar(vm);
        if (vm->typestack->data[b] == TYPE_LIST || vm->typestack->data[b] == TYPE_STRING)
        {
            vm->stack->data[index] = vm->stack->data[b];
            
        }
        else
        {
            vm->stack->data[index] = valueDuplicate(vm->stack->data[b], vm->typestack->data[b]);
            
            
        }
    }
    return result;
}

Int std_condition_not(VirtualMachine *vm, IntList *args)
{
    Int a = StackShift(*args);
    Int result = -1;
    if (vm->typestack->data[a] == TYPE_NUMBER)
    {
        result = newNumber(vm, !vm->stack->data[a].number);
    }
    else if (isTrue(vm->stack->data[a], vm->typestack->data[a]))
    {
        result = newNumber(vm, 0);
    }
    else
    {
        result = newNumber(vm, 1);
    }
    
    if (vm->typestack->data[a] != TYPE_LIST || vm->typestack->data[a] != TYPE_STRING)
    {
        
    }

    return result;
}

Int std_condition_if(VirtualMachine *vm, IntList *args)
{
    Int condition = StackShift(*args);
    Int _then = StackShift(*args);
    Int result = -1;
    if (isTrue(vm->stack->data[condition], vm->typestack->data[condition]))
    {
        result = eval(vm, vm->stack->data[_then].string);
    }
    return result;
}

Int std_condition_ifelse(VirtualMachine *vm, IntList *args)
{
    Int condition = StackShift(*args);
    Int _then = StackShift(*args);
    Int _else = StackShift(*args);
    Int result = -1;
    if (isTrue(vm->stack->data[condition], vm->typestack->data[condition]))
    {
        result = eval(vm, vm->stack->data[_then].string);
    }
    else
    {
        result = eval(vm, vm->stack->data[_else].string);
    }
    return result;
}


void initStd(VirtualMachine *vm)
{
    spawnFunction(vm, "temp", std_temp);
    spawnFunction(vm, "clear", std_clear);
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