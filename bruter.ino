extern "C"
{
#include "./src/bruter.h"
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
        Serial.println("]");
    }
    freeref(vm, _ref);
    return -1;
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
        hashset(this->vm, name, (Int)func);
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
        this->registerFunction((char*)"inoprint", _ino_print);
    }
};

void setup()
{
    Serial.begin(115200);
    Bruter *session = new Bruter();
    session->run((char*)"inoprint !(Hello, World!;)");
    session->free();
    delete session;
}

void loop()
{

}