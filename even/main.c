#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

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

StringStack specialSplit(const char* str) 
{
    StringStack stack;
    StackInit(stack);

    int n = strlen(str);
    char token[1024];
    int tokenIndex = 0;
    int insideParens = 0;

    for (int i = 0; i <= n; ++i) 
    {
        char c = str[i];

        // Check for delimiters
        if (c == '(') 
        {
            insideParens++;
        } 
        else if (c == ')') 
        {
            if (insideParens > 0) 
            {
                insideParens--;
            }
        }

        // Add character to current token
        if (i < n && (!isspace(c) || insideParens > 0)) 
        {
            token[tokenIndex++] = c;
        } 
        else if (tokenIndex > 0) 
        {
            // End of token
            token[tokenIndex] = '\0';
            char* newToken = (char*)malloc((tokenIndex + 1) * sizeof(char));
            strcpy(newToken, token);
            StackPush(stack, newToken);
            tokenIndex = 0;
        }
    }

    return stack;
}

StringStack stringSplit(const char* str, const char delim) 
{
    StringStack splited;
    StackInit(splited);
    int n = 0;
    //split string by ; but not in parentheses
    while (n < strlen(str))
    {
        char token[1024];
        int tokenIndex = 0;
        int insideParens = 0;
        while (n < strlen(str))
        {
            char c = str[n];
            if (c == '(') 
            {
                insideParens++;
            } 
            else if (c == ')') 
            {
                if (insideParens > 0) 
                {
                    insideParens--;
                }
            }
            if (c == delim && insideParens == 0)
            {
                break;
            }
            token[tokenIndex++] = c;
            n++;
        }
        token[tokenIndex] = '\0';
        char* newToken = (char*)malloc((tokenIndex + 1) * sizeof(char));
        strcpy(newToken, token);
        StackPush(splited, newToken);
        n++;
    }
    return splited;
}

Table parse(State state, char *cmd)
{
    Table result;
    StackInit(result);

    StringStack splited = specialSplit(cmd);

    for (int i = 0; i < splited.size; ++i) 
    {
        char* str = splited.data[i];

        if (str[0] == '$') 
        {
            // Variable
            Variable v;
            if(str[1] == '(')
            {
                v.value.s = strndup(str + 2, strlen(str) - 3);
                v.type = 3;
            }
            else
            {
                v = HashTableGet(Variable, state, str + 1);
            }
            
            StackPush(result, v);
        }
        else if (str[0] == '(') 
        {
            // expression
            Variable v;
            Variable expression;
            Table args = parse(state, strndup(str + 1, strlen(str) - 2));

            v = ((Function)HashTableGet(Variable, state, "interpret").value.p)(&state, &args);
            StackPush(result, v);
        }
        else if ((str[0] >= '0' && str[0] <= '9') || str[0] == '-') 
        {
            // Number
            Variable v;
            v.value.f = atof(str);
            v.type = 2;
            StackPush(result, v);
        }
        else 
        {
            // string
            Variable v;
            v.value.s = strdup(str);
            v.type = 3;
            StackPush(result, v);
        }
        
        free(str);

    }

    StackFree(splited);

    return result;
}


Variable interpret(State *state, char* input) 
{
    Table args = parse(*state, input);
    char* funcName = StackShift(args).value.s;
    if (HashTableGet(Variable, *state, funcName).type == 4) 
    {
        Variable result = ((Function)HashTableGet(Variable, *state, funcName).value.p)(state, &args);
        StackFree(args);
        return result;
    }
    StackFree(args);
    return Nil;
}

Variable bulkInterpret(State *state, const char* input) 
{
    StringStack splited = stringSplit(input, ';');
    Variable result = Nil;
    while (splited.size > 0)
    {
        result = interpret(state, StackShift(splited));
        if (result.type != 0)
        {
            break;
        }
    }
    StackFree(splited);
    return result;
}

Variable _interpret(State *state, Table* args) 
{
    Variable result = Nil;
    if (args->size == 1)
    {
        return bulkInterpret(state, StackShift(*args).value.s);
    }
    else
    {
        char* funcName = StackShift(*args).value.s;
        if (HashTableGet(Variable, *state, funcName).type == 4) 
        {
            result = ((Function)HashTableGet(Variable, *state, funcName).value.p)(state, args);
            StackFree(*args);
            return result;
        }
        StackFree(*args);
        return result;
    }
}


// list of core functions
Variable _set(State *state, Table* args) 
{
    Variable _key = StackShift(*args);
    Variable _value = StackShift(*args);
    HashTableInsert(*state, _key.value.s, _value);
    return Nil;
}

Variable _print(State *state, Table* args) 
{
    while (args->size > 0) 
    {
        Variable v = StackShift(*args);
        if (v.type == 2) 
        {
            printf("%f ", v.value.f);
        } 
        else if (v.type == 3) 
        {
            printf("%s ", v.value.s);
        }
    }
    printf("\n");
    return Nil;
}

// math functions

Variable _add(State *state, Table* args) 
{
    Variable result = Nil;
    result.value.f = StackShift(*args).value.f + StackShift(*args).value.f;
    result.type = 2;
    return result;
}

Variable _sub(State *state, Table* args) 
{
    Variable result = Nil;
    result.value.f = StackShift(*args).value.f - StackShift(*args).value.f;
    result.type = 2;
    return result;
}

Variable _mul(State *state, Table* args) 
{
    Variable result = Nil;
    result.value.f = StackShift(*args).value.f * StackShift(*args).value.f;
    result.type = 2;
    return result;
}

Variable _div(State *state, Table* args) 
{
    Variable result = Nil;
    result.value.f = StackShift(*args).value.f / StackShift(*args).value.f;
    result.type = 2;
    return result;
}

Variable _mod(State *state, Table* args) 
{
    Variable result = Nil;
    result.value.f = (int)StackShift(*args).value.f % (int)StackShift(*args).value.f;
    result.type = 2;
    return result;
}

Variable _pow(State *state, Table* args) 
{
    Variable result = Nil;
    result.value.f = pow(StackShift(*args).value.f, StackShift(*args).value.f);
    result.type = 2;
    return result;
}

Variable _ceil(State *state, Table* args) 
{
    Variable result = Nil;
    result.value.f = ceil(StackShift(*args).value.f);
    result.type = 2;
    return result;
}

Variable _floor(State *state, Table* args) 
{
    Variable result = Nil;
    result.value.f = floor(StackShift(*args).value.f);
    result.type = 2;
    return result;
}

Variable _round(State *state, Table* args) 
{
    Variable result = Nil;
    result.value.f = round(StackShift(*args).value.f);
    result.type = 2;
    return result;
}


// State functions
void registerFunction(State *state, char* name, Function func) 
{
    Variable tempFunc = Nil;
    tempFunc.type = 4;
    tempFunc.value.p = func;
    HashTableInsert(*state, name, tempFunc);
}


int main(int argc, char** argv) 
{
    
    State state;
    HashTableInit(state);

    
    registerFunction(&state, "add", _add);
    registerFunction(&state, "sub", _sub);
    registerFunction(&state, "mul", _mul);
    registerFunction(&state, "div", _div);
    registerFunction(&state, "mod", _mod);
    registerFunction(&state, "pow", _pow);
    registerFunction(&state, "ceil", _ceil);
    registerFunction(&state, "floor", _floor);
    registerFunction(&state, "round", _round);
    

    registerFunction(&state, "set", _set);
    registerFunction(&state, "print", _print);
    registerFunction(&state, "interpret", _interpret);

    
    
    bulkInterpret(&state,
        "set a 103.4;"
        "set b 2;"
        
        "set c ($(print abc;add $a $b;));"
        "set d ($(sub $a $b;));"
        "set e ($(mul $a $b;));"
        "set f ($(div $a $b;));"

        "print A = $a;"
        "print B = $b;"
        "print C = $c;"
        "print D = $d;"
        "print E = $e;"
        "print F = $f;"
    );

    return 0;
}