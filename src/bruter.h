#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

const char* Version = "0.3.4";

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
#define HashTable(T) struct { char **keys; T *ValueStack; int size; int capacity; }
#define HashTableInit(h) do { (h).keys = NULL; (h).ValueStack = NULL; (h).size = 0; (h).capacity = 0; } while (0)
#define HashTableFree(h) do { free((h).keys); free((h).ValueStack); } while (0)
#define HashTableInsert(h, k, v) do { \
    if ((h).size == (h).capacity) { \
        (h).capacity = (h).capacity == 0 ? 1 : (h).capacity * 2; \
        (h).keys = realloc((h).keys, (h).capacity * sizeof(*(h).keys)); \
        (h).ValueStack = realloc((h).ValueStack, (h).capacity * sizeof(*(h).ValueStack)); \
    } \
    (h).keys[(h).size] = strdup(k); \
    (h).ValueStack[(h).size++] = (v); \
} while (0)
#define HashTableRemove(h, k) do { \
    for (int i = 0; i < (h).size; i++) { \
        if (strcmp((h).keys[i], k) == 0) { \
            free((h).keys[i]); \
            for (int j = i; j < (h).size - 1; j++) { \
                (h).keys[j] = (h).keys[j + 1]; \
                (h).ValueStack[j] = (h).ValueStack[j + 1]; \
            } \
            (h).size--; \
            break; \
        } \
    } \
} while (0)
#define HashTableGet(T, h, k) ({ \
    T ret = {0}; \
    for (int i = 0; i < (h).size; i++) { \
        if (strcmp((h).keys[i], k) == 0) { \
            ret = (h).ValueStack[i]; \
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

typedef VariableHashTable State;
typedef VariableStack Table;

typedef Variable (*Function)(State*, Table*);

const Variable Nil = {.type = 0, .value = {0}};


