#ifndef BRUTER_H
#define BRUTER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define Int long
#define Float double

#define VERSION "0.4.3"

#define TYPE_ERROR -2
#define TYPE_NIL -1
//#define TYPE_REFERENCE 1
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
    //Int index;
    void* pointer;
} Value;

//Variable
typedef struct 
{
    Value value;
    char type;
    char isTemp;
    char isRef;
} Variable;

typedef struct
{
    Int index;
    Variable *variable;
} Reference;

//Hash
typedef struct
{
    char *key;
    Int index;
} Hash;



//List
typedef Stack(Variable*) List;
typedef Stack(Hash*) HashList;
typedef Stack(Int) IntList;
typedef Stack(char*) StringList;
typedef Stack(char) CharList;

typedef struct
{
    List *stack;
    HashList *hashes;
    IntList *empty;
    Int bytesInUse;
} VirtualMachine;

//Function
typedef Int (*Function)(VirtualMachine*, IntList*);

//String
StringList* splitString(char *str, char *delim);
StringList* specialSplit(char *str);

//Variable(.c)

//make
Variable* makeVariable(char type, Value value);
List* makeList();
IntList* makeIntList();
VirtualMachine* makeVM();

//Var
Int newVar(VirtualMachine *vm);
void freeVar(VirtualMachine *vm, Int index);
void setVar(VirtualMachine *vm, Int index, char type, Value value, char isRef);

// new 
Int newNumber(VirtualMachine *vm, Float number);
Int newString(VirtualMachine *vm, char* string);
Int newReference(VirtualMachine *vm, Int index);
Int newFunction(VirtualMachine *vm, Function function);
Int newList(VirtualMachine *vm);
//Int newError(VirtualMachine *vm, int errorid);

// spawn
Int spawnNumber(VirtualMachine *vm, char *varname, Float number);
Int spawnString(VirtualMachine *vm, char *varname, char* string);
Int spawnReference(VirtualMachine *vm, char *varname, Int index);
Int spawnFunction(VirtualMachine *vm, char *varname, Function function);
Int spawnList(VirtualMachine *vm, char *varname);
//Int spawnError(VirtualMachine *vm, char *varname, int errorid);

// create
Variable* createNumber(VirtualMachine *vm, Float number);
Variable* createString(VirtualMachine *vm, char* string);
Variable* createReference(VirtualMachine *vm, Int index);
Variable* createFunction(VirtualMachine *vm, Function function);
Variable* createList(VirtualMachine *vm);
//Variable* createError(VirtualMachine *vm, char* string);

// ref
Reference refget(VirtualMachine *vm, Int index);
void freeref(VirtualMachine *vm, Reference ref);


//Hash(.c)
Int hashfind(VirtualMachine *vm, char *varname);
void hashset(VirtualMachine *vm, char* varname, Int index);
void hashunset(VirtualMachine *vm, char* varname);
Variable* hashget(VirtualMachine *vm, char* varname);

//Args(.c)
Reference argget(VirtualMachine *vm, IntList *args, Int index);
Reference argshift(VirtualMachine *vm, IntList *args);
void argpush(VirtualMachine *vm, IntList *args, Int index);
void argunshift(VirtualMachine *vm, IntList *args, Int index);
void arginsert(VirtualMachine *vm, IntList *args, Int index, Int value);
Reference argremove(VirtualMachine *vm, IntList *args, Int index);
Int argsize(VirtualMachine *vm, IntList *args);
void argfree(VirtualMachine *vm, IntList *args);

//List(.c)
void listpush(VirtualMachine *vm, Int list, Int value);
void listunshift(VirtualMachine *vm, Int list, Int value);
Int listpop(VirtualMachine *vm, Int list);
Int listshift(VirtualMachine *vm, Int list);
void listinsert(VirtualMachine *vm, Int list, Int index, Int value);
Int listremove(VirtualMachine *vm, Int list, Int index);
Int listsize(VirtualMachine *vm, Int list);

//stc(.c) functions
Int _set(VirtualMachine *vm, IntList *args);
//Int _get(VirtualMachine *vm, IntList *args);
Int _print(VirtualMachine *vm, IntList *args);
Int _eval(VirtualMachine *vm, IntList *args);
Int _help(VirtualMachine *vm, IntList *args);
Int _ls(VirtualMachine *vm, IntList *args);
Int ___exit(VirtualMachine *vm, IntList *args);

void print(VirtualMachine *vm, Int index);

void initStd(VirtualMachine *vm);
int main();

//bruter(.c) a.k.a. main
void freeVM(VirtualMachine *vm);
Int interpret(VirtualMachine *vm, char *str);
Int eval(VirtualMachine *vm, char *str);

char* toString(CharList *list);
void freeString(VirtualMachine *vm, CharList *string);


#endif