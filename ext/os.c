#include "../src/bruter.h"

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
    Thread* thread = (Thread*)arg;
    VirtualMachine* vm = thread->vm;
    StringList* strings = thread->strings;
    //  init transfer


    VirtualMachine* localvm = make_vm();
    // <extensions>src/std.c
    if (strings->size > 0) 
    {
        //status 0 = not ready
        //status 1 = ready (will process a string)
        //status 2 = busy (it is processing a string)
        //status 3 = idle (has no strings to process, waiting for one)
        //status 4 = destroyed
        thread->status = 1;// status 1 = ready, status 0 = not ready, status 2 = busy
    }

    Int thread_index = new_var(localvm);
    localvm->stack->data[thread_index].pointer = thread;
    localvm->typestack->data[thread_index] = TYPE_THREAD;
    hash_set(localvm, "thread.self", thread_index);
    hold_var(localvm, thread_index);
    while (1) 
    {
        if (strings->size == 0) 
        {
            thread->status = 3;// status 3 = idle
            while (strings->size == 0) {}
            thread->status = 1;// status 1 = ready
        }
        pthread_mutex_lock(thread->strings_lock); 
        thread->status = 2;// status 2 = busy
        char* current = stack_shift(*strings);
        pthread_mutex_unlock(thread->strings_lock);

        if (current[0] == 't' && strcmp(current, "terminate") == 0) 
        {
            free(current);
            break;
        }
        
        eval(localvm, current);
        free(current);
    }

    hash_unset (localvm, "thread.self");

    while (strings->size > 0) 
    {
        free(stack_shift(*strings));
    }

    free_vm(localvm);
    thread->status = 4;
    return NULL;
}

Thread* make_thread(VirtualMachine* vm, char* str, ...)
{
    Thread* thread = (Thread*)malloc(sizeof(Thread));
    thread->vm = vm;
    thread->strings = make_string_list();
    
    thread->strings_lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    thread->thread_lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    
    pthread_mutex_init(thread->strings_lock, NULL); 
    pthread_mutex_init(thread->thread_lock, NULL);

    thread->status = 0;

    thread->transfer = (ThreadTransfer*)malloc(sizeof(ThreadTransfer));
    thread->transfer->stack = make_value_list();
    thread->transfer->typestack = make_char_list();
    
    va_list args;
    va_start(args, str);
    char* current = str;
    while (current != NULL) 
    {
        stack_push(*thread->strings, str_duplicate(current));
        current = va_arg(args, char*);
    }
    va_end(args);

    pthread_t thread_t;
    pthread_create(&thread_t, NULL, permanent_thread, thread); 

    thread->thread = (pthread_t*)malloc(sizeof(pthread_t));  
    *(thread->thread) = thread_t;

    return thread;
}


Int std_thread_create(VirtualMachine* vm, IntList* args) 
{
    Int index = new_var(vm);
    hold_var(vm, index);

    Thread* thread = make_thread(vm, "print (string.concat 'thread v' VERSION)", NULL);
    vm->stack->data[index].pointer = thread;
    vm->typestack->data[index] = TYPE_THREAD;

    if (args->size > 0) 
    {
        Int name = stack_shift(*args);
        hash_set(vm, vm->stack->data[name].string, index);
        // status MUST be volatile, otherwise it freezes thread creation
        while (thread->status == 0) {}
        return -1;
    }
    else 
    {
        // status MUST be volatile, otherwise it freezes thread creation
        while (thread->status == 0) {}
        return index;
    }
}


Int std_thread_send(VirtualMachine* vm, IntList* args)
{
    Int thread_i = stack_shift(*args);
    Int message = stack_shift(*args);
    Thread* thread = (Thread*)vm->stack->data[thread_i].pointer;

    pthread_mutex_lock(thread->strings_lock);
    stack_push(*thread->strings, str_duplicate(vm->stack->data[message].string));
    pthread_mutex_unlock(thread->strings_lock);
    return -1;
}

Int std_thread_await(VirtualMachine* vm, IntList* args)
{
    Int thread_i = stack_shift(*args);
    Thread* thread = (Thread*)vm->stack->data[thread_i].pointer;

    while (thread->status != 3) {}  // wait until the thread is idle(no strings to process)
    return -1;
}

void thread_destroy(Thread* thread)
{
    while (thread->strings->size > 0) 
    {
        free(stack_shift(*thread->strings));
    }

    while (thread->transfer->stack->size > 0) 
    {
        Value v = stack_shift(*thread->transfer->stack);
        char t = stack_shift(*thread->transfer->typestack);
        if (t == TYPE_STRING || t == TYPE_FUNCTION || t == TYPE_OTHER)
        {
            free(v.string);
        }
    }
    
    pthread_mutex_lock(thread->strings_lock);
    stack_push(*thread->strings, str_duplicate("terminate"));
    pthread_mutex_unlock(thread->strings_lock);
    pthread_join(*(thread->thread), NULL);
    pthread_mutex_destroy(thread->strings_lock);
    pthread_mutex_destroy(thread->thread_lock);
    free(thread->strings_lock);
    free(thread->thread_lock);
    free(thread->thread);
    stack_free(*thread->transfer->stack);
    stack_free(*thread->transfer->typestack);
    free(thread->transfer);
    stack_free(*thread->strings);
    free(thread);
}

Int std_thread_destroy(VirtualMachine* vm, IntList* args) 
{
    Int thread = stack_shift(*args);
    thread_destroy((Thread*)vm->stack->data[thread].pointer);
    return -1;
}

// thread transfer functions
Int std_thread_transfer_push(VirtualMachine* vm, IntList* args)
{
    Int thread = stack_shift(*args);
    Int value = stack_shift(*args);
    
    Thread* thread_arg = (Thread*)vm->stack->data[thread].pointer;
    pthread_mutex_lock(thread_arg->thread_lock);
    // these two stack pushes are leaking memory, 9 bytes each time called, to fix this, we need to free the memory of the strings
    stack_push(*thread_arg->transfer->stack, value_duplicate(vm->stack->data[value], vm->typestack->data[value]));
    stack_push(*thread_arg->transfer->typestack, vm->typestack->data[value]);
    
    pthread_mutex_unlock(thread_arg->thread_lock);

    return -1;
}

Int std_thread_transfer_pop(VirtualMachine* vm, IntList* args)
{
    Int thread = stack_shift(*args);
    Int value = stack_shift(*args);
    Int result = new_var(vm);

    Thread* thread_arg = (Thread*)vm->stack->data[thread].pointer;
    
    pthread_mutex_lock(thread_arg->thread_lock);
    vm->stack->data[result] = stack_pop(*thread_arg->transfer->stack);
    vm->typestack->data[result] = stack_pop(*thread_arg->transfer->typestack);
    pthread_mutex_unlock(thread_arg->thread_lock);
    return result;
}

Int std_thread_transfer_unshift(VirtualMachine* vm, IntList* args)
{
    Int thread = stack_shift(*args);
    Int value = stack_shift(*args);
    
    Thread* thread_arg = (Thread*)vm->stack->data[thread].pointer;
    
    pthread_mutex_lock(thread_arg->thread_lock);
    stack_unshift(*thread_arg->transfer->stack, vm->stack->data[value]);
    stack_unshift(*thread_arg->transfer->typestack, vm->typestack->data[value]);
    pthread_mutex_unlock(thread_arg->thread_lock);
    
    return -1;
}

Int std_thread_transfer_shift(VirtualMachine* vm, IntList* args)
{
    Int thread = stack_shift(*args);
    Int value = stack_shift(*args);
    Int result = new_var(vm);

    Thread* thread_arg = (Thread*)vm->stack->data[thread].pointer;
    
    pthread_mutex_lock(thread_arg->thread_lock);
    vm->stack->data[result] = stack_shift(*thread_arg->transfer->stack);
    vm->typestack->data[result] = stack_shift(*thread_arg->transfer->typestack);
    pthread_mutex_unlock(thread_arg->thread_lock);
    return result;
}

Int std_thread_transfer_insert(VirtualMachine* vm, IntList* args)
{
    Int thread = stack_shift(*args);
    Int index = stack_shift(*args);
    Int value = stack_shift(*args);
    
    Thread* thread_arg = (Thread*)vm->stack->data[thread].pointer;
    
    pthread_mutex_lock(thread_arg->thread_lock);
    stack_insert(*thread_arg->transfer->stack, (Int)vm->stack->data[index].number, vm->stack->data[value]);
    stack_insert(*thread_arg->transfer->typestack, (Int)vm->stack->data[index].number, vm->typestack->data[value]);
    pthread_mutex_unlock(thread_arg->thread_lock);
    
    return -1;
}

Int std_thread_transfer_remove(VirtualMachine* vm, IntList* args)
{
    Int thread = stack_shift(*args);
    Int index = stack_shift(*args);
    Int result = new_var(vm);

    Thread* thread_arg = (Thread*)vm->stack->data[thread].pointer;
    
    pthread_mutex_lock(thread_arg->thread_lock);
    vm->stack->data[result] = stack_remove(*thread_arg->transfer->stack, (Int)vm->stack->data[index].number);
    vm->typestack->data[result] = stack_remove(*thread_arg->transfer->typestack, (Int)vm->stack->data[index].number);
    pthread_mutex_unlock(thread_arg->thread_lock);
    
    return result;
}

void init_pthreads(VirtualMachine *vm)
{
    hold_var(vm,spawn_builtin(vm, "thread.create", std_thread_create));
    hold_var(vm,spawn_builtin(vm, "thread.send", std_thread_send));
    hold_var(vm,spawn_builtin(vm, "thread.destroy", std_thread_destroy));
    hold_var(vm,spawn_builtin(vm, "thread.await", std_thread_await));

    hold_var(vm,spawn_builtin(vm, "transfer.push", std_thread_transfer_push));
    hold_var(vm,spawn_builtin(vm, "transfer.pop", std_thread_transfer_pop));
    hold_var(vm,spawn_builtin(vm, "transfer.unshift", std_thread_transfer_unshift));
    hold_var(vm,spawn_builtin(vm, "transfer.shift", std_thread_transfer_shift));
    hold_var(vm,spawn_builtin(vm, "transfer.insert", std_thread_transfer_insert));
    hold_var(vm,spawn_builtin(vm, "transfer.remove", std_thread_transfer_remove));

}

void init_os(VirtualMachine *vm)
{
    hold_var(vm,spawn_builtin(vm, "file.read", std_file_read));
    hold_var(vm,spawn_builtin(vm, "file.write", std_file_write));
    hold_var(vm,spawn_builtin(vm, "repl", std_repl));
    init_pthreads(vm);
    #ifdef _WIN32 
    #else
    //init_linux(vm);
    #endif
}

