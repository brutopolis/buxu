// this is just a simple and easy abstraction over TinyCC
// a proper libtcc wrapper is planned but not any soon


#ifndef ARDUINO

#include "buxu.h"
#include <libtcc.h>

typedef struct {
    void *symbolPointer;
    void *statePointer;
} SymbolAssociation;

typedef List(SymbolAssociation) SymbolAssociationList;
typedef void (*InitFunction)(VirtualMachine*);

SymbolAssociationList *tcc_states_temp;

const char* bruter_header = "#include \"buxu.h\"\n";

void add_common_symbols(TCCState *tcc)
{
    const void *core_funcs[] = {
        str_format,
        special_space_split,
        special_split,
        make_vm,
        free_vm,
        new_var,
        hash_find,
        hash_set,
        hash_unset,
        eval,
        interpret,
        parse
    };

    const char *core_names[] = {
        "str_format",
        "special_space_split",
        "special_split",
        "make_vm",
        "free_vm",
        "new_var",
        "hash_find",
        "hash_set",
        "hash_unset",
        "eval",
        "interpret",
        "parse"
    };

    for (Int i = 0; i < sizeof(core_funcs) / sizeof(core_funcs[0]); i++) 
    {
        tcc_add_symbol(tcc, core_names[i], core_funcs[i]);
    }
}


function(brl_tcc_clear_states)
{
    while (tcc_states_temp->size > 0) 
    {
        tcc_delete((TCCState *)(list_shift(*tcc_states_temp)).statePointer);
    }
    return -1;
}

function(brl_tcc_c_new_function) // a combo of new_state + compile + relocate + get_symbol
{
    TCCState *tcc = tcc_new();
    if (!tcc) 
    {
        fprintf(stderr, "error initializing TCC\n");
        return -1;
    }

    // Configurar saída para memória
    tcc_set_output_type(tcc, TCC_OUTPUT_MEMORY);

    Int result = new_var(vm, TYPE_DATA, pun(tcc, p, i));

    char *code = str_format("%s\n%s", bruter_header, arg(0).s);
    
    add_common_symbols(tcc);

    if (tcc_compile_string(tcc, code) < 0) 
    {
        fprintf(stderr, "could not compile string\n");
        return -1;
    }

    if (tcc_relocate(tcc) < 0) 
    {
        fprintf(stderr, "could not relocate code\n");
        return -1;
    }

    for (Int i = 1; i < args->size; i++) 
    {
        void *func = tcc_get_symbol(tcc, arg(i).s);
        if (!func) 
        {
            fprintf(stderr, "could not obtain '%s' symbol\n", arg(i).s);
            return -1;
        }

        SymbolAssociation _syass;
        _syass.symbolPointer = func;
        _syass.statePointer = tcc;

        list_push(*tcc_states_temp, _syass);

        register_var(vm, arg(i).s, TYPE_DATA, pun(func, p, i));
    }
    free(code);
    return -1;
}

void _terminate_tcc_at_exit_handler()
{
    brl_tcc_clear_states(NULL, NULL);
    list_free(*tcc_states_temp);
}

init(dyc)
{
    tcc_states_temp = list_init(SymbolAssociationList);

    add_function(vm, "dycc.clear", brl_tcc_clear_states);
    add_function(vm, "dycc.compile", brl_tcc_c_new_function);

    atexit(_terminate_tcc_at_exit_handler);
}

#endif