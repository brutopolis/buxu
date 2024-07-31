#include "bruter.h"

// List functions
void ListInit(List *list) {
    list->data = NULL;
    list->size = 0;
    list->capacity = 0;
}

void ListPush(List *list, Variable value) {
    if (list->size == list->capacity) {
        list->capacity = list->capacity == 0 ? 1 : list->capacity * 2;
        list->data = realloc(list->data, list->capacity * sizeof(*list->data));
    }
    list->data[list->size++] = value;
}

Variable ListPop(List *list) {
    return list->data[--list->size];
}

Variable ListShift(List *list) {
    Variable ret = list->data[0];
    for (MaxSize i = 0; i < list->size - 1; i++) {
        list->data[i] = list->data[i + 1];
    }
    list->size--;
    return ret;
}

Variable ListUnshift(List *list, Variable value) {
    if (list->size == list->capacity) {
        list->capacity = list->capacity == 0 ? 1 : list->capacity * 2;
        list->data = realloc(list->data, list->capacity * sizeof(*list->data));
    }
    for (MaxSize i = list->size; i > 0; i--) {
        list->data[i] = list->data[i - 1];
    }
    list->data[0] = value;
    list->size++;
    return value;
}

void ListFree(List *list) {
    free(list->data);
}

void ListMove(List *list, MaxSize i1, MaxSize i2) {
    Variable tmp = list->data[i1];
    list->data[i1] = list->data[i2];
    list->data[i2] = tmp;
}

void ListInsert(List *list, MaxSize i, Variable value) {
    if (list->size == list->capacity) {
        list->capacity = list->capacity == 0 ? 1 : list->capacity * 2;
        list->data = realloc(list->data, list->capacity * sizeof(*list->data));
    }
    for (MaxSize j = list->size; j > i; j--) {
        list->data[j] = list->data[j - 1];
    }
    list->data[i] = value;
    list->size++;
}

Variable ListRemove(List *list, MaxSize i) {
    Variable ret = list->data[i];
    for (MaxSize j = i; j < list->size - 1; j++) {
        list->data[j] = list->data[j + 1];
    }
    list->size--;
    return ret;
}

// Table functions
void TableInit(Table *table) {
    table->keys = NULL;
    table->ValueStack = NULL;
    table->size = 0;
    table->capacity = 0;
}

void TableFree(Table *table) {
    for (MaxSize i = 0; i < table->size; i++) {
        free(table->keys[i]);
    }
    free(table->keys);
    free(table->ValueStack);
}

void TableInsert(Table *table, const char *key, Variable value) {
    for (MaxSize i = 0; i < table->size; i++) {
        if (strcmp(table->keys[i], key) == 0) {
            table->ValueStack[i] = value;
            return;
        }
    }
    if (table->size == table->capacity) {
        table->capacity = table->capacity == 0 ? 1 : table->capacity * 2;
        table->keys = realloc(table->keys, table->capacity * sizeof(*(table->keys)));
        table->ValueStack = realloc(table->ValueStack, table->capacity * sizeof(*(table->ValueStack)));
    }
    table->keys[table->size] = strdup(key);
    table->ValueStack[table->size++] = value;
}

void TableRemove(Table *table, const char *key) {
    for (MaxSize i = 0; i < table->size; i++) {
        if (strcmp(table->keys[i], key) == 0) {
            free(table->keys[i]);
            for (MaxSize j = i; j < table->size - 1; j++) {
                table->keys[j] = table->keys[j + 1];
                table->ValueStack[j] = table->ValueStack[j + 1];
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
            ret = table->ValueStack[i];
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
    Table* newTable = (Table*)malloc(sizeof(Table));
    TableInit(newTable);
    return newTable;
}

// Split functions
List specialSplit(const char* str) 
{
    List stack;
    ListInit(&stack);

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
            v.type = 3;
            v.value.s = newToken;
            ListPush(&stack, v);
            tokenIndex = 0;
        }
    }

    return stack;
}

List stringSplit(const char* str, const char delim) 
{
    List splited;
    ListInit(&splited);
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
        v.type = 3;
        v.value.s = newToken;

        ListPush(&splited, v);
        n++;
    }
    return splited;
}

// Recursive functions
Variable recursiveGet(Table *state, char* key) 
{
    List splited = stringSplit(key, '.');
    char* currentKey = ListShift(&splited).value.s;
    Variable v = TableGet(state, currentKey);

    while (splited.size > 0) 
    {
        if (v.type == 1)
        {
            currentKey = ListShift(&splited).value.s;
            v = TableGet((Table*)v.value.p, currentKey);
        }
        else if (v.type == 5)
        {
            currentKey = ListShift(&splited).value.s;
            v = ((List*)v.value.p)->data[atoi(currentKey)];
        }
        else 
        {
            v.type = -1; // Define um tipo de erro se não for uma tabela
            v.value.s = strdup("Not a table");
            break;
        }
    }

    return v;
}

void recursiveSet(Table *state, char* key, Variable value) 
{
    List splited = stringSplit(key, '.');

    if (splited.size == 1) 
    {
        TableInsert(state, key, value);
        return;
    }

    char* currentKey = ListShift(&splited).value.s;
    Variable v = TableGet(state, currentKey);

    while (splited.size > 1) 
    {
        if (v.type != 1) 
        {
            Table *newTable = createNewTable();
            Variable newVar;
            newVar.type = 1;
            newVar.value.p = newTable;
            TableInsert(state, currentKey, newVar);
            v = newVar;
        }

        state = (Table*)v.value.p;
        currentKey = ListShift(&splited).value.s;
        v = TableGet(state, currentKey);
    }

    if (v.type == 1) 
    {
        currentKey = ListShift(&splited).value.s;
        TableInsert((Table*)v.value.p, currentKey, value);
    } 
    else 
    {
        TableInsert(state, currentKey, value);
    }
}

void recursiveUnset(Table *state, char* key) {
    List splited = stringSplit(key, '.');

    if (splited.size == 1) 
    {
        TableRemove(state, key);
        return;
    }

    char* currentKey = ListShift(&splited).value.s;
    Variable v = TableGet(state, currentKey);

    while (splited.size > 1) 
    {
        if (v.type == 1) // Verifica se é do tipo tabela
        {
            state = (Table*)v.value.p;
            currentKey = ListShift(&splited).value.s;
            v = TableGet(state, currentKey);
        } 
        else 
        {
            v.type = -1; // Define um tipo de erro se não for uma tabela
            v.value.s = strdup("Not a table");
            return;
        }
    }

    if (v.type == 1) 
    {
        currentKey = ListShift(&splited).value.s;
        TableRemove((Table*)v.value.p, currentKey);
    } 
    else 
    {
        TableRemove(state, key);
    }
}

// Parser functions
List parse(Table *state, char *cmd) 
{
    List result;
    ListInit(&result);

    List splited = specialSplit(cmd);

    for (int i = 0; i < splited.size; ++i) 
    {
        char* str = splited.data[i].value.s;

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
                v = recursiveGet(state, strdup(str + 1));
            }
            ListPush(&result, v);
        } 
        else if (str[0] == '(') 
        {
            // expression
            Variable v;
            Variable expression;
            List args = parse(state, strndup(str + 1, strlen(str) - 2));

            v = ((Function)TableGet(state, "eval").value.p)(state, &args);
            ListPush(&result, v);
        } 
        else if ((str[0] >= '0' && str[0] <= '9') || str[0] == '-') 
        {
            // Number
            Variable v;
            v.value.f = atof(str);
            v.type = 2;
            ListPush(&result, v);
        } 
        else 
        {
            // string
            Variable v;
            v.value.s = strdup(str);
            v.type = 3;
            ListPush(&result, v);
        }

        free(str);
    }

    ListFree(&splited);

    return result;
}

Variable interpret(Table *state, char* input) 
{
    List args = parse(state, input);
    char* funcName = ListShift(&args).value.s;
    Variable result = Nil;
    Variable var = recursiveGet(state, funcName);
    if (var.type == 4) 
    {
        result = ((Function)var.value.p)(state, &args);
        ListFree(&args);
        return result;
    } 
    else 
    {
        result.type = -1;
        char* ___err = (char*)malloc(19 + strlen(funcName));
        sprintf(___err, "Unknown function %s", funcName);
        result.value.s = ___err;
    }
    ListFree(&args);
    return result;
}

Variable bulkInterpret(Table *state, const char* input) 
{
    List splited = stringSplit(input, ';');
    Variable result = Nil;
    while (splited.size > 0) 
    {
        result = interpret(state, ListShift(&splited).value.s);
        if (result.type != 0) 
        {
            break;
        }
    }
    ListFree(&splited);
    return result;
}

// Functions
Variable _interpret(Table *state, List* args) 
{
    Variable result = Nil;
    if (args->size == 1) 
    {
        return bulkInterpret(state, ListShift(&*args).value.s);
    }
    else 
    {
        char* funcName = ListShift(&*args).value.s;
        Variable var = recursiveGet(state, funcName);
        if (var.type == 4) 
        {
            result = ((Function)var.value.p)(state, args);
            ListFree(&*args);
            return result;
        } 
        else 
        {
            result.type = -1;
            char* ___err = (char*)malloc(19 + strlen(funcName));
            sprintf(___err, "Unknown function %s", funcName);
            result.value.s = ___err;
        }
        ListFree(&*args);
        return result;
    }
}

Variable _table(Table *state, List* args) 
{
    Table* newTable = (Table*)malloc(sizeof(Table));
    TableInit(newTable);
    return (Variable){.type = 1, .value = {.p = newTable}};
}

Variable _list(Table *state, List* args) 
{
    List array;
    ListInit(&array);
    while (args->size > 0) 
    {
        ListPush(&array, ListShift(&*args));
    }
    return (Variable){.type = 5, .value = {.p = &array}};
}

Variable _unset(Table *state, List* args) 
{
    Variable _key = ListShift(&*args);
    recursiveUnset(state, _key.value.s);
    return Nil;
}

Variable _set(Table *state, List* args) 
{
    Variable _key = ListShift(&*args);
    Variable _value = ListShift(&*args);
    recursiveSet(state, _key.value.s, _value);

    return Nil;
}

Variable _print(Table *state, List* args) 
{
    while (args->size > 0) 
    {
        Variable v = ListShift(&*args);
        if (v.type == 2) 
        {
            printf("%f ", v.value.f);
        }
        else if (v.type == 3) 
        {
            printf("%s ", v.value.s);
        }
        else if (v.type == 1) 
        {
            printf("[table] ");
        }
        else if (v.type == 4) 
        {
            printf("[function] ");
        }
    }
    printf("\n");
    return Nil;
}

Variable _ls(Table *_state, List* args) 
{
    Table *state;
    if (args->size == 0) 
    {
        state = _state;
    }
    else
    {
        Variable _ = ListShift(&*args);
        //printf("_ type = %d\n", _.type);
        //printf("_ value = %s\n", _.value.s);
        if (_.type == 1) 
        {
            state = _.value.p;
        }
        else
        {
            return Nil;
        }
    }

    for (int i = 0; i < state->size; i++) 
    {
        switch (state->ValueStack[i].type) 
        {
            case -1:
                printf("[error] %s : %s \n", state->keys[i], state->ValueStack[i].value.s);
                break;
            case 0:
                printf("[void] %s\n", state->keys[i]);
                break;
            case 1:
                printf("[table] %s\n", state->keys[i], state->ValueStack[i].value.p);
                break;
            case 2:
                printf("[number] %s : %f\n", state->keys[i], state->ValueStack[i].value.f);
                break;
            case 3:
                printf("[string] %s : %s\n", state->keys[i], state->ValueStack[i].value.s);
                break;
            case 4:
                printf("[function] %s : %p\n", state->keys[i], state->ValueStack[i].value.p);
                break;
        } 
    }
    return Nil;
}

Variable __exit(Table *state, List* args) 
{
    exit(0);
}

// conditions functions
Variable _equals(Table *state, List* args) 
{
    Variable result = Nil;
    Variable a = ListShift(&*args);
    Variable b = ListShift(&*args);
    if (a.type == 2 && b.type == 2) 
    {
        result.value.f = a.value.f == b.value.f;
    } 
    else if (a.type == 3 && b.type == 3) 
    {
        result.value.f = strcmp(a.value.s, b.value.s) == 0;
    } 
    else 
    {
        result.value.f = 0;
    }
    result.type = 2;
    return result;
}

Variable _notEquals(Table *state, List* args) 
{
    Variable result = _equals(state, args);
    result.value.f = !result.value.f;
    return result;
}

Variable _greaterThan(Table *state, List* args) 
{
    Variable result = Nil;
    Variable a = ListShift(&*args);
    Variable b = ListShift(&*args);
    if (a.type == 2 && b.type == 2) 
    {
        result.value.f = a.value.f > b.value.f;
    } 
    else 
    {
        result.value.f = 0;
    }
    result.type = 2;
    return result;
}

Variable _lessThan(Table *state, List* args) 
{
    Variable result = Nil;
    Variable a = ListShift(&*args);
    Variable b = ListShift(&*args);
    if (a.type == 2 && b.type == 2) 
    {
        result.value.f = a.value.f < b.value.f;
    } 
    else 
    {
        result.value.f = 0;
    }
    result.type = 2;
    return result;
}

Variable _greaterOrEqual(Table *state, List* args) 
{
    Variable result = Nil;
    Variable a = ListShift(&*args);
    Variable b = ListShift(&*args);
    if (a.type == 2 && b.type == 2) 
    {
        result.value.f = a.value.f >= b.value.f;
    } 
    else 
    {
        result.value.f = 0;
    }
    result.type = 2;
    return result;
}

Variable _lessOrEqual(Table *state, List* args) 
{
    Variable result = Nil;
    Variable a = ListShift(&*args);
    Variable b = ListShift(&*args);
    if (a.type == 2 && b.type == 2) 
    {
        result.value.f = a.value.f <= b.value.f;
    } 
    else 
    {
        result.value.f = 0;
    }
    result.type = 2;
    return result;
}


// math functions
Variable _add(Table *state, List* args) 
{
    Variable result = Nil;
    Variable a = ListShift(&*args);
    Variable b = ListShift(&*args);
    result.value.f = a.value.f + b.value.f;
    result.type = 2;
    return result;
}

Variable _sub(Table *state, List* args) 
{
    Variable result = Nil;
    Variable a = ListShift(&*args);
    Variable b = ListShift(&*args);
    result.value.f = a.value.f - b.value.f;
    result.type = 2;
    return result;
}

Variable _mul(Table *state, List* args) 
{
    Variable result = Nil;
    result.value.f = ListShift(&*args).value.f * ListShift(&*args).value.f;
    result.type = 2;
    return result;
}

Variable _div(Table *state, List* args) 
{
    Variable result = Nil;
    Variable a = ListShift(&*args);
    Variable b = ListShift(&*args);
    result.value.f = ListShift(&*args).value.f / ListShift(&*args).value.f;
    result.type = 2;
    
    return result;
}

Variable _mod(Table *state, List* args) 
{
    Variable result = Nil;
    result.value.f = (int)ListShift(&*args).value.f % (int)ListShift(&*args).value.f;
    result.type = 2;
    return result;
}

Variable _pow(Table *state, List* args) 
{
    Variable result = Nil;
    result.value.f = pow(ListShift(&*args).value.f, ListShift(&*args).value.f);
    result.type = 2;
    return result;
}

Variable _len(Table *state, List* args) 
{
    Variable _ = ListShift(&*args);
    if (_.type == 3) 
    {
        return (Variable){.type = 2, .value = {.f = strlen(_.value.s)}};
    } 
    else if (_.type == 1) 
    {
        Table* _table = _.value.p;
        return (Variable){.type = 2, .value = {.f = _table->size}};
    }
    else if (_.type == 2) 
    {
        //length of number in string,
        return (Variable){.type = 2, .value = {.f = floor(log10(_.value.f) + 1)}};
    }
    return Nil;
}


// math functions
Variable _ceil(Table *state, List* args) 
{
    Variable result = Nil;
    result.value.f = ceil(ListShift(&*args).value.f);
    result.type = 2;
    return result;
}

Variable _floor(Table *state, List* args) 
{
    Variable result = Nil;
    result.value.f = floor(ListShift(&*args).value.f);
    result.type = 2;
    return result;
}

Variable _round(Table *state, List* args) 
{
    Variable result = Nil;
    result.value.f = round(ListShift(&*args).value.f);
    result.type = 2;
    return result;
}

// string functions
Variable _string_find(Table *state, List* args) 
{
    Variable result = Nil;
    char* str = ListShift(&*args).value.s;
    char* substr = ListShift(&*args).value.s;
    int index = -1;
    char* found = strstr(str, substr);
    if (found != NULL) 
    {
        index = found - str;
    }
    result.value.f = index;
    result.type = 2;
    return result;
}

Variable _string_replace(Table *state, List* args) 
{
    Variable result = Nil;
    char* str = ListShift(&*args).value.s;
    char* substr = ListShift(&*args).value.s;
    char* replacement = ListShift(&*args).value.s;
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
    result.type = 3;
    return result;
}

Variable _string_byte(Table *state, List* args) 
{
    Variable result = Nil;
    char* str = ListShift(&*args).value.s;
    int index = (int)ListShift(&*args).value.f;
    result.value.f = str[index];
    result.type = 2;
    return result;
}

Variable _string_char(Table *state, List* args) 
{
    Variable result = Nil;
    char* str = (char*)malloc(2);
    str[0] = (char)ListShift(&*args).value.f;
    str[1] = '\0';
    result.value.s = str;
    result.type = 3;
    return result;
}

Variable _string_substring(Table *state, List* args) 
{
    Variable result = Nil;
    char* str = ListShift(&*args).value.s;
    int start = (int)ListShift(&*args).value.f;
    int length = (int)ListShift(&*args).value.f;
    char* newStr = (char*)malloc(length + 1);
    strncpy(newStr, str + start, length);
    newStr[length] = '\0';
    result.value.s = newStr;
    result.type = 3;
    return result;
}

Variable _string_concat(Table *state, List* args)
{
    Variable a = ListShift(&*args);
    Variable b = ListShift(&*args);
    Variable result = Nil;
    char* str; 
    if (b.type == 2)
    {
        str = (char*)malloc(strlen(a.value.s) + 15);
        sprintf(str, "%s%.2f", a.value.s, b.value.f);
    }
    else if (b.type == 3)
    {
        str = (char*)malloc(strlen(a.value.s) + strlen(b.value.s) + 1);
        sprintf(str, "%s%s", a.value.s, b.value.s);
    }
    result.value.s = str;
    result.type = 3;
    return result;
}

Variable _string_split(Table *state, List* args)
{
    Variable str = ListShift(&*args);
    Variable delim = ListShift(&*args);
    Variable result = Nil;
    List splited = stringSplit(str.value.s, delim.value.s[0]);
    Table* newTable = createNewTable();
    for (int i = 0; i < splited.size; i++) 
    {
        char* key = (char*)malloc(11);
        sprintf(key, "%d", i);
        Variable _var = ListShift(&splited);
        TableInsert(newTable, key, _var);
    }
    result.type = 1;
    result.value.p = newTable;
    ListFree(&splited);
    return result;
}

// table functions
Variable _table_keys(Table *state, List* args) 
{
    Variable result = Nil;
    Variable table = ListShift(&*args);
    Table* _table = (Table*)table.value.p;
    Table* newTable = createNewTable();
    for (int i = 0; i < _table->size; i++) 
    {
        char* key = (char*)malloc(strlen(_table->keys[i]) + 1);
        strcpy(key, _table->keys[i]);
        Variable _var;
        _var.type = 3;
        _var.value.s = key;
        TableInsert(newTable, key, _var);
    }
    result.type = 1;
    result.value.p = newTable;
    return result;
}

// list functions
Variable _list_push(Table *state, List* args) 
{
    Variable result = Nil;
    Variable list = ListShift(&*args);
    Variable value = ListShift(&*args);
    ListPush((List*)list.value.p, value);
    return result;
}

Variable _list_pop(Table *state, List* args) 
{
    Variable result = Nil;
    Variable list = ListShift(&*args);
    return ListPop((List*)list.value.p);
}

Variable _list_shift(Table *state, List* args) 
{
    Variable result = Nil;
    Variable list = ListShift(&*args);
    return ListShift((List*)list.value.p);
}

Variable _list_unshift(Table *state, List* args) 
{
    Variable result = Nil;
    Variable list = ListShift(&*args);
    Variable value = ListShift(&*args);
    return ListUnshift((List*)list.value.p, value);
}

Variable _list_free(Table *state, List* args) 
{
    Variable result = Nil;
    Variable list = ListShift(&*args);
    ListFree((List*)list.value.p);
    return result;
}

Variable _list_move(Table *state, List* args) 
{
    Variable result = Nil;
    Variable list = ListShift(&*args);
    MaxSize i1 = (MaxSize)ListShift(&*args).value.f;
    MaxSize i2 = (MaxSize)ListShift(&*args).value.f;
    ListMove((List*)list.value.p, i1, i2);
    return result;
}

Variable _list_insert(Table *state, List* args) 
{
    Variable result = Nil;
    Variable list = ListShift(&*args);
    MaxSize i = (MaxSize)ListShift(&*args).value.f;
    Variable value = ListShift(&*args);
    ListInsert((List*)list.value.p, i, value);
    return result;
}

Variable _list_remove(Table *state, List* args) 
{
    Variable result = Nil;
    Variable list = ListShift(&*args);
    MaxSize i = (MaxSize)ListShift(&*args).value.f;
    return ListRemove((List*)list.value.p, i);
}

// Table functions
void registerFunction(Table *state, char* name, Function func) 
{
    Variable tempFunc = Nil;
    tempFunc.type = 4;
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
            if (result.type == 2) 
            {
                printf("returned: %f\n", result.value.f);
            } 
            else if (result.type == 3) 
            {
                printf("returned: %s\n", result.value.s);
            }
        }
        else if(result.type == -1)
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
    List args;
    ListInit(&args);

    printf("Table size: %d\n", sizeof(Table));

    for (int i = 1; i < argc; i++)
    {
        Variable v = {.type = 3, .value = {.s = argv[i]}};
        ListPush(&args, v);
    }

    while (args.size > 0)
    {
        char* arg = ListShift(&args).value.s;
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

    

    registerFunction(&state, "set", _set);
    registerFunction(&state, "unset", _unset);
    registerFunction(&state, "print", _print);
    registerFunction(&state, "eval", _interpret);
    registerFunction(&state, "ls", _ls);
    registerFunction(&state, "exit", __exit);
    registerFunction(&state, "table", _table);
    registerFunction(&state, "list", _list);



    interpret(&state, "set string (table)");
    registerFunction(&state, "string.find", _string_find);
    registerFunction(&state, "string.replace", _string_replace);
    registerFunction(&state, "string.byte", _string_byte);
    registerFunction(&state, "string.char", _string_char);
    registerFunction(&state, "string.substr", _string_substring);
    registerFunction(&state, "string.concat", _string_concat);
    registerFunction(&state, "string.split", _string_split);
    



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