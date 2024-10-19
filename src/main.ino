extern "C"
{
    #include "./src/bruter.h"
    
}

Int _ino_serial_begin(VirtualMachine *vm, IntList *args)
{
    Int _ref = stack_shift(*args);
    Serial.begin((Int)vm->stack->data[_ref].number);
    return -1;
}

Int _ino_print(VirtualMachine *vm, IntList *args)
{
    Int var = stack_shift(*args);
    Int _type = -1;
    //printf("Type: %d\n", vm->typestack->data[var]);

    Value temp = vm->stack->data[var];
    _type = vm->typestack->data[var];
    
    if (_type == TYPE_NUMBER)
    {
        printf("%f\n", temp.number);
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
    else if (_type == TYPE_BUILTIN)
    {
        printf("(function)%p\n", temp.pointer);
    }
    else if (_type == TYPE_FUNCTION)
    {
        printf("(function)%s\n", temp.string);
    }
    else
    {
        printf("(unknown)\n");
    }
    
    return -1;
}

Int _ino_delay(VirtualMachine *vm, IntList *args)
{
    Int _ref = stack_shift(*args);
    delay((Int)vm->stack->data[_ref].number);
    return -1;
}

Int _ino_millis(VirtualMachine *vm, IntList *args)
{
    return millis();
}

Int _ino_tone(VirtualMachine *vm, IntList *args)
{
    Int _ref = stack_shift(*args);
    Int _ref2 = stack_shift(*args);
    tone((int)vm->stack->data[_ref].number, (int)vm->stack->data[_ref2].number);
    return -1;
}


Int _ino_noTone(VirtualMachine *vm, IntList *args)
{
    Int _ref = stack_shift(*args);
    noTone((Int)vm->stack->data[_ref].number);
    return -1;
}

Int _ino_pinmode(VirtualMachine *vm, IntList *args)
{
    Int _ref = stack_shift(*args);
    Int _ref2 = stack_shift(*args);
    pinMode((Int)vm->stack->data[_ref].number, (Int)vm->stack->data[_ref2].number);
    return -1;
}

Int _ino_digitalwrite(VirtualMachine *vm, IntList *args)
{
    Int _ref = stack_shift(*args);
    Int _ref2 = stack_shift(*args);
    digitalWrite((Int)vm->stack->data[_ref].number, (Int)vm->stack->data[_ref2].number);
    return -1;
}

Int _ino_digitalread(VirtualMachine *vm, IntList *args)
{
    Int _ref = stack_shift(*args);
    return digitalRead((Int)vm->stack->data[_ref].number);
}

Int _ino_analogwrite(VirtualMachine *vm, IntList *args)
{
    Int _ref = stack_shift(*args);
    Int _ref2 = stack_shift(*args);
    analogWrite((Int)vm->stack->data[_ref].number, (Int)vm->stack->data[_ref2].number);
    return -1;
}

Int _ino_analogread(VirtualMachine *vm, IntList *args)
{
    Int _ref = stack_shift(*args);
    return analogRead((Int)vm->stack->data[_ref].number);
}



class Bruter
{
    public:
    void free()
    {
        free_vm(this->vm);
    };
    void registerFunction(char *name, Function func)
    {
        spawn_builtin(this->vm, name, func);
    };
    Int run(char *str)
    {
        return eval(this->vm, str);
    };
    VirtualMachine *vm;
    Bruter()
    {
        this->vm = make_vm();
        // <libraries init>
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
    session->run((char*)"hash.set 'str' 'looping';");
    session->run((char*)"Serial.println str;");
    session->run((char*)"Serial.println str;");
}

void loop()
{
    delay(1000);

    session->run((char*)"Serial.println str;");
}