#include "bruter.h"
#ifndef ARDUINO
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

Int os_file_read(VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    char *code = readfile(vm->stack->data[filename].string);
    Int index = -1;
    if (code != NULL)
    {
        index = new_var(vm);
        vm->stack->data[index].string = code;
        vm->typestack->data[index] = TYPE_STRING;
    }
    return index;
}

Int os_file_write(VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    Int code = stack_shift(*args);
    writefile(vm->stack->data[filename].string, vm->stack->data[code].string);
    return -1;
}

Int os_file_delete(VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    remove(vm->stack->data[filename].string);
    return -1;
}

Int os_repl(VirtualMachine *vm, IntList *args)
{
    printf("bruter v%s\n", VERSION);
    char *cmd;
    Int result = -1;
    int junk = 0;
    while (result == -1)
    {
        cmd = (char*)malloc(1024);
        printf("@> ");
        junk = scanf("%[^\n]%*c", cmd);
        if (junk == 0)
        {
            free(cmd);
            break;
        }
        result = eval(vm, cmd);
        free(cmd);
    }

    printf("repl returned: @%ld\n", result);
    print_element(vm, result);
    printf("\n");
    return result;
}

Int os_dofile(VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    char *code = readfile(vm->stack->data[filename].string);
    Int result = -1;
    if (code != NULL)
    {
        result = eval(vm, code);
        free(code);
    }
    else 
    {
        printf("Error: file %s not found\n", vm->stack->data[filename].string);
    }
    return result;
}

Int os_file_exists(VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    FILE *file = fopen(vm->stack->data[filename].string, "r");
    Int result = new_number(vm, file != NULL);
    if (file != NULL)
    {
        fclose(file);
    }
    return result;
}

Int os_file_rename(VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    Int newname = stack_shift(*args);
    rename(vm->stack->data[filename].string, vm->stack->data[newname].string);
    return -1;
}

Int os_file_copy(VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    Int newname = stack_shift(*args);
    FILE *file = fopen(vm->stack->data[filename].string, "r");
    if (file == NULL)
    {
        return -1;
    }
    fclose(file);
    char *code = readfile(vm->stack->data[filename].string);
    writefile(vm->stack->data[newname].string, code);
    free(code);
    return -1;
}

Int os_file_size (VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    FILE *file = fopen(vm->stack->data[filename].string, "r");
    if (file == NULL)
    {
        return -1;
    }
    fseek(file, 0, SEEK_END);
    Int result = new_number(vm, ftell(file));
    fclose(file);
    return result;
}

Int os_time_now(VirtualMachine *vm, IntList *args)
{
    return new_number(vm, time(NULL));
}

Int os_time_clock(VirtualMachine *vm, IntList *args)
{
    return new_number(vm, clock());
}

Int os_time_sleep(VirtualMachine *vm, IntList *args)
{
    Int seconds = stack_shift(*args);
    sleep(vm->stack->data[seconds].number);
    return -1;
}

#ifndef _WIN32

// process type
typedef struct {
    int parent_to_child[2]; // Pipe para comunicação: Pai -> Filho
    int child_to_parent[2]; // Pipe para comunicação: Filho -> Pai
    pid_t pid;              // PID do processo filho
} process_t;

// Função para criar um processo filho e configurar os pipes
int fork_process(process_t* process, void (*child_function)(process_t*, VirtualMachine*), VirtualMachine* vm) {
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

int is_pipe_open(int fd) 
{
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

    free(process);
}

// Função do processo filho
void default_interpreter(process_t* process, VirtualMachine* vm) 
{
    int index = new_var(vm);
    vm->stack->data[index].pointer = process;
    vm->typestack->data[index] = TYPE_OTHER;
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
    vm->stack->data[index].pointer = NULL;
    vm->typestack->data[index] = TYPE_NIL;
    send_dynamic_string(process, "destroyd", 1);
}

// process function declarations
Int os_process_fork(VirtualMachine *vm, IntList *args)
{
    Int name = -1;
    if (args->size > 0)
    {
        name = stack_shift(*args);
    }
    process_t *process = (process_t*)malloc(sizeof(process_t));
    // Criar o processo filho
    if (fork_process(process, default_interpreter, vm) == -1) 
    {
        perror("Erro ao criar processo");
        exit(EXIT_FAILURE);
    }

    Int result = new_var(vm);
    
    

    if (process->pid == 0) 
    {
        vm->stack->data[result].pointer = NULL;
        vm->typestack->data[result] = TYPE_NIL;
        vm->stack->data[result].integer = 0;
        free(process);
        return new_number(vm, 0);
    }
    vm->typestack->data[result] = TYPE_OTHER;
    vm->stack->data[result].pointer = process;
    hold_var(vm, result);
    
    if (name >= 0)
    {
        hash_set(vm, vm->stack->data[name].string, result);
        return -1;
    }
    
    return result;
}

Int os_process_host_send(VirtualMachine *vm, IntList *args)
{
    Int process = stack_shift(*args);
    Int message = stack_shift(*args);
    send_dynamic_string((process_t*)vm->stack->data[process].pointer, vm->stack->data[message].string, 0);
    return -1;
}

Int os_process_host_receive(VirtualMachine *vm, IntList *args)
{
    Int process = stack_shift(*args);
    char *received = receive_dynamic_string((process_t*)vm->stack->data[process].pointer, 0);
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

Int os_process_host_await(VirtualMachine *vm, IntList *args)
{
    Int process = stack_shift(*args);
    char *received = receive_dynamic_string((process_t*)vm->stack->data[process].pointer, 0);
    while (strcmp(received, "") == 0)
    {
        free(received);
        received = receive_dynamic_string((process_t*)vm->stack->data[process].pointer, 0);
    }
    Int result = new_string(vm, received);
    free(received);
    return result;
}

Int os_process_child_send(VirtualMachine *vm, IntList *args)
{
    Int process = stack_shift(*args);
    Int message = stack_shift(*args);
    send_dynamic_string((process_t*)vm->stack->data[process].pointer, vm->stack->data[message].string, 1);
    return -1;
}

Int os_process_child_receive(VirtualMachine *vm, IntList *args)
{
    Int process = stack_shift(*args);
    char *received = receive_dynamic_string((process_t*)vm->stack->data[process].pointer, 1);
    Int result = eval(vm, received);
    free(received);
    return result;
}

Int os_process_destroy(VirtualMachine *vm, IntList *args)
{
    Int process = stack_shift(*args);
    process_destroy((process_t*)vm->stack->data[process].pointer);
    unuse_var(vm, process);
    return -1;
}
#else //if windows
// no process functions for windows yet
#endif

void init_os(VirtualMachine *vm)
{
    registerBuiltin(vm, "file.read", os_file_read);
    registerBuiltin(vm, "file.write", os_file_write);
    registerBuiltin(vm, "file.delete", os_file_delete);
    registerBuiltin(vm, "file.exists", os_file_exists);
    registerBuiltin(vm, "file.rename", os_file_rename);
    registerBuiltin(vm, "file.copy", os_file_copy);
    registerBuiltin(vm, "file.size", os_file_size);

    registerBuiltin(vm, "os.time", os_time_now);
    registerBuiltin(vm, "os.clock", os_time_clock);
    registerBuiltin(vm, "os.sleep", os_time_sleep);

    registerBuiltin(vm, "dofile", os_dofile);
    registerBuiltin(vm, "repl", os_repl);

    #ifdef _WIN32 
    // no process functions for windows yet
    #else
    registerBuiltin(vm, "process.fork", os_process_fork);
    registerBuiltin(vm, "process.send", os_process_host_send);
    registerBuiltin(vm, "process.await", os_process_host_await);
    registerBuiltin(vm, "process.destroy", os_process_destroy);
    registerBuiltin(vm, "process.receive", os_process_host_receive);
    registerBuiltin(vm, "process.child.send", os_process_child_send);
    registerBuiltin(vm, "process.child.receive", os_process_child_receive);
    #endif
}

#else 
void init_os(VirtualMachine *vm) {}
#endif
