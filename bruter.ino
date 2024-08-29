extern "C"
{
    #include "./src/bruter.h"
    
}

Int _ino_serial_begin(VirtualMachine *vm, VariableList *args)
{
    Variable _ref = StackShift(*args);
    Serial.begin((Int)_ref.value.number);
    freerawvar(_ref);
    return -1;
}

Int _ino_print(VirtualMachine *vm, VariableList *args)
{
    while (args->size > 0)
    {
        Variable var = StackShift(*args);
        Int _type = -1;
        printf("Type: %d\n", var.type);

        Value temp = var.value;
        _type = var.type;

        if (var.type == TYPE_POINTER)
        {
            _type = vm->typestack->data[(Int)var.value.number];
            temp = vm->stack->data[(Int)var.value.number];
        }
        
        if (_type == TYPE_NUMBER)
        {
            printf("number: %f\n", temp.number);
        }
        else if (_type == TYPE_STRING)
        {
            printf("%s\n", temp.string);
        }
        else if (_type == TYPE_LIST)
        {
            printf("[");
            IntList *list = (IntList*)temp.pointer;
            for (Int i = 0; i < (list->size-1); i++)
            {
                printf("%d, ", list->data[i]);
            }
            printf("%d", list->data[list->size-1]);
            printf("]\n");
        }
        else if (_type == TYPE_ERROR)
        {
            printf("Error: %s\n", temp.string);
        }
        else if (_type == TYPE_FUNCTION)
        {
            printf("Function : %p\n", temp.pointer);
        }
        else
        {
            printf("Unknown type\n");
        }
        freerawvar(var);
    }
    return -1;
}

Int _ino_delay(VirtualMachine *vm, VariableList *args)
{
    Variable _ref = StackShift(*args);
    delay((Int)_ref.value.number);
    return -1;
}

Int _ino_millis(VirtualMachine *vm, VariableList *args)
{
    return millis();
}

Int _ino_tone(VirtualMachine *vm, VariableList *args)
{
    Variable _ref = StackShift(*args);
    Variable _ref2 = StackShift(*args);
    tone((int)_ref.value.number, (int)_ref2.value.number);
    freerawvar(_ref);
    freerawvar(_ref2);
    return -1;
}


Int _ino_noTone(VirtualMachine *vm, VariableList *args)
{
    Variable _ref = StackShift(*args);
    noTone((Int)_ref.value.number);
    freerawvar(_ref);
    return -1;
}

Int _ino_pinmode(VirtualMachine *vm, VariableList *args)
{
    Variable _ref = StackShift(*args);
    Variable _ref2 = StackShift(*args);
    pinMode((Int)_ref.value.number, (Int)_ref2.value.number);
    freerawvar(_ref);
    freerawvar(_ref2);
    return -1;
}

Int _ino_digitalwrite(VirtualMachine *vm, VariableList *args)
{
    Variable _ref = StackShift(*args);
    Variable _ref2 = StackShift(*args);
    digitalWrite((Int)_ref.value.number, (Int)_ref2.value.number);
    freerawvar(_ref);
    freerawvar(_ref2);
    return -1;
}

Int _ino_digitalread(VirtualMachine *vm, VariableList *args)
{
    Variable _ref = StackShift(*args);
    return digitalRead((Int)_ref.value.number);
}

Int _ino_analogwrite(VirtualMachine *vm, VariableList *args)
{
    Variable _ref = StackShift(*args);
    Variable _ref2 = StackShift(*args);
    analogWrite((Int)_ref.value.number, (Int)_ref2.value.number);
    freerawvar(_ref);
    freerawvar(_ref2);
    return -1;
}

Int _ino_analogread(VirtualMachine *vm, VariableList *args)
{
    Variable _ref = StackShift(*args);
    return analogRead((Int)_ref.value.number);
}



class Bruter
{
    public:
    void free()
    {
        freevm(this->vm);
    };
    void registerFunction(char *name, Function func)
    {
        spawnFunction(this->vm, name, func);
    };
    Int run(char *str)
    {
        return eval(this->vm, str);
    };
    VirtualMachine *vm;
    Bruter()
    {
        this->vm = makeVM();
        initStd(this->vm);
        this->registerFunction((char*)"Serial.begin", _ino_serial_begin);
        this->registerFunction((char*)"Serial.println", _ino_print);
        this->registerFunction((char*)"delay", _ino_delay);
        this->registerFunction((char*)"millis", _ino_millis);
        this->registerFunction((char*)"tone", _ino_tone);
        this->registerFunction((char*)"noTone", _ino_noTone);
        this->registerFunction((char*)"pinMode", _ino_pinmode);
        this->registerFunction((char*)"digitalWrite", _ino_digitalwrite);
        this->registerFunction((char*)"digitalRead", _ino_digitalread);
        this->registerFunction((char*)"analogWrite", _ino_analogwrite);
        this->registerFunction((char*)"analogRead", _ino_analogread);

    }
};

Bruter *session = new Bruter();

void setup()
{
    Serial.begin(115200);
    session->run((char*)"@set str !(looping);");
    session->run((char*)"@Serial.println @str;");
    session->run((char*)"@Serial.println @str;");
    //session->free();
    //delete session;
}

void loop()
{
    delay(1000);

    session->run((char*)"@Serial.println @str;");
}