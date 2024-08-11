#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define Int long
#define Float double

const char* Version = "0.4.2h";

// type -1 is error, it can contain a string with the error message
const enum 
{
    TYPE_ERROR = -1,
    TYPE_NIL = 0,
    TYPE_REFERENCE = 1,
    TYPE_NUMBER = 2,
    TYPE_STRING = 3,
    TYPE_FUNCTION = 4,
    TYPE_LIST = 5,
} Types;

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
    char* string;
    void* pointer;
} Value;

//Variable
typedef struct 
{
    Value value;
    char type;
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

typedef struct
{
    List *stack;
    HashList *hashes;
    IntList *empty;
} VirtualMachine;

//Function
typedef Int (*Function)(VirtualMachine*, IntList*);