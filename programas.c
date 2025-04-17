#include "programas.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

void programaAleatorioRepeticoes(RAM *ram, CPU *cpu)
{
    INSTRUCAO **trecho1 = (INSTRUCAO **)malloc(10001 * sizeof(INSTRUCAO *));

    if (!trecho1)
    {
        fprintf(stderr, "Erro ao alocar memória para trecho1\n");
        return;
    }

    FILE *file = fopen("programa.txt", "r");
    if (file == NULL)
    {
        printf("Erro ao abrir txt\n");
        free(trecho1);
        return;
    }

    char linha[256];
    int index = 0;

    while (fgets(linha, sizeof(linha), file) && index < 10000)
    {
        INSTRUCAO *umaInstrucao = (INSTRUCAO *)malloc(sizeof(INSTRUCAO));
        if (!umaInstrucao)
        {
            fprintf(stderr, "Erro ao alocar instrução\n");
            continue;
        }

        char *palavras[7];
        char *token = strtok(linha, ":");

        int i = 0;

        while (token != NULL && i < 7)
        {
            palavras[i++] = token;
            token = strtok(NULL, ":");
        }

        umaInstrucao->opcode = atoi(palavras[0]);
        // Alocar e preencher endereços
        umaInstrucao->add1 = (ENDERECO *)malloc(sizeof(ENDERECO));
        umaInstrucao->add2 = (ENDERECO *)malloc(sizeof(ENDERECO));
        umaInstrucao->add3 = (ENDERECO *)malloc(sizeof(ENDERECO));

        if (!umaInstrucao->add1 || !umaInstrucao->add2 || !umaInstrucao->add3)
        {
            free(umaInstrucao->add1);
            free(umaInstrucao->add2);
            free(umaInstrucao->add3);
            free(umaInstrucao);
            continue;
        }

        umaInstrucao->add1->endbloco = atoi(palavras[1]);
        umaInstrucao->add1->endPalavra = atoi(palavras[2]) % 4;
        umaInstrucao->add2->endbloco = atoi(palavras[3]);
        umaInstrucao->add2->endPalavra = atoi(palavras[4]) % 4;
        umaInstrucao->add3->endbloco = atoi(palavras[5]);
        umaInstrucao->add3->endPalavra = atoi(palavras[6]) % 4;

        trecho1[index++] = umaInstrucao;

        /*
        ENDERECO *add1 = (ENDERECO *)malloc(sizeof(ENDERECO));
        add1->endbloco = atoi(palavras[1]);
        add1->endPalavra = atoi(palavras[2]) % 4;
        umaInstrucao->add1 = add1;

        ENDERECO *add2 = (ENDERECO *)malloc(sizeof(ENDERECO));
        add2->endbloco = atoi(palavras[3]);
        add2->endPalavra = atoi(palavras[4]) % 4;
        umaInstrucao->add2 = add2;

        ENDERECO *add3 = (ENDERECO *)malloc(sizeof(ENDERECO));
        add3->endbloco = atoi(palavras[5]);
        add3->endPalavra = atoi(palavras[6]) % 4;
        umaInstrucao->add3 = add3;

        trecho1[index++] = umaInstrucao;
        */
    }
    fclose(file);

    INSTRUCAO *halt = (INSTRUCAO *)malloc(sizeof(INSTRUCAO));
    if (halt)
    {
        halt->opcode = -1;
        halt->add1 = NULL;
        halt->add2 = NULL;
        halt->add3 = NULL;
        trecho1[index] = halt;
    }

    definirPrograma(cpu, trecho1, index + 1);

    // ram = criaRAM_aleatoria(256);
    // iniciar(cpu, ram, 16, 32, 64);
}
int proximoInteiro(int bound)
{
    return rand() % bound;
}
void programaAleatorio(RAM *ram, CPU *cpu, int qtdInstrucoes, int tamanhoRam, HD *hd)
{
    INSTRUCAO **trecho1 = (INSTRUCAO **)malloc(qtdInstrucoes * sizeof(INSTRUCAO *));
    srand(time(NULL));

    for (int i = 0; i < qtdInstrucoes - 1; i++)
    {
        INSTRUCAO *umaInstrucao = (INSTRUCAO *)malloc(sizeof(INSTRUCAO));
        umaInstrucao->opcode = proximoInteiro(2);

        umaInstrucao->add1 = (ENDERECO *)malloc(sizeof(ENDERECO));
        umaInstrucao->add1->endbloco = proximoInteiro(tamanhoRam);
        umaInstrucao->add1->endPalavra = proximoInteiro(4);

        umaInstrucao->add2 = (ENDERECO *)malloc(sizeof(ENDERECO));
        umaInstrucao->add2->endbloco = proximoInteiro(tamanhoRam);
        umaInstrucao->add2->endPalavra = proximoInteiro(4);

        umaInstrucao->add3 = (ENDERECO *)malloc(sizeof(ENDERECO));
        umaInstrucao->add3->endbloco = proximoInteiro(tamanhoRam);
        umaInstrucao->add3->endPalavra = proximoInteiro(4);

        trecho1[i] = umaInstrucao;
    }

    INSTRUCAO *ultimaInstrucao = (INSTRUCAO *)malloc(sizeof(INSTRUCAO));
    ultimaInstrucao->opcode = -1;
    ultimaInstrucao->add1 = NULL;
    ultimaInstrucao->add2 = NULL;
    ultimaInstrucao->add3 = NULL;
    trecho1[qtdInstrucoes - 1] = ultimaInstrucao;

    ram = criaRAM_aleatoria(tamanhoRam);
    definirPrograma(cpu, trecho1, qtdInstrucoes);
    for (int i = 0; i < qtdInstrucoes; i++)
    {
        if (trecho1[i])
        {
            free(trecho1[i]->add1);
            free(trecho1[i]->add2);
            free(trecho1[i]->add3);
            free(trecho1[i]);
        }
    }
    free(trecho1);
    iniciar(cpu, ram, 16, 32, 64, hd);

    for (int i = 0; i < qtdInstrucoes; i++)
    {
        free(trecho1[i]->add1);
        free(trecho1[i]->add2);
        free(trecho1[i]->add3);
        free(trecho1[i]);
    }
    free(trecho1);
}

void carregarTratadorInterrupcao(RAM *ram)
{
    INSTRUCAO tratador[] = {
        {0x00, NULL, NULL, NULL}, // Não opera
        {-1, NULL, NULL, NULL},   // HALT

    };
    for (int i = 0; i < 2; i++)
    {
        ram->bloco[0].palavras[i] = tratador[i].opcode;
    }
    if (ram && ram->bloco)
    {
        ram->bloco[0].palavras[0] = 0;  // NOP
        ram->bloco[0].palavras[1] = -1; // HALT
    }
}

void carregarTratadorIO(RAM *ram)
{
    INSTRUCAO tratador[] = {
        {0x10, NULL, NULL, NULL}, // Lê do console
        {0x11, NULL, NULL, NULL}, // Escreve no console
        {-1, NULL, NULL, NULL}    // HALT
    };
    for (int i = 0; i < 3; i++)
    {
        ram->bloco[1].palavras[i] = tratador[i].opcode;
    }
    if (ram && ram->bloco)
    {
        ram->bloco[1].palavras[0] = 0x10; // IO_READ
        ram->bloco[1].palavras[1] = 0x11; // IO_WRITE
        ram->bloco[1].palavras[2] = -1;   // HALT
    }
}

void carregarTratadorDMA(RAM *ram)
{
    INSTRUCAO tratador[] = {
        {0x20, NULL, NULL, NULL}, // DMA_READ (opcode customizado)
        {-1, NULL, NULL, NULL}    // HALT
    };
    for (int i = 0; i < 2; i++)
    {
        ram->bloco[2].palavras[i] = tratador[i].opcode;
    }
    if (ram && ram->bloco)
    {
        ram->bloco[2].palavras[0] = 0x20; // DMA_START
        ram->bloco[2].palavras[1] = -1;   // HALT
    }
}