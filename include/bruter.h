#ifndef BRUTER_H
#define BRUTER_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <time.h>

#define VERSION "0.6.7a"

#define TYPE_NIL 0
#define TYPE_NUMBER 1
#define TYPE_STRING 2
#define TYPE_LIST 3
#define TYPE_BUILTIN 4
#define TYPE_RAW 5
#define TYPE_INTEGER 6
#define TYPE_OTHER 8


#if __SIZEOF_POINTER__ == 8
    #define Int long
    #define Float double
#else
    #define Int int
    #define Float float
#endif

#ifndef NULL
#define NULL 0
#endif


//stack implementation
#define Stack(T) struct \
{ \
    T *data; \
    Int size; \
    Int capacity; \
}

#define stack_init(s) do \
{ \
    (s).data = NULL; \
    (s).size = 0; \
    (s).capacity = 0;\
} while (0)

#define stack_push(s, v) do { \
    if ((s).size == (s).capacity) { \
        (s).capacity = (s).capacity == 0 ? 1 : (s).capacity * 2; \
        (s).data = realloc((s).data, (s).capacity * sizeof(*(s).data)); \
    } \
    (s).data[(s).size++] = (v); \
} while (0)

#define stack_unshift(s, v) do { \
    if ((s).size == (s).capacity) { \
        (s).capacity = (s).capacity == 0 ? 1 : (s).capacity * 2; \
        (s).data = realloc((s).data, (s).capacity * sizeof(*(s).data)); \
    } \
    for (Int i = (s).size; i > 0; i--) { \
        (s).data[i] = (s).data[i - 1]; \
    } \
    (s).data[0] = (v); \
    (s).size++; \
} while (0)

#define stack_pop(s) ((s).data[--(s).size])

#define stack_shift(s) ({ \
    typeof((s).data[0]) ret = (s).data[0]; \
    for (Int i = 0; i < (s).size - 1; i++) { \
        (s).data[i] = (s).data[i + 1]; \
    } \
    (s).size--; \
    ret; \
})

#define stack_free(s) ({free((s).data);free(&s);})

//swap elements from index i1 to index i2
#define stack_move(s, i1, i2) do { \
    typeof((s).data[i1]) tmp = (s).data[i1]; \
    (s).data[i1] = (s).data[i2]; \
    (s).data[i2] = tmp; \
} while (0)

//insert element v at index i
#define stack_insert(s, i, v) do { \
    if ((s).size == (s).capacity) { \
        (s).capacity = (s).capacity == 0 ? 1 : (s).capacity * 2; \
        (s).data = realloc((s).data, (s).capacity * sizeof(*(s).data)); \
    } \
    for (Int j = (s).size; j > i; j--) { \
        (s).data[j] = (s).data[j - 1]; \
    } \
    (s).data[i] = (v); \
    (s).size++; \
} while (0)

//remove element at index i and return it
#define stack_remove(s, i) ({ \
    typeof((s).data[i]) ret = (s).data[i]; \
    for (Int j = i; j < (s).size - 1; j++) { \
        (s).data[j] = (s).data[j + 1]; \
    } \
    (s).size--; \
    ret; \
})

#define stack_find(s, v) ({ \
    Int i = 0; \
    while (i < (s).size && (s).data[i] != (v)) { \
        i++; \
    } \
    i == (s).size ? -1 : i; \
})

//Value
typedef union 
{
    Float number;
    Int integer;
    char* string;
    void* pointer;
    unsigned char byte[sizeof(Float)];
} Value;

//Hash
typedef struct
{
    char *key;
    Int index;
} Hash;

//List
typedef Stack(Value) ValueList;
typedef Stack(Hash) HashList;
typedef Stack(char*) StringList;
typedef Stack(Int) IntList;
typedef Stack(char) CharList;

typedef struct
{
    ValueList *stack;
    CharList *typestack;
    HashList *hashes;
    IntList *unused;
    IntList *temp;
} VirtualMachine;

//Function
typedef Int (*Function)(VirtualMachine*, IntList*);

//String
extern char* str_duplicate(const char *str);
extern char* str_nduplicate(const char *str, Int n);
extern char* str_format(const char *fmt, ...);
extern char* str_sub(const char *str, Int start, Int end);
extern char* str_concat(const char *str1, const char *str2);
extern Int str_find(const char *str, const char *substr);
extern char* str_replace(const char *str, const char *substr, const char *replacement);
extern char* str_replace_all(const char *str, const char *substr, const char *replacement);

extern StringList* split_string(char *str, char *delim);
extern StringList* split_string_by_char(char *str, char delim);
extern StringList* special_space_split(char *str);
extern StringList* special_split(char *str, char delim);

#define is_true(value, __type) (__type == TYPE_NUMBER ? (round(value.number) != 0) : (__type == TYPE_NIL ? 0 : 1))

#define is_space(c) (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f')

// variable

extern ValueList* make_value_list();
extern IntList* make_int_list();
extern StringList* make_string_list();
extern CharList* make_char_list();

extern VirtualMachine* make_vm();
extern void free_vm(VirtualMachine *vm);
extern void free_var(VirtualMachine *vm, Int index);
extern void unuse_var(VirtualMachine *vm, Int index);
extern void use_var(VirtualMachine *vm, Int index);

extern Int new_number(VirtualMachine *vm, Float number);
extern Int new_string(VirtualMachine *vm, char *str);
extern Int new_builtin(VirtualMachine *vm, Function function);
extern Int new_var(VirtualMachine *vm);
extern Int new_list(VirtualMachine *vm);

extern void hold_var(VirtualMachine *vm, Int index);
extern void unhold_var(VirtualMachine *vm, Int index);

extern Value value_duplicate(Value value, char type);

extern Int spawn_var(VirtualMachine *vm, char* varname);
extern Int spawn_string(VirtualMachine *vm, char* varname, char* string);
extern Int spawn_number(VirtualMachine *vm, char* varname, Float number);
extern Int spawn_builtin(VirtualMachine *vm, char* varname, Function function);
extern Int spawn_list(VirtualMachine *vm, char* varname);


extern void register_builtin(VirtualMachine *vm, char* name, Function function);
extern void register_number(VirtualMachine *vm, char* name, Float number);
extern void register_string(VirtualMachine *vm, char* name, char* string);
extern void register_list(VirtualMachine *vm, char* name);


extern Int hash_find(VirtualMachine *vm, char *key);
extern void hash_set(VirtualMachine *vm, char *key, Int index);
extern void hash_unset(VirtualMachine *vm, char *key);

// eval
extern Int eval(VirtualMachine *vm, char *cmd);
extern Int interpret(VirtualMachine *vm, char* cmd);
extern IntList* parse(VirtualMachine *vm, char *cmd);

extern void collect_garbage(VirtualMachine *vm);

// <libraries header>

#ifndef ARDUINO

extern char* readfile(char *filename);
extern void writefile(char *filename, char *content);
extern Int repl(VirtualMachine *vm);
extern void print_element(VirtualMachine *vm, Int index);

#endif

#endif