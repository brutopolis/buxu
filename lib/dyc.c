#ifndef ARDUINO

#include "bruter.h"
#include <libtcc.h>

typedef struct {
    void *symbolPointer;
    void *statePointer;
} SymbolAssociation;

typedef Stack(SymbolAssociation) SymbolAssociationList;
typedef void (*InitFunction)(VirtualMachine*);

SymbolAssociationList *tcc_states_temp;
const char *bruter_header = "#ifndef BRUTER_H\n"
"#define BRUTER_H 1\n"
"\n"
"#include <stdio.h>\n"
"#include <stdlib.h>\n"
"#include <string.h>\n"
"#include <math.h>\n"
"#include <stdarg.h>\n"
"#include <time.h>\n"
"\n"
"#define VERSION \""
VERSION
"\"\n"
"\n"
"#define TYPE_NIL 0\n"
"#define TYPE_NUMBER 1\n"
"#define TYPE_STRING 2\n"
"#define TYPE_LIST 3\n"
"#define TYPE_BUILTIN 4\n"
"#define TYPE_RAW 5\n"
"#define TYPE_INTEGER 6\n"
"#define TYPE_OTHER 8\n"
"\n"
"\n"
"#if __SIZEOF_POINTER__ == 8\n"
"    #define Int long\n"
"    #define Float double\n"
"#else\n"
"    #define Int int\n"
"    #define Float float\n"
"#endif\n"
"\n"
"#ifndef NULL\n"
"#define NULL 0\n"
"#endif\n"
"\n"
"\n"
"//stack implementation\n"
"#define Stack(T) struct \\\n"
"{ \\\n"
"    T *data; \\\n"
"    Int size; \\\n"
"    Int capacity; \\\n"
"}\n"
"\n"
"#define stack_init(s) do \\\n"
"{ \\\n"
"    (s).data = NULL; \\\n"
"    (s).size = 0; \\\n"
"    (s).capacity = 0;\\\n"
"} while (0)\n"
"\n"
"#define stack_push(s, v) do { \\\n"
"    if ((s).size == (s).capacity) { \\\n"
"        (s).capacity = (s).capacity == 0 ? 1 : (s).capacity * 2; \\\n"
"        (s).data = realloc((s).data, (s).capacity * sizeof(*(s).data)); \\\n"
"    } \\\n"
"    (s).data[(s).size++] = (v); \\\n"
"} while (0)\n"
"\n"
"#define stack_unshift(s, v) do { \\\n"
"    if ((s).size == (s).capacity) { \\\n"
"        (s).capacity = (s).capacity == 0 ? 1 : (s).capacity * 2; \\\n"
"        (s).data = realloc((s).data, (s).capacity * sizeof(*(s).data)); \\\n"
"    } \\\n"
"    for (Int i = (s).size; i > 0; i--) { \\\n"
"        (s).data[i] = (s).data[i - 1]; \\\n"
"    } \\\n"
"    (s).data[0] = (v); \\\n"
"    (s).size++; \\\n"
"} while (0)\n"
"\n"
"#define stack_pop(s) ((s).data[--(s).size])\n"
"\n"
"#define stack_shift(s) ({ \\\n"
"    typeof((s).data[0]) ret = (s).data[0]; \\\n"
"    for (Int i = 0; i < (s).size - 1; i++) { \\\n"
"        (s).data[i] = (s).data[i + 1]; \\\n"
"    } \\\n"
"    (s).size--; \\\n"
"    ret; \\\n"
"})\n"
"\n"
"#define stack_free(s) ({free((s).data);free(&s);})\n"
"\n"
"//swap elements from index i1 to index i2\n"
"#define stack_move(s, i1, i2) do { \\\n"
"    typeof((s).data[i1]) tmp = (s).data[i1]; \\\n"
"    (s).data[i1] = (s).data[i2]; \\\n"
"    (s).data[i2] = tmp; \\\n"
"} while (0)\n"
"\n"
"//insert element v at index i\n"
"#define stack_insert(s, i, v) do { \\\n"
"    if ((s).size == (s).capacity) { \\\n"
"        (s).capacity = (s).capacity == 0 ? 1 : (s).capacity * 2; \\\n"
"        (s).data = realloc((s).data, (s).capacity * sizeof(*(s).data)); \\\n"
"    } \\\n"
"    for (Int j = (s).size; j > i; j--) { \\\n"
"        (s).data[j] = (s).data[j - 1]; \\\n"
"    } \\\n"
"    (s).data[i] = (v); \\\n"
"    (s).size++; \\\n"
"} while (0)\n"
"\n"
"//remove element at index i and return it\n"
"#define stack_remove(s, i) ({ \\\n"
"    typeof((s).data[i]) ret = (s).data[i]; \\\n"
"    for (Int j = i; j < (s).size - 1; j++) { \\\n"
"        (s).data[j] = (s).data[j + 1]; \\\n"
"    } \\\n"
"    (s).size--; \\\n"
"    ret; \\\n"
"})\n"
"\n"
"#define stack_find(s, v) ({ \\\n"
"    Int i = 0; \\\n"
"    while (i < (s).size && (s).data[i] != (v)) { \\\n"
"        i++; \\\n"
"    } \\\n"
"    i == (s).size ? -1 : i; \\\n"
"})\n"
"\n"
"//Value\n"
"typedef union \n"
"{\n"
"    Float number;\n"
"    Int integer;\n"
"    char* string;\n"
"    void* pointer;\n"
"    unsigned char byte[sizeof(Float)];\n"
"} Value;\n"
"\n"
"//Hash\n"
"typedef struct\n"
"{\n"
"    char *key;\n"
"    Int index;\n"
"} Hash;\n"
"\n"
"//List\n"
"typedef Stack(Value) ValueList;\n"
"typedef Stack(Hash) HashList;\n"
"typedef Stack(char*) StringList;\n"
"typedef Stack(Int) IntList;\n"
"typedef Stack(char) CharList;\n"
"\n"
"typedef struct\n"
"{\n"
"    ValueList *stack;\n"
"    CharList *typestack;\n"
"    HashList *hashes;\n"
"    IntList *unused;\n"
"    IntList *temp;\n"
"} VirtualMachine;\n"
"\n"
"//Function\n"
"typedef Int (*Function)(VirtualMachine*, IntList*);\n"
"\n"
"//String\n"
"extern char* str_duplicate(const char *str);\n"
"extern char* str_nduplicate(const char *str, Int n);\n"
"extern char* str_format(const char *fmt, ...);\n"
"extern char* str_sub(const char *str, Int start, Int end);\n"
"extern char* str_concat(const char *str1, const char *str2);\n"
"extern Int str_find(const char *str, const char *substr);\n"
"extern char* str_replace(const char *str, const char *substr, const char *replacement);\n"
"extern char* str_replace_all(const char *str, const char *substr, const char *replacement);\n"
"\n"
"extern StringList* split_string(char *str, char *delim);\n"
"extern StringList* split_string_by_char(char *str, char delim);\n"
"extern StringList* special_space_split(char *str);\n"
"extern StringList* special_split(char *str, char delim);\n"
"\n"
"#define is_true(value, __type) (__type == TYPE_NUMBER ? (round(value.number) != 0) : (__type == TYPE_NIL ? 0 : 1))\n"
"\n"
"#define is_space(c) (c == ' ' || c == '\\t' || c == '\\n' || c == '\\r' || c == '\\v' || c == '\\f')\n"
"\n"
"// variable\n"
"\n"
"extern ValueList* make_value_list();\n"
"extern IntList* make_int_list();\n"
"extern StringList* make_string_list();\n"
"extern CharList* make_char_list();\n"
"\n"
"extern VirtualMachine* make_vm();\n"
"extern void free_vm(VirtualMachine *vm);\n"
"extern void free_var(VirtualMachine *vm, Int index);\n"
"extern void unuse_var(VirtualMachine *vm, Int index);\n"
"extern void use_var(VirtualMachine *vm, Int index);\n"
"\n"
"extern Int new_number(VirtualMachine *vm, Float number);\n"
"extern Int new_string(VirtualMachine *vm, char *str);\n"
"extern Int new_builtin(VirtualMachine *vm, Function function);\n"
"extern Int new_var(VirtualMachine *vm);\n"
"extern Int new_list(VirtualMachine *vm);\n"
"\n"
"extern void hold_var(VirtualMachine *vm, Int index);\n"
"extern void unhold_var(VirtualMachine *vm, Int index);\n"
"\n"
"extern Value value_duplicate(Value value, char type);\n"
"\n"
"extern Int spawn_var(VirtualMachine *vm, char* varname);\n"
"extern Int spawn_string(VirtualMachine *vm, char* varname, char* string);\n"
"extern Int spawn_number(VirtualMachine *vm, char* varname, Float number);\n"
"extern Int spawn_builtin(VirtualMachine *vm, char* varname, Function function);\n"
"extern Int spawn_list(VirtualMachine *vm, char* varname);\n"
"\n"
"\n"
"extern void register_builtin(VirtualMachine *vm, char* name, Function function);\n"
"extern void register_number(VirtualMachine *vm, char* name, Float number);\n"
"extern void register_string(VirtualMachine *vm, char* name, char* string);\n"
"extern void register_list(VirtualMachine *vm, char* name);\n"
"\n"
"\n"
"extern Int hash_find(VirtualMachine *vm, char *key);\n"
"extern void hash_set(VirtualMachine *vm, char *key, Int index);\n"
"extern void hash_unset(VirtualMachine *vm, char *key);\n"
"\n"
"// eval\n"
"extern Int eval(VirtualMachine *vm, char *cmd);\n"
"extern Int interpret(VirtualMachine *vm, char* cmd);\n"
"extern IntList* parse(VirtualMachine *vm, char *cmd);\n"
"\n"
"extern void collect_garbage(VirtualMachine *vm);\n"
"\n"
"#ifndef ARDUINO\n"
"\n"
"extern char* readfile(char *filename);\n"
"extern void writefile(char *filename, char *content);\n"
"extern Int repl(VirtualMachine *vm);\n"
"extern void print_element(VirtualMachine *vm, Int index);\n"
"\n"
"#endif\n"
"\n"
"#endif";


void add_common_symbols(TCCState *tcc)
{
    const void *core_funcs[] = {
        str_duplicate,
        str_nduplicate,
        str_format,
        str_sub,
        str_concat,
        str_find,
        str_replace,
        str_replace_all,
        split_string,
        split_string_by_char,
        special_space_split,
        special_split,
        make_value_list,
        make_int_list,
        make_string_list,
        make_char_list,
        make_vm,
        free_vm,
        free_var,
        unuse_var,
        use_var,
        new_number,
        new_string,
        new_builtin,
        new_var,
        new_list,
        hold_var,
        unhold_var,
        value_duplicate,
        spawn_var,
        spawn_string,
        spawn_number,
        spawn_builtin,
        spawn_list,
        register_builtin,
        register_number,
        register_string,
        register_list,
        hash_find,
        hash_set,
        hash_unset,
        eval,
        interpret,
        parse,
        collect_garbage,
        #ifndef ARDUINO
        readfile,
        writefile,
        repl,
        print_element
        #endif
    };

    const char *core_names[] = {
        "str_duplicate",
        "str_nduplicate",
        "str_format",
        "str_sub",
        "str_concat",
        "str_find",
        "str_replace",
        "str_replace_all",
        "split_string",
        "split_string_by_char",
        "special_space_split",
        "special_split",
        "make_value_list",
        "make_int_list",
        "make_string_list",
        "make_char_list",
        "make_vm",
        "free_vm",
        "free_var",
        "unuse_var",
        "use_var",
        "new_number",
        "new_string",
        "new_builtin",
        "new_var",
        "new_list",
        "hold_var",
        "unhold_var",
        "value_duplicate",
        "spawn_var",
        "spawn_string",
        "spawn_number",
        "spawn_builtin",
        "spawn_list",
        "register_builtin",
        "register_number",
        "register_string",
        "register_list",
        "hash_find",
        "hash_set",
        "hash_unset",
        "eval",
        "interpret",
        "parse",
        "collect_garbage",
    #ifndef ARDUINO
        "readfile",
        "writefile",
        "repl",
        "print_element"
    #endif
    };

    for (Int i = 0; i < sizeof(core_funcs) / sizeof(core_funcs[0]); i++) 
    {
        //printf("Adding symbol %s\n", core_names[i]);
        tcc_add_symbol(tcc, core_names[i], core_funcs[i]);
    }

}

Int brl_tcc_clear_states(VirtualMachine *vm, IntList *args)
{
    while (tcc_states_temp->size > 0) 
    {
        tcc_delete((TCCState *)(stack_shift(*tcc_states_temp)).statePointer);
    }
    return -1;
}

Int brl_tcc_c_new_function(VirtualMachine *vm, IntList *args) // a combo of new_state + compile + relocate + get_symbol
{
    TCCState *tcc = tcc_new();
    if (!tcc) 
    {
        fprintf(stderr, "error initializing TCC\n");
        return -1;
    }

    // Configurar saída para memória
    tcc_set_output_type(tcc, TCC_OUTPUT_MEMORY);

    Int result = new_var(vm);
    vm->stack->data[result].pointer = tcc;
    vm->typestack->data[result] = TYPE_OTHER;
    hold_var(vm, result);


    char* _symbol = str_format("_symbol%d", clock() + time(NULL) + vm->stack->size);
    Int _code = stack_shift(*args);
    char *code;

    if (args->size > 1)
    {
        code = str_format("%s\n\n%s\n\nInt %s(VirtualMachine *vm, IntList *args) {%s}", bruter_header, vm->stack->data[stack_shift(*args)].string, _symbol, vm->stack->data[stack_shift(*args)].string);
    }
    else
    {
        code = str_format("%s\n\nInt %s(VirtualMachine *vm, IntList *args) {%s}", bruter_header, _symbol, vm->stack->data[_code].string);
    }
    
    add_common_symbols(tcc);

    if (tcc_compile_string(tcc, code) < 0) 
    {
        fprintf(stderr, "could not compilar string\n");
        return -1;
    }

    if (tcc_relocate(tcc) < 0) 
    {
        fprintf(stderr, "could not relocate code\n");
        return -1;
    }

    void *func = tcc_get_symbol(tcc, _symbol);
    if (!func) 
    {
        fprintf(stderr, "could not obtain '%s' symbol\n", _symbol);
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

Int brl_tcc_c_delete_function(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    void *func = vm->stack->data[index].pointer;
    for (Int i = 0; i < tcc_states_temp->size; i++) 
    {
        if (tcc_states_temp->data[i].symbolPointer == func) 
        {
            tcc_delete((TCCState *)tcc_states_temp->data[i].statePointer);
            stack_remove(*tcc_states_temp, i);
            unuse_var(vm, index);
            return -1;
        }
    }
    return -1;
}

Int brl_tcc_c_include(VirtualMachine *vm, IntList *args)
{
    Int _filepath_id = stack_shift(*args);
    char* _filepath = vm->stack->data[_filepath_id].string;
    char* _filename_without_extension_and_path = str_sub(_filepath, str_find(_filepath, "/") + 1, str_find(_filepath, "."));
    char* _code = readfile(_filepath);
    char* ___special_header = str_format("%s\n\nvoid _libr_%s_handler() {};", bruter_header, _filename_without_extension_and_path);
    char* __code = str_replace(_code, "#include \"bruter.h\"", ___special_header);
    char* _symbol = str_format("init_%s", _filename_without_extension_and_path);
    char* _dummy_symbol = str_format("_libr_%s_handler", _filename_without_extension_and_path);
    printf("Including %s\n", _filepath);
    TCCState *tcc = tcc_new();
    if (!tcc) 
    {
        fprintf(stderr, "error initializing TCC\n");
        return -1;
    }

    // Configurar saída para memória
    tcc_set_output_type(tcc, TCC_OUTPUT_MEMORY);

    add_common_symbols(tcc);

    tcc_compile_string(tcc, __code);
    
    tcc_relocate(tcc);

    void *_init_func = tcc_get_symbol(tcc, _symbol);
    void *_dummy_func = tcc_get_symbol(tcc, _dummy_symbol);
    
    SymbolAssociation _syass;
    _syass.symbolPointer = _dummy_func;
    _syass.statePointer = tcc;

    stack_push(*tcc_states_temp, _syass);
    
    Int result = new_var(vm);
    vm->stack->data[result].pointer = _dummy_func;
    vm->typestack->data[result] = TYPE_OTHER;
    hold_var(vm, result);

    //declare function type
    ((InitFunction)_init_func)(vm); 

    free(_filename_without_extension_and_path);
    free(_code);
    free(___special_header);
    free(__code);
    free(_symbol);
    free(_dummy_symbol);

    return result;
}

void _terminate_tcc_at_exit_handler()
{
    brl_tcc_clear_states(NULL, NULL);
    stack_free(*tcc_states_temp);
}

void init_dyc(VirtualMachine* vm)
{
    tcc_states_temp = (SymbolAssociationList*)malloc(sizeof(SymbolAssociationList));
    stack_init(*tcc_states_temp);

    register_builtin(vm, "dyc.clear", brl_tcc_clear_states);
    register_builtin(vm, "dyc.new", brl_tcc_c_new_function);
    register_builtin(vm, "dyc.delete", brl_tcc_c_delete_function);
    register_builtin(vm, "dyc.include", brl_tcc_c_include);

    atexit(_terminate_tcc_at_exit_handler);
}

#endif