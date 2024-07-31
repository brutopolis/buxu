#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MaxSize int

#define FloatSize double

const char* Version = "0.3.8";

// type -1 is error, it can contain a string with the error message
/*const enum 
{
    ERROR = -1,
    VOID = 0,
    TABLE = 1,
    NUMBER = 2,
    STRING = 3,
    FUNCTION = 4
} Types;*/

typedef union 
{
    FloatSize f;
    char* s;
    void* p;
} Value;

typedef struct 
{
    Value value;
    char type;
} Variable;

//List
typedef struct 
{
    Variable *data;
    MaxSize size; 
    MaxSize capacity; 
} List;

//Table
typedef struct {
    char **keys;
    Variable *ValueStack;
    MaxSize size;
    MaxSize capacity;
} Table;

//Function
typedef Variable (*Function)(Table*, List*);

#define Nil (Variable){.type = 0, .value = {0}}