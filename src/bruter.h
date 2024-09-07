#ifndef BRUTER_H
#define BRUTER_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

#define Int long
#define Float double

#define VERSION "0.4.6"

#define TYPE_ERROR -2
#define TYPE_NIL -1
#define TYPE_UNUSED 0
#define TYPE_POINTER 1
#define TYPE_NUMBER 2
#define TYPE_STRING 3
#define TYPE_FUNCTION 4
#define TYPE_LIST 5

#define Nil -1

#ifndef __null
#define __null 0
#endif

#ifndef NULL
#define NULL 0
#endif


#define isSpace(c) (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f')

//stack implementation
#define Stack(T) struct \
{ \
    T *data; \
    int size; \
    int capacity; \
}

#define StackInit(s) do \
{ \
    (s).data = NULL; \
    (s).size = 0; \
    (s).capacity = 0;\
} while (0)

#define StackPush(s, v) do { \
    if ((s).size == (s).capacity) { \
        (s).capacity = (s).capacity == 0 ? 1 : (s).capacity * 2; \
        (s).data = realloc((s).data, (s).capacity * sizeof(*(s).data)); \
    } \
    (s).data[(s).size++] = (v); \
} while (0)

#define StackUnshift(s, v) do { \
    if ((s).size == (s).capacity) { \
        (s).capacity = (s).capacity == 0 ? 1 : (s).capacity * 2; \
        (s).data = realloc((s).data, (s).capacity * sizeof(*(s).data)); \
    } \
    for (int i = (s).size; i > 0; i--) { \
        (s).data[i] = (s).data[i - 1]; \
    } \
    (s).data[0] = (v); \
    (s).size++; \
} while (0)

#define StackPop(s) ((s).data[--(s).size])

#define StackShift(s) ({ \
    typeof((s).data[0]) ret = (s).data[0]; \
    for (int i = 0; i < (s).size - 1; i++) { \
        (s).data[i] = (s).data[i + 1]; \
    } \
    (s).size--; \
    ret; \
})

#define StackFree(s) ({free((s).data);free(&s);})

//swap elements from index i1 to index i2
#define StackMove(s, i1, i2) do { \
    typeof((s).data[i1]) tmp = (s).data[i1]; \
    (s).data[i1] = (s).data[i2]; \
    (s).data[i2] = tmp; \
} while (0)

//insert element v at index i
#define StackInsert(s, i, v) do { \
    if ((s).size == (s).capacity) { \
        (s).capacity = (s).capacity == 0 ? 1 : (s).capacity * 2; \
        (s).data = realloc((s).data, (s).capacity * sizeof(*(s).data)); \
    } \
    for (int j = (s).size; j > i; j--) { \
        (s).data[j] = (s).data[j - 1]; \
    } \
    (s).data[i] = (v); \
    (s).size++; \
} while (0)

//remove element at index i and return it
#define StackRemove(s, i) ({ \
    typeof((s).data[i]) ret = (s).data[i]; \
    for (int j = i; j < (s).size - 1; j++) { \
        (s).data[j] = (s).data[j + 1]; \
    } \
    (s).size--; \
    ret; \
})

//Value
typedef union 
{
    Float number;
    char* string;
    void* pointer;
} Value;

//Hash
typedef struct
{
    char *key;
    Int index;
} Hash;

typedef struct 
{
    Value value;
    char type;
} Variable;


//List
typedef Stack(Value) List;
typedef Stack(Variable) VariableList; 
typedef Stack(Hash) HashList;
typedef Stack(char*) StringList;
typedef Stack(Int) IntList;
typedef Stack(char) CharList;

typedef struct
{
    List *stack;
    CharList *typestack;
    HashList *hashes;
    IntList *empty;
    //Int bytesInUse;
} VirtualMachine;

//Function
typedef Int (*Function)(VirtualMachine*, VariableList*);

//String
char* strduplicate(const char *str);
char* strnduplicate(const char *str, Int n);
char* strf(const char *fmt, ...);
char* strsub(const char *str, Int start, Int end);
char* strconcat(const char *str1, const char *str2);
Int strfind(const char *str, const char *substr);
char* strreplace(const char *str, const char *substr, const char *replacement);

StringList* splitString(char *str, char *delim);
StringList* specialSplit(char *str);

char isTrue(Variable var);

// variable

List* makeList();
IntList* makeIntList();
CharList* makeCharList();
VariableList* makeVariableList();
VirtualMachine* makeVM();
void freevm(VirtualMachine *vm);
void freevar(VirtualMachine *vm, Int index);
void freerawvar(Variable var);
void unusevar(VirtualMachine *vm, Int index);
void registerVar(VirtualMachine *vm, char *varname, Variable var);
Int newNumber(VirtualMachine *vm, Float number);
Int newString(VirtualMachine *vm, char *str);
Int newFunction(VirtualMachine *vm, Function function);
Int newPointer(VirtualMachine *vm, Int index);
Int newError(VirtualMachine *vm, char *error);
Int newList(VirtualMachine *vm);
Int newvar(VirtualMachine *vm);

Value valueDuplicate(Value value, char type);

void spawnVar(VirtualMachine *vm, char* varname);
void spawnString(VirtualMachine *vm, char* varname, char* string);
void spawnNumber(VirtualMachine *vm, char* varname, Float number);
void spawnFunction(VirtualMachine *vm, char* varname, Function function);
void spawnError(VirtualMachine *vm, char* varname, char* error);
void spawnPointer(VirtualMachine *vm, char* varname, Int index);
void spawnList(VirtualMachine *vm, char* varname);

Variable createNil();
Variable createString(char *str);
Variable createNumber(Float number);
Variable createFunction(Function function);
Variable createError(char *error);
Variable createPointer(Int index);
Variable createList();

Int hashfind(VirtualMachine *vm, char *key);
void hashset(VirtualMachine *vm, char *key, Int index);
void hashunset(VirtualMachine *vm, char *key);



// eval
Int eval(VirtualMachine *vm, char *cmd);
Int interpret(VirtualMachine *vm, char* cmd);
VariableList* parse(VirtualMachine *vm, char *cmd);


//std
Int std_set(VirtualMachine *vm, VariableList *args);
Int std_new(VirtualMachine *vm, VariableList *args);
Int std_print(VirtualMachine *vm, VariableList *args);
Int std_ls(VirtualMachine *vm, VariableList *args);
Int std_help(VirtualMachine *vm, VariableList *args);
Int std_eval(VirtualMachine *vm, VariableList *args);
Int std_delete(VirtualMachine *vm, VariableList *args);
Int std_type(VirtualMachine *vm, VariableList *args);
Int std_get(VirtualMachine *vm, VariableList *args);

//math
Int std_math_add(VirtualMachine *vm, VariableList *args);
Int std_math_sub(VirtualMachine *vm, VariableList *args);
Int std_math_mul(VirtualMachine *vm, VariableList *args);
Int std_math_div(VirtualMachine *vm, VariableList *args);
Int std_math_mod(VirtualMachine *vm, VariableList *args);
Int std_math_pow(VirtualMachine *vm, VariableList *args);
Int std_math_sqrt(VirtualMachine *vm, VariableList *args);
Int std_math_abs(VirtualMachine *vm, VariableList *args);
Int std_math_random(VirtualMachine *vm, VariableList *args);
Int std_math_floor(VirtualMachine *vm, VariableList *args);
Int std_math_ceil(VirtualMachine *vm, VariableList *args);
Int std_math_round(VirtualMachine *vm, VariableList *args);


//list
Int std_list_new(VirtualMachine *vm, VariableList *args);
Int std_list_insert(VirtualMachine *vm, VariableList *args);
Int std_list_push(VirtualMachine *vm, VariableList *args);
Int std_list_pop(VirtualMachine *vm, VariableList *args);
Int std_list_shift(VirtualMachine *vm, VariableList *args);
Int std_list_unshift(VirtualMachine *vm, VariableList *args);
Int std_list_remove(VirtualMachine *vm, VariableList *args);

Int std_list_concat(VirtualMachine *vm, VariableList *args);
Int std_list_find(VirtualMachine *vm, VariableList *args);

Int std_list_get(VirtualMachine *vm, VariableList *args);

//string
Int std_string_length(VirtualMachine *vm, VariableList *args);
Int std_string_concat(VirtualMachine *vm, VariableList *args);
Int std_string_find(VirtualMachine *vm, VariableList *args);
Int std_string_replace(VirtualMachine *vm, VariableList *args);
Int std_string_split(VirtualMachine *vm, VariableList *args);
Int std_string_new(VirtualMachine *vm, VariableList *args);
Int std_string_ndup(VirtualMachine *vm, VariableList *args);
Int std_string_split(VirtualMachine *vm, VariableList *args);

//conditions
Int std_condition_equals(VirtualMachine *vm, VariableList *args);
Int std_condition_not_equals(VirtualMachine *vm, VariableList *args);
Int std_condition_greater(VirtualMachine *vm, VariableList *args);
Int std_condition_greater_equals(VirtualMachine *vm, VariableList *args);
Int std_condition_less(VirtualMachine *vm, VariableList *args);
Int std_condition_less_equals(VirtualMachine *vm, VariableList *args);
Int std_condition_and(VirtualMachine *vm, VariableList *args);
Int std_condition_or(VirtualMachine *vm, VariableList *args);
Int std_condition_not(VirtualMachine *vm, VariableList *args);
Int std_condition_if(VirtualMachine *vm, VariableList *args);
Int std_condition_ifelse(VirtualMachine *vm, VariableList *args);




void initDefaultVars(VirtualMachine *vm);
void initStd(VirtualMachine *vm);
void initMath(VirtualMachine *vm);
void initList(VirtualMachine *vm);
void initString(VirtualMachine *vm);
void initConditions(VirtualMachine *vm);
void initAll(VirtualMachine *vm);

#endif
