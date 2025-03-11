// this is just a simple and easy abstraction over TinyCC
// a proper libtcc wrapper is planned but not any soon


#ifndef ARDUINO

#include "bruter.h"
#include <libtcc.h>

typedef struct {
    void *symbolPointer;
    void *statePointer;
} SymbolAssociation;

typedef List(SymbolAssociation) SymbolAssociationList;
typedef void (*InitFunction)(VirtualMachine*);

SymbolAssociationList *tcc_states_temp;

const char *bruter_header = "// bruter 0.7.5b\n"
"\n"
"// type agnostic,\n"
"// header only,\n"
"// easy to use,\n"
"// c list implementation\n"
"// by @jardimdanificado\n"
"// c_list.h\n"
"\n"
"// example usage:\n"
"/*\n"
"    List(Int) *list = list_init(List(Int));\n"
"    list_push(*list, 1);\n"
"    list_unshift(*list, 2);\n"
"    list_insert(*list, 1, 3);\n"
"    list_remove(*list, 1);\n"
"    list_pop(*list);\n"
"    list_shift(*list);\n"
"    Int index = list_find(*list, 2);\n"
"    list_reverse(*list);\n"
"    list_set(*list, 0, 4);\n"
"    Int value = list_get(*list, 0);\n"
"    list_half(*list);\n"
"    list_double(*list);\n"
"    list_free(*list);\n"
"*/\n"
"\n"
"\n"
"#ifndef C_LIST_H\n"
"\n"
"#define List(T) struct \\\n"
"{ \\\n"
"    T *data; \\\n"
"    Int size; \\\n"
"    Int capacity; \\\n"
"}\n"
"\n"
"// malloc and initialize a new list\n"
"#define list_init(type) ({ \\\n"
"    type *list = (type*)malloc(sizeof(type)); \\\n"
"    list->data = NULL; \\\n"
"    list->size = 0; \\\n"
"    list->capacity = 0; \\\n"
"    list; \\\n"
"})\n"
"\n"
"// increase the capacity of the stack\n"
"#define list_double(s) do { \\\n"
"    (s).capacity = (s).capacity == 0 ? 1 : (s).capacity * 2; \\\n"
"    (s).data = realloc((s).data, (s).capacity * sizeof(*(s).data)); \\\n"
"} while (0)\n"
"\n"
"// decrease the capacity of the stack\n"
"#define list_half(s) do { \\\n"
"    (s).capacity /= 2; \\\n"
"    (s).data = realloc((s).data, (s).capacity * sizeof(*(s).data)); \\\n"
"    if ((s).size > (s).capacity) { \\\n"
"        (s).size = (s).capacity; \\\n"
"    } \\\n"
"} while (0)\n"
"\n"
"#define list_push(s, v) do { \\\n"
"    if ((s).size == (s).capacity) { \\\n"
"        list_double(s); \\\n"
"    } \\\n"
"    (s).data[(s).size++] = (v); \\\n"
"} while (0)\n"
"\n"
"#define list_unshift(s, v) do { \\\n"
"    if ((s).size == (s).capacity) { \\\n"
"        list_double(s); \\\n"
"    } \\\n"
"    for (Int i = (s).size; i > 0; i--) { \\\n"
"        (s).data[i] = (s).data[i - 1]; \\\n"
"    } \\\n"
"    (s).data[0] = (v); \\\n"
"    (s).size++; \\\n"
"} while (0)\n"
"\n"
"#define list_pop(s) ((s).data[--(s).size])\n"
"\n"
"#define list_shift(s) ({ \\\n"
"    typeof((s).data[0]) ret = (s).data[0]; \\\n"
"    for (Int i = 0; i < (s).size - 1; i++) { \\\n"
"        (s).data[i] = (s).data[i + 1]; \\\n"
"    } \\\n"
"    (s).size--; \\\n"
"    ret; \\\n"
"})\n"
"\n"
"#define list_free(s) ({free((s).data);free(&s);})\n"
"\n"
"//swap elements from index i1 to index i2\n"
"#define list_swap(s, i1, i2) do { \\\n"
"    typeof((s).data[i1]) tmp = (s).data[i1]; \\\n"
"    (s).data[i1] = (s).data[i2]; \\\n"
"    (s).data[i2] = tmp; \\\n"
"} while (0)\n"
"\n"
"//insert element v at index i\n"
"#define list_insert(s, i, v) do { \\\n"
"    if ((s).size == (s).capacity) { \\\n"
"        list_double(s); \\\n"
"    } \\\n"
"    for (Int j = (s).size; j > i; j--) { \\\n"
"        (s).data[j] = (s).data[j - 1]; \\\n"
"    } \\\n"
"    (s).data[i] = (v); \\\n"
"    (s).size++; \\\n"
"} while (0)\n"
"\n"
"//remove element at index i and return it\n"
"#define list_remove(s, i) ({ \\\n"
"    typeof((s).data[i]) ret = (s).data[i]; \\\n"
"    for (Int j = i; j < (s).size - 1; j++) { \\\n"
"        (s).data[j] = (s).data[j + 1]; \\\n"
"    } \\\n"
"    (s).size--; \\\n"
"    ret; \\\n"
"})\n"
"\n"
"//same as remove but does a swap and pop, faster but the order of the elements will change\n"
"#define list_fast_remove(s, i) ({ \\\n"
"    typeof((s).data[i]) ret = (s).data[i]; \\\n"
"    list_swap(s, i, (s).size - 1); \\\n"
"    list_pop(s); \\\n"
"    ret; \\\n"
"})\n"
"\n"
"#define list_find(s, v) ({ \\\n"
"    Int i = 0; \\\n"
"    while (i < (s).size && (s).data[i] != (v)) { \\\n"
"        i++; \\\n"
"    } \\\n"
"    i == (s).size ? -1 : i; \\\n"
"})\n"
"\n"
"#define list_reverse(s) do { \\\n"
"    for (Int i = 0; i < (s).size / 2; i++) { \\\n"
"        list_swap((s), i, (s).size - i - 1); \\\n"
"    } \\\n"
"} while (0)\n"
"\n"
"#define list_set(s, i, v) ((s).data[i] = (v))\n"
"\n"
"#define list_get(s, i) ((s).data[i])\n"
"\n"
"#endif"
"\n"
"#ifndef BRUTER_H\n"
"#define BRUTER_H 1\n"
"\n"
"#include <stdio.h>\n"
"#include <stdlib.h>\n"
"#include <string.h>\n"
"#include <math.h>\n"
"#include <stdarg.h>\n"
"#include <time.h>\n"
"#include <unistd.h>\n"
"#include <ctype.h>\n"
"\n"
"#ifndef ARDUINO\n"
"    #include \"dlfcn.h\"\n"
"#endif\n"
"\n"
"#define VERSION \"0.7.7a\"\n"
"\n"
"#define TYPE_ANY 0\n"
"#define TYPE_NUMBER 1\n"
"#define TYPE_STRING 2\n"
"#define TYPE_LIST 3\n"
"\n"
"// we use Int and Float instead of int and float because we need to use always the pointer size for any type that might share the fundamental union type;\n"
"// bruter use a union as universal type, and bruter is able to manipulate and use pointers direcly so we need to use the pointer size;\n"
"#if __SIZEOF_POINTER__ == 8\n"
"    #define Int long\n"
"    #define Float double\n"
"#else\n"
"    #define Int int\n"
"    #define Float float\n"
"#endif\n"
"\n"
"\n"
"#ifndef NULL\n"
"#define NULL 0\n"
"#endif\n"
"\n"
"//Value\n"
"typedef union \n"
"{\n"
"    Float number;\n"
"    Int integer;\n"
"    char* string;\n"
"    void* pointer;\n"
"    char byte[sizeof(Float)];\n"
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
"typedef List(Value) ValueList;\n"
"typedef List(Hash) HashList;\n"
"typedef List(char*) StringList;\n"
"typedef List(Int) IntList;\n"
"typedef List(char) CharList;\n"
"\n"
"typedef List(IntList) IntListList;\n"
"\n"
"\n"
"typedef struct\n"
"{\n"
"    ValueList *stack;\n"
"    CharList *typestack;\n"
"    HashList *hashes;\n"
"    IntList *unused;\n"
"} VirtualMachine;\n"
"\n"
"//Function\n"
"typedef Int (*Function)(VirtualMachine*, IntList*);\n"
"typedef void (*InitFunction)(VirtualMachine*);\n"
"\n"
"//String\n"
"char* str_duplicate(const char *str);\n"
"char* str_nduplicate(const char *str, Int n);\n"
"char* str_format(const char *fmt, ...);\n"
"char* str_sub(const char *str, Int start, Int end);\n"
"char* str_concat(const char *str1, const char *str2);\n"
"Int str_find(const char *str, const char *substr);\n"
"char* str_replace(const char *str, const char *substr, const char *replacement);\n"
"char* str_replace_all(const char *str, const char *substr, const char *replacement);\n"
"\n"
"StringList* str_split(char *str, char *delim);\n"
"StringList* str_split_char(char *str, char delim);\n"
"StringList* special_space_split(char *str);\n"
"StringList* special_split(char *str, char delim);\n"
"\n"
"#define is_true(value, __type) (__type == value.integer == 0 ? 0 : 1)\n"
"\n"
"// #define is_space(c) (c == ' ' || c == '\\t' || c == '\\n' || c == '\\r' || c == '\\v' || c == '\\f')\n"
"\n"
"// variable\n"
"\n"
"VirtualMachine* make_vm();\n"
"void free_vm(VirtualMachine *vm);\n"
"void unuse_var(VirtualMachine *vm, Int index);\n"
"\n"
"Int new_number(VirtualMachine *vm, Float number);\n"
"Int new_string(VirtualMachine *vm, char *str);\n"
"Int new_builtin(VirtualMachine *vm, Function function);\n"
"Int new_var(VirtualMachine *vm);\n"
"Int new_list(VirtualMachine *vm);\n"
"\n"
"Value value_duplicate(Value value, char type);\n"
"\n"
"Int register_var(VirtualMachine *vm, char* varname);\n"
"Int register_string(VirtualMachine *vm, char* varname, char* string);\n"
"Int register_number(VirtualMachine *vm, char* varname, Float number);\n"
"Int register_builtin(VirtualMachine *vm, char* varname, Function function);\n"
"Int register_list(VirtualMachine *vm, char* varname);\n"
"\n"
"Int hash_find(VirtualMachine *vm, char *key);\n"
"void hash_set(VirtualMachine *vm, char *key, Int index);\n"
"void hash_unset(VirtualMachine *vm, char *key);\n"
"\n"
"// eval\n"
"Int eval(VirtualMachine *vm, char *cmd);\n"
"\n"
"void print_element(VirtualMachine *vm, Int index);\n"
"\n"
"\n"
"// macros\n"
"#define data(index) (vm->stack->data[index])\n"
"#define data_t(index) (vm->typestack->data[index])\n"
"\n"
"#define hash(index) (vm->hashes->data[index])\n"
"\n"
"#define arg(index) (vm->stack->data[args->data[index]])\n"
"#define arg_i(index) (args->data[index])\n"
"#define arg_t(index) (vm->typestack->data[args->data[index]])\n"
"\n"
"#define function(name) Int name(VirtualMachine *vm, IntList *args)\n"
"#define init(name) void init_##name(VirtualMachine *vm)\n"
"\n"
"Int interpret_args(VirtualMachine *vm, IntList *args);\n"
"Int interpret(VirtualMachine *vm, char *cmd);\n"
"\n"
"// functions\n"
"IntList* parse(void* _vm, char* cmd);\n"
"\n"
"// stringify function\n"
"char* list_stringify(VirtualMachine* vm, IntList *list);\n"
"\n"
"#ifndef ARDUINO\n"
"\n"
"char* readfile(char *filename);\n"
"void writefile(char *filename, char *content);\n"
"\n"
"#endif\n"
"\n"
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
        str_split,
        str_split_char,
        special_space_split,
        special_split,
        make_vm,
        free_vm,
        unuse_var,
        new_number,
        new_string,
        new_builtin,
        new_var,
        new_list,
        value_duplicate,
        register_var,
        register_string,
        register_number,
        register_builtin,
        register_list,
        hash_find,
        hash_set,
        hash_unset,
        eval,
        print_element,
        interpret_args,
        interpret,
        parse,
        list_stringify,
#ifndef ARDUINO
        readfile,
        writefile,
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
        "str_split",
        "str_split_char",
        "special_space_split",
        "special_split",
        "make_vm",
        "free_vm",
        "unuse_var",
        "new_number",
        "new_string",
        "new_builtin",
        "new_var",
        "new_list",
        "value_duplicate",
        "register_var",
        "register_string",
        "register_number",
        "register_builtin",
        "register_list",
        "hash_find",
        "hash_set",
        "hash_unset",
        "eval",
        "print_element",
        "interpret_args",
        "interpret",
        "parse",
        "list_stringify",
#ifndef ARDUINO
        "readfile",
        "writefile",
#endif
    };

    for (Int i = 0; i < sizeof(core_funcs) / sizeof(core_funcs[0]); i++) {
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

    Int result = new_var(vm);
    data(result).pointer = tcc;
    data_t(result) = TYPE_ANY;

    char *code = str_format("%s\n%s", bruter_header, arg(0).string);
    
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
        if (arg_t(i) != TYPE_STRING) 
        {
            fprintf(stderr, "expected string, got %d\n", arg_t(i));
            return -1;
        }
        void *func = tcc_get_symbol(tcc, arg(i).string);
        if (!func) 
        {
            fprintf(stderr, "could not obtain '%s' symbol\n", arg(i).string);
            return -1;
        }

        SymbolAssociation _syass;
        _syass.symbolPointer = func;
        _syass.statePointer = tcc;

        list_push(*tcc_states_temp, _syass);

        Int result2 = new_var(vm);
        data(result2).pointer = func;
        data_t(result2) = TYPE_ANY;
        hash_set(vm, arg(i).string, result2);
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

    register_builtin(vm, "dycc.clear", brl_tcc_clear_states);
    register_builtin(vm, "dycc.compile", brl_tcc_c_new_function);

    atexit(_terminate_tcc_at_exit_handler);
}

#endif