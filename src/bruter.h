#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

const char* Version = "0.3.5";

// type -1 is error, it can contain a string with the error message
const enum 
{
    ERROR = -1,
    VOID = 0,
    TABLE = 1,
    NUMBER = 2,
    STRING = 3,
    FUNCTION = 4
} Types;


//stack implementation
#define Stack(T) struct { T *data; int size; int capacity; }
#define StackInit(s) do { (s).data = NULL; (s).size = 0; (s).capacity = 0; } while (0)
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
#define StackFree(s) free((s).data)
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


//hashtables implementation
#define HashTable(Type) struct { char **keys; Type *ValueStack; int size; int capacity; }
#define HashTableInit(_table) do { (_table).keys = NULL; (_table).ValueStack = NULL; (_table).size = 0; (_table).capacity = 0; } while (0)
#define HashTableFree(_table) do { free((_table).keys); free((_table).ValueStack); } while (0)
//verify if key k already exists in the hashtable and change the value else insert a new key-value pair
#define HashTableInsert(_table, key, v) do { \
    for (int i = 0; i < (_table).size; i++) { \
        if (strcmp((_table).keys[i], key) == 0) { \
            (_table).ValueStack[i] = (v); \
            break; \
        } \
    } \
    if ((_table).size == (_table).capacity) { \
        (_table).capacity = (_table).capacity == 0 ? 1 : (_table).capacity * 2; \
        (_table).keys = realloc((_table).keys, (_table).capacity * sizeof(*(_table).keys)); \
        (_table).ValueStack = realloc((_table).ValueStack, (_table).capacity * sizeof(*(_table).ValueStack)); \
    } \
    (_table).keys[(_table).size] = strdup(key); \
    (_table).ValueStack[(_table).size++] = (v); \
} while (0)

#define HashTableRemove(_table, key) do { \
    for (int i = 0; i < (_table).size; i++) { \
        if (strcmp((_table).keys[i], key) == 0) { \
            free((_table).keys[i]); \
            for (int j = i; j < (_table).size - 1; j++) { \
                (_table).keys[j] = (_table).keys[j + 1]; \
                (_table).ValueStack[j] = (_table).ValueStack[j + 1]; \
            } \
            (_table).size--; \
            break; \
        } \
    } \
} while (0)
#define HashTableGet(Type, _table, key) ({ \
    Type ret = {0}; \
    for (int i = 0; i < (_table).size; i++) { \
        if (strcmp((_table).keys[i], key) == 0) { \
            ret = (_table).ValueStack[i]; \
            break; \
        } \
    } \
    ret; \
})
#define HashTableFind(_table, key) ({ \
    int ret = -1; \
    for (int i = 0; i < (_table).size; i++) { \
        if (strcmp((_table).keys[i], key) == 0) { \
            ret = i; \
            break; \
        } \
    } \
    ret; \
})

typedef union 
{
    float f;
    char* s;
    void* p;
} Value;

typedef struct 
{
    Value value;
    int type;
} Variable;

typedef Stack(Variable) VariableStack;
typedef Stack(char*) StringStack;

typedef HashTable(Variable) VariableHashTable;

typedef VariableHashTable Table;
typedef VariableStack List;

typedef Variable (*Function)(Table*, List*);

const Variable Nil = {.type = 0, .value = {0}};


