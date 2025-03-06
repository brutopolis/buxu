// this file is the utility tool for bruter

#ifdef BRUTER_EXECUTABLE

// bruter header
#include "../include/bruter.h"
#include "../include/entrypoint.h"

int main(int argc, char **argv)
{
    // <embed code> 

    Int result = 0;
    VirtualMachine *vm = make_vm();
    
    // libraries init is not a merely comment
    // <libraries init>

    register_list(vm, "file.args");
    IntList *fileargs = (IntList*)data(hash_find(vm, "file.args")).pointer;
    for (int i = 1; i < argc; i++)
    {
        list_push(*fileargs, new_string(vm, argv[i]));
    }
    
    // embed code tag is replaced by main_br and main_br_len during build
    result = eval(vm, src_main_br, NULL);

    free_vm(vm);
    return result;
}

#else

#ifndef ARDUINO

// bruter header
#include "../include/bruter.h"


function(brl_main_dl_open)
{
    
    void *handle = dlopen(arg(0).string, RTLD_LAZY);
    Int index = -1;
    if (handle != NULL)
    {
        index = new_var(vm);
        data(index).pointer = handle;
        data_t(index) = TYPE_ANY;
    }
    else 
    {
        printf("error: %s\n", dlerror());
    }

    StringList *splited = str_split_char(arg(0).string, '/');
    StringList *splited2 = str_split_char(splited->data[splited->size - 1], '.');
    char *libname = splited2->data[0];

    // now lets get the init_name function
    InitFunction _init = dlsym(handle, str_format("init_%s", libname));
    if (_init != NULL)
    {
        _init(vm);
        
        // look for a hash called dl.list
        Int dl_list = hash_find(vm, "dl.list");
        if (dl_list == -1)
        {
            dl_list = new_list(vm);
            hash_set(vm, "dl.list", dl_list);
        }
        
        IntList *list = (IntList*)data(dl_list).pointer;
        list_push(*list, index);
    }
    else 
    {
        printf("error: %s\n", dlerror());
        printf("error: init_%s not found\n", libname);
    }

    for (Int i = 0; i < splited->size; i++)
    {
        free(splited->data[i]);
    }

    for (Int i = 0; i < splited2->size; i++)
    {
        free(splited2->data[i]);
    }

    free(splited);
    free(splited2);

    return -1;
}

function(brl_main_dl_close)
{
    dlclose(data(arg_i(0)).pointer);
    // remove from dl.list
    Int dl_list = hash_find(vm, "dl.list");
    if (dl_list != -1)
    {
        IntList *list = (IntList*)data(dl_list).pointer;
        for (Int i = 0; i < list->size; i++)
        {
            if (list->data[i] == arg_i(0))
            {
                list_fast_remove(*list, i);
                break;
            }
        }
    }
    else
    {
        printf("warning: dl.list not found.\n");
    }
    return -1;
}

int main(int argc, char **argv)
{
    Int result = 0;
    StringList *args = list_init(StringList);
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
            list_push(*args, argv[i]);
        }
    }
    
    VirtualMachine *vm = make_vm();
    
    // libraries init is not a merely comment
    // <libraries init>

    // dynamic library functions are declared here because it is available exclusively in the non-wasm && non-arduino version;
    // if you want to include libs in the interpreter use the recommended, usual way using lib/ folder, documented in the README.md;
    register_builtin(vm, "dl.open", brl_main_dl_open);
    register_builtin(vm, "dl.close", brl_main_dl_close);

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
        char *path = list_shift(*args);
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
            list_push(*fileargs, new_string(vm, list_shift(*args)));
        }
        result = eval(vm, _code, NULL);
        free(_code);
    }
    Int dllist = hash_find(vm, "dl.list");
    if (dllist != -1)
    {
        IntList *list = (IntList*)data(dllist).pointer;
        for (Int i = 0; i < list->size; i++)
        {
            dlclose(data(list->data[i]).pointer);
        }
    }
    list_free(*args);
    free_vm(vm);
    return result;
}

#else
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
        // <libraries init>
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

#endif
#endif