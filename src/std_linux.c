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

// Função para receber uma string dinamicamente alocada
char* receive_dynamic_string(process_t* process, int from_parent) 
{
    size_t len;
    char* buffer;

    if (from_parent) 
    {
        // Receber do pai (pipe pai -> filho)
        if (read(process->parent_to_child[0], &len, sizeof(len)) == -1) 
        {
            perror("Erro ao receber tamanho da string do pai");
            exit(EXIT_FAILURE);
        }
        buffer = (char*)malloc(len);
        if (buffer == NULL) 
        {
            perror("Erro ao alocar memória");
            exit(EXIT_FAILURE);
        }
        if (read(process->parent_to_child[0], buffer, len) == -1) 
        {
            perror("Erro ao receber string do pai");
            exit(EXIT_FAILURE);
        }
    } else 
    {
        // Receber do filho (pipe filho -> pai)
        if (read(process->child_to_parent[0], &len, sizeof(len)) == -1) 
        {
            perror("Erro ao receber tamanho da string do filho");
            exit(EXIT_FAILURE);
        }
        buffer = (char*)malloc(len);
        if (buffer == NULL) 
        {
            perror("Erro ao alocar memória");
            exit(EXIT_FAILURE);
        }
        if (read(process->child_to_parent[0], buffer, len) == -1) 
        {
            perror("Erro ao receber string do filho");
            exit(EXIT_FAILURE);
        }
    }

    return buffer;  // Retorna a string dinamicamente alocada
}


int is_pipe_open(int fd) {
    // Usa fcntl para verificar se o descritor de arquivo está aberto
    return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}

void terminate_process(process_t* process) 
{
    // Verifica se os pipes estão abertos antes de tentar usá-los
    if (is_pipe_open(process->parent_to_child[1])) {
        send_dynamic_string(process, "terminate", 0);
        char* str = receive_dynamic_string(process, 0);
        free(str);
    } else {
        printf("Pipe pai->filho está fechado. Não é possível enviar 'terminate'.\n");
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
    
    while (1) 
    {
        char* received = receive_dynamic_string(process, 1);  // Receber do pai
        if (!received) 
        {
            printf("Erro ao receber mensagem no filho\n");
            continue;
        }
        if (strcmp(received, "terminate") == 0) 
        {
            free(received);
            break;  // Encerra o processo se receber "terminate"
        }
        
        eval(vm, received);  // Processa o comando na VM
        free(received);

        send_dynamic_string(process, "ok", 1);  // Enviar para o pai
    }
    vm->stack->data[index].process = NULL;
    vm->typestack->data[index] = TYPE_NIL;
    send_dynamic_string(process, "terminated", 1);  // Enviar para o pai
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

Int std_process_terminate(VirtualMachine *vm, IntList *args)
{
    Int process = stack_shift(*args);
    terminate_process((process_t*)vm->stack->data[process].process);
    unuse_var(vm, process);
    return -1;
}

void init_linux(VirtualMachine *vm)
{
    hold_var(vm,spawn_builtin(vm, "process.fork", std_process_fork));
    hold_var(vm,spawn_builtin(vm, "process.send", std_process_host_send));
    hold_var(vm,spawn_builtin(vm, "process.receive", std_process_host_receive));
    hold_var(vm,spawn_builtin(vm, "process.child.send", std_process_child_send));
    hold_var(vm,spawn_builtin(vm, "process.child.receive", std_process_child_receive));
    hold_var(vm,spawn_builtin(vm, "process.terminate", std_process_terminate));
}