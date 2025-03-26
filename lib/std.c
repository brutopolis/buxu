#include "buxu.h"

// functions defitions for buxu
// functions defitions for buxu
// functions defitions for buxu

// MAX priority is always 0
// MIN priority is BASE_PRIORITY
Int BASE_PRIORITY = 10;

#ifndef ARDUINO

function(brl_os_file_read)
{
    char *code = readfile(arg(0).string);
    Int index = -1;
    if (code != NULL)
    {
        index = new_var(vm);
        data(index).string = code;
        data_t(index) = TYPE_STRING;
    }
    return index;
}

function(brl_os_file_write)
{
    writefile(arg(0).string, arg(1).string);
    return -1;
}

function(brl_os_system)
{
    system(arg(0).string);
    return -1;
}


#endif

function(brl_std_hash_new)
{
    hash_set(vm, arg(0).string, arg_i(1));
    return -1;
}

function(brl_std_hash_delete)
{
    hash_unset(vm, arg(0).string);
    return -1;
}

/*function(brl_std_hash_rename)
{
    Int index = hash_find(vm, arg(0).string);
    if (index != -1)
    {
        free(hash(index).key);
        hash(index).key = str_duplicate(arg(1).string);
    }
    return -1;
}*/

function(brl_std_io_scan) // always get string
{
    printf("%s: ", EMOTICON_CONFUSED);
    char *str = (char*)malloc(1024);
    fgets(str, 1024, stdin);
    str[strlen(str) - 1] = '\0';
    Int index = new_var(vm);
    data(index).string = str;
    data_t(index) = TYPE_STRING;
    return index;
}

function(brl_std_io_ls)
{
    for (Int i = 0; i < vm->stack->size; i++)
    {
        printf("  |%ld    ", i);
        switch (data_t(i)) 
        {
            case TYPE_STRING:
                printf("string   ");
                break;
            case TYPE_NUMBER:
                printf("number   ");
                break;
            case TYPE_ARRAY:
                printf("array   ");
                break;
            case TYPE_ANY:
                printf("any   ");
                break;
        }
        print_element(vm, i);
        printf("\n");
    }

    return -1;
}

function(brl_std_io_ls_hashes)
{
    Int max_len = 0;
    for (Int i = 0; i < vm->hash_names->size; i++) 
    {
        int len = snprintf(NULL, 0, "  |%ld   %s", vm->hash_indexes->data[i], vm->hash_names->data[i]);
        if ((Int)len > max_len) 
        {
            max_len = len;
        }
    }
    Int col_width = max_len + 2;
    for (Int i = 0; i < vm->hash_names->size; i++) 
    {
        int len = snprintf(NULL, 0, "  |%ld   %s", vm->hash_indexes->data[i], vm->hash_names->data[i]);
        printf("  |%ld   %s", vm->hash_indexes->data[i], vm->hash_names->data[i]);
        Int pad = col_width > (Int)len ? col_width - len : 0;
        for (Int j = 0; j < pad; j++) 
        {
            printf(" ");
        }
        switch (vm->typestack->data[vm->hash_indexes->data[i]]) 
        {
            case TYPE_STRING:
                printf("|string   ");
                break;
            case TYPE_NUMBER:
                printf("|number   ");
                break;
            case TYPE_ARRAY:
                printf("|array   ");
                break;
            case TYPE_ANY:
                printf("|any   ");
                break;
        }
        print_element(vm, vm->hash_indexes->data[i]);
        printf("\n");
    }

    return -1;
}

function(brl_std_ignore)
{
    return -1;
}

function(brl_std_return)
{
    return arg_i(0);
}




// list functions
// list functions
// list functions
// list functions

function(brl_std_array_new)
{
    Int index;
    if (args->size == 0)
    {
        index = new_array(vm, 0);
    }
    else
    {
        index = new_array(vm, arg(0).number);
    }
    return index;
}

function(brl_std_array_set)
{
    if (args->size == 2)
    {
        if (arg_i(0) >= 0 && arg_i(0) < vm->stack->size)
        {
            // if the value is a string, we need to free it before
            if (arg_t(0) == TYPE_STRING)
            {
                unuse_var(vm, arg_i(0));
            }

            arg(0) = arg(1);
            arg_t(0) = arg_t(1);
        }
    }
    else if (arg_t(0) == TYPE_ARRAY)
    {
        Int list_size = arg(0).number;
        Int index = arg_i(0);

        
        if (index >= 0 && index < arg_i(0) + list_size /*&& index < vm->stack->size*/)
        {
            data(index) = value_duplicate(arg(1), arg_t(1));
        }
        else 
        {
            buxu_error("index %ld out of range in list %ld of size %ld", index, arg_i(0), list_size);
        }
        return -1;
    }
    else if (arg_t(0) == TYPE_STRING)
    {
        char *str = arg(0).string;
        Int index = arg(1).number;
        char c = arg_i(2);
        if (args->size > 3) // receive a bunch of chars
        {
            for (Int i = 3; i < args->size - 2; i++)
            {
                str[index + i] = arg_i(i + 2);
            }
        }
        else if (index >= 0 && index < strlen(str))
        {
            str[index] = c;
        }
        else 
        {
            buxu_error("index %ld out of range in string '%s' of size %ld", index, arg(0).string, strlen(str));
        }
        return -1;
    }
    else 
    {
        Int index = (Int)arg(1).number;
        Int value = arg_i(0);
        if (index >= 0 && index < sizeof(Float))
        {
            arg(0).u8[index] = (char)value;
        }
    }
    return -1;
}

function(brl_std_array_length)
{
    if (args->size == 0)
    {
        return new_number(vm, vm->stack->size);
    }
    else if (arg_t(0) == TYPE_ARRAY)
    {
        return new_number(vm, arg(0).number);
    }
    else if (arg_t(0) == TYPE_STRING)
    {
        return new_number(vm, strlen(arg(0).string));
    }
    /*
    else 
    {
        return new_number(vm, sizeof(Float));
    }
    */
    return -1;
}

function(brl_std_array_get)
{
    if (args->size == 1) // global
    {
        return arg(0).number;
    }
    else if (arg_t(0) == TYPE_ARRAY) // array
    {
        Int index = arg_i(0);
        Int list_size = data(index).number;
        Int i = arg_i(1);
        if (i >= 0 && i < list_size)
        {
            return index + 1 + i;
        }
        else 
        {
            buxu_error("index %ld out of range in array %ld of size %ld", i, index, list_size);
        }
    }
    else if (arg_t(0) == TYPE_STRING) // 
    {
        Int index = arg_i(0);
        Int i = arg_i(1);
        if (i >= 0 && i < strlen(arg(0).string))
        {
            return arg(0).string[i];
        }
        else 
        {
            buxu_error("index %ld out of range in string '%s' of size %ld", i, arg(0).string, strlen(arg(0).string));
        }
    }
    else 
    {
        Int index = (Int)arg(0).integer;
        Int i = arg_i(1);
        if (i >= 0 && i < sizeof(Float))
        {
            return arg(0).u8[i];
        }
    }
    return -1;
}

function(buxu_std_array_resize)
{
    // we reallocate the stack data
    Int array_index = arg_i(0);
    Int new_size = arg(1).number;
    Int old_size = data(array_index).number;
    Int diff = new_size - old_size;

    if (diff > 0)
    {
        // lets see if the capacity is enough
        if (vm->stack->capacity < vm->stack->size + diff)
        {
            list_double(*vm->stack);
            list_double(*vm->typestack);
        }
        
        // lets move the elements manually with memmove then we update the size
        memmove(&(vm->stack->data[array_index + 1 + new_size]), &(vm->stack->data[array_index + 1 + old_size]), (vm->stack->size - (array_index + 1 + old_size)) * sizeof(Value));
        memmove(&(vm->typestack->data[array_index + 1 + new_size]), &(vm->typestack->data[array_index + 1 + old_size]), (vm->typestack->size - (array_index + 1 + old_size)) * sizeof(Byte));
        
        // lets update the size
        vm->stack->size += diff;
        vm->typestack->size += diff;
        
        // lets update the size of the array
        data(array_index).number = new_size;

        // lets initialize the new elements with zeroes
        for (Int i = 0; i < diff; i++)
        {
            data(array_index + 1 + old_size + i).number = 0;
            data_t(array_index + 1 + old_size + i) = TYPE_ANY;
        }
    }
    else if (diff < 0)
    {
        // lets move the elements manually with memmove then we update the size
        memmove(&(vm->stack->data[array_index + 1 + new_size]), &(vm->stack->data[array_index + 1 + old_size]), (vm->stack->size - (array_index + 1 + new_size)) * sizeof(Value));
        memmove(&(vm->typestack->data[array_index + 1 + new_size]), &(vm->typestack->data[array_index + 1 + old_size]), (vm->typestack->size - (array_index + 1 + new_size)) * sizeof(Byte));
        
        // lets update the size
        vm->stack->size += diff;
        vm->typestack->size += diff;
        
        // lets update the size of the array
        data(array_index).number = new_size;
    }
    else 
    {
        // nothing to do
        buxu_warn("resizing array @%ld to the same size: %ld", array_index, new_size);
        return -1;
    }

    // lets update the hashes
    for (Int i = 0; i < vm->hash_names->size; i++)
    {
        if (vm->hash_indexes->data[i] > array_index + old_size)
        {
            vm->hash_indexes->data[i] += diff;
        }
    }
    return -1;
}

function(buxu_std_array_fill)
{
    if (arg_t(0) == TYPE_ARRAY)
    {
        Int index = arg_i(0);
        Int size = data(index).number;
        memset(&(data(index + 1)), arg_i(1), size * sizeof(Value));
        if (args->size == 3)
        {
            memset(&(data_t(index + 1)), arg_i(2), size * sizeof(Byte));
        }
        else 
        {
            memset(&(data_t(index + 1)), 0, size * sizeof(Byte));
        }
    }
    else 
    {
        buxu_error("@%ld is not an array", arg_i(0));
    }
    return -1;
}

function(buxu_std_array_clear)
{
    if (arg_t(0) == TYPE_ARRAY)
    {
        Int index = arg_i(0);
        Int size = data(index).number;
        memset(&(data(index + 1)), 0, size * sizeof(Value));
        memset(&(data_t(index + 1)), 0, size * sizeof(Byte));
        for (Int i = arg_i(0) + 1; i < arg_i(0) + 1 + data(arg_i(0)).number; i++)
        {
            list_push(*vm->unused, i);
        }
    }
    
    return -1;
}

function(brl_std_index)
{
    return (new_number(vm, arg_i(0)));
}

function(brl_mem_copy)
{
    Int newvar = new_var(vm);
    data(newvar) = value_duplicate(data(arg_i(0)), data_t(arg_i(0)));
    return newvar;
}

function(brl_mem_delete)
{
    for (Int i = 0; i < args->size; i++)
    {
        unuse_var(vm, arg_i(i));
    }
    return -1;
}

function(brl_std_deplace)
{
    Int newindex = new_var(vm);
    Int func = arg_i(0);
    data(newindex) = value_duplicate(data(arg_i(1)), data_t(arg_i(1)));
    data_t(newindex) = data_t(arg_i(1));
    arg_i(1) = newindex;
    interpret_args(vm, args);
    return newindex;
}

// std type
// std type
// std type

function(brl_std_type_get)
{
    return arg_t(0);
}

function(brl_std_type_set)
{
    arg_t(0) = (Int)arg(1).number;
    return -1;
}


function(brl_std_type_cast)
{
    switch (arg_t(0))
    {
        char* _str;
        Int index;
        case TYPE_NUMBER:
            switch ((Int)arg(1).number)
            {
                case TYPE_STRING:
                    _str =  str_format("%ld", (Int)arg(0).number);
                    arg(0).string = _str;
                    arg_t(0) = TYPE_STRING;
                    return -1;
                    break;
                case TYPE_ANY:
                    arg(0).integer = arg(0).number;
                    arg_t(0) = TYPE_ANY;
                    return -1;
                    break;
            }
            break;
        case TYPE_STRING:
            _str = arg(0).string;
            switch ((Int)arg(1).number)
            {
                case TYPE_NUMBER:
                    arg(0).number = atof(arg(0).string);
                    arg_t(0) = TYPE_NUMBER;
                    free(_str);
                    return -1;
                    break;
                case TYPE_ANY:
                    arg(0).integer = atoi(arg(0).string);
                    arg_t(0) = TYPE_ANY;
                    free(_str);
                    return -1;
                    break;
            }
            break;
        case TYPE_ARRAY:
            switch ((Int)arg(1).number)
            {
                case TYPE_STRING:
                {
                    char* _str = list_stringify(vm, arg_i(0));
                    Int result = new_string(vm, _str);
                    free(_str);
                    return result;
                    break;
                }
            }
            break;
        case TYPE_ANY:
            switch ((Int)arg(1).number)
            {
                case TYPE_STRING:
                    _str = str_format("%ld", arg(0).integer);
                    arg(0).string = _str;
                    arg_t(0) = TYPE_STRING;
                    return -1;
                    break;
                case TYPE_NUMBER:
                    arg(0).number = arg(0).integer;
                    arg_t(0) = TYPE_NUMBER;
                    return -1;
                    break;
            }
            break;
    }
    return -1;
}

// math functions
// math functions
// math functions
// math functions


function(brl_std_math_add)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
        case TYPE_ARRAY:
            for (Int i = 1; i < args->size; i++)
            {
                switch (arg_t(i))
                {
                    case TYPE_NUMBER:
                    case TYPE_ARRAY:
                        arg(0).number += arg(i).number;
                        break;
                    default:
                        arg(0).number += (Float)arg(i).integer;
                        break;
                }
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                switch (arg_t(i))
                {
                    case TYPE_NUMBER:
                    case TYPE_ARRAY:
                        arg(0).integer += (Int)arg(i).number;
                        break;
                    default:
                        arg(0).integer += arg(i).integer;
                        break;
                }
            }
            break;
    }
    return -1;
}

function(brl_std_math_sub)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
        case TYPE_ARRAY:
            for (Int i = 1; i < args->size; i++)
            {
                switch (arg_t(i))
                {
                    case TYPE_NUMBER:
                    case TYPE_ARRAY:
                        arg(0).number -= arg(i).number;
                        break;
                    default:
                        arg(0).number -= (Float)arg(i).integer;
                        break;
                }
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                switch (arg_t(i))
                {
                    case TYPE_NUMBER:
                    case TYPE_ARRAY:
                        arg(0).integer -= (Int)arg(i).number;
                        break;
                    default:
                        arg(0).integer -= arg(i).integer;
                        break;
                }
            }
            break;
    }
    return -1;
}

function(brl_std_math_mul)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
        case TYPE_ARRAY:
            for (Int i = 1; i < args->size; i++)
            {
                switch (arg_t(i))
                {
                    case TYPE_NUMBER:
                    case TYPE_ARRAY:
                        arg(0).number *= arg(i).number;
                        break;
                    default:
                        arg(0).number *= (Float)arg(i).integer;
                        break;
                }
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                switch (arg_t(i))
                {
                    case TYPE_NUMBER:
                    case TYPE_ARRAY:
                        arg(0).integer *= (Int)arg(i).number;
                        break;
                    default:
                        arg(0).integer *= arg(i).integer;
                        break;
                }
            }
            break;
    }
    return -1;
}

function(brl_std_math_div)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
            for (Int i = 1; i < args->size; i++)
            {
                switch (arg_t(i))
                {
                    case TYPE_NUMBER:
                    case TYPE_ARRAY:
                        arg(0).number /= arg(i).number;
                        break;
                    default:
                        arg(0).number /= (Float)arg(i).integer;
                        break;
                }
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                switch (arg_t(i))
                {
                    case TYPE_NUMBER:
                    case TYPE_ARRAY:
                        arg(0).integer /= (Int)arg(i).number;
                        break;
                    default:
                        arg(0).integer /= arg(i).integer;
                        break;
                }
            }
            break;
    }
    return -1;
}

function(brl_std_math_mod)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
        case TYPE_ARRAY:
            #if __SIZEOF_POINTER__ == 8
                arg(0).number = fmod(arg(0).number, arg(1).number);
            #else
                arg(0).number = fmodf(arg(0).number, arg(1).number);
            #endif
            break;
        default:
            arg(0).integer %= arg(1).integer;
            break;
    }
    
    return -1;
}

function(brl_std_math_random)
{
    switch (args->size)
    {
        case 2: // (min, max)
            arg(0).number = rand() % (Int)arg(1).number + (Int)arg(0).number;
            break;
        
        default:
            arg(0).number = rand();
            break;
    }
    return -1;
}

function(brl_std_math_seed)
{
    srand(arg(0).integer);
    return -1;
}

function(brl_std_math_round)
{
    #if __SIZEOF_POINTER__ == 8
        arg(0).number = round(arg(0).number);
    #else
        arg(0).number = roundf(arg(0).number);
    #endif
    return -1;
}

function(brl_std_math_floor)
{
    #if __SIZEOF_POINTER__ == 8
        arg(0).number = floor(arg(0).number);
    #else
        arg(0).number = floorf(arg(0).number);
    #endif
    return -1;
}

function(brl_std_math_ceil)
{
    #if __SIZEOF_POINTER__ == 8
        arg(0).number = ceil(arg(0).number);
    #else
        arg(0).number = ceilf(arg(0).number);
    #endif
    return -1;
}

function(brl_std_math_sin)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
        case TYPE_ARRAY:
            #if __SIZEOF_POINTER__ == 8
                arg(0).number = sin(arg(0).number);
            #else
                arg(0).number = sinf(arg(0).number);
            #endif
            break;
        default:
            arg(0).number = sin((Float)arg(0).integer);
            arg_t(0) = TYPE_NUMBER;
            break;
    }
    return -1;
}

function(brl_std_math_cos)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
        case TYPE_ARRAY:
            #if __SIZEOF_POINTER__ == 8
                arg(0).number = cos(arg(0).number);
            #else
                arg(0).number = cosf(arg(0).number);
            #endif
            break;
        default:
            arg(0).number = cos((Float)arg(0).integer);
            arg_t(0) = TYPE_NUMBER;
            break;
    }
    return -1;  
}

function(brl_std_math_tan)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
        case TYPE_ARRAY:
            #if __SIZEOF_POINTER__ == 8
                arg(0).number = tan(arg(0).number);
            #else
                arg(0).number = tanf(arg(0).number);
            #endif
            break;
        default:
            arg(0).number = tan((Float)arg(0).integer);
            arg_t(0) = TYPE_NUMBER;
            break;
    }
    return -1;
}

function(brl_std_math_bitwise_and)
{
    for (Int i = 1; i < args->size; i++)
    {
        arg(0).integer &= arg(i).integer;
    }
    return -1;
}

function(brl_std_math_bitwise_or)
{
    for (Int i = 1; i < args->size; i++)
    {
        arg(0).integer |= arg(i).integer;
    }
    return -1;
}

function(brl_std_math_bitwise_xor)
{
    for (Int i = 1; i < args->size; i++)
    {
        arg(0).integer ^= arg(i).integer;
    }
    return -1;
}

function(brl_std_math_bitwise_not)
{
    arg(0).integer = ~arg(0).integer;
    return -1;
}

function(brl_std_math_bitwise_shift_left)
{
    for (Int i = 1; i < args->size; i++)
    {
        arg(0).integer <<= arg(i).integer;
    }
    return -1;
}

function(brl_std_math_bitwise_shift_right)
{
    for (Int i = 1; i < args->size; i++)
    {
        arg(0).integer >>= arg(i).integer;
    }
    return -1;
}

function(brl_std_min)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
        case TYPE_ARRAY:
        {
            Float min = arg(0).number;
            for (Int i = 1; i < args->size; i++)
            {
                #if __SIZEOF_POINTER__ == 8
                    min = fmin(min, arg(i).number);
                #else
                    min = fminf(min, arg(i).number);
                #endif
            }
            arg(0).number = min;
        }
        default:
        {
            Int min = arg(0).integer;
            for (Int i = 1; i < args->size; i++)
            {
                min = min < arg(i).integer ? min : arg(i).integer;
            }
            arg(0).integer = min;
            break;
        }
    }
    return -1;
}

function(brl_std_max)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
        case TYPE_ARRAY:
        {
            Float max = arg(0).number;
            for (Int i = 1; i < args->size; i++)
            {
                #if __SIZEOF_POINTER__ == 8
                    max = fmax(max, arg(i).number);
                #else
                    max = fmaxf(max, arg(i).number);
                #endif
            }
            arg(0).number = max;
        }
        default:
        {
            Int max = arg(0).integer;
            for (Int i = 1; i < args->size; i++)
            {
                max = max > arg(i).integer ? max : arg(i).integer;
            }
            arg(0).integer = max;
            break;
        }
    }
    return -1;
}


// std conditions
// std conditions
// std conditions

function(brl_std_condition_if)
{
    Int result = -1;
    if (args->size == 2)
    {
        if (eval(vm, arg(0).string))
        {
            result = eval(vm, arg(1).string);
        }
    }
    else if (args->size == 3) // ifelse
    {
        if (eval(vm, arg(0).string))
        {
            result = eval(vm, arg(1).string);
        }
        else
        {
            result = eval(vm, arg(2).string);
        }
    }
    return result;
}

function(brl_std_condition_equals)// ==
{
    Int result = arg_i(0);
    if (result == 0)
    {
        for (Int i = 1; i < args->size; i++)
        {
            if (arg_i(i) != 0)
            {
                result = i;
                break;
            }
        }

        if (result == 0)
        {
            result = 1;
        }
    }
    
    for (Int i = 1; i < args->size; i++)
    {
        if (arg_t(i - 1) != arg_t(i))
        {
            return 0;
        }
        else
        {
            switch (arg_t(i - 1))
            {
                case TYPE_STRING:
                    if (strcmp(arg(i - 1).string, arg(i).string) != 0)
                    {
                        return 0;
                    }
                    break;
                default:
                    if (arg(i - 1).integer != arg(i).integer)
                    {
                        return 0;
                    }
                    break;
            }
        }
    }
    return result;
}

function(brl_std_condition_not_equals)// !=
{
    Int result = arg_i(0);
    if (result == 0)
    {
        for (Int i = 1; i < args->size; i++)
        {
            if (arg_i(i) != 0)
            {
                result = i;
                break;
            }
        }

        if (result == 0)
        {
            result = 1;
        }
    }

    for (Int i = 1; i < args->size; i++)
    {
        if (arg_t(i - 1) != arg_t(i))
        {
            return result;
        }
        else
        {
            switch (arg_t(i - 1))
            {
                case TYPE_STRING:
                    if (strcmp(arg(i - 1).string, arg(i).string) == 0)
                    {
                        return 0;
                    }
                    break;
                default:
                    if (arg(i - 1).integer == arg(i).integer)
                    {
                        return 0;
                    }
                    break;
            }
        }
    }
    return result;
}


function(brl_std_condition_greater)
{
    Int result = arg_i(0);
    if (result == 0)
    {
        for (Int i = 1; i < args->size; i++)
        {
            if (arg_i(i) != 0)
            {
                result = i;
                break;
            }
        }

        if (result == 0)
        {
            result = 1;
        }
    }

    switch (arg_t(1))
    {
        case TYPE_NUMBER:
        case TYPE_ARRAY:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).number <= arg(i).number)
                {
                    return 0;
                }
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).integer <= arg(i).integer)
                {
                    return 0;
                }
            }
            break;
    }
    return result;
}

function(brl_std_condition_greater_equals)
{
    Int result = arg_i(0);
    if (result == 0)
    {
        for (Int i = 1; i < args->size; i++)
        {
            if (arg_i(i) != 0)
            {
                result = i;
                break;
            }
        }

        if (result == 0)
        {
            result = 1;
        }
    }

    switch (arg_t(1))
    {
        case TYPE_NUMBER:
        case TYPE_ARRAY:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).number < arg(i).number)
                {
                    return 0;
                }
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).integer < arg(i).integer)
                {
                    return 0;
                }
            }
            break;
    }
    return result;
}

function(brl_std_condition_less)
{
    Int result = arg_i(0);
    if (result == 0)
    {
        for (Int i = 1; i < args->size; i++)
        {
            if (arg_i(i) != 0)
            {
                result = i;
                break;
            }
        }

        if (result == 0)
        {
            result = 1;
        }
    }

    switch (arg_t(1))
    {
        case TYPE_NUMBER:
        case TYPE_ARRAY:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).number >= arg(i).number)
                {
                    return 0;
                }
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).integer >= arg(i).integer)
                {
                    return 0;
                }
            }
            break;
    }
    return result;
}

function(brl_std_condition_less_equals)
{
    Int result = arg_i(0);
    if (result == 0)
    {
        for (Int i = 1; i < args->size; i++)
        {
            if (arg_i(i) != 0)
            {
                result = i;
                break;
            }
        }

        if (result == 0)
        {
            result = 1;
        }
    }

    switch (arg_t(1))
    {
        case TYPE_NUMBER:
        case TYPE_ARRAY:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).number > arg(i).number)
                {
                    return 0;
                }
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).integer > arg(i).integer)
                {
                    return 0;
                }
            }
            break;
    }
    return result;
}

function(brl_std_condition_and)
{
    Int result = arg_i(0);
    if (result == 0)
    {
        for (Int i = 1; i < args->size; i++)
        {
            if (arg_i(i) != 0)
            {
                result = i;
                break;
            }
        }

        if (result == 0)
        {
            result = 1;
        }
    }

    for (Int i = 1; i < args->size; i++)
    {
        if (arg_i(i) == 0)
        {
            return 0;
        }
    }
    return result;
}

function(brl_std_condition_raw_or)
{
    for (Int i = 0; i < args->size; i++)
    {
        if (arg_i(i) > 0)
        {
            return arg_i(i);
        }
    }
    return 0;
}


function(brl_std_loop_while)
{
    Int result = -1;
    while (eval(vm,arg(0).string))
    {
        result = eval(vm,arg(1).string);
        switch (result)
        {
            case -1:
                break;
            default:
                return result;
        }
    }
    return result;
}

function(brl_std_loop_forever)
{
    Int result = -1;
    while (1)
    {
        result = eval(vm,arg(0).string);
        switch (result)
        {
            case -1:
                break;
            default:
                return result;
        }
    }
    return result;
}

function(brl_std_loop_repeat)
{
    Int result = -1;
    for (int i = 0; i < (Int)arg(0).number; i++)
    {
        result = eval(vm,arg(1).string);
        switch (result)
        {
            case -1:
                break;
            default:
                return result;
        }
    }
    return result;
}

function(brl_std_string_format)
{
    list_reverse(*args);
    Int str = list_pop(*args);
    Int result = -1;
    char* _str = str_duplicate(data(str).string);
    for (Int i = 0; i < strlen(_str); i++)
    {
        if (_str[i] == '%')
        {
            if (_str[i+1] == 'd')
            {
                Int value = list_pop(*args);
                char* _value = str_format("%ld", (Int)data(value).number);
                char* _newstr = str_replace(_str, "\%d", _value);
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] == 's')
            {
                Int value = list_pop(*args);
                char* _value = data(value).string;
                char* _newstr = str_replace(_str, "\%s", _value);
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] == 'f')
            {
                Int value = list_pop(*args);
                char* _value = str_format("%f", data(value).number);
                char* _newstr = str_replace(_str, "\%f", _value);
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] == 'p')
            {
                Int value = list_pop(*args);
                char* _value = str_format("%p", data(value).pointer);
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
    data(result).string = _str;
    data_t(result) = TYPE_STRING;
    return result;
}

function(buxulib_error)
{
    printf("%s: ", EMOTICON_ERROR);
    for (Int i = 0; i < args->size; i++)
    {
        if (arg_i(i) >= 0 || arg_i(i) < vm->stack->size)
        {
            print_element(vm, arg_i(i));
        }
        else 
        {
            buxu_error("(out of stack)");
        }

        if (args->size > 0)
        {
            printf(" ");
        }
    }
    printf("\n");
    return -1;
}

function(buxulib_warn)
{
    printf("%s: ", EMOTICON_WARNING);
    for (Int i = 0; i < args->size; i++)
    {
        if (arg_i(i) >= 0 || arg_i(i) < vm->stack->size)
        {
            print_element(vm, arg_i(i));
        }
        else 
        {
            buxu_error("(out of stack)");
        }

        if (args->size > 0)
        {
            printf(" ");
        }
    }
    printf("\n");
    return -1;
}

function(buxulib_print)
{
    printf("%s: ", EMOTICON_DEFAULT);
    for (Int i = 0; i < args->size; i++)
    {
        if (arg_i(i) >= 0 || arg_i(i) < vm->stack->size)
        {
            print_element(vm, arg_i(i));
        }
        else 
        {
            buxu_error("(out of stack)");
        }

        if (args->size > 0)
        {
            printf(" ");
        }
    }
    printf("\n");
    return -1;
}

function(buxulib_print_custom)
{
    printf("%s: ", arg(0).string);
    for (Int i = 1; i < args->size; i++)
    {
        if (arg_i(i) >= 0 || arg_i(i) < vm->stack->size)
        {
            print_element(vm, arg_i(i));
        }
        else 
        {
            buxu_error("(out of stack)");
        }

        if (args->size > 0)
        {
            printf(" ");
        }
    }
    printf("\n");
    return -1;
}

// inits
#ifndef ARDUINO
init(std_os)
{
    register_builtin(vm, "read", brl_os_file_read);
    register_builtin(vm, "write", brl_os_file_write);
    register_builtin(vm, "system", brl_os_system);
}
#endif

init(std_basics)
{
    register_builtin(vm, "#", brl_std_ignore);
    
    register_builtin(vm, "return", brl_std_return);

    register_builtin(vm, "ls", brl_std_io_ls);
    register_builtin(vm, "ls.hash", brl_std_io_ls_hashes);

    register_builtin(vm, "$", brl_std_deplace);

    register_builtin(vm, "while", brl_std_loop_while);
    register_builtin(vm, "repeat", brl_std_loop_repeat);
    register_builtin(vm, "forever", brl_std_loop_forever);

    register_builtin(vm, "format", brl_std_string_format);

}

init(std_hash)
{
    register_builtin(vm, "new", brl_std_hash_new);
    register_builtin(vm, "delete", brl_std_hash_delete);
}

init(std_array)
{
    register_builtin(vm, "array", brl_std_array_new);
    register_builtin(vm, "set", brl_std_array_set);
    register_builtin(vm, "get", brl_std_array_get);
    register_builtin(vm, "len", brl_std_array_length);
    register_builtin(vm, "resize", buxu_std_array_resize);
    register_builtin(vm, "fill", buxu_std_array_fill);
    register_builtin(vm, "clear", buxu_std_array_clear);
}

init(std_mem)
{
    register_builtin(vm, "mem.copy", brl_mem_copy);
    register_builtin(vm, "mem.delete", brl_mem_delete);

    register_builtin(vm, "index", brl_std_index);
}

// destructive/inplace!!
init(std_type)
{
    // type size(4 or 8 bytes)
    register_number(vm, "type.size", sizeof(Value));

    // types
    register_number(vm, "type.any", TYPE_ANY);
    register_number(vm, "type.number", TYPE_NUMBER);
    register_number(vm, "type.string", TYPE_STRING);
    register_number(vm, "type.array", TYPE_ARRAY);

    register_builtin(vm, "type", brl_std_type_get);
    register_builtin(vm, "pun", brl_std_type_set);
    register_builtin(vm, "cast", brl_std_type_cast);
}

// destructive/inplace!!
init(std_math)
{
    // math functions are inplace(destructive), you might want to use $ to do non-destructive operations
    // e.g. $ + a 2; // a is not changed
    register_builtin(vm, "+", brl_std_math_add);
    register_builtin(vm, "-", brl_std_math_sub);
    register_builtin(vm, "*", brl_std_math_mul);
    register_builtin(vm, "/", brl_std_math_div);
    register_builtin(vm, "\%", brl_std_math_mod);

    register_builtin(vm, "random", brl_std_math_random);
    register_builtin(vm, "seed", brl_std_math_seed);

    register_builtin(vm, "round", brl_std_math_round);
    register_builtin(vm, "floor", brl_std_math_floor);
    register_builtin(vm, "ceil", brl_std_math_ceil);

    register_builtin(vm, "sin", brl_std_math_sin);
    register_builtin(vm, "cos", brl_std_math_cos);
    register_builtin(vm, "tan", brl_std_math_tan);

    register_builtin(vm, "min", brl_std_min);
    register_builtin(vm, "max", brl_std_max);

    // bitwise
    register_builtin(vm, "&", brl_std_math_bitwise_and);
    register_builtin(vm, "|", brl_std_math_bitwise_or);
    register_builtin(vm, "^", brl_std_math_bitwise_xor);
    register_builtin(vm, "~", brl_std_math_bitwise_not);
    register_builtin(vm, "<<", brl_std_math_bitwise_shift_left);
    register_builtin(vm, ">>", brl_std_math_bitwise_shift_right);
}

// index-based!!
init(std_condition)
{
    register_builtin(vm, "==", brl_std_condition_equals);
    register_builtin(vm, "!=", brl_std_condition_not_equals);

    register_builtin(vm, ">", brl_std_condition_greater);
    register_builtin(vm, ">=", brl_std_condition_greater_equals);
    register_builtin(vm, "<", brl_std_condition_less);
    register_builtin(vm, "<=", brl_std_condition_less_equals);
    
    register_builtin(vm, "&&", brl_std_condition_and);
    register_builtin(vm, "||", brl_std_condition_raw_or);

    register_builtin(vm, "if", brl_std_condition_if);
}

init(buxu)
{
    register_builtin(vm, "warn", buxulib_warn);
    register_builtin(vm, "error", buxulib_error);
    register_builtin(vm, "print", buxulib_print);
    register_builtin(vm, "print.custom", buxulib_print_custom);

    #ifndef ARDUINO
        register_builtin(vm, "scan", brl_std_io_scan);// not avaliable on arduino
    #endif

    register_string(vm, "bu.default", EMOTICON_DEFAULT);
    register_string(vm, "bu.warning", EMOTICON_WARNING);
    register_string(vm, "bu.error", EMOTICON_ERROR);
    register_string(vm, "bu.idle", EMOTICON_IDLE);
    register_string(vm, "bu.confused", EMOTICON_CONFUSED);
}

// std init presets
init(std)
{
    #ifndef ARDUINO
    init_std_os(vm);
    #endif
    
    init_buxu(vm);

    init_std_basics(vm);
    init_std_hash(vm);
    init_std_array(vm);
    init_std_mem(vm);

    init_std_type(vm);
    init_std_math(vm);
    init_std_condition(vm);


    register_string(vm, "VERSION", VERSION);// version
}