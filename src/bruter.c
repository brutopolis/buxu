#include "bruter.h"

Table* createNewTable() {
    Table* newTable = (Table*)malloc(sizeof(Table));
    HashTableInit(*newTable);
    return newTable;
}

StringStack specialSplit(const char* str) {
    StringStack stack;
    StackInit(stack);

    int n = strlen(str);
    char token[1024];
    int tokenIndex = 0;
    int insideParens = 0;

    for (int i = 0; i <= n; ++i) {
        char c = str[i];

        // Check for delimiters
        if (c == '(') {
            insideParens++;
        } else if (c == ')') {
            if (insideParens > 0) {
                insideParens--;
            }
        }

        // Add character to current token
        if (i < n && (!isspace(c) || insideParens > 0)) {
            token[tokenIndex++] = c;
        } else if (tokenIndex > 0) {
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

StringStack stringSplit(const char* str, const char delim) {
    StringStack splited;
    StackInit(splited);
    int n = 0;
    //split string by ; but not in parentheses
    while (n < strlen(str)) {
        char token[1024];
        int tokenIndex = 0;
        int insideParens = 0;
        while (n < strlen(str)) {
            char c = str[n];
            if (c == '(') {
                insideParens++;
            } else if (c == ')') {
                if (insideParens > 0) {
                    insideParens--;
                }
            }
            if (c == delim && insideParens == 0) {
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

Variable recursiveGet(Table *state, char* key) {
    StringStack splited = stringSplit(key, '.');
    char* currentKey = StackShift(splited);
    Variable v = HashTableGet(Variable, *state, currentKey);

    while (splited.size > 0) {
        if (v.type == 1) // Verifica se é do tipo tabela
        {
            currentKey = StackShift(splited);
            v = HashTableGet(Variable, *(Table*)v.value.p, currentKey);
        } else {
            v.type = -1; // Define um tipo de erro se não for uma tabela
            v.value.s = strdup("Not a table");
            break;
        }
    }

    return v;
}

void recursiveSet(Table *state, char* key, Variable value) {
    StringStack splited = stringSplit(key, '.');

    if (splited.size == 1) {
        HashTableInsert(*state, key, value);
        return;
    }

    char* currentKey = StackShift(splited);
    Variable v = HashTableGet(Variable, *state, currentKey);

    while (splited.size > 1) {
        if (v.type != 1) 
        {
            Table *newTable = createNewTable();
            Variable newVar;
            newVar.type = 1;
            newVar.value.p = newTable;
            HashTableInsert(*state, currentKey, newVar);
            v = newVar;
        }

        state = (Table*)v.value.p;
        currentKey = StackShift(splited);
        v = HashTableGet(Variable, *state, currentKey);
    }

    if (v.type == 1) {
        currentKey = StackShift(splited);
        HashTableInsert(*(Table*)v.value.p, currentKey, value);
    } else {
        HashTableInsert(*state, currentKey, value);
    }
}

void recursiveUnset(Table *state, char* key) {
    StringStack splited = stringSplit(key, '.');

    if (splited.size == 1) {
        HashTableRemove(*state, key);
        return;
    }

    char* currentKey = StackShift(splited);
    Variable v = HashTableGet(Variable, *state, currentKey);

    while (splited.size > 1) {
        if (v.type == 1) // Verifica se é do tipo tabela
        {
            state = (Table*)v.value.p;
            currentKey = StackShift(splited);
            v = HashTableGet(Variable, *state, currentKey);
        } else {
            v.type = -1; // Define um tipo de erro se não for uma tabela
            v.value.s = strdup("Not a table");
            return;
        }
    }

    if (v.type == 1) {
        currentKey = StackShift(splited);
        HashTableRemove(*(Table*)v.value.p, currentKey);
    } else {
        HashTableRemove(*state, key);
    }
}

List parse(Table *state, char *cmd) 
{
    List result;
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
                v = recursiveGet(state, strdup(str + 1));
            }
            StackPush(result, v);
        } 
        else if (str[0] == '(') 
        {
            // expression
            Variable v;
            Variable expression;
            List args = parse(state, strndup(str + 1, strlen(str) - 2));

            v = ((Function)HashTableGet(Variable, *state, "eval").value.p)(state, &args);
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

Variable interpret(Table *state, char* input) 
{
    List args = parse(state, input);
    char* funcName = StackShift(args).value.s;
    Variable result = Nil;
    Variable var = recursiveGet(state, funcName);
    if (var.type == 4) 
    {
        result = ((Function)var.value.p)(state, &args);
        StackFree(args);
        return result;
    } 
    else 
    {
        result.type = -1;
        char* ___err = (char*)malloc(19 + strlen(funcName));
        sprintf(___err, "Unknown function %s", funcName);
        result.value.s = ___err;
    }
    StackFree(args);
    return result;
}

Variable bulkInterpret(Table *state, const char* input) {
    StringStack splited = stringSplit(input, ';');
    Variable result = Nil;
    while (splited.size > 0) {
        result = interpret(state, StackShift(splited));
        if (result.type != 0) {
            break;
        }
    }
    StackFree(splited);
    return result;
}

Variable _interpret(Table *state, List* args) 
{
    Variable result = Nil;
    if (args->size == 1) 
    {
        return bulkInterpret(state, StackShift(*args).value.s);
    } 
    else 
    {
        char* funcName = StackShift(*args).value.s;
        Variable var = recursiveGet(state, funcName);
        if (var.type == 4) 
        {
            result = ((Function)var.value.p)(state, args);
            StackFree(*args);
            return result;
        } 
        else 
        {
            result.type = -1;
            char* ___err = (char*)malloc(19 + strlen(funcName));
            sprintf(___err, "Unknown function %s", funcName);
            result.value.s = ___err;
        }
        StackFree(*args);
        return result;
    }
}

Variable _table(Table *state, List* args) 
{
    Table* newTable = (Table*)malloc(sizeof(Table));
    HashTableInit(*newTable);
    return (Variable){.type = 1, .value = {.p = newTable}};
}

Variable _unset(Table *state, List* args) 
{
    Variable _key = StackShift(*args);
    recursiveUnset(state, _key.value.s);
    return Nil;
}

Variable _set(Table *state, List* args) 
{
    Variable _key = StackShift(*args);
    Variable _value = StackShift(*args);
    recursiveSet(state, _key.value.s, _value);

    return Nil;
}

Variable _print(Table *state, List* args) 
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
        Variable _ = StackShift(*args);
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
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
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
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
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
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
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
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
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
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
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
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
    if (a.type == 3)// like javascript + operator for strings
    {
        if (b.type == 2)
        {
            char* str = (char*)malloc(strlen(a.value.s) + 15);
            sprintf(str, "%s%.2f", a.value.s, b.value.f);
            result.value.s = str;
            result.type = 3;
            return result;
        }
        else if (b.type == 3)
        {
            char* str = (char*)malloc(strlen(a.value.s) + strlen(b.value.s) + 1);
            sprintf(str, "%s%s", a.value.s, b.value.s);
            result.value.s = str;
            result.type = 3;
            return result;
        }
    }
    else if (a.type == 2 && b.type == 2) 
    {
        result.value.f = a.value.f + b.value.f;
        result.type = 2;
        return result;
    }
    else if (a.type == 1 && b.type == 1) 
    {
        Table* newTable = createNewTable();
        Table *tableA = a.value.p;
        Table *tableB = b.value.p;
        for (int i = 0; i < tableA->size; i++) 
        {
            HashTableInsert(*newTable, tableA->keys[i], tableA->ValueStack[i]);
        }

        for (int i = 0; i < tableB->size; i++) 
        {
            HashTableInsert(*newTable, tableB->keys[i], tableB->ValueStack[i]);
        }
        result.type = 1;
        result.value.p = newTable;
        return result;
    }
    return result;
}

Variable _sub(Table *state, List* args) 
{
    Variable result = Nil;
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
    if (a.type == 3 && b.type == 3) // remove b from a
    {
        char* str = (char*)malloc(strlen(a.value.s) - strlen(b.value.s) + 1);
        char* found = strstr(a.value.s, b.value.s);
        if (found != NULL) 
        {
            strncpy(str, a.value.s, found - a.value.s);
            strcpy(str + (found - a.value.s), found + strlen(b.value.s));
            result.value.s = str;
            result.type = 3;
            return result;
        }
    }
    else if (a.type == 2) 
    {
        result.value.f = a.value.f - b.value.f;
        result.type = 2;
        return result;
    }
    else if (a.type == 1 && b.type == 1) 
    {
        Table* newTable = createNewTable();
        Table *tableA = a.value.p;
        Table *tableB = b.value.p;
        for (int i = 0; i < tableA->size; i++) 
        {
            HashTableInsert(*newTable, tableA->keys[i], tableA->ValueStack[i]);
        }

        for (int i = 0; i < tableB->size; i++) 
        {
            HashTableRemove(*newTable, tableB->keys[i]);
        }
        result.type = 1;
        result.value.p = newTable;
        return result;
    }
    
    return result;
}

Variable _mul(Table *state, List* args) 
{
    Variable result = Nil;
    result.value.f = StackShift(*args).value.f * StackShift(*args).value.f;
    result.type = 2;
    return result;
}

Variable _div(Table *state, List* args) 
{
    Variable result = Nil;
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
    if (a.type == 3 && b.type == 3) // split a by delimiter b
    {
        StringStack splited = stringSplit(a.value.s, b.value.s[0]);
        Table* newTable = createNewTable();
        for (int i = 0; i < splited.size; i++) 
        {
            char* key = (char*)malloc(11);
            sprintf(key, "%d", i);
            Variable _var = (Variable){.type = 3, .value = {.s = splited.data[i]}};
            HashTableInsert(*newTable, key, _var);
        }
        result.type = 1;
        result.value.p = newTable;
        StackFree(splited);
        return result;
    }
    else
    {
        result.value.f = StackShift(*args).value.f / StackShift(*args).value.f;
        result.type = 2;
    }
    
    return result;
}

Variable _mod(Table *state, List* args) 
{
    Variable result = Nil;
    result.value.f = (int)StackShift(*args).value.f % (int)StackShift(*args).value.f;
    result.type = 2;
    return result;
}

Variable _pow(Table *state, List* args) 
{
    Variable result = Nil;
    result.value.f = pow(StackShift(*args).value.f, StackShift(*args).value.f);
    result.type = 2;
    return result;
}

Variable _len(Table *state, List* args) 
{
    Variable _ = StackShift(*args);
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
    result.value.f = ceil(StackShift(*args).value.f);
    result.type = 2;
    return result;
}

Variable _floor(Table *state, List* args) 
{
    Variable result = Nil;
    result.value.f = floor(StackShift(*args).value.f);
    result.type = 2;
    return result;
}

Variable _round(Table *state, List* args) 
{
    Variable result = Nil;
    result.value.f = round(StackShift(*args).value.f);
    result.type = 2;
    return result;
}

// string functions
Variable _string_find(Table *state, List* args) 
{
    Variable result = Nil;
    char* str = StackShift(*args).value.s;
    char* substr = StackShift(*args).value.s;
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
    char* str = StackShift(*args).value.s;
    char* substr = StackShift(*args).value.s;
    char* replacement = StackShift(*args).value.s;
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
    char* str = StackShift(*args).value.s;
    int index = (int)StackShift(*args).value.f;
    result.value.f = str[index];
    result.type = 2;
    return result;
}

Variable _string_char(Table *state, List* args) 
{
    Variable result = Nil;
    char* str = (char*)malloc(2);
    str[0] = (char)StackShift(*args).value.f;
    str[1] = '\0';
    result.value.s = str;
    result.type = 3;
    return result;
}

Variable _string_substring(Table *state, List* args) 
{
    Variable result = Nil;
    char* str = StackShift(*args).value.s;
    int start = (int)StackShift(*args).value.f;
    int length = (int)StackShift(*args).value.f;
    char* newStr = (char*)malloc(length + 1);
    strncpy(newStr, str + start, length);
    newStr[length] = '\0';
    result.value.s = newStr;
    result.type = 3;
    return result;
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
    StringStack args;
    StackInit(args);

    for (int i = 1; i < argc; i++)
    {
        StackPush(args, argv[i]);
    }

    while (args.size > 0)
    {
        char* arg = StackShift(args);
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
    
    registerFunction(&state, "+", _add);
    registerFunction(&state, "-", _sub);
    registerFunction(&state, "*", _mul);
    registerFunction(&state, "/", _div);
    registerFunction(&state, "\%", _mod);
    registerFunction(&state, "^", _pow);
    registerFunction(&state, "#", _len);

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


    interpret(&state, "set string (table)");
    registerFunction(&state, "string.find", _string_find);
    registerFunction(&state, "string.replace", _string_replace);
    registerFunction(&state, "string.byte", _string_byte);
    registerFunction(&state, "string.char", _string_char);
    registerFunction(&state, "string.substr", _string_substring);


    printf("Table size: %d\n", sizeof(Table));

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