#include "bruter.h"

// Função para criar um processo filho e configurar os pipes
int create_process(process_t* process, void (*child_function)(process_t*, VirtualMachine*), VirtualMachine* vm) {
    // Criar pipe para comunicação pai -> filho
    if (pipe(process->parent_to_child) == -1) {
        perror("Erro ao criar pipe pai -> filho");
        return -1;
    }

    // Criar pipe para comunicação filho -> pai
    if (pipe(process->child_to_parent) == -1) {
        perror("Erro ao criar pipe filho -> pai");
        close(process->parent_to_child[0]);  // Fechar as extremidades dos pipes
        close(process->parent_to_child[1]);
        return -1;
    }

    // Criar o processo filho
    process->pid = fork();
    if (process->pid == -1) {
        perror("Erro ao criar processo filho");
        close(process->parent_to_child[0]);  // Fechar as extremidades dos pipes
        close(process->parent_to_child[1]);
        close(process->child_to_parent[0]);
        close(process->child_to_parent[1]);
        return -1;
    }


    if (process->pid == 0) {
        // Processo filho: fechar extremidades dos pipes não utilizadas
        close(process->parent_to_child[1]);  // Fecha escrita do pai -> filho
        close(process->child_to_parent[0]);  // Fecha leitura do filho -> pai

        // Chamar a função do processo filho
        child_function(process, vm);

        // Fechar os pipes após a execução
        close(process->parent_to_child[0]);
        close(process->child_to_parent[1]);

        //exit(0);  // Termina o processo filho após a execução
    } else {
        // Processo pai: fechar extremidades dos pipes não utilizadas
        close(process->parent_to_child[0]);  // Fecha leitura do pai -> filho
        close(process->child_to_parent[1]);  // Fecha escrita do filho -> pai
    }

    return 0;
}

// Função para enviar uma string dinamicamente
void send_dynamic_string(process_t* process, const char* str, int to_parent) 
{
    size_t len = strlen(str) + 1;  // Inclui o null terminator

    if (to_parent) {
        // Enviar para o pai (pipe filho -> pai)
        if (write(process->child_to_parent[1], &len, sizeof(len)) == -1) {
            perror("Erro ao enviar tamanho da string para o pai");
            exit(EXIT_FAILURE);
        }
        if (write(process->child_to_parent[1], str, len) == -1) {
            perror("Erro ao enviar string para o pai");
            exit(EXIT_FAILURE);
        }
    } else {
        // Enviar para o filho (pipe pai -> filho)
        if (write(process->parent_to_child[1], &len, sizeof(len)) == -1) {
            perror("Erro ao enviar tamanho da string para o filho");
            exit(EXIT_FAILURE);
        }
        if (write(process->parent_to_child[1], str, len) == -1) {
            perror("Erro ao enviar string para o filho");
            exit(EXIT_FAILURE);
        }
    }
}

char* receive_dynamic_string(process_t* process, int from_parent) 
{
    size_t len;
    char* buffer;
    int fd = from_parent ? process->parent_to_child[0] : process->child_to_parent[0];

    // Estrutura para o poll()
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;

    // Verifica se há algo no pipe antes de bloquear
    int ret = poll(&pfd, 1, 1000);  // Timeout de 1 segundo

    if (ret == -1) 
    {
        perror("error verifying data in pipe");
        exit(EXIT_FAILURE);
    } 
    else if (ret == 0) 
    {
        printf("no data on pipe.\n");
        return NULL;  // Nenhum dado disponível
    }

    // Dados disponíveis, proceder com a leitura
    if (read(fd, &len, sizeof(len)) == -1) 
    {
        perror("error receiving string length");
        exit(EXIT_FAILURE);
    }

    buffer = (char*)malloc(len);
    if (buffer == NULL) 
    {
        perror("error allocating memory for string");
        exit(EXIT_FAILURE);
    }

    if (read(fd, buffer, len) == -1) 
    {
        perror("error receiving string");
        exit(EXIT_FAILURE);
    }

    return buffer;  // Retorna a string dinamicamente alocada
}

int is_pipe_open(int fd) {
    // Usa fcntl para verificar se o descritor de arquivo está aberto
    return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}

void process_destroy(process_t* process) 
{
    // Verifica se os pipes estão abertos antes de tentar usá-los
    if (is_pipe_open(process->parent_to_child[1])) {
        send_dynamic_string(process, "destroy", 0);
        char* str = receive_dynamic_string(process, 0);
        free(str);
    } else {
        printf("Pipe pai->filho está fechado. Não é possível enviar 'destroy'.\n");
    }

    if (process->pid > 0) {
        // Espera o processo filho terminar
        if (waitpid(process->pid, NULL, WNOHANG) == 0) {
            // O processo ainda está rodando, então podemos esperar ele terminar
            waitpid(process->pid, NULL, 0);
        } else {
            printf("Processo já foi encerrado.\n");
        }
    }

    // Verifica se os pipes estão abertos e fecha-os se necessário
    if (is_pipe_open(process->parent_to_child[1])) {
        close(process->parent_to_child[1]);
    }
    if (is_pipe_open(process->parent_to_child[0])) {
        close(process->parent_to_child[0]);
    }
    if (is_pipe_open(process->child_to_parent[1])) {
        close(process->child_to_parent[1]);
    }
    if (is_pipe_open(process->child_to_parent[0])) {
        close(process->child_to_parent[0]);
    }
}

// Função do processo filho
void default_interpreter(process_t* process, VirtualMachine* vm) 
{
    int index = new_var(vm);
    vm->stack->data[index].process = process;
    vm->typestack->data[index] = TYPE_PROCESS;
    hold_var(vm, index);
    hash_set(vm, "process.child", index);
    char* _str;
    Int result = -1;
    
    while (1) 
    {
        char* received = receive_dynamic_string(process, 1);  
        if (!received) 
        {
            printf("error receiving string\n");
            continue;
        }
        if (strcmp(received, "destroy") == 0) 
        {
            free(received);
            break;  
        }
        
        result = eval(vm, received);  
        free(received);
        
        if (result > -1) 
        {
            if (vm->typestack->data[result] == TYPE_STRING) 
            {
                _str = vm->stack->data[result].string;
                send_dynamic_string(process, _str, 1);  
            } 
            else 
            {
                _str = str_format("%d", vm->stack->data[result].number);
                send_dynamic_string(process, _str, 1);  
                free(_str);
            }
        }
        else 
        {
            send_dynamic_string(process, "", 1);
        }
        
    }
    vm->stack->data[index].process = NULL;
    vm->typestack->data[index] = TYPE_NIL;
    send_dynamic_string(process, "destroyd", 1);
}

// process function declarations
Int std_process_fork(VirtualMachine *vm, IntList *args)
{
    Int name = -1;
    if (args->size > 0)
    {
        name = stack_shift(*args);
    }
    process_t *process = (process_t*)malloc(sizeof(process_t));
    // Criar o processo filho
    if (create_process(process, default_interpreter, vm) == -1) 
    {
        perror("Erro ao criar processo");
        exit(EXIT_FAILURE);
    }

    Int result = new_var(vm);
    
    

    if (process->pid == 0) 
    {
        vm->stack->data[result].process = NULL;
        vm->typestack->data[result] = TYPE_NIL;
        vm->stack->data[result].integer = 0;
        free(process);
        return new_number(vm, 0);
    }
    vm->typestack->data[result] = TYPE_PROCESS;
    vm->stack->data[result].process = process;
    hold_var(vm, result);
    
    if (name >= 0)
    {
        hash_set(vm, vm->stack->data[name].string, result);
        return -1;
    }
    
    return result;
}

Int std_process_host_send(VirtualMachine *vm, IntList *args)
{
    Int process = stack_shift(*args);
    Int message = stack_shift(*args);
    send_dynamic_string((process_t*)vm->stack->data[process].process, vm->stack->data[message].string, 0);
    return -1;
}

Int std_process_host_receive(VirtualMachine *vm, IntList *args)
{
    Int process = stack_shift(*args);
    char *received = receive_dynamic_string((process_t*)vm->stack->data[process].process, 0);
    if (strcmp(received, "") == 0)
    {
        free(received);
        return -1;
    }
    else 
    {
        Int result = new_string(vm, received);
        free(received);
        return result;
    }
}

Int std_process_host_await(VirtualMachine *vm, IntList *args)
{
    Int process = stack_shift(*args);
    char *received = receive_dynamic_string((process_t*)vm->stack->data[process].process, 0);
    while (strcmp(received, "") == 0)
    {
        free(received);
        received = receive_dynamic_string((process_t*)vm->stack->data[process].process, 0);
    }
    Int result = new_string(vm, received);
    free(received);
    return result;
}

Int std_process_child_send(VirtualMachine *vm, IntList *args)
{
    Int process = stack_shift(*args);
    Int message = stack_shift(*args);
    send_dynamic_string((process_t*)vm->stack->data[process].process, vm->stack->data[message].string, 1);
    return -1;
}

Int std_process_child_receive(VirtualMachine *vm, IntList *args)
{
    Int process = stack_shift(*args);
    char *received = receive_dynamic_string((process_t*)vm->stack->data[process].process, 1);
    Int result = eval(vm, received);
    free(received);
    return result;
}

Int std_process_destroy(VirtualMachine *vm, IntList *args)
{
    Int process = stack_shift(*args);
    process_destroy((process_t*)vm->stack->data[process].process);
    unuse_var(vm, process);
    return -1;
}

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


// inits

void init_linux_process(VirtualMachine *vm)
{
    hold_var(vm,spawn_builtin(vm, "process.fork", std_process_fork));
    hold_var(vm,spawn_builtin(vm, "process.send", std_process_host_send));
    hold_var(vm,spawn_builtin(vm, "process.await", std_process_host_await));
    hold_var(vm,spawn_builtin(vm, "process.destroy", std_process_destroy));
    hold_var(vm,spawn_builtin(vm, "process.receive", std_process_host_receive));
    hold_var(vm,spawn_builtin(vm, "process.child.send", std_process_child_send));
    hold_var(vm,spawn_builtin(vm, "process.child.receive", std_process_child_receive));
}

void init_linux_thread(VirtualMachine *vm)
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

void init_linux(VirtualMachine *vm)
{
    init_linux_process(vm);
    init_linux_thread(vm);
}