#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
    int i;
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

typedef Variable (*Function)(VariableHashTable, VariableStack*);

typedef VariableStack Table;
typedef VariableHashTable State;

StringStack splitString(const char* str) 
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

Table parse(State state, char *cmd)
{
    Table result;
    StackInit(result);

    StringStack splited = splitString(cmd);

    for (int i = 0; i < splited.size; ++i) 
    {
        char* str = splited.data[i];

        if (str[0] == ':') 
        {
            // String
            Variable v;
            // Remove the colon and the parentheses
            v.value.s = strndup(str + 2, strlen(str) - 3);
            v.type = 3;
            StackPush(result, v);
        }
        else if (str[0] == '$') 
        {
            // Variable
            Variable v;
            v = HashTableGet(Variable, state, str + 1);
            
            StackPush(result, v);
        }
        else if (str[0] == '(') 
        {
        }
        else if ((str[0] >= '0' && str[0] <= '9') || str[0] == '-') 
        {
            // Number
            Variable v;
            if (strchr(str, '.')) 
            {
                v.value.f = atof(str);
                v.type = 2;
            } 
            else 
            {
                v.value.i = atoi(str);
                v.type = 1;
            }
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

// list of core functions

Variable _set(State variables, Table* args) 
{
    Variable _key = StackShift(*args);
    Variable _value = StackShift(*args);
    return _key;
}

Variable _add(VariableHashTable variables, Table* args) 
{
    Variable a = StackPop(*args);
    Variable b = StackPop(*args);
    Variable result;
    if (a.type == 1 && b.type == 1) 
    {
        result.value.i = a.value.i + b.value.i;
        result.type = 1;
    } 
    else if (a.type == 2 && b.type == 2) 
    {
        result.value.f = a.value.f + b.value.f;
        result.type = 2;
    } 
    else if (a.type == 3 && b.type == 3) 
    {
        result.value.s = (char*)malloc((strlen(a.value.s) + strlen(b.value.s) + 1) * sizeof(char));
        strcpy(result.value.s, a.value.s);
        strcat(result.value.s, b.value.s);
        result.type = 3;
    } 
    else 
    {
        result.value.i = 0;
        result.type = 1;
    }
    return result;
}

void interpret(State state, char* input) 
{
    Table args = parse(state, input);
    char* funcName = StackShift(args).value.s;
    if (HashTableGet(Variable, state, funcName).type == 4) 
    {
        Variable result = ((Function)HashTableGet(Variable, state, funcName).value.p)(state, &args);
        if (result.type == 1) 
        {
            printf("Result: %d\n", result.value.i);
        } 
        else if (result.type == 2) 
        {
            printf("Result: %f\n", result.value.f);
        } 
        else if (result.type == 3) 
        {
            printf("Result: %s\n", result.value.s);
        }
    }
}

int main() 
{
    

    State state;
    HashTableInit(state);

    Variable tempFunc;
    tempFunc.type = 4;
    
    tempFunc.value.p = _add;
    HashTableInsert(state, "add", tempFunc);

    tempFunc.value.p = _set;
    HashTableInsert(state, "set", tempFunc);
    
    interpret(state, "set a 10");
    interpret(state, "set b 20");
    interpret(state, "add 10 20");
    Variable result = HashTableGet(Variable, state, "add");

    
    
    return 0;
}