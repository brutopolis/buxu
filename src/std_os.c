#include "bruter.h"

char* readfile(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        return NULL;
    }
    char *code = (char*)malloc(1);
    code[0] = '\0';
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, file) != -1)
    {
        code = (char*)realloc(code, strlen(code) + strlen(line) + 1);
        strcat(code, line);
    }
    free(line);
    fclose(file);
    return code;
};

void writefile(char *filename, char *code)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        return;
    }
    fprintf(file, "%s", code);
    fclose(file);
}

Int std_file_read(VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    char *code = readfile(vm->stack->data[filename].string);
    Int result = -1;
    if (code == NULL)
    {
    }
    else
    {
        result = eval(vm, code);
        free(code);
    }
    //freeRawVar(filename);
    return result;
}

Int std_file_write(VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    Int code = stack_shift(*args);
    writefile(vm->stack->data[filename].string, vm->stack->data[code].string);
    return -1;
}

Int std_repl(VirtualMachine *vm, IntList *args)
{
    printf("bruter v%s\n", VERSION);
    char *cmd;
    Int result = -1;
    while (result == -1)
    {
        cmd = (char*)malloc(1024);
        printf("@> ");
        scanf("%[^\n]%*c", cmd);
        result = eval(vm, cmd);
        free(cmd);
    }

    printf("repl returned: @%d ", result);
    char * str = str_format("print @%d", result);
    eval(vm, str);
    free(str);
    return result;
}

// threads


void* permanent_thread(void* arg)
{
    Thread* thread_arg = (Thread*)arg;
    VirtualMachine* vm = thread_arg->vm;
    StringList* strings = thread_arg->strlist;
    VirtualMachine* localvm = make_vm();
    preset_all(localvm);
    if (strings->size > 0) 
    {
        //status 0 = not ready
        //status 1 = ready (will process a string)
        //status 2 = busy (it is processing a string)
        //status 3 = idle (has no strings to process, waiting for one)
        //status 4 = destroyed
        thread_arg->status = 1;// status 1 = ready, status 0 = not ready, status 2 = busy
    }

    while (1) 
    {
        if (strings->size == 0) 
        {
            thread_arg->status = 3;// status 3 = idle
            while (strings->size == 0) 
            {
                printf("waiting for strings\n");
            }
        }
        pthread_mutex_lock(thread_arg->argslock); 
        thread_arg->status = 2;// status 2 = busy
        
        char* current = stack_shift(*strings);
        pthread_mutex_unlock(thread_arg->argslock);

        if (strcmp(current, "terminate") == 0) 
        {
            free(current);
            break;
        }

        pthread_mutex_lock(thread_arg->vmlock);
        eval(localvm, current);
        pthread_mutex_unlock(thread_arg->vmlock);
        free(current);
        thread_arg->status = 1;// status 1 = ready
    }

    while (strings->size > 0) 
    {
        free(stack_shift(*strings));
    }

    free_vm(localvm);
    thread_arg->status = 4;
    return NULL;
}



// Função para criar Thread e inicializar a thread
Thread* make_thread_arg(VirtualMachine* vm, Int vmlock, char* str, ...)
{
    Thread* thread_arg = (Thread*)malloc(sizeof(Thread));
    thread_arg->vm = vm;
    thread_arg->strlist = make_string_list();
    
    // Aloca e inicializa o mutex e a variável de condição
    thread_arg->argslock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    
    pthread_mutex_init(thread_arg->argslock, NULL);  // Inicializa o mutex
    
    thread_arg->vmlock = (pthread_mutex_t*)vm->stack->data[vmlock].pointer;
    thread_arg->status = 0;

    // Processa os argumentos
    va_list args;
    va_start(args, str);
    char* current = str;
    while (current != NULL) 
    {
        stack_push(*thread_arg->strlist, str_duplicate(current));
        current = va_arg(args, char*);
    }
    va_end(args);

    pthread_t thread;
    pthread_create(&thread, NULL, permanent_thread, thread_arg);  // Passa o argumento correto

    thread_arg->thread = (pthread_t*)malloc(sizeof(pthread_t));  // Aloca espaço para a thread
    *(thread_arg->thread) = thread;  // Armazena a thread

    return thread_arg;
}


Int std_thread_create(VirtualMachine* vm, IntList* args) 
{
    Int index = new_var(vm);
    hold_var(vm, index);

    Thread* thread_arg = make_thread_arg(vm, eval(vm, "return vm.lock"), "print (string.concat 'thread v' VERSION)", NULL);
    vm->stack->data[index].pointer = thread_arg;
    vm->typestack->data[index] = TYPE_THREAD;

    if (args->size > 0) 
    {
        Int name = stack_shift(*args);
        hash_set(vm, vm->stack->data[name].string, index);
        // status MUST be volatile, otherwise it freezes thread creation
        while (thread_arg->status == 0) {}
        return -1;
    }
    else 
    {
        // status MUST be volatile, otherwise it freezes thread creation
        while (thread_arg->status == 0) {}
        return index;
    }
}


Int std_thread_send(VirtualMachine* vm, IntList* args)
{
    Int thread = stack_shift(*args);
    Int message = stack_shift(*args);
    Thread* thread_arg = (Thread*)vm->stack->data[thread].pointer;

    pthread_mutex_lock(thread_arg->argslock);  // Protege a lista de strings
    stack_push(*thread_arg->strlist, str_duplicate(vm->stack->data[message].string));
    pthread_mutex_unlock(thread_arg->argslock);  // Libera o mutex

    return -1;
}

Int std_thread_await(VirtualMachine* vm, IntList* args)
{
    Int thread = stack_shift(*args);
    Thread* thread_arg = (Thread*)vm->stack->data[thread].pointer;

    while (thread_arg->status == 2) {}  // wait until the thread is not busy

    return -1;
}

Int std_thread_join(VirtualMachine* vm, IntList* args)
{
    Int thread = stack_shift(*args);
    Thread* thread_arg = (Thread*)vm->stack->data[thread].pointer;

    while (thread_arg->status != 3) {}  // wait until the thread is idle(no strings to process)

    return -1;
}

void thread_destroy(Thread* thread_arg)
{
    pthread_mutex_lock(thread_arg->argslock);  // Protege a lista de strings
    stack_push(*thread_arg->strlist, str_duplicate("terminate"));
    pthread_mutex_unlock(thread_arg->argslock);  // Libera o mutex

    pthread_join(*thread_arg->thread, NULL);  // Espera a thread terminar
    pthread_mutex_destroy(thread_arg->argslock);  // Destrói o mutex
    pthread_mutex_destroy(thread_arg->vmlock);  // Destrói o mutex
    free(thread_arg->argslock);  // Libera a memória do mutex
    free(thread_arg->thread);  // Libera a memória da thread
    stack_free(*thread_arg->strlist);  // Libera a lista de strings
    free(thread_arg);
}

// Função para terminar uma thread no contexto da VM
Int std_thread_destroy(VirtualMachine* vm, IntList* args) 
{
    Int thread = stack_shift(*args);
    thread_destroy((Thread*)vm->stack->data[thread].pointer);
    return -1;
}

void init_pthreads(VirtualMachine *vm)
{
    Int vmlockindex = new_var(vm);
    hold_var(vm,vmlockindex);
    hash_set(vm, "vm.lock", vmlockindex);
    vm->typestack->data[vmlockindex] = TYPE_OTHER;
    vm->stack->data[vmlockindex].pointer = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init((pthread_mutex_t*)vm->stack->data[vmlockindex].pointer, NULL);
    hold_var(vm,spawn_builtin(vm, "thread.create", std_thread_create));
    hold_var(vm,spawn_builtin(vm, "thread.send", std_thread_send));
    hold_var(vm,spawn_builtin(vm, "thread.destroy", std_thread_destroy));
    hold_var(vm,spawn_builtin(vm, "thread.await", std_thread_await));
}

void init_std_os(VirtualMachine *vm)
{
    hold_var(vm,spawn_builtin(vm, "file.read", std_file_read));
    hold_var(vm,spawn_builtin(vm, "file.write", std_file_write));
    hold_var(vm,spawn_builtin(vm, "repl", std_repl));
    #ifdef _WIN32 
    #else
    init_linux(vm);
    #endif
}

