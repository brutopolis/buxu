// this file is the utility tool for bruter

#ifndef ARDUINO

// bruter header
#include "./bruter.h"

int main(int argc, char **argv)
{
    StringList *args = make_string_list();
    for (int i = 1; i < argc; i++)
    {
        stack_push(*args, argv[i]);
    }
    VirtualMachine *vm = make_vm();

    // <libraries init>

    // read file pointed by argv[1]
    if (args->size == 0)
    {
        interpret(vm, "repl");
    }
    else if (args->size == 1)
    {
        char *_code = readfile(argv[1]);
        if (_code == NULL)
        {
            printf("file %s not found\n", argv[1]);
            return 1;
        }
        Int filepathindex = new_var(vm);
        // path without file name
        vm->typestack->data[filepathindex] = TYPE_STRING;
        // remove file name
        char *path = stack_shift(*args);
        char *last = strrchr(path, '/');
        if (last == NULL)
        {
            vm->stack->data[filepathindex].string = str_duplicate("");
        }
        else
        {
            vm->stack->data[filepathindex].string = str_nduplicate(path, last - path + 1);
        }
        hash_set(vm, "file.path", filepathindex);
        Int result = eval(vm, _code);
        free(_code);
        char * str = str_format("print 'returned:' @%d", result);
        eval(vm, str);
        free(str);
    }
    stack_free(*args);
    free_vm(vm);
}

#else
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
    else if (_type == TYPE_BUILTIN)
    {
        printf("(function)%p\n", temp.pointer);
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

#endif