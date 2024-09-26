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

Int std_include(VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    char *code = readfile(vm->stack->data[filename].string);
    Int result = -1;
    if (code == NULL)
    {
        printf("File %s not found\n", filename);
        exit(1);
    }
    else
    {
        result = eval(vm, code);
        free(code);
    }
    return result;
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
    char * str = str_format("print (get %d)", result);
    eval(vm, str);
    free(str);
    return result;
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

        // Processa o comando recebido (exemplo de eval na VM)
        printf("Filho recebeu: %s\n", received);
        eval(vm, received);  // Processa o comando na VM
        free(received);

        send_dynamic_string(process, "ok", 1);  // Enviar para o pai
    }
    vm->stack->data[index].process = NULL;
    vm->typestack->data[index] = TYPE_NIL;
    send_dynamic_string(process, "terminated", 1);  // Enviar para o pai


    free_vm(vm);

    char * str = str_format("process %d terminated", getpid());
    execlp("echo", "echo", "process terminated", NULL);
    perror("Exec failed");

    exit(EXIT_SUCCESS);
}

// process function declarations
Int std_process_fork(VirtualMachine *vm, IntList *args)
{
    process_t *process = (process_t*)malloc(sizeof(process_t));
    // Criar o processo filho
    if (create_process(process, default_interpreter, vm) == -1) 
    {
        perror("Erro ao criar processo");
        exit(EXIT_FAILURE);
    }

    //if on child
    if (process->pid == 0) 
    {
        // Processo filho

        return -1;
    }
    Int result = new_var(vm);
    vm->stack->data[result].process = process;
    vm->typestack->data[result] = TYPE_PROCESS;
    // remove from temp
    hold_var(vm, result);
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



void init_std_os(VirtualMachine *vm)
{
    hold_var(vm,spawn_builtin(vm, "file.read", std_file_read));
    hold_var(vm,spawn_builtin(vm, "file.write", std_file_write));
    hold_var(vm,spawn_builtin(vm, "include", std_include));
    hold_var(vm,spawn_builtin(vm, "repl", std_repl));
    hold_var(vm,spawn_builtin(vm, "process.fork", std_process_fork));
    hold_var(vm,spawn_builtin(vm, "process.send", std_process_host_send));
    hold_var(vm,spawn_builtin(vm, "process.receive", std_process_host_receive));
    hold_var(vm,spawn_builtin(vm, "process.child.send", std_process_child_send));
    hold_var(vm,spawn_builtin(vm, "process.child.receive", std_process_child_receive));
    hold_var(vm,spawn_builtin(vm, "process.terminate", std_process_terminate));
}

