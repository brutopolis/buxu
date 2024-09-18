#include "bruter.h"

Int std_clear(VirtualMachine *vm, IntList *args)
{
    while (vm->temp->size > 0)
    {
        unuse_var(vm, stack_shift(*vm->temp));
    }

    return -1;
}

Int std_hold(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    if (args->size > 0)
    {
        Int as = stack_shift(*args);
        Int _type = vm->typestack->data[index];
    }
    hold_var(vm, index);
    return -1;
}

Int std_unhold(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    unhold_var(vm, index);
    return -1;
}

Int std_rebase(VirtualMachine *vm, IntList *args)
{
    Int index = vm->stack->size - 1;
    while (vm->unused->size > 0)
    {
        index = stack_pop(*vm->unused);
        for (Int i = 0; i < vm->hashes->size; i++)
        {
            if (vm->hashes->data[i].index == index)
            {
                free(vm->hashes->data[i].key);
                stack_remove(*vm->hashes, i);
            }
        }
        for (Int i = 0; i < vm->unused->size; i++)
        {
            if (vm->unused->data[i] == index)
            {
                stack_remove(*vm->unused, i);
            }
        }
        for (Int i = 0; i < vm->temp->size; i++)
        {
            if (vm->temp->data[i] == index)
            {
                stack_remove(*vm->temp, i);
            }
        }
        for (Int i = 0; i < vm->stack->size; i++)
        {
            if (vm->typestack->data[i] == TYPE_LIST)
            {
                IntList *list = (IntList*)vm->stack->data[i].pointer;
                for (Int j = 0; j < list->size; j++)
                {
                    if (list->data[j] == index)
                    {
                        list->data[j] = -1;
                    }
                    else if (list->data[j] > index)
                    {
                        list->data[j]--;
                    }
                }
            }
        }
        for (Int i = 0; i < vm->temp->size; i++)
        {
            if (vm->temp->data[i] == index)
            {
                vm->temp->data[i] = -1;
            }
            else if (vm->temp->data[i] > index)
            {
                vm->temp->data[i]--;
            }
        }
        for (Int i = 0; i < vm->unused->size; i++)
        {
            if (vm->unused->data[i] == index)
            {
                vm->unused->data[i] = -1;
            }
            else if (vm->unused->data[i] > index)
            {
                vm->unused->data[i]--;
            }
        }
        for (Int i = 0; i < vm->hashes->size; i++)
        {
            if (vm->hashes->data[i].index == index)
            {
                vm->hashes->data[i].index = -1;
            }
            else if (vm->hashes->data[i].index > index)
            {
                vm->hashes->data[i].index--;
            }
        }

        stack_remove(*vm->stack, index);
        stack_remove(*vm->typestack, index);
    }

    return -1;
}

Int std_set(VirtualMachine *vm, IntList *args)
{
    Int varname = stack_shift(*args);
    Int value = stack_shift(*args);

    char * name = vm->stack->data[varname].string;
    Int index = hash_find(vm, name);

    if (index >= 0)
    {
        hash_unset(vm, name);
    }
    hash_set(vm, name, value);
    return -1;
}

Int std_edit(VirtualMachine *vm, IntList *args)
{
    Int variable = stack_shift(*args);
    Int value = stack_shift(*args);
    
    if (variable >= 0 && variable < vm->stack->size)
    {
        if(vm->typestack->data[variable] == TYPE_STRING ||
           vm->typestack->data[variable] == TYPE_LIST ||
           vm->typestack->data[variable] == TYPE_OTHER)
        {
            char found = 0;
            for (Int i = 0; i < vm->stack->size; i++)
            {
                if (i != variable)
                {
                    if (vm->stack->data[i].pointer == vm->stack->data[variable].pointer)
                    {
                        found = 1;
                        break;
                    }
                }
            }
            if (!found)
            {
                if (vm->typestack->data[variable] == TYPE_STRING)
                {
                    free(vm->stack->data[variable].string);
                }
                else if (vm->typestack->data[variable] == TYPE_LIST)
                {
                    stack_free(*((IntList*)vm->stack->data[variable].pointer));
                }
                else
                {
                    free(vm->stack->data[variable].pointer);
                }
            }
        }
        
        vm->stack->data[variable] = vm->stack->data[value];
        vm->typestack->data[variable] = vm->typestack->data[value];
    }
    
    return -1;
}

Int std_change(VirtualMachine *vm, IntList *args)
{
    Int varname = stack_shift(*args);
    Int type = stack_shift(*args);
    
    if (vm->typestack->data[varname] == TYPE_STRING)
    {
        char * name = vm->stack->data[varname].string;
        Int index = hash_find(vm, name);

        if (index >= 0)
        {
            vm->typestack->data[index] = vm->typestack->data[type];
        }
    }
    else if (vm->typestack->data[varname] == TYPE_NUMBER)
    {
        Int index = (Int)vm->stack->data[varname].number;
        if (index >= 0)
        {
            vm->typestack->data[index] = vm->typestack->data[type];
        }
    }
    
    return -1;
}

Int std_io_print(VirtualMachine *vm, IntList *args)
{
    while (args->size > 0)
    {
        Int var = stack_shift(*args);
        Int _type = -1;

        Value temp = vm->stack->data[var];
        _type = vm->typestack->data[var];
        
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
        else if (_type == TYPE_NIL)
        {
            printf("{nil}");
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

Int std_io_ls(VirtualMachine *vm, IntList *args)
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
            else if (vm->typestack->data[i] == TYPE_NIL)
            {
                printf("[%d] {nil}\n", i);
            }
            else
            {
                printf("[%d] {unknown}\n", i);
            }
        }
    }
    else
    {
        Int _var = stack_shift(*args);
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
                    else if (vm->typestack->data[index] == TYPE_NIL)
                    {
                        printf("[%s] {nil}\n", name);
                    }
                    else
                    {
                        printf("[%s] {unknown}\n", name);
                    }
                }
            }
            else if(strcmp("temp", vm->stack->data[_var].string) == 0)
            {
                for (Int i = 0; i < vm->temp->size; i++)
                {
                    if (vm->typestack->data[vm->temp->data[i]] == TYPE_FUNCTION)
                    {
                        printf("[%d] {function}: %p\n", vm->temp->data[i], vm->stack->data[vm->temp->data[i]].pointer);
                    }
                    else if (vm->typestack->data[vm->temp->data[i]] == TYPE_NUMBER)
                    {
                        if (round(vm->stack->data[vm->temp->data[i]].number) == vm->stack->data[vm->temp->data[i]].number)
                        {
                            printf("[%d] {number}: %d\n", vm->temp->data[i], (Int)vm->stack->data[vm->temp->data[i]].number);
                        }
                        else
                        {
                            printf("[%d] {number}: %f\n", vm->temp->data[i], vm->stack->data[vm->temp->data[i]].number);
                        }
                    }
                    else if (vm->typestack->data[vm->temp->data[i]] == TYPE_STRING)
                    {
                        printf("[%d] {string}: %s\n", vm->temp->data[i], vm->stack->data[vm->temp->data[i]].string);
                    }
                    else if (vm->typestack->data[vm->temp->data[i]] == TYPE_LIST)
                    {
                        printf("[%d] {list}: [", vm->temp->data[i]);
                        IntList *list = (IntList*)vm->stack->data[vm->temp->data[i]].pointer;
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
                    else if (vm->typestack->data[vm->temp->data[i]] == TYPE_NIL)
                    {
                        printf("[%d] {nil}\n", vm->temp->data[i]);
                    }
                    else
                    {
                        printf("[%d] {unknown}\n", vm->temp->data[i]);
                    }
                }
            }
            else if(strcmp("unused", vm->stack->data[_var].string) == 0 || 
                    strcmp("free", vm->stack->data[_var].string) == 0)
            {
                for (Int i = 0; i < vm->unused->size; i++)
                {
                    if (vm->typestack->data[vm->unused->data[i]] == TYPE_FUNCTION)
                    {
                        printf("[%d] {function}: %p\n", vm->unused->data[i], vm->stack->data[vm->unused->data[i]].pointer);
                    }
                    else if (vm->typestack->data[vm->unused->data[i]] == TYPE_NUMBER)
                    {
                        if (round(vm->stack->data[vm->unused->data[i]].number) == vm->stack->data[vm->unused->data[i]].number)
                        {
                            printf("[%d] {number}: %d\n", vm->unused->data[i], (Int)vm->stack->data[vm->unused->data[i]].number);
                        }
                        else
                        {
                            printf("[%d] {number}: %f\n", vm->unused->data[i], vm->stack->data[vm->unused->data[i]].number);
                        }
                    }
                    else if (vm->typestack->data[vm->unused->data[i]] == TYPE_STRING)
                    {
                        printf("[%d] {string}: %s\n", vm->unused->data[i], vm->stack->data[vm->unused->data[i]].string);
                    }
                    else if (vm->typestack->data[vm->unused->data[i]] == TYPE_LIST)
                    {
                        printf("[%d] {list}: [", vm->unused->data[i]);
                        IntList *list = (IntList*)vm->stack->data[vm->unused->data[i]].pointer;
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
                    else if (vm->typestack->data[vm->unused->data[i]] == TYPE_NIL)
                    {
                        printf("[%d] {nil}\n", vm->unused->data[i]);
                    }
                    else
                    {
                        printf("[%d] {unknown}\n", vm->unused->data[i]);
                    }
                }
            }

        }
        else if (vm->typestack->data[_var] == TYPE_LIST)
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
                else if (vm->typestack->data[list->data[i]] == TYPE_NIL)
                {
                    printf("[%d] (%d) {nil}\n", i, list->data[i]);
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
    Int str = stack_shift(*args);
    char* _str = vm->stack->data[str].string;
    Int result = eval(vm, _str);
    return result;
}

Int std_rm(VirtualMachine *vm, IntList *args)
{
    Int index;
    while (args->size > 0)
    {
        index = stack_shift(*args);
        if (vm->typestack->data[index] == TYPE_NUMBER)
        {
            unuse_var(vm, vm->stack->data[index].number);
        }
        else if (vm->typestack->data[index] == TYPE_STRING)
        {
            Int _index = hash_find(vm, vm->stack->data[index].string);
            if (_index != -1)
            {
                unuse_var(vm, _index);
            }
        }
        else
        {
            printf("invalid free!\n");
        }
    }
    return -1;
}

Int std_ignore(VirtualMachine *vm, IntList *args)
{
    while (args->size > 0)
    {
        stack_shift(*args);
    }
    return -1;
}

Int std_return(VirtualMachine *vm, IntList *args)
{
    return stack_shift(*args);
}

Int std_type(VirtualMachine *vm, IntList *args)
{
    Int var = stack_shift(*args);
    Int result = new_number(vm, vm->typestack->data[var]);
    return result;
}

Int std_get(VirtualMachine *vm, IntList *args)
{
    Int var = stack_shift(*args);
    Int result = -1;
    if (vm->typestack->data[var] == TYPE_STRING)
    {
        Int index = hash_find(vm, vm->stack->data[var].string);
        if (index == -1)
        {
            char *error = str_format("Variable %s not found", vm->stack->data[var].string);
            free(error);
        }
        else
        {
            result = index;
        }
    }
    else if (vm->typestack->data[var] == TYPE_NUMBER)
    {
        Int index = (Int)vm->stack->data[var].number;
        if (index >= 0 && index < vm->stack->size)
        {
            result = index;
        }
        else 
        {
            char *error = str_format("Variable %d not found", index);
            //error throw goes here
            
            free(error);
        }
    }
    return result;
}

Int std_size(VirtualMachine *vm, IntList *args)
{
    Int totalsize = vm->stack->size * (sizeof(Value) + 1);
    while (args->size > 0)
    {
        Int var = stack_shift(*args);
        if (vm->typestack->data[var] == TYPE_LIST)
        {
            IntList *list = (IntList*)vm->stack->data[var].pointer;
            totalsize += list->size * sizeof(Int);
            totalsize += sizeof(IntList);
        }
        else if (vm->typestack->data[var] == TYPE_STRING)
        {
            totalsize += strlen(vm->stack->data[var].string);
        }
    }
    Int result = new_number(vm, totalsize);
    return result;
}

// math functions
// math functions
// math functions
// math functions


Int std_math_add(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = new_number(vm, vm->stack->data[a].number + vm->stack->data[b].number);
    return result;
}

Int std_math_sub(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = new_number(vm, vm->stack->data[a].number - vm->stack->data[b].number);
    return result;
}

Int std_math_mul(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = new_number(vm, vm->stack->data[a].number * vm->stack->data[b].number);
    return result;
}

Int std_math_div(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = new_number(vm, vm->stack->data[a].number / vm->stack->data[b].number);
    return result;
}

Int std_math_mod(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = new_number(vm, (Int)vm->stack->data[a].number % (Int)vm->stack->data[b].number);
    return result;
}

Int std_math_pow(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = new_number(vm, pow(vm->stack->data[a].number, vm->stack->data[b].number));
    return result;
}


Int std_math_sqrt(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int result = new_number(vm, sqrt(vm->stack->data[a].number));
    return result;
}

Int std_math_abs(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int result = new_number(vm, fabs(vm->stack->data[a].number));
    return result;
}

Int std_math_random(VirtualMachine *vm, IntList *args)
{
    Int ___min = stack_shift(*args);
    Int ___max = stack_shift(*args);
    Int result = new_number(vm, rand() % (Int)vm->stack->data[___max].number + (Int)vm->stack->data[___min].number);
    return result;
}

Int std_math_seed(VirtualMachine *vm, IntList *args)
{
    Int seed = stack_shift(*args);
    srand((Int)vm->stack->data[seed].number);
    return -1;
}

Int std_math_floor(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int result = new_number(vm, floor(vm->stack->data[a].number));
    return result;;
}

Int std_math_ceil(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int result = new_number(vm, ceil(vm->stack->data[a].number));
    return result;
}

Int std_math_round(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int result = new_number(vm, round(vm->stack->data[a].number));
    return result;
}


// list functions
// list functions
// list functions
// list functions

Int std_list_new(VirtualMachine *vm, IntList *args) 
{
    Int index = new_list(vm);
    IntList *list = (IntList*)vm->stack->data[index].pointer;
    while (args->size > 0)
    {
        Int var = stack_shift(*args);
        Int tmp = new_var(vm);
        vm->stack->data[tmp] = value_duplicate(vm->stack->data[var], vm->typestack->data[var]);
        vm->typestack->data[tmp] = vm->typestack->data[var];
        stack_push(*list, tmp);
    }
    return index;
}

Int std_list_insert(VirtualMachine *vm, IntList *args)
{
    Int list = stack_shift(*args);
    Int index = stack_shift(*args);
    Int value = stack_shift(*args);

    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        stack_insert(*lst, (Int)vm->stack->data[index].number, (Int)vm->stack->data[value].number);
    }
    return -1;
}

Int std_list_push(VirtualMachine *vm, IntList *args)
{
    Int list = stack_shift(*args);
    Int value = stack_shift(*args);

    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        stack_push(*lst, (Int)vm->stack->data[value].number);
    }
    return -1;
}

Int std_list_unshift(VirtualMachine *vm, IntList *args) // returns the removed element
{
    Int list = stack_shift(*args);
    Int value = stack_shift(*args);
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        stack_unshift(*lst, (Int)vm->stack->data[value].number);
    }
    return -1;   
}

Int std_list_remove(VirtualMachine *vm, IntList *args)// returns the removed element
{
    Int list = stack_shift(*args);
    Int index = stack_shift(*args);
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        Int result = stack_remove(*lst, list);
        return result;
    }
    return -1;
}

Int std_list_pop(VirtualMachine *vm, IntList *args) // returns the removed element
{
    Int list = stack_shift(*args);
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        Int result = stack_pop(*lst);
        return result;
    }
    return -1;
}

Int std_list_shift(VirtualMachine *vm, IntList *args)
{
    Int list = stack_shift(*args);
    if (vm->typestack->data[list]== TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        Int result = stack_shift(*lst);
        return result;
    }
    return -1;
}

Int std_list_concat(VirtualMachine *vm, IntList *args)
{
    Int list1 = stack_shift(*args);
    Int list2 = stack_shift(*args);
    Int _newlist = new_list(vm);
    IntList *newlist = (IntList*)vm->stack->data[_newlist].pointer;
    if (vm->typestack->data[list1] == TYPE_LIST && vm->typestack->data[list2] == TYPE_LIST)
    {
        IntList *lst1 = (IntList*)vm->stack->data[list1].pointer;
        IntList *lst2 = (IntList*)vm->stack->data[list2].pointer;
        for (Int i = 0; i < lst1->size; i++)
        {
            stack_push(*newlist, lst1->data[i]);
        }
        for (Int i = 0; i < lst2->size; i++)
        {
            stack_push(*newlist, lst2->data[i]);
        }
    }
    return _newlist;
}

Int std_list_find(VirtualMachine *vm, IntList *args)
{
    Int list = stack_shift(*args);
    Int value = stack_shift(*args);
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        for (Int i = 0; i < lst->size; i++)
        {
            if (lst->data[i] == list)
            {
                return i;
            }
        }
    }
    return -1;
}

Int std_list_get(VirtualMachine *vm, IntList *args)
{
    Int list = stack_shift(*args);
    Int index = stack_shift(*args);
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        if (vm->typestack->data[index] == TYPE_NUMBER)
        {
            if (vm->stack->data[index].number >= 0 && vm->stack->data[index].number < lst->size)
            {
                return lst->data[(Int)vm->stack->data[index].number];
            }
        }
    }
    return -1;
}

// std string

Int std_string_concat(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = -1;

    char* _newstr = str_concat(vm->stack->data[a].string, vm->stack->data[b].string);
    result = new_string(vm, _newstr);
    free(_newstr);
    return result;
}

Int std_string_find(VirtualMachine *vm, IntList *args)
{
    Int str = stack_shift(*args);
    Int substr = stack_shift(*args);
    Int result = -1;
    if ((vm->typestack->data[str] == TYPE_STRING) && (vm->typestack->data[substr] == TYPE_STRING))
    {
        char* _str = vm->stack->data[str].string;
        char* _substr = vm->stack->data[substr].string;
        char* _result = strstr(_str, _substr);
        if (_result != NULL)
        {
            result = new_number(vm, _result - _str);
        }
    }
    return result;
}

Int std_string_ndup(VirtualMachine *vm, IntList *args)
{
    Int str = stack_shift(*args);
    Int start = stack_shift(*args);
    Int end = stack_shift(*args);
    Int result = -1;
    if ((vm->typestack->data[str] == TYPE_STRING) && vm->typestack->data[start] == TYPE_NUMBER && vm->typestack->data[end] == TYPE_NUMBER)
    {
        char* _str = strndup(vm->stack->data[str].string + (Int)vm->stack->data[start].number, (Int)vm->stack->data[end].number - (Int)vm->stack->data[start].number);
        printf("%s\n", _str);
        result = new_string(vm, _str);
        printf("STRING:%s\n", _str);
        free(_str);
    }
    return result;
}

Int std_string_split(VirtualMachine *vm, IntList *args)
{
    Int str = stack_shift(*args);
    Int delim = stack_shift(*args);
    Int result = -1;
    if ((vm->typestack->data[str] == TYPE_STRING) && (vm->typestack->data[delim] == TYPE_STRING))
    {
        StringList *list = splitString(vm->stack->data[str].string, vm->stack->data[delim].string);
        result = new_list(vm);
        IntList *newlist = (IntList*)vm->stack->data[result].pointer;
        while (list->size > 0)
        {
            char* _str = stack_shift(*list);
            printf("STRING:%s\n", _str);
            char* cmd = str_format("list.push (get %d) (get %d)", result, new_string(vm, _str));
            eval(vm, cmd);
            free(cmd);
            free(_str);
        }
        stack_free(*list);
    }
    return result;
}

Int std_string_replace(VirtualMachine *vm, IntList *args)
{
    Int str = stack_shift(*args);
    Int substr = stack_shift(*args);
    Int replacement = stack_shift(*args);
    Int result = -1;
    if (vm->typestack->data[str] == TYPE_STRING && vm->typestack->data[substr] == TYPE_STRING && vm->typestack->data[replacement] == TYPE_STRING)
    {
        char* _str = vm->stack->data[str].string;
        char* _substr = vm->stack->data[substr].string;
        char* _replacement = vm->stack->data[replacement].string;
        char* _newstr = str_replace(_str, _substr, _replacement);
        result = new_string(vm, _newstr);
        free(_newstr);
    }
    return result;
}

Int std_string_length(VirtualMachine *vm, IntList *args)
{
    Int str = stack_shift(*args);
    Int result = -1;
    if (vm->typestack->data[str] == TYPE_STRING)
    {
        result = new_number(vm, strlen(vm->stack->data[str].string));
    }
    return result;
}

// std conditions
// std conditions
// std conditions

Int std_condition_equals(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = -1;
    if (vm->typestack->data[a] == vm->typestack->data[b])
    {
        if (vm->typestack->data[a] == TYPE_NUMBER)
        {
            result = new_number(vm, vm->stack->data[a].number == vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_STRING)
        {
            result = new_number(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) == 0);
        }
    }
    return result;
}

Int std_condition_not_equals(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = -1;
    if (vm->typestack->data[a] == vm->typestack->data[b])
    {
        if (vm->typestack->data[a] == TYPE_NUMBER)
        {
            result = new_number(vm, vm->stack->data[a].number != vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_STRING)
        {
            result = new_number(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) != 0);
        }
    }
    
    return result;
}

Int std_condition_greater(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = -1;
    if (vm->typestack->data[a] == vm->typestack->data[b])
    {
        if (vm->typestack->data[a] == TYPE_NUMBER)
        {
            result = new_number(vm, vm->stack->data[a].number > vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_STRING)
        {
            result = new_number(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) > 0);
        }
    }
    return result;
}

Int std_condition_less(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = -1;
    if (vm->typestack->data[a] == vm->typestack->data[b])
    {
        if (vm->typestack->data[a] == TYPE_NUMBER)
        {
            result = new_number(vm, vm->stack->data[a].number < vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_STRING)
        {
            result = new_number(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) < 0);
        }
    }
    return result;
}

Int std_condition_greater_equals(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = -1;
    if (vm->typestack->data[a] == vm->typestack->data[b])
    {
        if (vm->typestack->data[a] == TYPE_NUMBER)
        {
            result = new_number(vm, vm->stack->data[a].number >= vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_STRING)
        {
            result = new_number(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) >= 0);
        }
    }
    return result;
}

Int std_condition_less_equals(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = -1;
    if (vm->typestack->data[a] == vm->typestack->data[b])
    {
        if (vm->typestack->data[a] == TYPE_NUMBER)
        {
            result = new_number(vm, vm->stack->data[a].number <= vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_STRING)
        {
            result = new_number(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) <= 0);
        }
    }
    return result;
}

Int std_condition_and(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    return new_number(vm, (is_true(vm->stack->data[a], vm->typestack->data[a]) && is_true(vm->stack->data[b], vm->typestack->data[b])));
}

Int std_condition_or(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = 0;
    if (is_true(vm->stack->data[a], vm->typestack->data[a]))
    {
        Int index = new_var(vm);
        if (vm->typestack->data[a] == TYPE_LIST || vm->typestack->data[a] == TYPE_STRING)
        {
            vm->stack->data[index] = vm->stack->data[a];
            
        }
        else
        {
            vm->stack->data[index] = value_duplicate(vm->stack->data[a], vm->typestack->data[a]);
            
            
        }
        vm->typestack->data[index] = vm->typestack->data[a];
        result = index;
    }
    else
    {
        Int index = new_var(vm);
        if (vm->typestack->data[b] == TYPE_LIST || vm->typestack->data[b] == TYPE_STRING)
        {
            vm->stack->data[index] = vm->stack->data[b];
        }
        else
        {
            vm->stack->data[index] = value_duplicate(vm->stack->data[b], vm->typestack->data[b]);
        }
    }
    return result;
}

Int std_condition_not(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int result = 0;
    if (vm->typestack->data[a] == TYPE_NUMBER)
    {
        result = new_number(vm, !vm->stack->data[a].number);
    }
    else if (is_true(vm->stack->data[a], vm->typestack->data[a]))
    {
        result = new_number(vm, 0);
    }
    else
    {
        result = new_number(vm, 1);
    }

    return result;
}

Int std_condition_if(VirtualMachine *vm, IntList *args)
{
    Int condition = stack_shift(*args);
    Int _then = stack_shift(*args);
    Int result = -1;
    if (is_true(vm->stack->data[condition], vm->typestack->data[condition]))
    {
        result = eval(vm, vm->stack->data[_then].string);
    }
    return result;
}

Int std_condition_ifelse(VirtualMachine *vm, IntList *args)
{
    Int condition = stack_shift(*args);
    Int _then = stack_shift(*args);
    Int _else = stack_shift(*args);
    Int result = -1;
    if (is_true(vm->stack->data[condition], vm->typestack->data[condition]))
    {
        result = eval(vm, vm->stack->data[_then].string);
    }
    else
    {
        result = eval(vm, vm->stack->data[_else].string);
    }
    return result;
}

// std loop
// std loop
// std loop

Int std_while(VirtualMachine *vm, IntList *args)
{
    Int condition_str = stack_shift(*args);
    Int _do_str = stack_shift(*args);
    Int condition_result = eval(vm, vm->stack->data[condition_str].string);
    Int result = -1;
    while (is_true(vm->stack->data[condition_result], vm->typestack->data[condition_result]))
    {
        result = eval(vm, vm->stack->data[_do_str].string);
        condition_result = eval(vm, vm->stack->data[condition_str].string);
    }
    return result;
}

void init_std(VirtualMachine *vm)
{
    //spawn_function(vm, "temp", std_temp);
    hold_var(vm,spawn_function(vm, "clear", std_clear));
    hold_var(vm,spawn_function(vm, "hold", std_hold));
    hold_var(vm,spawn_function(vm, "unhold", std_unhold));
    hold_var(vm,spawn_function(vm, "rebase", std_rebase));
    hold_var(vm,spawn_function(vm, "set", std_set));
    hold_var(vm,spawn_function(vm, "get", std_get));
    hold_var(vm,spawn_function(vm, "eval", std_eval));
    hold_var(vm,spawn_function(vm, "rm", std_rm));
    hold_var(vm,spawn_function(vm, "return", std_return));
    hold_var(vm,spawn_function(vm, "type", std_type));
    hold_var(vm,spawn_function(vm, "size", std_size));
    hold_var(vm,spawn_function(vm, "edit", std_edit));
    hold_var(vm,spawn_function(vm, "change", std_change));
    hold_var(vm,spawn_function(vm, "#", std_ignore));
    hold_var(vm,spawn_function(vm, "while", std_while));
}

void init_io(VirtualMachine *vm)
{
    hold_var(vm,spawn_function(vm, "print", std_io_print));
    hold_var(vm,spawn_function(vm, "ls", std_io_ls));
}

void init_math(VirtualMachine *vm)
{
    hold_var(vm,spawn_function(vm, "add", std_math_add));
    hold_var(vm,spawn_function(vm, "sub", std_math_sub));
    hold_var(vm,spawn_function(vm, "mul", std_math_mul));
    hold_var(vm,spawn_function(vm, "div", std_math_div));
    hold_var(vm,spawn_function(vm, "mod", std_math_mod));
    hold_var(vm,spawn_function(vm, "pow", std_math_pow));
    hold_var(vm,spawn_function(vm, "abs", std_math_abs));
    hold_var(vm,spawn_function(vm, "sqrt", std_math_sqrt));
    hold_var(vm,spawn_function(vm, "ceil", std_math_ceil));
    hold_var(vm,spawn_function(vm, "seed", std_math_seed));
    hold_var(vm,spawn_function(vm, "floor", std_math_floor));
    hold_var(vm,spawn_function(vm, "round", std_math_round));
    hold_var(vm,spawn_function(vm, "random", std_math_random));
}

void init_list(VirtualMachine *vm)
{
    hold_var(vm,spawn_function(vm, "list.new", std_list_new));
    hold_var(vm,spawn_function(vm, "list.insert", std_list_insert));
    hold_var(vm,spawn_function(vm, "list.push", std_list_push));
    hold_var(vm,spawn_function(vm, "list.unshift", std_list_unshift));
    hold_var(vm,spawn_function(vm, "list.remove", std_list_remove));
    hold_var(vm,spawn_function(vm, "list.pop", std_list_pop));
    hold_var(vm,spawn_function(vm, "list.shift", std_list_shift));
    hold_var(vm,spawn_function(vm, "list.concat", std_list_concat));
    hold_var(vm,spawn_function(vm, "list.find", std_list_find));
    hold_var(vm,spawn_function(vm, "list.get", std_list_get));
}

void init_string(VirtualMachine *vm)
{
    //spawn_function(vm, "string.format", std_string_new);
    hold_var(vm,spawn_function(vm, "string.concat", std_string_concat));
    hold_var(vm,spawn_function(vm, "string.find", std_string_find));
    hold_var(vm,spawn_function(vm, "string.sub", std_string_ndup));
    hold_var(vm,spawn_function(vm, "string.split", std_string_split));
    hold_var(vm,spawn_function(vm, "string.replace", std_string_replace));
    hold_var(vm,spawn_function(vm, "string.len", std_string_length));
}

void init_condition(VirtualMachine *vm)
{
    hold_var(vm,spawn_function(vm, "==", std_condition_equals));
    hold_var(vm,spawn_function(vm, "!=", std_condition_not_equals));
    hold_var(vm,spawn_function(vm, ">", std_condition_greater));
    hold_var(vm,spawn_function(vm, "<", std_condition_less));
    hold_var(vm,spawn_function(vm, ">=", std_condition_greater_equals));
    hold_var(vm,spawn_function(vm, "<=", std_condition_less_equals));
    hold_var(vm,spawn_function(vm, "and", std_condition_and));
    hold_var(vm,spawn_function(vm, "or", std_condition_or));
    hold_var(vm,spawn_function(vm, "not", std_condition_not));
    hold_var(vm,spawn_function(vm, "if", std_condition_if));
    hold_var(vm,spawn_function(vm, "ifelse", std_condition_ifelse));
}

void init_default_vars(VirtualMachine *vm)
{
    hold_var(vm,spawn_number(vm, "PI", 3.14159265358979323846));// PI number
    hold_var(vm,spawn_number(vm, "NIL", TYPE_NIL));// nil type
    hold_var(vm,spawn_number(vm, "OTHER", TYPE_OTHER));// unused type
    hold_var(vm,spawn_number(vm, "NUMBER", TYPE_NUMBER));// number type
    hold_var(vm,spawn_number(vm, "STRING", TYPE_STRING));// string type
    hold_var(vm,spawn_number(vm, "FUNCTION", TYPE_FUNCTION));// function type
    hold_var(vm,spawn_number(vm, "LIST", TYPE_LIST));// list type
    hold_var(vm,spawn_string(vm, "VERSION", VERSION));// version
}

void init_all(VirtualMachine *vm)
{
    init_std(vm);
    init_io(vm);
    init_math(vm);
    init_list(vm);
    init_string(vm);
    init_condition(vm);
    init_default_vars(vm);
}