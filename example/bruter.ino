// bruter on arduino example
// just put this, the include folder and the src folder in the same folder
// and you might be ready to go

extern "C"
{
    #include "./src/bruter.h"
    
}

function(_ino_serial_begin)
{
    Serial.begin((Int)arg(0).number);
    return -1;
}

function(_ino_print)
{
    for (Int i = 0; i < args->size; i++)
    {
        print_element(vm, arg_i(i));
    }
    
    return -1;
}

function(_ino_delay)
{
    delay((Int)arg(0).number);
    return -1;
}

function(_ino_millis)
{
    return millis();
}

function(_ino_tone)
{
    tone((int)arg(0).number, (int)arg(1).number);
    return -1;
}


function(_ino_noTone)
{
    noTone((Int)arg(0).number);
    return -1;
}

function(_ino_pinmode)
{
    pinMode((Int)arg(0).number, (Int)arg(1).number);
    return -1;
}

function(_ino_digitalwrite)
{
    digitalWrite((Int)arg(0).number, (Int)arg(1).number);
    return -1;
}

function(_ino_digitalread)
{
    return digitalRead((Int)arg(0).number);
}

function(_ino_analogwrite)
{
    analogWrite((Int)arg(0).number, (Int)arg(1).number);
    return -1;
}

function(_ino_analogread)
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
        this->registerFunction((char*)"Serial.begin", _ino_serial_begin);
        this->registerFunction((char*)"print", _ino_print);
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
    session->run((char*)"print 'aoba\n';");
}

void loop()
{
    delay(1000);

    session->run((char*)"print 'aoba\n';");
}
