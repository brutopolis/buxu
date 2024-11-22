#ifndef ARDUINO

#include "bruter.h"
#include <libtcc.h>

int multiply(int a, int b)
{
    return a * b;
}

double divide(double a, double b)
{
    return a / b;
}

IntList *tcc_states;

Int new_state(VirtualMachine *vm)
{
    
    TCCState *tcc = tcc_new();
    if (!tcc) {
        fprintf(stderr, "Erro ao inicializar TCC\n");
        return -1;
    }

    // Configurar saída para memória
    tcc_set_output_type(tcc, TCC_OUTPUT_MEMORY);

    // Adicionar símbolo das funções disponíveis no programa mestre
    tcc_add_symbol(tcc, "multiply", multiply);
    tcc_add_symbol(tcc, "divide", divide);

    // Adicionar caminho do header
    tcc_add_include_path(tcc, "./include/"); // Certifique-se de que o header esteja neste diretório

    // Código dinâmico que usa funções definidas no programa mestre
    const char *code = 
        "#include \"bruter.h\"\n"
        "int calculate() {\n"
        "    int prod = multiply(6, 7);\n"
        "    double div = divide(prod, 2.0);\n"
        "    return (int)div;\n"
        "}";

    // Compilar o código dinâmico
    if (tcc_compile_string(tcc, code) == -1) {
        fprintf(stderr, "Erro ao compilar código dinâmico\n");
        tcc_delete(tcc);
        return -1;
    }

    // Realocar código
    if (tcc_relocate(tcc) < 0) {
        fprintf(stderr, "Erro ao realocar código\n");
        tcc_delete(tcc);
        return -1;
    }

    // Obter função calculada
    int (*calculate)() = tcc_get_symbol(tcc, "calculate");
    if (!calculate) {
        fprintf(stderr, "Erro ao localizar símbolo 'calculate'\n");
        tcc_delete(tcc);
        return -1;
    }

    // Chamar a função
    printf("Resultado de calculate(): %d\n", calculate()); // Esperado: 21

    // Limpar contexto
    tcc_delete(tcc);
}

void init_tcc(VirtualMachine* vm)
{
    tcc_states = make_int_list(vm);
    for (int i = 0; i < 10; i++)
    {
        stack_push(*tcc_states, new_state(vm));
    }
    stack_free(*tcc_states);
}

#endif