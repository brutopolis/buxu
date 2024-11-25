#include "bruter.h"

#ifndef ARDUINO

#include <poll.h>
#include <errno.h>
#include <fcntl.h>


#include <sys/wait.h>

Int brl_os_file_read(VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    char *code = readfile(vm->stack->data[filename].string);
    Int index = -1;
    if (code != NULL)
    {
        index = new_var(vm);
        vm->stack->data[index].string = code;
        vm->typestack->data[index] = TYPE_STRING;
    }
    return index;
}

Int brl_os_file_write(VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    Int code = stack_shift(*args);
    writefile(vm->stack->data[filename].string, vm->stack->data[code].string);
    return -1;
}

Int brl_os_file_delete(VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    remove(vm->stack->data[filename].string);
    return -1;
}

Int brl_os_repl(VirtualMachine *vm, IntList *args)
{
    return repl(vm);
}

Int brl_os_dofile(VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    char *code = readfile(vm->stack->data[filename].string);
    Int result = -1;
    if (code != NULL)
    {
        result = eval(vm, code);
        free(code);
    }
    else 
    {
        printf("Error: file %s not found\n", vm->stack->data[filename].string);
    }
    return result;
}

Int brl_os_file_exists(VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    FILE *file = fopen(vm->stack->data[filename].string, "r");
    Int result = new_number(vm, file != NULL);
    if (file != NULL)
    {
        fclose(file);
    }
    return result;
}

Int brl_os_file_rename(VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    Int newname = stack_shift(*args);
    rename(vm->stack->data[filename].string, vm->stack->data[newname].string);
    return -1;
}

Int brl_os_file_copy(VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    Int newname = stack_shift(*args);
    FILE *file = fopen(vm->stack->data[filename].string, "r");
    if (file == NULL)
    {
        return -1;
    }
    fclose(file);
    char *code = readfile(vm->stack->data[filename].string);
    writefile(vm->stack->data[newname].string, code);
    free(code);
    return -1;
}

Int brl_os_file_size (VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    FILE *file = fopen(vm->stack->data[filename].string, "r");
    if (file == NULL)
    {
        return -1;
    }
    fseek(file, 0, SEEK_END);
    Int result = new_number(vm, ftell(file));
    fclose(file);
    return result;
}

Int brl_os_time_now(VirtualMachine *vm, IntList *args)
{
    return new_number(vm, time(NULL));
}

Int brl_os_time_clock(VirtualMachine *vm, IntList *args)
{
    return new_number(vm, clock());
}

#endif

Int brl_std_hash_set(VirtualMachine *vm, IntList *args)
{
    Int varname = stack_shift(*args);
    Int value = stack_shift(*args);

    char * name = vm->stack->data[varname].string;
    hash_set(vm, name, value);
    hold_var(vm, value);
    return -1;
}

Int brl_std_hash_get(VirtualMachine *vm, IntList *args)
{
    Int varname = stack_shift(*args);
    Int index = hash_find(vm, vm->stack->data[varname].string);
    if (index >= 0)
    {
        return vm->hashes->data[index].index;
    }
    return -1;
}

Int brl_std_hash_delete(VirtualMachine *vm, IntList *args)
{
    Int varname = stack_shift(*args);
    hash_unset(vm, vm->stack->data[varname].string);
    return -1;
}

Int brl_std_hash_rename(VirtualMachine *vm, IntList *args)
{
    Int varname = stack_shift(*args);
    Int newname = stack_shift(*args);
    Int index = hash_find(vm, vm->stack->data[varname].string);
    if (index >= 0)
    {
        free(vm->hashes->data[index].key);
        vm->hashes->data[index].key = strdup(vm->stack->data[newname].string);
    }
    return -1;
}

Int brl_std_type_set(VirtualMachine *vm, IntList *args)
{
    Int var = stack_shift(*args);
    Int type = stack_shift(*args);
    
    vm->typestack->data[var] = (Int)vm->stack->data[type].number;
    
    return -1;
}

Int brl_std_io_print(VirtualMachine *vm, IntList *args)
{
    while (args->size > 0)
    {
        Int var = stack_shift(*args);
        if (var >= 0 || var < vm->stack->size)
        {
            print_element(vm, var);
        }
        else 
        {
            printf("{out of stack}");
        }

        if (args->size > 0)
        {
            printf(" ");
        }
    }
    printf("\n");
    return -1;
}

Int brl_std_io_ls(VirtualMachine *vm, IntList *args)
{
    Int _var = -1;
    if (args->size > 0)
    {
        _var = stack_shift(*args);
    }

    if (_var > -1 && vm->typestack->data[_var] == TYPE_LIST)
    {
        IntList *list = (IntList*)vm->stack->data[(Int)vm->stack->data[_var].number].pointer;
        for (Int i = 0; i < list->size; i++)
        {
            printf("%d[%d]: ", (Int)vm->stack->data[_var].number, i);
            print_element(vm, list->data[i]);
            printf("\n");
        }
    }
    else 
    {
        for (Int i = 0; i < vm->stack->size; i++)
        {
            printf("[%ld]: ", i);
            print_element(vm, i);
            printf("\n");
        }
    }

    return -1;
}

Int brl_std_io_ls_types(VirtualMachine *vm, IntList *args)
{
    for (Int i = 0; i < vm->stack->size; i++)
    {
        printf("[%ld]: %d\n", i, vm->typestack->data[i]);
    }
    return -1;
}

Int brl_std_io_ls_hashes(VirtualMachine *vm, IntList *args)
{
    for (Int i = 0; i < vm->hashes->size; i++)
    {
        printf("[%s] {%d} @%ld: ", vm->hashes->data[i].key, vm->typestack->data[vm->hashes->data[i].index], vm->hashes->data[i].index);
        print_element(vm, vm->hashes->data[i].index);
        printf("\n");
    }
    return -1;
}

Int brl_std_io_ls_temp(VirtualMachine *vm, IntList *args)
{
    for (Int i = 0; i < vm->temp->size; i++)
    {
        printf("[%ld] {temp}: ", vm->temp->data[i]);
        print_element(vm, vm->temp->data[i]);
        printf("\n");
    }
    return -1;
}

Int brl_std_io_ls_unused(VirtualMachine *vm, IntList *args)
{
    for (Int i = 0; i < vm->unused->size; i++)
    {
        printf("[%ld] {unused}: ", vm->unused->data[i]);
        print_element(vm, vm->unused->data[i]);
        printf("\n");
    }
    return -1;
}

Int brl_std_loop_each(VirtualMachine *vm, IntList *args)
{
    Int _list = stack_shift(*args);
    Int _varname = stack_shift(*args);
    Int _cmd = stack_shift(*args);
    IntList *list = (IntList*)vm->stack->data[_list].pointer;
    for (Int i = 0; i < list->size; i++)
    {
        hash_set(vm, vm->stack->data[_varname].string, list->data[i]);
        eval(vm, vm->stack->data[_cmd].string);
    }
    return -1;
}

Int brl_std_do(VirtualMachine *vm, IntList *args)
{
    Int str = stack_shift(*args);
    char* _str = vm->stack->data[str].string;
    if (args->size > 0)
    {
        Int _args = new_list(vm);
        while (args->size > 0)
        {
            stack_push(*((IntList*)vm->stack->data[_args].pointer), stack_shift(*args));
        }
        hash_set(vm, "args", _args);
    }

    Int result = eval(vm, _str);
    return result;
}


Int brl_std_ignore(VirtualMachine *vm, IntList *args)
{
    while (args->size > 0)
    {
        stack_shift(*args);
    }
    return -1;
}

Int brl_std_return(VirtualMachine *vm, IntList *args)
{
    return stack_shift(*args);
}

Int brl_std_gindex(VirtualMachine *vm, IntList *args)
{
    Int var = stack_shift(*args);
    return new_number(vm, var);
}

Int brl_std_type_get(VirtualMachine *vm, IntList *args)
{
    Int var = stack_shift(*args);
    Int result = new_number(vm, vm->typestack->data[var]);
    return result;
}


// math functions
// math functions
// math functions
// math functions


Int brl_std_math_add(VirtualMachine *vm, IntList *args)
{
    Int result = stack_shift(*args);
    while (args->size > 0)
    {
        vm->stack->data[result].number += vm->stack->data[stack_shift(*args)].number;
    }
    return result;
}

Int brl_std_math_sub(VirtualMachine *vm, IntList *args)
{
    Int result = stack_shift(*args);
    while (args->size > 0)
    {
        vm->stack->data[result].number -= vm->stack->data[stack_shift(*args)].number;
    }
    return result;
}

Int brl_std_math_mul(VirtualMachine *vm, IntList *args)
{
    Int result = stack_shift(*args);
    while (args->size > 0)
    {
        vm->stack->data[result].number *= vm->stack->data[stack_shift(*args)].number;
    }
    return result;
}

Int brl_std_math_div(VirtualMachine *vm, IntList *args)
{
    Int result = stack_shift(*args);
    while (args->size > 0)
    {
        vm->stack->data[result].number /= vm->stack->data[stack_shift(*args)].number;
    }
    return result;
}

Int brl_std_math_mod(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    vm->stack->data[a].number = fmod(vm->stack->data[a].number, vm->stack->data[b].number);
    return a;
}

Int brl_std_math_pow(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    vm->stack->data[a].number = pow(vm->stack->data[a].number, vm->stack->data[b].number);
    return a;
}

Int brl_std_math_sqrt(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    vm->stack->data[a].number = sqrt(vm->stack->data[a].number);
    return a;
}

Int brl_std_math_abs(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    vm->stack->data[a].number = fabs(vm->stack->data[a].number);
    return a;
}

Int brl_std_math_random(VirtualMachine *vm, IntList *args)
{
    Int ___min = stack_shift(*args);
    Int ___max = stack_shift(*args);
    ___min = rand() % (Int)vm->stack->data[___max].number + (Int)vm->stack->data[___min].number;
    return ___min;
}

Int brl_std_math_seed(VirtualMachine *vm, IntList *args)
{
    Int seed = stack_shift(*args);
    srand((Int)vm->stack->data[seed].number);
    return -1;
}

Int brl_std_math_floor(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    vm->stack->data[a].number = floor(vm->stack->data[a].number);
    while (args->size > 0)
    {
        Int b = stack_shift(*args);
        vm->stack->data[b].number = floor(vm->stack->data[b].number);
    }
    return a;
}

Int brl_std_math_ceil(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    vm->stack->data[a].number = ceil(vm->stack->data[a].number);
    while (args->size > 0)
    {
        Int b = stack_shift(*args);
        vm->stack->data[b].number = ceil(vm->stack->data[b].number);
    }
    return a;
}

Int brl_std_math_round(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    vm->stack->data[a].number = round(vm->stack->data[a].number);
    while (args->size > 0)
    {
        Int b = stack_shift(*args);
        vm->stack->data[b].number = round(vm->stack->data[b].number);
    }
    return a;
}

Int brl_std_math_increment(VirtualMachine *vm, IntList *args)
{
    while (args->size > 0)
    {
        Int a = stack_shift(*args);
        vm->stack->data[a].number++;
    }
    return -1;
}

Int brl_std_math_decrement(VirtualMachine *vm, IntList *args)
{
    while (args->size > 0)
    {
        Int a = stack_shift(*args);
        vm->stack->data[a].number--;
    }
    return -1;
}


Int int_from_float(VirtualMachine *vm, IntList *args)
{
    Int result = new_var(vm);
    vm->typestack->data[result] = TYPE_RAW;
    vm->stack->data[result].integer = (Int)vm->stack->data[stack_shift(*args)].number;
    return result;
}

Int int_to_float(VirtualMachine *vm, IntList *args)
{
    Int result = new_var(vm);
    vm->typestack->data[result] = TYPE_NUMBER;
    vm->stack->data[result].number = (Float)vm->stack->data[stack_shift(*args)].integer;
    return result;
}

// list functions
// list functions
// list functions
// list functions

Int brl_std_list_new(VirtualMachine *vm, IntList *args) 
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
Int brl_std_list_insert(VirtualMachine *vm, IntList *args)
{
    Int list = stack_shift(*args);
    Int index = stack_shift(*args);
    Int value;
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        while (args->size > 0)
        {
            value = stack_shift(*args);
            stack_insert(*lst, (Int)vm->stack->data[index].number, value);
        }
    }
    return -1;
}
Int brl_std_list_push(VirtualMachine *vm, IntList *args)
{
    Int list = stack_shift(*args);
    Int value;
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        while (args->size > 0)
        {
            value = stack_shift(*args);
            stack_push(*lst, value);
        }
    }
    return -1;
}
Int brl_std_list_unshift(VirtualMachine *vm, IntList *args) 
{
    Int list = stack_shift(*args);
    Int value;
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        while (args->size > 0)
        {
            value = stack_shift(*args);
            stack_unshift(*lst, value);
        }
    }
    return -1;   
}
Int brl_std_list_remove(VirtualMachine *vm, IntList *args)// returns the removed element
{
    Int list = stack_shift(*args);
    Int index = stack_shift(*args);
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        Int result = stack_remove(*lst, (Int)vm->stack->data[index].number);
        return result;
    }
    return -1;
}

Int brl_std_list_pop(VirtualMachine *vm, IntList *args)// returns the removed element
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

Int brl_std_list_shift(VirtualMachine *vm, IntList *args)// returns the removed element
{
    Int list = stack_shift(*args);
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        Int result = stack_shift(*lst);
        return result;
    }
    return -1;
}

Int brl_std_list_concat(VirtualMachine *vm, IntList *args)
{
    Int _newlist = new_list(vm);
    IntList *newlist = (IntList*)vm->stack->data[_newlist].pointer;
    while (args->size > 0)
    {
        Int list = stack_shift(*args);
        if (vm->typestack->data[list] == TYPE_LIST)
        {
            IntList *lst = (IntList*)vm->stack->data[list].pointer;
            for (Int i = 0; i < lst->size; i++)
            {
                stack_push(*newlist, lst->data[i]);
            }
        }
    }
    return _newlist;
}
Int brl_std_list_find(VirtualMachine *vm, IntList *args)
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
Int brl_std_list_get(VirtualMachine *vm, IntList *args)
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
Int brl_std_list_length(VirtualMachine *vm, IntList *args)
{
    Int list = stack_shift(*args);
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        return new_number(vm, lst->size);
    }
    return -1;
}

/*Int brl_std_list_first(VirtualMachine *vm, IntList *args)
{
    Int list = stack_shift(*args);
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        return lst->data[0];
    }
    return -1;
}*/

Int brl_std_list_last(VirtualMachine *vm, IntList *args)
{
    Int list = stack_shift(*args);
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        return lst->data[lst->size-1];
    }
    return -1;
}


// std string

Int brl_std_string_concat(VirtualMachine *vm, IntList *args)
{
    Int _newstr = new_string(vm, "");
    char* _newstr_str = vm->stack->data[_newstr].string;
    while (args->size > 0)
    {
        Int str = stack_shift(*args);
        char* _str = vm->stack->data[str].string;
        char* _tmp = str_concat(_newstr_str, _str);
        free(_newstr_str);
        _newstr_str = _tmp;
    }
    vm->stack->data[_newstr].string = _newstr_str;
    return _newstr;
}

Int brl_std_string_find(VirtualMachine *vm, IntList *args)
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

Int brl_std_string_ndup(VirtualMachine *vm, IntList *args)
{
    Int str = stack_shift(*args);
    Int start = stack_shift(*args);
    Int end = stack_shift(*args);
    Int result = -1;
    if ((vm->typestack->data[str] == TYPE_STRING) && vm->typestack->data[start] == TYPE_NUMBER && vm->typestack->data[end] == TYPE_NUMBER)
    {
        char* _str = strndup(vm->stack->data[str].string + (Int)vm->stack->data[start].number, (Int)vm->stack->data[end].number - (Int)vm->stack->data[start].number);
        result = new_string(vm, _str);
        free(_str);
    }
    return result;
}

Int brl_std_string_split(VirtualMachine *vm, IntList *args)
{
    Int str = stack_shift(*args);
    Int delim = stack_shift(*args);
    if ((vm->typestack->data[str] == TYPE_STRING) && (vm->typestack->data[delim] == TYPE_STRING))
    {
        StringList *list = split_string(vm->stack->data[str].string, vm->stack->data[delim].string);
        char * _tmp = str_format("list.new", list->size);
        Int _arr = eval(vm, _tmp);
        free(_tmp);
        while (list->size > 0)
        {
            Int _str = new_var(vm);
            vm->stack->data[_str].string = stack_shift(*list);
            vm->typestack->data[_str] = TYPE_STRING;
        }
        stack_free(*list);
        return _arr;
    }
    return -1;
}

Int brl_std_string_replace(VirtualMachine *vm, IntList *args)
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

Int brl_std_string_replace_all(VirtualMachine *vm, IntList *args)
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
        char* _newstr = str_replace_all(_str, _substr, _replacement);
        result = new_string(vm, _newstr);
        free(_newstr);
    }
    return result;
}

Int brl_std_string_to_number(VirtualMachine *vm, IntList *args)
{
    Int str = stack_shift(*args);
    Int result = -1;
    if (vm->typestack->data[str] == TYPE_STRING)
    {
        result = new_number(vm, atof(vm->stack->data[str].string));
    }
    return result;
}

Int brl_std_string_length(VirtualMachine *vm, IntList *args)
{
    Int str = stack_shift(*args);
    Int result = -1;
    if (vm->typestack->data[str] == TYPE_STRING)
    {
        result = new_number(vm, strlen(vm->stack->data[str].string));
    }
    return result;
}

Int brl_std_string_format(VirtualMachine *vm, IntList *args)
{
    Int str = stack_shift(*args);
    Int result = -1;
    char* _str = str_duplicate(vm->stack->data[str].string);
    for (Int i = 0; i < strlen(_str); i++)
    {
        if (_str[i] == '%')
        {
            if (_str[i+1] == 'd')
            {
                Int value = stack_shift(*args);
                char* _value = str_format("%ld", (Int)vm->stack->data[value].number);
                char* _newstr = str_replace(_str, "\%d", _value);
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] == 's')
            {
                Int value = stack_shift(*args);
                char* _value = vm->stack->data[value].string;
                char* _newstr = str_replace(_str, "\%s", _value);
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] == 'f')
            {
                Int value = stack_shift(*args);
                char* _value = str_format("%f", vm->stack->data[value].number);
                char* _newstr = str_replace(_str, "\%f", _value);
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] == 'p')
            {
                Int value = stack_shift(*args);
                char* _value = str_format("%p", vm->stack->data[value].pointer);
                char* _newstr = str_replace(_str, "\%p", _value);
                free(_str);
                _str = _newstr;
            }
        }
        else if (_str[i] == '\\')
        {
            if (_str[i+1] == 'n')
            {
                char* _newstr = str_replace(_str, "\\n", "\n");
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] == 't')
            {
                char* _newstr = str_replace(_str, "\\t", "\t");
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] == 'r')
            {
                char* _newstr = str_replace(_str, "\\r", "\r");
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] >= '0' && _str[i+1] <= '9')
            {
                char* _newstr = str_format("%s%c", _str + i + 1, (char)atoi(_str + i + 1));
                free(_str);
                _str = _newstr;
            }
        }
    }
    result = new_var(vm);
    vm->stack->data[result].string = _str;
    vm->typestack->data[result] = TYPE_STRING;
    return result;
}

// std conditions
// std conditions
// std conditions
Int brl_std_condition_equals(VirtualMachine *vm, IntList *args)
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

Int brl_std_condition_not_equals(VirtualMachine *vm, IntList *args)
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

Int brl_std_condition_greater(VirtualMachine *vm, IntList *args)
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

Int brl_std_condition_less(VirtualMachine *vm, IntList *args)
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

Int brl_std_condition_greater_equals(VirtualMachine *vm, IntList *args)
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

Int brl_std_condition_less_equals(VirtualMachine *vm, IntList *args)
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

Int brl_std_condition_and(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    return new_number(vm, (is_true(vm->stack->data[a], vm->typestack->data[a]) && is_true(vm->stack->data[b], vm->typestack->data[b])));
}

Int brl_std_condition_or(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = 0;
    if (is_true(vm->stack->data[a], vm->typestack->data[a]))
    {
        Int index = new_var(vm);
        if (vm->typestack->data[b] == TYPE_LIST || vm->typestack->data[a] == TYPE_STRING)
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

Int brl_std_condition_not(VirtualMachine *vm, IntList *args)
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

Int brl_std_condition_if(VirtualMachine *vm, IntList *args)
{
    Int condition = stack_shift(*args);
    Int _then = stack_shift(*args);
    Int result = eval(vm, vm->stack->data[condition].string);
    if (is_true(vm->stack->data[result], vm->typestack->data[result]))
    {
        result = eval(vm, vm->stack->data[_then].string);
    }
    return result;
}

Int brl_std_condition_ifelse(VirtualMachine *vm, IntList *args)
{
    Int condition = stack_shift(*args);
    Int _then = stack_shift(*args);
    Int _else = stack_shift(*args);
    Int result = eval(vm, vm->stack->data[condition].string);
    if (is_true(vm->stack->data[result], vm->typestack->data[result]))
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

Int brl_std_loop_while(VirtualMachine *vm, IntList *args)
{
    Int condition_str = stack_shift(*args);
    Int _do_str = stack_shift(*args);
    Int condition = eval(vm, vm->stack->data[condition_str].string);
    while (is_true(vm->stack->data[condition], vm->typestack->data[condition]))
    {
        eval(vm, vm->stack->data[_do_str].string);
        condition = eval(vm, vm->stack->data[condition_str].string);
    }
    return -1;
}

Int brl_std_loop_repeat(VirtualMachine *vm, IntList *args)
{
    Int times = stack_shift(*args);
    Int _do_str = stack_shift(*args);
    for (Int i = 0; i < vm->stack->data[times].number; i++)
    {
        eval(vm, vm->stack->data[_do_str].string);
    }
    return -1;
}

// memory functions


Int brl_mem_clear(VirtualMachine *vm, IntList *args)
{
    while (args->size > 0)
    {
        Int index = stack_shift(*args);
        if (stack_find(*vm->temp, index) >= 0)
        {
            unuse_var(vm, index);
        }
    }

    return -1;
}

Int brl_mem_hold(VirtualMachine *vm, IntList *args)
{
    Int index;
    while (args->size > 0)
    {
        index = stack_shift(*args);
        if (index >= 0 && index < vm->stack->size)
        {
            hold_var(vm, index);
        } 
    }
    
    return -1;
}

Int brl_mem_keep(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    hold_var(vm, index);
    return index;
}

Int brl_mem_unhold(VirtualMachine *vm, IntList *args)
{
    Int index;
    while (args->size > 0)
    {
        index = stack_shift(*args);
        if (index >= 0 && index < vm->stack->size)
        {
            unhold_var(vm, index);
        }
    }
    return -1;
}

Int brl_mem_rebase(VirtualMachine *vm, IntList *args)
{
    Int index = vm->stack->size - 1;
    while (vm->unused->size > 0)
    {
        index = stack_pop(*vm->unused);
        // removes
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

        // updates
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

Int brl_mem_collect(VirtualMachine *vm, IntList *args)
{
    collect_garbage(vm);
    return -1;
}

Int brl_mem_swap(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    if (a >= 0 && a < vm->stack->size && b >= 0 && b < vm->stack->size)
    {
        Value temp = vm->stack->data[a];
        char type = vm->typestack->data[a];
        vm->stack->data[a] = vm->stack->data[b];
        vm->typestack->data[a] = vm->typestack->data[b];
        vm->stack->data[b] = temp;
        vm->typestack->data[b] = type;
    }
    return -1;
}

Int brl_mem_set(VirtualMachine *vm, IntList *args)
{
    Int variable = stack_shift(*args);
    Int value = stack_shift(*args);
    
    if (variable >= 0 && variable < vm->stack->size)
    {
        Int found = stack_find(*vm->temp, variable);
        if (found >= 0)
        {
            unuse_var(vm, variable);
        }
        vm->stack->data[variable] = vm->stack->data[value];
        vm->typestack->data[variable] = vm->typestack->data[value];
    }
    
    return -1;
}

Int brl_mem_use(VirtualMachine *vm, IntList *args)
{
    Int index;
    while (args->size > 0)
    {
        index = stack_shift(*args);
        if (index >= 0 && index < vm->stack->size)
        {
            use_var(vm, index);
        }
    }
    return -1;
}

Int brl_mem_unuse(VirtualMachine *vm, IntList *args)
{
    Int index;
    while (args->size > 0)
    {
        index = stack_shift(*args);
        if (index >= 0 && index < vm->stack->size)
        {
            unuse_var(vm, index);
        }
    }
    return -1;
}

Int brl_mem_copy(VirtualMachine *vm, IntList *args)
{
    Int variable = stack_shift(*args);
    Int value = stack_shift(*args);
    Int newvar = new_var(vm);
    
    if (variable >= 0 && variable < vm->stack->size)
    {
        if (value >= 0 && value < vm->stack->size)
        {
            vm->stack->data[newvar] = value_duplicate(vm->stack->data[value], vm->typestack->data[value]);
            vm->typestack->data[newvar] = vm->typestack->data[value];
        }
    }
    
    return newvar;
}

Int brl_mem_get(VirtualMachine *vm, IntList *args)
{
    Int index = (Int)stack_shift(*args);
    if (index >= 0 && index < vm->stack->size)
    {
        return index;
    }
    
    return -1;
}


Int brl_mem_delete(VirtualMachine *vm, IntList *args)
{
    while (args->size > 0)
    {
        unuse_var(vm, stack_shift(*args));
    }
    return -1;
}


Int brl_mem_length(VirtualMachine *vm, IntList *args)
{
    return new_number(vm, vm->stack->size);
}

Int brl_mem_next(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    stack_unshift(*vm->unused, index);
    while (args->size > 0)
    {
        stack_unshift(*vm->unused, stack_shift(*args));
    }
    return -1;
}

Int brl_mem_sector_new(VirtualMachine *vm, IntList *args)
{
    Int _size = stack_shift(*args);
    Int index = new_var(vm);
    while ((vm->stack->data[_size].number - 1) > 0)
    {
        new_var(vm);
        vm->stack->data[_size].number--;
    }
    return index;
}

Int brl_mem_sector_copy(VirtualMachine *vm, IntList *args)
{
    Int origin = stack_shift(*args);
    Int destination = stack_shift(*args);
    Int _size = stack_shift(*args);
    while (vm->stack->data[_size].number > 0)
    {
        if (origin >= 0 && origin < vm->stack->size)
        {
            if (destination >= 0 && destination < vm->stack->size)
            {
                vm->stack->data[destination] = value_duplicate(vm->stack->data[origin], vm->typestack->data[origin]);
                vm->typestack->data[destination] = vm->typestack->data[origin];
            }
        }
        origin++;
        destination++;
        vm->stack->data[_size].number--;
    }
    return -1;
}

Int brl_mem_sector_swap(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int _size = stack_shift(*args);
    while (vm->stack->data[_size].number > 0)
    {
        if (a >= 0 && a < vm->stack->size && b >= 0 && b < vm->stack->size)
        {
            Value temp = vm->stack->data[a];
            char type = vm->typestack->data[a];
            vm->stack->data[a] = vm->stack->data[b];
            vm->typestack->data[a] = vm->typestack->data[b];
            vm->stack->data[b] = temp;
            vm->typestack->data[b] = type;
        }
        a++;
        b++;
        vm->stack->data[_size].number--;
    }
    return -1;
}

// inits
#ifndef ARDUINO
void init_os(VirtualMachine *vm)
{
    register_builtin(vm, "file.read", brl_os_file_read);
    register_builtin(vm, "file.write", brl_os_file_write);
    register_builtin(vm, "file.delete", brl_os_file_delete);
    register_builtin(vm, "file.exists", brl_os_file_exists);
    register_builtin(vm, "file.rename", brl_os_file_rename);
    register_builtin(vm, "file.copy", brl_os_file_copy);
    register_builtin(vm, "file.size", brl_os_file_size);

    register_builtin(vm, "os.time", brl_os_time_now);
    register_builtin(vm, "os.clock", brl_os_time_clock);
    //register_builtin(vm, "os.sleep", brl_os_time_sleep);

    register_builtin(vm, "dofile", brl_os_dofile);
    register_builtin(vm, "repl", brl_os_repl);
}
#endif

void init_basics(VirtualMachine *vm)
{
    register_builtin(vm, "#", brl_std_ignore);
    register_builtin(vm, "do", brl_std_do);
    register_builtin(vm, "return", brl_std_return);
    register_builtin(vm, "gindex", brl_std_gindex);
#ifndef ARDUINO
    register_builtin(vm, "ls", brl_std_io_ls);
    register_builtin(vm, "ls.type", brl_std_io_ls_types);
    register_builtin(vm, "ls.hash", brl_std_io_ls_hashes);
    register_builtin(vm, "ls.temp", brl_std_io_ls_temp);
    register_builtin(vm, "ls.free", brl_std_io_ls_unused);
    register_builtin(vm, "print", brl_std_io_print);
#endif
}

void init_type(VirtualMachine *vm)
{
    // types
    register_number(vm, "type.size", sizeof(Value));
    register_number(vm, "type.nil", TYPE_NIL);
    register_number(vm, "type.number", TYPE_NUMBER);
    register_number(vm, "type.integer", TYPE_INTEGER);
    register_number(vm, "type.string", TYPE_STRING);
    register_number(vm, "type.builtin", TYPE_BUILTIN);
    register_number(vm, "type.list", TYPE_LIST);
    register_number(vm, "type.raw", TYPE_RAW);
    register_number(vm, "type.other", TYPE_OTHER);

    // type functions
    register_builtin(vm, "type.get", brl_std_type_get);
    register_builtin(vm, "type.set", brl_std_type_set);
}

void init_loop(VirtualMachine *vm)
{
    register_builtin(vm, "while", brl_std_loop_while);
    register_builtin(vm, "repeat", brl_std_loop_repeat);
    register_builtin(vm, "each", brl_std_loop_each);
}

void init_hash(VirtualMachine *vm)
{
    register_builtin(vm, "hash.set", brl_std_hash_set);
    register_builtin(vm, "hash.get", brl_std_hash_get);
    register_builtin(vm, "hash.delete", brl_std_hash_delete);
    register_builtin(vm, "hash.rename", brl_std_hash_rename);
}

void init_math(VirtualMachine *vm)
{
    register_builtin(vm, "add", brl_std_math_add);
    register_builtin(vm, "sub", brl_std_math_sub);
    register_builtin(vm, "mul", brl_std_math_mul);
    register_builtin(vm, "div", brl_std_math_div);
    register_builtin(vm, "mod", brl_std_math_mod);
    register_builtin(vm, "pow", brl_std_math_pow);
    register_builtin(vm, "abs", brl_std_math_abs);
    register_builtin(vm, "sqrt", brl_std_math_sqrt);
    register_builtin(vm, "ceil", brl_std_math_ceil);
    register_builtin(vm, "seed", brl_std_math_seed);
    register_builtin(vm, "floor", brl_std_math_floor);
    register_builtin(vm, "round", brl_std_math_round);
    register_builtin(vm, "random", brl_std_math_random);
    register_builtin(vm, "incr", brl_std_math_increment);
    register_builtin(vm, "decr", brl_std_math_decrement);
    register_builtin(vm, "int.from.float", int_from_float);
    register_builtin(vm, "float.from.int", int_to_float);
}

void init_string(VirtualMachine *vm)
{
    register_builtin(vm, "string.sub", brl_std_string_ndup);
    register_builtin(vm, "string.find", brl_std_string_find);
    register_builtin(vm, "string.len", brl_std_string_length);
    register_builtin(vm, "string.split", brl_std_string_split);
    register_builtin(vm, "string.concat", brl_std_string_concat);
    register_builtin(vm, "string.format", brl_std_string_format);
    register_builtin(vm, "string.replace", brl_std_string_replace);
    register_builtin(vm, "string.to.number", brl_std_string_to_number);
    register_builtin(vm, "string.replace.all", brl_std_string_replace_all);
}

void init_condition(VirtualMachine *vm)
{
    register_builtin(vm, "or", brl_std_condition_or);
    register_builtin(vm, "if", brl_std_condition_if);
    register_builtin(vm, "<", brl_std_condition_less);
    register_builtin(vm, "and", brl_std_condition_and);
    register_builtin(vm, "not", brl_std_condition_not);
    register_builtin(vm, "==", brl_std_condition_equals);
    register_builtin(vm, ">", brl_std_condition_greater);
    register_builtin(vm, "ifelse", brl_std_condition_ifelse);
    register_builtin(vm, "!=", brl_std_condition_not_equals);
    register_builtin(vm, "<=", brl_std_condition_less_equals);
    register_builtin(vm, ">=", brl_std_condition_greater_equals);
}

void init_list(VirtualMachine *vm)
{
    register_builtin(vm, "list.new", brl_std_list_new);
    register_builtin(vm, "list.pop", brl_std_list_pop);
    register_builtin(vm, "list.get", brl_std_list_get);
    register_builtin(vm, "list.len", brl_std_list_length);
    register_builtin(vm, "list.push", brl_std_list_push);
    register_builtin(vm, "list.find", brl_std_list_find);
    register_builtin(vm, "list.shift", brl_std_list_shift);
    register_builtin(vm, "list.insert", brl_std_list_insert);
    register_builtin(vm, "list.remove", brl_std_list_remove);
    register_builtin(vm, "list.concat", brl_std_list_concat);
    register_builtin(vm, "list.unshift", brl_std_list_unshift);
    register_builtin(vm, "list.last", brl_std_list_last);
}

void init_sector(VirtualMachine *vm)
{
    register_builtin(vm, "sector.new", brl_mem_sector_new);
    register_builtin(vm, "sector.copy", brl_mem_sector_copy);
    register_builtin(vm, "sector.swap", brl_mem_sector_swap);
}

void init_mem(VirtualMachine *vm)
{
    register_number(vm, "mem.size", sizeof(Float));
    register_builtin(vm, "mem.use", brl_mem_use);
    register_builtin(vm, "mem.unuse", brl_mem_unuse);
    register_builtin(vm, "mem.get", brl_mem_get);
    register_builtin(vm, "mem.hold", brl_mem_hold);
    register_builtin(vm, "mem.keep", brl_mem_keep);
    register_builtin(vm, "mem.set", brl_mem_set);
    register_builtin(vm, "mem.copy", brl_mem_copy);
    register_builtin(vm, "mem.len", brl_mem_length);
    register_builtin(vm, "mem.clear", brl_mem_clear);
    register_builtin(vm, "mem.rebase", brl_mem_rebase);
    register_builtin(vm, "mem.unhold", brl_mem_unhold);
    register_builtin(vm, "mem.delete", brl_mem_delete);
    register_builtin(vm, "mem.collect", brl_mem_collect);
    register_builtin(vm, "mem.swap", brl_mem_swap);
    register_builtin(vm, "mem.next", brl_mem_next);

    init_sector(vm);
}

void init_default_vars(VirtualMachine *vm)
{
    hold_var(vm,spawn_string(vm, "VERSION", VERSION));// version
}

// std init presets
void init_std(VirtualMachine *vm)
{
    #ifndef ARDUINO
    init_os(vm);
    #endif
    init_basics(vm);
    init_type(vm);
    init_loop(vm);
    init_hash(vm);
    init_list(vm);
    init_math(vm);
    init_string(vm);
    init_condition(vm);
    init_default_vars(vm);
}