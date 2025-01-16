// this file is the utility tool for bruter

#ifndef ARDUINO

// bruter header
#include "../include/bruter.h"

int main(int argc, char **argv)
{
    Int result = 0;
    StringList *args = make_string_list();
    char* code = NULL;
    char* path = NULL;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
        {
            printf("bruter v%s\n", VERSION);
            return 0;
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            printf("bruter v%s\n", VERSION);
            printf("usage: %s [file]\n", argv[0]);
            printf("  -v, --version\t\tprint version\n");
            printf("  -h, --help\t\tprint this help\n");
            return 0;
        }
        else
        {
            stack_push(*args, argv[i]);
        }
    }
    
    VirtualMachine *vm = make_vm();
    
    // libraries init is not a merely comment
    // <libraries init>

    if (args->size == 0)
    {
        repl(vm);
    }
    else if (args->size > 0) 
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
        register_list(vm, "file.args");
        IntList *fileargs = (IntList*)data(hash_find(vm, "file.args")).pointer;
        while (args->size > 0)
        {
            stack_push(*fileargs, new_string(vm, stack_shift(*args)));
        }
        result = eval(vm, _code, NULL);
        free(_code);
    }
    stack_free(*args);
    free_vm(vm);
    return result;
}

#else
extern "C"
{
    #include "./src/bruter.h"
    
}

Int _ino_serial_begin(VirtualMachine *vm, IntList *args)
{
    Serial.begin((Int)arg(0).number);
    return -1;
}

Int _ino_print(VirtualMachine *vm, IntList *args)
{
    //printf("Type: %d\n", vm->typestack->data[var]);

    Value temp = arg(1);
    Int _type = arg_t(1);
    
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
    delay((Int)arg(0).number);
    return -1;
}

Int _ino_millis(VirtualMachine *vm, IntList *args)
{
    return millis();
}

Int _ino_tone(VirtualMachine *vm, IntList *args)
{
    tone((int)arg(0).number, (int)arg(1).number);
    return -1;
}


Int _ino_noTone(VirtualMachine *vm, IntList *args)
{
    noTone((Int)arg(0).number);
    return -1;
}

Int _ino_pinmode(VirtualMachine *vm, IntList *args)
{
    pinMode((Int)arg(0).number, (Int)arg(1).number);
    return -1;
}

Int _ino_digitalwrite(VirtualMachine *vm, IntList *args)
{
    digitalWrite((Int)arg(0).number, (Int)arg(1).number);
    return -1;
}

Int _ino_digitalread(VirtualMachine *vm, IntList *args)
{
    return digitalRead((Int)arg(0).number);
}

Int _ino_analogwrite(VirtualMachine *vm, IntList *args)
{
    analogWrite((Int)arg(0).number, (Int)arg(1).number);
    return -1;
}

Int _ino_analogread(VirtualMachine *vm, IntList *args)
{
    return analogRead((Int)arg(0).number);
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
        register_builtin(this->vm, name, func);
    };
    Int run(char *str)
    {
        return eval(this->vm, str, NULL);
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
    session->run((char*)"hash.set 'str' (@@looping);");
    session->run((char*)"Serial.println (gindex str);");
    session->run((char*)"Serial.println (gindex str);");
}

void loop()
{
    delay(1000);

    session->run((char*)"Serial.println (gindex str);");
}

#endif