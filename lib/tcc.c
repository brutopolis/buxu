#ifndef ARDUINO

#include "bruter.h"
#include <libtcc.h>

typedef struct {
    void *symbolPointer;
    void *statePointer;
} SymbolAssociation;

typedef Stack(SymbolAssociation) SymbolAssociationList;

IntList *tcc_states;
SymbolAssociationList *tcc_states_temp;

Int new_state(VirtualMachine *vm, IntList *args)
{
    TCCState *tcc = tcc_new();
    if (!tcc) 
    {
        fprintf(stderr, "Erro ao inicializar TCC\n");
        return -1;
    }

    // Configurar saída para memória
    tcc_set_output_type(tcc, TCC_OUTPUT_MEMORY);
    Int result = new_var(vm);
    vm->stack->data[result].pointer = tcc;
    vm->typestack->data[result] = TYPE_OTHER;
    hold_var(vm, result);
    stack_push(*tcc_states, (Int)tcc);
    return result;
}

Int delete_state(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    TCCState *tcc = (TCCState *)vm->stack->data[index].pointer;
    tcc_delete(tcc);
    unuse_var(vm, index);
    index = stack_find(*tcc_states, (Int)tcc);
    if (index >= 0) 
    {
        stack_remove(*tcc_states, index);
    }
    return -1;
}

Int compile_string(VirtualMachine *vm, IntList *args)
{
    Int _state = stack_shift(*args);
    Int _code = stack_shift(*args);
    TCCState *tcc = (TCCState *)vm->stack->data[_state].pointer;
    char *code = vm->stack->data[_code].string;
    if (tcc_compile_string(tcc, code) < 0) 
    {
        fprintf(stderr, "Erro ao compilar código\n");
        return -1;
    }
    return -1;
}

Int br_tcc_relocate(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    TCCState *tcc = (TCCState *)vm->stack->data[index].pointer;
    if (tcc_relocate(tcc) < 0) 
    {
        fprintf(stderr, "Erro ao realocar código\n");
        return -1;
    }
    return -1;
}

Int get_symbol(VirtualMachine *vm, IntList *args)
{
    Int _tcc = stack_shift(*args);
    Int _symbol = stack_shift(*args);
    TCCState *tcc = (TCCState *)vm->stack->data[_tcc].pointer;
    char *symbol = vm->stack->data[_symbol].string;
    void *func = tcc_get_symbol(tcc, symbol);
    if (!func) 
    {
        fprintf(stderr, "Erro ao obter símbolo '%s'\n", symbol);
        return -1;
    }
    Int result = new_var(vm);
    vm->stack->data[result].pointer = func;
    vm->typestack->data[result] = TYPE_BUILTIN;
    hold_var(vm, result);
    return result;
}

Int add_path(VirtualMachine *vm, IntList *args)
{
    Int _tcc = stack_shift(*args);
    Int _path = stack_shift(*args);
    TCCState *tcc = (TCCState *)vm->stack->data[_tcc].pointer;
    char *path = vm->stack->data[_path].string;
    tcc_add_include_path(tcc, path);
    return -1;
}

Int clear_tcc_states(VirtualMachine *vm, IntList *args)
{
    while (tcc_states->size > 0) 
    {
        TCCState *tcc = (TCCState *)stack_shift(*tcc_states);
        tcc_delete(tcc);
    }
    return -1;
}

Int br_tcc_add_symbol(VirtualMachine *vm, IntList *args)
{
    Int _tcc = stack_shift(*args);
    Int _symbol = stack_shift(*args);
    Int _func = stack_shift(*args);
    TCCState *tcc = (TCCState *)vm->stack->data[_tcc].pointer;
    char *symbol = vm->stack->data[_symbol].string;
    void *func = vm->stack->data[_func].pointer;
    tcc_add_symbol(tcc, symbol, func);
    return -1;
}

Int br_c_new_function(VirtualMachine *vm, IntList *args) // a combo of new_state + compile + relocate + get_symbol
{
    TCCState *tcc = tcc_new();
    if (!tcc) 
    {
        fprintf(stderr, "Erro ao inicializar TCC\n");
        return -1;
    }

    // Configurar saída para memória
    tcc_set_output_type(tcc, TCC_OUTPUT_MEMORY);
    Int result = new_var(vm);
    vm->stack->data[result].pointer = tcc;
    vm->typestack->data[result] = TYPE_OTHER;
    hold_var(vm, result);
    stack_push(*tcc_states, (Int)tcc);

    

    // include <bruter.h>
    tcc_add_include_path(tcc, "./include");

    char* _symbol = str_format("_symbol%d", clock() + time(NULL) + vm->stack->size);
    Int _code = stack_shift(*args);
    char *code = str_format("#include <bruter.h>\n Int %s(VirtualMachine *vm, IntList *args) {%s}", _symbol, vm->stack->data[stack_shift(*args)].string);
    if (tcc_compile_string(tcc, code) < 0) 
    {
        fprintf(stderr, "Erro ao compilar código\n");
        return -1;
    }

    if (tcc_relocate(tcc) < 0) 
    {
        fprintf(stderr, "Erro ao realocar código\n");
        return -1;
    }

    void *func = tcc_get_symbol(tcc, _symbol);
    if (!func) 
    {
        fprintf(stderr, "Erro ao obter símbolo '%s'\n", _symbol);
        return -1;
    }

    SymbolAssociation _syass;
    _syass.symbolPointer = func;
    _syass.statePointer = tcc;

    stack_push(*tcc_states_temp, _syass);

    Int result2 = new_var(vm);
    vm->stack->data[result2].pointer = func;
    vm->typestack->data[result2] = TYPE_BUILTIN;
    hold_var(vm, result2);
    free(_symbol);
    free(code);
    return result2;
}

Int br_c_delete_function(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    void *func = vm->stack->data[index].pointer;
    for (Int i = 0; i < tcc_states_temp->size; i++) 
    {
        if (tcc_states_temp->data[i].symbolPointer == func) 
        {
            tcc_delete((TCCState *)tcc_states_temp->data[i].statePointer);
            stack_remove(*tcc_states_temp, i);
            stack_remove(*tcc_states, stack_find(*tcc_states, (Int)tcc_states_temp->data[i].statePointer));
            unuse_var(vm, index);
            return -1;
        }
    }
    return -1;
}

void _terminate_tcc_at_exit_handler()
{
    clear_tcc_states(NULL, NULL);
    stack_free(*tcc_states_temp);
    stack_free(*tcc_states);
}

void init_tcc(VirtualMachine* vm)
{
    tcc_states = make_int_list(vm);
    tcc_states_temp = (SymbolAssociationList*)malloc(sizeof(SymbolAssociationList));
    stack_init(*tcc_states_temp);
    registerBuiltin(vm, "tcc.new", new_state);
    registerBuiltin(vm, "tcc.delete", delete_state);
    registerBuiltin(vm, "tcc.compile", compile_string);
    registerBuiltin(vm, "tcc.relocate", br_tcc_relocate);
    registerBuiltin(vm, "tcc.get.symbol", get_symbol);
    registerBuiltin(vm, "tcc.add.path", add_path);
    registerBuiltin(vm, "tcc.clear", clear_tcc_states);
    registerBuiltin(vm, "tcc.add.symbol", br_tcc_add_symbol);

    registerBuiltin(vm, "c.new", br_c_new_function);
    registerBuiltin(vm, "c.delete", br_c_delete_function);

    atexit(_terminate_tcc_at_exit_handler);
}

#endif