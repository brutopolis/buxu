extern "C"
{
    #include "./src/bruter.h"
    
}

Int _ino_serial_begin(VirtualMachine *vm, IntList *args)
{
    Reference _ref = argshift(vm, args);
    Serial.begin((Int)vm->stack->data[_ref.index]->value.number);
    freeref(vm, _ref);
    return -1;
}

Int _ino_print(VirtualMachine *vm, IntList *args)
{
    Reference _ref = argshift(vm, args);
    //print(vm, _ref.index);
    if (vm->stack->data[_ref.index]->type == TYPE_NUMBER)
    {
        Serial.println(vm->stack->data[_ref.index]->value.number);
    }
    else if (vm->stack->data[_ref.index]->type == TYPE_STRING)
    {
        char * temp = toString(static_cast<CharList*>(vm->stack->data[_ref.index]->value.pointer));
        Serial.println(temp);
        free(temp);
    }
    else if (vm->stack->data[_ref.index]->type == TYPE_LIST)
    {
        Serial.print("[");
        IntList *list = (IntList*)vm->stack->data[_ref.index]->value.pointer;
        for (Int i = 0; i < (list->size-1); i++)
        {
            Serial.print(list->data[i]);
            Serial.print(", ");
        }
        Serial.print(list->data[list->size-1]);
        Serial.print("]\n");
    }
    freeref(vm, _ref);
    return -1;
}

Int _ino_delay(VirtualMachine *vm, IntList *args)
{
    Reference _ref = argshift(vm, args);
    delay(vm->stack->data[_ref.index]->value.number);
    freeref(vm, _ref);
    return -1;
}

Int _ino_millis(VirtualMachine *vm, IntList *args)
{
    return newNumber(vm, millis());
}

Int _ino_tone(VirtualMachine *vm, IntList *args)
{
    Reference _ref = argshift(vm, args);
    Reference _ref2 = argshift(vm, args);
    Reference _ref3 = argshift(vm, args);
    tone(vm->stack->data[_ref.index]->value.number, vm->stack->data[_ref2.index]->value.number, vm->stack->data[_ref3.index]->value.number);
    freeref(vm, _ref);
    freeref(vm, _ref2);
    freeref(vm, _ref3);
    return -1;
}

Int _ino_noTone(VirtualMachine *vm, IntList *args)
{
    Reference _ref = argshift(vm, args);
    noTone(vm->stack->data[_ref.index]->value.number);
    freeref(vm, _ref);
    return -1;
}

Int _ino_pinmode(VirtualMachine *vm, IntList *args)
{
    Reference _ref = argshift(vm, args);
    Reference _ref2 = argshift(vm, args);
    pinMode(vm->stack->data[_ref.index]->value.number, vm->stack->data[_ref2.index]->value.number);
    freeref(vm, _ref);
    freeref(vm, _ref2);
    return -1;
}

Int _ino_digitalwrite(VirtualMachine *vm, IntList *args)
{
    Reference _ref = argshift(vm, args);
    Reference _ref2 = argshift(vm, args);
    digitalWrite(vm->stack->data[_ref.index]->value.number, vm->stack->data[_ref2.index]->value.number);
    freeref(vm, _ref);
    freeref(vm, _ref2);
    return -1;
}

Int _ino_digitalread(VirtualMachine *vm, IntList *args)
{
    Reference _ref = argshift(vm, args);
    return digitalRead((Int)vm->stack->data[_ref.index]->value.number);
}

Int _ino_analogwrite(VirtualMachine *vm, IntList *args)
{
    Reference _ref = argshift(vm, args);
    Reference _ref2 = argshift(vm, args);
    analogWrite(vm->stack->data[_ref.index]->value.number, vm->stack->data[_ref2.index]->value.number);
    freeref(vm, _ref);
    freeref(vm, _ref2);
    return -1;
}

Int _ino_analogread(VirtualMachine *vm, IntList *args)
{
    Reference _ref = argshift(vm, args);
    return analogRead((Int)vm->stack->data[_ref.index]->value.number);
}



class Bruter
{
    public:
    void free()
    {
        freeVM(this->vm);
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
    session->run((char*)"set str !(looping);");
    //session->free();
    //delete session;
}

void loop()
{
    delay(1000);

    session->run((char*)"Serial.println @str;");
}