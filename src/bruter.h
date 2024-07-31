#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MaxSize int

#define FloatSize double

const char* Version = "0.3.9";

// type -1 is error, it can contain a string with the error message
const enum 
{
    TYPE_ERROR = -1,
    TYPE_VOID = 0,
    TYPE_TABLE = 1,
    TYPE_NUMBER = 2,
    TYPE_STRING = 3,
    TYPE_FUNCTION = 4,
    TYPE_ARRAY = 5
} Types;

//Value
typedef union 
{
    FloatSize f;
    char* s;
    void* p;
} Value;

//Variable
typedef struct 
{
    Value value;
    char type;
} Variable;

//Array
typedef struct 
{
    Variable *data;
    MaxSize size; 
    MaxSize capacity; 
} Array;

//Table
typedef struct {
    char **keys;
    Variable *data;
    MaxSize size;
    MaxSize capacity;
} Table;

//Function
typedef Variable (*Function)(Table*, Array*);

#define Nil (Variable){.type = 0, .value = {0}}