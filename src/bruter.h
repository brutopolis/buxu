#ifndef BRUTER_H
#define BRUTER_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define Int long
#define Float double

#define VERSION "0.4.4"

#define TYPE_ERROR -2
#define TYPE_NIL -1
#define TYPE_UNUSED 0
#define TYPE_REFERENCE 1
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
StringList* splitString(char *str, char *delim);
StringList* specialSplit(char *str);

// variable

List* makeList();
IntList* makeIntList();
CharList* makeCharList();
VariableList* makeVariableList();
VirtualMachine* makeVM();
void freevm(VirtualMachine *vm);
void freeVar(VirtualMachine *vm, Int index);
void freerawvar(Variable var);
void unusevar(VirtualMachine *vm, Int index);
void registerVar(VirtualMachine *vm, char *varname, Variable var);
Int newNumber(VirtualMachine *vm, Float number);
Int newString(VirtualMachine *vm, char *str);
Int newFunction(VirtualMachine *vm, Function function);
Int newReference(VirtualMachine *vm, Int index);
Int newError(VirtualMachine *vm, char *error);
Int newVar(VirtualMachine *vm);

void spawnVar(VirtualMachine *vm, char* varname);
void spawnString(VirtualMachine *vm, char* varname, char* string);
void spawnNumber(VirtualMachine *vm, char* varname, Float number);
void spawnFunction(VirtualMachine *vm, char* varname, Function function);
void spawnError(VirtualMachine *vm, char* varname, char* error);
void spawnReference(VirtualMachine *vm, char* varname, Int index);
void spawnList(VirtualMachine *vm, char* varname);

Int hashfind(VirtualMachine *vm, char *key);
void hashset(VirtualMachine *vm, char *key, Int index);



// eval
Int eval(VirtualMachine *vm, char *cmd);
Int interpret(VirtualMachine *vm, char* cmd);
VariableList* parse(VirtualMachine *vm, char *cmd);


//std
Int _set(VirtualMachine *vm, VariableList *args);
Int _print(VirtualMachine *vm, VariableList *args);
Int _ls(VirtualMachine *vm, VariableList *args);
Int _help(VirtualMachine *vm, VariableList *args);
Int _eval(VirtualMachine *vm, VariableList *args);
Int _unset(VirtualMachine *vm, VariableList *args);
Int _add(VirtualMachine *vm, VariableList *args);
Int _list(VirtualMachine *vm, VariableList *args);
Int _push(VirtualMachine *vm, VariableList *args);

void initStd(VirtualMachine *vm);

#endif
