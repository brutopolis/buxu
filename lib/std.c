#include "buxu.h"

// functions defitions for buxu
// functions defitions for buxu
// functions defitions for buxu


// print 
void print_element(VirtualMachine *vm, Int index)
{
    if (index < 0 || index >= vm->stack->size)
    {
        printf("(return (get -1))");
        return;
    }

    Value temp = vm->stack->data[index];

    switch (data_t(index))
    {
        char* _str;
        char* strbak;
        char* stringified;
            
        case TYPE_NUMBER:
            if (((Int)data(index).f) == data(index).f)
            {
                printf("%ld", (Int)data(index).f);
            }
            else
            {
                printf("%f", data(index).f);
            }
            break;
        case TYPE_STRING:
            printf("%s", data(index).s);
            break;
        default:
            printf("%ld", (Int)data(index).p);
            break;
    }
}

function(brl_os_file_read)
{
    char *code = readfile(arg(0).s);
    Int index = -1;
    if (code != NULL)
    {
        index = new_var(vm);
        data(index).s = code;
        data_t(index) = TYPE_STRING;
    }
    return index;
}

function(brl_os_file_write)
{
    writefile(arg(0).s, arg(1).s);
    return -1;
}

function(brl_os_system)
{
    system(arg(0).s);
    return -1;
}

function(brl_std_io_scan) // always get string
{
    printf("%s: ", EMOTICON_CONFUSED);
    char *str = (char*)malloc(1024);
    fgets(str, 1024, stdin);
    str[strlen(str) - 1] = '\0';
    Int index = new_var(vm);
    data(index).s = str;
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
            case TYPE_ANY:
                printf("|any   ");
                break;
        }
        print_element(vm, vm->hash_indexes->data[i]);
        printf("\n");
    }

    return -1;
}

function(brl_std_return)
{
    return arg_i(0);
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
    arg_t(0) = (Int)arg(1).f;
    return -1;
}


function(brl_std_type_cast)
{
    switch (arg_t(0))
    {
        char* _str;
        Int index;
        case TYPE_NUMBER:
            switch ((Int)arg(1).f)
            {
                case TYPE_STRING:
                    _str =  str_format("%ld", (Int)arg(0).f);
                    arg(0).s = _str;
                    arg_t(0) = TYPE_STRING;
                    return -1;
                    break;
                case TYPE_ANY:
                    arg(0).i = arg(0).f;
                    arg_t(0) = TYPE_ANY;
                    return -1;
                    break;
            }
            break;
        case TYPE_STRING:
            _str = arg(0).s;
            switch ((Int)arg(1).f)
            {
                case TYPE_NUMBER:
                    arg(0).f = atof(arg(0).s);
                    arg_t(0) = TYPE_NUMBER;
                    free(_str);
                    return -1;
                    break;
                case TYPE_ANY:
                    arg(0).i = atoi(arg(0).s);
                    arg_t(0) = TYPE_ANY;
                    free(_str);
                    return -1;
                    break;
            }
            break;
        case TYPE_ANY:
            switch ((Int)arg(1).f)
            {
                case TYPE_STRING:
                    _str = str_format("%ld", arg(0).i);
                    arg(0).s = _str;
                    arg_t(0) = TYPE_STRING;
                    return -1;
                    break;
                case TYPE_NUMBER:
                    arg(0).f = arg(0).i;
                    arg_t(0) = TYPE_NUMBER;
                    return -1;
                    break;
            }
            break;
    }
    return -1;
}

function(brl_std_loop_while)
{
    Int result = -1;
    while (eval(vm,arg(0).s))
    {
        result = eval(vm,arg(1).s);
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
        result = eval(vm,arg(0).s);
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
    for (int i = 0; i < (Int)arg(0).f; i++)
    {
        result = eval(vm,arg(1).s);
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
    char* _str = str_duplicate(data(str).s);
    for (Int i = 0; i < strlen(_str); i++)
    {
        if (_str[i] == '%')
        {
            if (_str[i+1] == 'd')
            {
                Int value = list_pop(*args);
                char* _value = str_format("%ld", (Int)data(value).f);
                char* _newstr = str_replace(_str, "\%d", _value);
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] == 's')
            {
                Int value = list_pop(*args);
                char* _value = data(value).s;
                char* _newstr = str_replace(_str, "\%s", _value);
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] == 'f')
            {
                Int value = list_pop(*args);
                char* _value = str_format("%f", data(value).f);
                char* _newstr = str_replace(_str, "\%f", _value);
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] == 'p')
            {
                Int value = list_pop(*args);
                char* _value = str_format("%p", data(value).p);
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
    data(result).s = _str;
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

function(brl_std_hash_new)
{
    hash_set(vm, arg(0).s, arg_i(1));
    return -1;
}

function(brl_std_hash_delete)
{
    hash_unset(vm, arg(0).s);
    return -1;
}


// inits
init(std_os)
{
    register_builtin(vm, "read", brl_os_file_read);
    register_builtin(vm, "write", brl_os_file_write);
    register_builtin(vm, "system", brl_os_system);
    register_builtin(vm, "scan", brl_std_io_scan);

    register_builtin(vm, "warn", buxulib_warn);
    register_builtin(vm, "error", buxulib_error);
    register_builtin(vm, "print", buxulib_print);
}

init(std_basics)
{
    register_builtin(vm, "return", brl_std_return);

    register_builtin(vm, "ls", brl_std_io_ls);
    register_builtin(vm, "ls.hash", brl_std_io_ls_hashes);

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

    register_builtin(vm, "type", brl_std_type_get);
    register_builtin(vm, "pun", brl_std_type_set);
    register_builtin(vm, "cast", brl_std_type_cast);
}

init(buxu)
{
    register_string(vm, "bu.default", EMOTICON_DEFAULT);
    register_string(vm, "bu.warning", EMOTICON_WARNING);
    register_string(vm, "bu.error", EMOTICON_ERROR);
    register_string(vm, "bu.idle", EMOTICON_IDLE);
    register_string(vm, "bu.confused", EMOTICON_CONFUSED);
}

// std init presets
init(std)
{
    init_std_os(vm);
    init_buxu(vm);

    init_std_basics(vm);
    init_std_hash(vm);
    init_std_mem(vm);

    init_std_type(vm);

    register_string(vm, "VERSION", VERSION);// version
}