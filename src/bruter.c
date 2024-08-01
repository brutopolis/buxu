#include "bruter.h"

void ArrayPush(Array *array, Variable value) 
{
    if (array->size == array->capacity) 
    {
        array->capacity = array->capacity == 0 ? 1 : array->capacity * 2;
        array->data = realloc(array->data, array->capacity * sizeof(*array->data));
    }
    array->data[array->size++] = value;
}

Variable ArrayPop(Array *array) 
{
    return array->data[--array->size];
}

Variable ArrayShift(Array *array) 
{
    Variable ret = array->data[0];
    for (MaxSize i = 0; i < array->size - 1; i++) 
    {
        array->data[i] = array->data[i + 1];
    }
    array->size--;
    return ret;
}

void ArrayUnshift(Array *array, Variable value) 
{
    if (array->size == array->capacity) 
    {
        array->capacity = array->capacity == 0 ? 1 : array->capacity * 2;
        array->data = realloc(array->data, array->capacity * sizeof(*array->data));
    }
    for (MaxSize i = array->size; i > 0; i--) 
    {
        array->data[i] = array->data[i - 1];
    }
    array->data[0] = value;
    array->size++;
}

void ArrayFree(Array *array) 
{
    free(array->data);
}

void ArrayMove(Array *array, MaxSize i1, MaxSize i2) 
{
    Variable tmp = array->data[i1];
    array->data[i1] = array->data[i2];
    array->data[i2] = tmp;
}

void ArrayInsert(Array *array, MaxSize i, Variable value) {
    if (array->size == array->capacity) {
        array->capacity = array->capacity == 0 ? 1 : array->capacity * 2;
        array->data = realloc(array->data, array->capacity * sizeof(*array->data));
    }
    for (MaxSize j = array->size; j > i; j--) {
        array->data[j] = array->data[j - 1];
    }
    array->data[i] = value;
    array->size++;
}

// remove element from array and return it
Variable ArrayRemove(Array *array, MaxSize i) 
{
    Variable ret = array->data[i];
    for (MaxSize j = i; j < array->size - 1; j++) {
        array->data[j] = array->data[j + 1];
    }
    array->size--;
    return ret;
}

// Table functions

void TableFree(Table *table) {
    for (MaxSize i = 0; i < table->size; i++) {
        free(table->keys[i]);
    }
    free(table->keys);
    free(table->data);
}

void TableInsert(Table *table, const char *key, Variable value) {
    for (MaxSize i = 0; i < table->size; i++) {
        if (strcmp(table->keys[i], key) == 0) {
            table->data[i] = value;
            return;
        }
    }
    if (table->size == table->capacity) {
        table->capacity = table->capacity == 0 ? 1 : table->capacity * 2;
        table->keys = realloc(table->keys, table->capacity * sizeof(*(table->keys)));
        table->data = realloc(table->data, table->capacity * sizeof(*(table->data)));
    }
    table->keys[table->size] = strdup(key);
    table->data[table->size++] = value;
}

void TableRemove(Table *table, const char *key) {
    for (MaxSize i = 0; i < table->size; i++) {
        if (strcmp(table->keys[i], key) == 0) {
            free(table->keys[i]);
            for (MaxSize j = i; j < table->size - 1; j++) {
                table->keys[j] = table->keys[j + 1];
                table->data[j] = table->data[j + 1];
            }
            table->size--;
            return;
        }
    }
}

Variable TableGet(Table *table, const char *key) {
    Variable ret = {0, 0};
    for (MaxSize i = 0; i < table->size; i++) {
        if (strcmp(table->keys[i], key) == 0) {
            ret = table->data[i];
            break;
        }
    }
    return ret;
}

MaxSize TableFind(Table *table, const char *key) {
    MaxSize ret = (MaxSize) -1;
    for (MaxSize i = 0; i < table->size; i++) {
        if (strcmp(table->keys[i], key) == 0) {
            ret = i;
            break;
        }
    }
    return ret;
}

Table* createNewTable() 
{
    Table* table = (Table*)malloc(sizeof(Table));
    table->keys = NULL;
    table->data = NULL;
    table->size = 0;
    table->capacity = 0;
    return table;
}

Array* createNewArray() 
{
    Array* array = (Array*)malloc(sizeof(Array));
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
    return array;
}

// Split functions
Array* specialSplit(const char* str) 
{
    Array *stack = createNewArray();

    int n = strlen(str);
    char token[1024];
    int tokenIndex = 0;
    int insideParens = 0;

    for (int i = 0; i <= n; ++i) 
    {
        char c = str[i];
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
            Variable v;
            v.type = TYPE_STRING;
            v.value.s = newToken;
            ArrayPush(stack, v);
            tokenIndex = 0;
        }
    }

    return stack;
}

Array* stringSplit(const char* str, const char delim) 
{
    Array* splited = createNewArray();
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
        Variable v;
        v.type = TYPE_STRING;
        v.value.s = newToken;

        ArrayPush(splited, v);
        n++;
    }
    return splited;
}

// Recursive functions
Variable recursiveGet(Table *state, char* key) 
{
    Array *splited = stringSplit(key, '.');
    char* currentKey = ArrayShift(splited).value.s;
    Variable v = TableGet(state, currentKey);

    while (splited->size > 0) 
    {
        if (v.type == TYPE_TABLE) // Verifica se é do tipo tabela
        {
            currentKey = ArrayShift(splited).value.s;
            v = TableGet((Table*)v.value.p, currentKey);
        } 
        else 
        {
            v.type = TYPE_ERROR; // Define um tipo de erro se não for uma tabela
            v.value.s = strdup("Not a table");
            break;
        }
    }

    return v;
}

void recursiveSet(Table *state, char* key, Variable value) 
{
    Array* splited = stringSplit(key, '.');

    if (splited->size == 1) 
    {
        TableInsert(state, key, value);
        return;
    }

    char* currentKey = ArrayShift(splited).value.s;
    Variable v = TableGet(state, currentKey);

    while (splited->size > 1) 
    {
        if (v.type != 1) 
        {
            Table *newTable = createNewTable();
            Variable newVar;
            newVar.type = TYPE_TABLE;
            newVar.value.p = newTable;
            TableInsert(state, currentKey, newVar);
            v = newVar;
        }

        state = (Table*)v.value.p;
        currentKey = ArrayShift(splited).value.s;
        v = TableGet(state, currentKey);
    }

    if (v.type == TYPE_TABLE) 
    {
        currentKey = ArrayShift(splited).value.s;
        TableInsert((Table*)v.value.p, currentKey, value);
    } 
    else 
    {
        TableInsert(state, currentKey, value);
    }
}

void recursiveUnset(Table *state, char* key) {
    Array* splited = stringSplit(key, '.');

    if (splited->size == 1) 
    {
        TableRemove(state, key);
        return;
    }

    char* currentKey = ArrayShift(splited).value.s;
    Variable v = TableGet(state, currentKey);

    while (splited->size > 1) 
    {
        if (v.type == TYPE_TABLE) // Verifica se é do tipo tabela
        {
            state = (Table*)v.value.p;
            currentKey = ArrayShift(splited).value.s;
            v = TableGet(state, currentKey);
        } 
        else 
        {
            v.type = TYPE_ERROR; // Define um tipo de erro se não for uma tabela
            v.value.s = strdup("Not a table");
            return;
        }
    }

    if (v.type == TYPE_TABLE) 
    {
        currentKey = ArrayShift(splited).value.s;
        TableRemove((Table*)v.value.p, currentKey);
    } 
    else 
    {
        TableRemove(state, key);
    }
}

// Parser functions
Array* parse(Table *state, char *cmd) 
{
    Array *result = createNewArray();
    Array *splited = specialSplit(cmd);

    for (int i = 0; i < splited->size; ++i) 
    {
        char* str = splited->data[i].value.s;

        if (str[0] == '$') 
        {
            // Variable
            Variable v = Nil;
            if(str[1] == '(') 
            {
                v.value.s = strndup(str + 2, strlen(str) - 3);
                v.type = TYPE_STRING;
            } 
            else 
            {
                v = recursiveGet(state, strdup(str + 1));
            }
            ArrayPush(result, v);
        } 
        else if (str[0] == '(') 
        {
            // expression
            Variable v = Nil;
            Variable expression;
            Array* args = parse(state, strndup(str + 1, strlen(str) - 2));

            v = ((Function)TableGet(state, "eval").value.p)(state, args);
            ArrayPush(result, v);
        } 
        else if ((str[0] >= '0' && str[0] <= '9') || str[0] == '-') 
        {
            // Number
            Variable v = Nil;
            v.value.f = atof(str);
            v.type = TYPE_NUMBER;
            ArrayPush(result, v);
        } 
        else 
        {
            // string
            Variable v;
            v.value.s = strdup(str);
            v.type = TYPE_STRING;
            ArrayPush(result, v);
        }

        free(str);
    }

    ArrayFree(splited);

    return result;
}

Variable interpret(Table *state, char* input) 
{
    Array* args = parse(state, input);
    char* funcName = ArrayShift(args).value.s;
    Variable result = Nil;
    Variable var = recursiveGet(state, funcName);
    if (var.type == TYPE_FUNCTION) 
    {
        result = ((Function)var.value.p)(state, args);
        ArrayFree(args);
        return result;
    } 
    else 
    {
        result.type = TYPE_ERROR;
        char* ___err = (char*)malloc(19 + strlen(funcName));
        sprintf(___err, "Unknown function %s", funcName);
        result.value.s = ___err;
    }
    ArrayFree(args);
    return result;
}

Variable bulkInterpret(Table *state, const char* input) 
{
    Array* splited = stringSplit(input, ';');
    Variable result = Nil;
    while (splited->size > 0) 
    {
        result = interpret(state, ArrayShift(splited).value.s);
        if (result.type != 0) 
        {
            break;
        }
    }
    ArrayFree(splited);
    return result;
}

// Functions
Variable _interpret(Table *state, Array* args) 
{
    Variable result = Nil;
    if (args->size == 1) 
    {
        return bulkInterpret(state, ArrayShift(args).value.s);
    }
    else 
    {
        char* funcName = ArrayShift(args).value.s;
        Variable var = recursiveGet(state, funcName);
        if (var.type == TYPE_FUNCTION) 
        {
            result = ((Function)var.value.p)(state, args);
            ArrayFree(args);
            return result;
        } 
        else 
        {
            result.type = TYPE_ERROR;
            char* ___err = (char*)malloc(19 + strlen(funcName));
            sprintf(___err, "Unknown function %s", funcName);
            result.value.s = ___err;
        }
        ArrayFree(args);
        return result;
    }
}

Variable _type(Table *state, Array* args) 
{
    Variable target = ArrayShift(args);
    Variable result = Nil;
    result.type = TYPE_STRING;
    switch (target.type) 
    {
        case TYPE_ERROR:
            result.value.s = strdup("error");
            break;
        case TYPE_NIL:
            result.value.s = strdup("");
            break;
        case TYPE_TABLE:
            result.value.s = strdup("table");
            break;
        case TYPE_NUMBER:
            result.value.s = strdup("number");
            break;
        case TYPE_STRING:
            result.value.s = strdup("string");
            break;
        case TYPE_FUNCTION:
            result.value.s = strdup("function");
            break;
        case TYPE_ARRAY:
            result.value.s = strdup("array");
            break;
    }
    return result;
}

Variable _table(Table *state, Array* args) 
{
    Table* newTable = createNewTable();
    return (Variable){.type = TYPE_TABLE, .value = {.p = newTable}};
}

Variable _array(Table *state, Array* args) 
{
    Array *array = createNewArray();
    return (Variable){.type = TYPE_ARRAY, .value = {.p = array}};
}

Variable _unset(Table *state, Array* args) 
{
    Variable _key = ArrayShift(args);
    recursiveUnset(state, _key.value.s);
    return Nil;
}

Variable _set(Table *state, Array* args) 
{
    Variable _key = ArrayShift(args);
    Variable _value = ArrayShift(args);
    recursiveSet(state, _key.value.s, _value);
    return Nil;
}

Variable _print(Table *state, Array* args) 
{
    while (args->size > 0) 
    {
        Variable v = ArrayShift(args);
        if (v.type == TYPE_NUMBER) 
        {
            printf("%f ", v.value.f);
        }
        else if (v.type == TYPE_STRING) 
        {
            printf("%s ", v.value.s);
        }
        else if (v.type == TYPE_TABLE) 
        {
            printf("[table] ");
        }
        else if (v.type == TYPE_FUNCTION) 
        {
            printf("[function] ");
        }
    }
    printf("\n");
    return Nil;
}

Variable _ls(Table *_state, Array* args) 
{
    Table *state;
    if (args->size == 0) 
    {
        state = _state;
    }
    else
    {
        Variable _ = ArrayShift(args);
        if (_.type == TYPE_TABLE) 
        {
            state = _.value.p;
        }
        else if (_.type == TYPE_ARRAY) 
        {
            Array *array = _.value.p;
            for (int i = 0; i < array->size; i++) 
            {
                Variable _var = array->data[i];
                switch (_var.type) 
                {
                    case -1:
                        printf("%d [error] : %s\n", i, _var.value.s);
                        break;
                    case 0:
                        printf("%d []\n");
                        break;
                    case 1:
                        printf("%d [table] : %p\n", i, _var.value.p);
                        break;
                    case 2:
                        printf("%d [number] : %f\n", i, _var.value.f);
                        break;
                    case 3:
                        printf("%d [string] : %s\n", i, _var.value.s);
                        break;
                    case 4:
                        printf("%d [function] : %p\n", i, _var.value.p);
                        break;
                }
            }
            return Nil;
        }
        else
        {
            return Nil;
        }
    }

    for (int i = 0; i < state->size; i++) 
    {
        switch (state->data[i].type) 
        {
            case -1:
                printf("[error] %s : %s \n", state->keys[i], state->data[i].value.s);
                break;
            case 0:
                printf("[] %s\n", state->keys[i]);
                break;
            case 1:
                printf("[table] %s\n", state->keys[i], state->data[i].value.p);
                break;
            case 2:
                printf("[number] %s : %f\n", state->keys[i], state->data[i].value.f);
                break;
            case 3:
                printf("[string] %s : %s\n", state->keys[i], state->data[i].value.s);
                break;
            case 4:
                printf("[function] %s : %p\n", state->keys[i], state->data[i].value.p);
                break;
        } 
    }
    return Nil;
}

Variable __exit(Table *state, Array* args) 
{
    exit(0);
}

// conditions functions
unsigned char isTrue(Variable v) 
{
    if (v.type == TYPE_NUMBER && v.value.f == 0) 
    {
        return 0;
    }
    else if (v.type < 1) 
    {
        return 0;
    }
    else 
    {
        return 1;
    }
}

Variable _if(Table *state, Array* args) 
{
    Variable condition = bulkInterpret(state, ArrayShift(args).value.s);
    Variable block = ArrayShift(args);
    Variable block2 = Nil;
    if (args->size > 0) 
    {
        Variable _else = ArrayShift(args);
        if (_else.type == TYPE_STRING && strcmp(_else.value.s, "else") == 0) 
        {
            block2 = ArrayShift(args);
        }
        else 
        {
            block2 = _else;
        }
    } 
    
    if (isTrue(condition)) 
    {
        return bulkInterpret(state, block.value.s);
    }
    else if (block2.type == TYPE_STRING) 
    {
        return bulkInterpret(state, block2.value.s);
    }
    return Nil;
}

Variable _and(Table *state, Array* args) 
{
    Variable a = ArrayShift(args);
    Variable b = ArrayShift(args);
    return (Variable){.type = TYPE_NUMBER, .value = {.f = (isTrue(a) && isTrue(b))}};
}

Variable _or(Table *state, Array* args) 
{
    Variable a = ArrayShift(args);
    Variable b = ArrayShift(args);
    return(isTrue(a) ? a : b);
}

Variable _not(Table *state, Array* args) 
{
    Variable a = ArrayShift(args);
    Variable result = Nil;
    return (Variable){.type = TYPE_NUMBER, .value = {.f = !isTrue(a)}};
}

Variable _equals(Table *state, Array* args) 
{
    Variable result = Nil;
    Variable a = ArrayShift(args);
    Variable b = ArrayShift(args);
    if (a.type == TYPE_NUMBER && b.type == TYPE_NUMBER) 
    {
        result.value.f = a.value.f == b.value.f;
    } 
    else if (a.type == TYPE_STRING && b.type == TYPE_STRING) 
    {
        result.value.f = strcmp(a.value.s, b.value.s) == 0;
    }
    else if (a.type > 0 && b.type > 0) 
    {
        result.value.f = a.value.p == b.value.p;
    }
    else
    {
        result.value.f = 0;
    }
    result.type = TYPE_NUMBER;
    return result;
}

Variable _notEquals(Table *state, Array* args) 
{
    Variable result = _equals(state, args);
    result.value.f = !result.value.f;
    return result;
}

Variable _greaterThan(Table *state, Array* args) 
{
    Variable result = Nil;
    Variable a = ArrayShift(args);
    Variable b = ArrayShift(args);
    if (a.type == TYPE_NUMBER && b.type == TYPE_NUMBER) 
    {
        result.value.f = a.value.f > b.value.f;
    } 
    else 
    {
        result.value.f = 0;
    }
    result.type = TYPE_NUMBER;
    return result;
}

Variable _lessThan(Table *state, Array* args) 
{
    Variable result = Nil;
    Variable a = ArrayShift(args);
    Variable b = ArrayShift(args);
    if (a.type == TYPE_NUMBER && b.type == TYPE_NUMBER) 
    {
        result.value.f = a.value.f < b.value.f;
    } 
    else 
    {
        result.value.f = 0;
    }
    result.type = TYPE_NUMBER;
    return result;
}

Variable _greaterOrEqual(Table *state, Array* args) 
{
    Variable result = Nil;
    Variable a = ArrayShift(args);
    Variable b = ArrayShift(args);
    if (a.type == TYPE_NUMBER && b.type == TYPE_NUMBER) 
    {
        result.value.f = a.value.f >= b.value.f;
    } 
    else 
    {
        result.value.f = 0;
    }
    result.type = TYPE_NUMBER;
    return result;
}

Variable _lessOrEqual(Table *state, Array* args) 
{
    Variable result = Nil;
    Variable a = ArrayShift(args);
    Variable b = ArrayShift(args);
    if (a.type == TYPE_NUMBER && b.type == TYPE_NUMBER) 
    {
        result.value.f = a.value.f <= b.value.f;
    } 
    else 
    {
        result.value.f = 0;
    }
    result.type = TYPE_NUMBER;
    return result;
}

// loop functions
Variable _while(Table *state, Array* args) 
{
    char* conditionCode = ArrayShift(args).value.s;
    Variable block = ArrayShift(args);
    Variable condition = bulkInterpret(state, conditionCode);
    while (isTrue(condition)) 
    {
        bulkInterpret(state, block.value.s);
        condition = bulkInterpret(state, conditionCode);
    }
    return Nil;
}

// each:
// each key value in (table or array)
Variable _each(Table *state, Array* args) 
{
    Variable key = ArrayShift(args);
    Variable value = ArrayShift(args);
    Variable in = ArrayShift(args);
    Variable obj = ArrayShift(args);
    Variable block = ArrayShift(args);
    
    if (in.type == TYPE_STRING) 
    {
        if (obj.type == TYPE_TABLE) 
        {
            Table* _table = obj.value.p;
            for (int i = 0; i < _table->size; i++) 
            {
                Variable _key = (Variable){.type = TYPE_STRING, .value = {.s = _table->keys[i]}};
                Variable _value = _table->data[i];
                recursiveSet(state, key.value.s, _key);
                recursiveSet(state, value.value.s, _value);
                bulkInterpret(state, block.value.s);
            }
        } 
        else if (obj.type == TYPE_ARRAY) 
        {
            Array* _array = obj.value.p;
            for (int i = 0; i < _array->size; i++) 
            {
                Variable _key = (Variable){.type = TYPE_NUMBER, .value = {.f = i}};
                Variable _value = _array->data[i];
                recursiveSet(state, key.value.s, _key);
                recursiveSet(state, value.value.s, _value);
                bulkInterpret(state, block.value.s);
            }
        }
    }

    recursiveUnset(state, key.value.s);
    recursiveUnset(state, value.value.s);

    return Nil;
}


// math functions
Variable _add(Table *state, Array* args) 
{
    Variable result = Nil;
    Variable a = ArrayShift(args);
    Variable b = ArrayShift(args);
    result.value.f = a.value.f + b.value.f;
    result.type = TYPE_NUMBER;
    return result;
}

Variable _sub(Table *state, Array* args) 
{
    Variable result = Nil;
    Variable a = ArrayShift(args);
    Variable b = ArrayShift(args);
    result.value.f = a.value.f - b.value.f;
    result.type = TYPE_NUMBER;
    return result;
}

Variable _mul(Table *state, Array* args) 
{
    Variable result = Nil;
    result.value.f = ArrayShift(args).value.f * ArrayShift(args).value.f;
    result.type = TYPE_NUMBER;
    return result;
}

Variable _div(Table *state, Array* args) 
{
    Variable result = Nil;
    Variable a = ArrayShift(args);
    Variable b = ArrayShift(args);
    result.value.f = ArrayShift(args).value.f / ArrayShift(args).value.f;
    result.type = TYPE_NUMBER;
    
    return result;
}

Variable _mod(Table *state, Array* args) 
{
    Variable result = Nil;
    result.value.f = (int)ArrayShift(args).value.f % (int)ArrayShift(args).value.f;
    result.type = TYPE_NUMBER;
    return result;
}

Variable _pow(Table *state, Array* args) 
{
    Variable result = Nil;
    result.value.f = pow(ArrayShift(args).value.f, ArrayShift(args).value.f);
    result.type = TYPE_NUMBER;
    return result;
}

Variable _len(Table *state, Array* args) 
{
    Variable _ = ArrayShift(args);
    if (_.type == TYPE_STRING) 
    {
        return (Variable){.type = TYPE_NUMBER, .value = {.f = strlen(_.value.s)}};
    } 
    else if (_.type == TYPE_TABLE) 
    {
        Table* _table = _.value.p;
        return (Variable){.type = TYPE_NUMBER, .value = {.f = _table->size}};
    }
    else if (_.type == TYPE_NUMBER) 
    {
        //length of number in string,
        return (Variable){.type = TYPE_NUMBER, .value = {.f = floor(log10(_.value.f) + 1)}};
    }
    return Nil;
}


// math functions
Variable _ceil(Table *state, Array* args) 
{
    Variable result = Nil;
    result.value.f = ceil(ArrayShift(args).value.f);
    result.type = TYPE_NUMBER;
    return result;
}

Variable _floor(Table *state, Array* args) 
{
    Variable result = Nil;
    result.value.f = floor(ArrayShift(args).value.f);
    result.type = TYPE_NUMBER;
    return result;
}

Variable _round(Table *state, Array* args) 
{
    Variable result = Nil;
    result.value.f = round(ArrayShift(args).value.f);
    result.type = TYPE_NUMBER;
    return result;
}

// string functions
Variable _string_find(Table *state, Array* args) 
{
    Variable result = Nil;
    char* str = ArrayShift(args).value.s;
    char* substr = ArrayShift(args).value.s;
    int index = -1;
    char* found = strstr(str, substr);
    if (found != NULL) 
    {
        index = found - str;
    }
    result.value.f = index;
    result.type = TYPE_NUMBER;
    return result;
}

Variable _string_replace(Table *state, Array* args) 
{
    Variable result = Nil;
    char* str = ArrayShift(args).value.s;
    char* substr = ArrayShift(args).value.s;
    char* replacement = ArrayShift(args).value.s;
    char* found = strstr(str, substr);
    if (found != NULL) 
    {
        char* newStr = (char*)malloc(strlen(str) + strlen(replacement) - strlen(substr) + 1);
        strncpy(newStr, str, found - str);
        strcpy(newStr + (found - str), replacement);
        strcpy(newStr + (found - str) + strlen(replacement), found + strlen(substr));
        result.value.s = newStr;
    } 
    else 
    {
        result.value.s = strdup(str);
    }
    result.type = TYPE_STRING;
    return result;
}

Variable _string_byte(Table *state, Array* args) 
{
    Variable result = Nil;
    char* str = ArrayShift(args).value.s;
    int index = (int)ArrayShift(args).value.f;
    result.value.f = str[index];
    result.type = TYPE_NUMBER;
    return result;
}

Variable _string_char(Table *state, Array* args) 
{
    Variable result = Nil;
    char* str = (char*)malloc(2);
    str[0] = (char)ArrayShift(args).value.f;
    str[1] = '\0';
    result.value.s = str;
    result.type = TYPE_STRING;
    return result;
}

Variable _string_substring(Table *state, Array* args) 
{
    Variable result = Nil;
    char* str = ArrayShift(args).value.s;
    int start = (int)ArrayShift(args).value.f;
    int length = (int)ArrayShift(args).value.f;
    char* newStr = (char*)malloc(length + 1);
    strncpy(newStr, str + start, length);
    newStr[length] = '\0';
    result.value.s = newStr;
    result.type = TYPE_STRING;
    return result;
}

Variable _string_concat(Table *state, Array* args)
{
    Variable a = ArrayShift(args);
    Variable b = ArrayShift(args);
    Variable result = Nil;
    char* str; 
    if (b.type == TYPE_NUMBER)
    {
        str = (char*)malloc(strlen(a.value.s) + 15);
        sprintf(str, "%s%.2f", a.value.s, b.value.f);
    }
    else if (b.type == TYPE_STRING)
    {
        str = (char*)malloc(strlen(a.value.s) + strlen(b.value.s) + 1);
        sprintf(str, "%s%s", a.value.s, b.value.s);
    }
    result.value.s = str;
    result.type = TYPE_STRING;
    return result;
}

Variable _string_split(Table *state, Array* args)
{
    Variable str = ArrayShift(args);
    Variable delim = ArrayShift(args);
    Variable result = Nil;
    Array* splited = stringSplit(str.value.s, delim.value.s[0]);
    result.type = TYPE_ARRAY;
    result.value.p = splited;
    return result;
}

// table functions


// array functions
Variable _array_push(Table *state, Array* args) 
{
    Variable array = ArrayShift(args);
    Variable value = ArrayShift(args);
    ArrayPush((Array*)array.value.p, value);
    return Nil;
}

Variable _array_pop(Table *state, Array* args) 
{
    Variable array = ArrayShift(args);
    return ArrayPop((Array*)array.value.p);
}

Variable _array_shift(Table *state, Array* args) 
{
    Variable array = ArrayShift(args);
    return ArrayShift((Array*)array.value.p);
}

Variable _array_unshift(Table *state, Array* args) 
{
    Variable array = ArrayShift(args);
    Variable value = ArrayShift(args);
    ArrayUnshift((Array*)array.value.p, value);
    return Nil;
}

Variable _array_free(Table *state, Array* args) 
{
    Variable array = ArrayShift(args);
    ArrayFree((Array*)array.value.p);
    return Nil;
}

Variable _array_move(Table *state, Array* args) 
{
    Variable array = ArrayShift(args);
    MaxSize i1 = (MaxSize)ArrayShift(args).value.f;
    MaxSize i2 = (MaxSize)ArrayShift(args).value.f;
    ArrayMove((Array*)array.value.p, i1, i2);
    return Nil;
}

Variable _array_insert(Table *state, Array* args) 
{
    Variable array = ArrayShift(args);
    MaxSize i = (MaxSize)ArrayShift(args).value.f;
    Variable value = ArrayShift(args);
    ArrayInsert((Array*)array.value.p, i, value);
    return Nil;
}

Variable _array_remove(Table *state, Array* args) 
{
    Variable array = ArrayShift(args);
    MaxSize i = (MaxSize)ArrayShift(args).value.f;
    return ArrayRemove((Array*)array.value.p, i);
}

Variable _array_get(Table *state, Array* args) 
{
    Variable array = ArrayShift(args);
    MaxSize i = (MaxSize)ArrayShift(args).value.f;
    return ((Array*)array.value.p)->data[i];
}

Variable _array_set(Table *state, Array* args) 
{
    Variable array = ArrayShift(args);
    MaxSize i = (MaxSize)ArrayShift(args).value.f;
    Variable value = ArrayShift(args);
    ((Array*)array.value.p)->data[i] = value;
    return Nil;
}

// Table functions
void registerFunction(Table *state, char* name, Function func) 
{
    Variable tempFunc = Nil;
    tempFunc.type = TYPE_FUNCTION;
    tempFunc.value.p = func;
    recursiveSet(state, name, tempFunc);
}

void repl(Table *state) 
{
    char input[1024];
    printf("bruter %s\n", Version);
    while (1) 
    {
        printf("br> ");
        fgets(input, 1024, stdin);
        input[strlen(input) - 1] = '\0';
        Variable result = bulkInterpret(state, input);
        if (result.type > 0)
        {
            if (result.type == TYPE_NUMBER) 
            {
                printf("returned: %f\n", result.value.f);
            } 
            else if (result.type == TYPE_STRING) 
            {
                printf("returned: %s\n", result.value.s);
            }
        }
        else if(result.type == TYPE_ERROR)
        {
            printf("Error(%s): %s\n", result.value.s, input);
        }
    }
}



int main(int argc, char** argv) 
{
    
    Table state = *createNewTable();
    char* filename = NULL;
    char* filetxt = NULL;

    //turn args into a string stack
    Array *args = createNewArray();

    for (int i = 1; i < argc; i++)
    {
        Variable v = {.type = TYPE_STRING, .value = {.s = argv[i]}};
        ArrayPush(args, v);
    }

    while (args->size > 0)
    {
        char* arg = ArrayShift(args).value.s;
        if (arg[0] == '-')
        {
            if (strcmp(arg, "-h") == 0)
            {
                printf("Usage: bruter [options] [file]\n");
                printf("Options:\n");
                printf("  -h  Show this help message\n");
                return 0;
            }
            else if (strcmp(arg, "-v") == 0)
            {
                printf("bruter %s\n", Version);
                return 0;
            }
            else
            {
                printf("Unknown option: %s\n", arg);
                return 1;
            }
        }
        else if (filename == NULL)
        {
            filename = arg;
        }
    }

    
    if (filename != NULL && filetxt == NULL)
    {
        FILE* file = fopen(filename, "r");
        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET);
        filetxt = malloc(length + 1);
        fread(filetxt, 1, length, file);
        fclose(file);
        filetxt[length] = '\0';
    }
    
    registerFunction(&state, "add", _add);
    registerFunction(&state, "sub", _sub);
    registerFunction(&state, "mul", _mul);
    registerFunction(&state, "div", _div);
    registerFunction(&state, "mod", _mod);
    registerFunction(&state, "pow", _pow);
    registerFunction(&state, "len", _len);

    registerFunction(&state, "ceil", _ceil);
    registerFunction(&state, "floor", _floor);
    registerFunction(&state, "round", _round);

    registerFunction(&state, "==", _equals);
    registerFunction(&state, "!=", _notEquals);
    registerFunction(&state, ">", _greaterThan);
    registerFunction(&state, "<", _lessThan);
    registerFunction(&state, ">=", _greaterOrEqual);
    registerFunction(&state, "<=", _lessOrEqual);
    registerFunction(&state, "if", _if);
    registerFunction(&state, "and", _and);
    registerFunction(&state, "or", _or);

    registerFunction(&state, "while", _while);
    registerFunction(&state, "each", _each);

    

    registerFunction(&state, "set", _set);
    registerFunction(&state, "unset", _unset);
    registerFunction(&state, "print", _print);
    registerFunction(&state, "eval", _interpret);
    registerFunction(&state, "ls", _ls);
    registerFunction(&state, "exit", __exit);
    registerFunction(&state, "type", _type);

    recursiveSet(&state, "table", (Variable){.type = TYPE_TABLE, .value = {.p = createNewTable()}});
    registerFunction(&state, "table.new", _table);
    

    interpret(&state, "set string (table.new)");
    registerFunction(&state, "string.find", _string_find);
    registerFunction(&state, "string.replace", _string_replace);
    registerFunction(&state, "string.byte", _string_byte);
    registerFunction(&state, "string.char", _string_char);
    registerFunction(&state, "string.substr", _string_substring);
    registerFunction(&state, "string.concat", _string_concat);
    registerFunction(&state, "string.split", _string_split);

    interpret(&state, "set array (table.new)");
    registerFunction(&state, "array.new", _array);
    registerFunction(&state, "array.push", _array_push);
    registerFunction(&state, "array.pop", _array_pop);
    registerFunction(&state, "array.shift", _array_shift);
    registerFunction(&state, "array.unshift", _array_unshift);
    registerFunction(&state, "array.free", _array_free);
    registerFunction(&state, "array.move", _array_move);
    registerFunction(&state, "array.insert", _array_insert);
    registerFunction(&state, "array.remove", _array_remove);
    registerFunction(&state, "array.get", _array_get);
    registerFunction(&state, "array.set", _array_set);



    if (filetxt != NULL)
    {
        bulkInterpret(&state, filetxt);
    }
    else
    {
        repl(&state);
    }
    
    return 0;
}