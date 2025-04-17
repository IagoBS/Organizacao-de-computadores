#include <stdlib.h>
#include <stdio.h>
#include "cpu.h"
#include "programas.h"

CPU *criarCPU()
{
    CPU *cpu = (CPU *)malloc(sizeof(CPU));
    if (cpu == NULL)
    {
        fprintf(stderr, "Erro ao alocar CPU\n");
        return NULL;
    }

    // Inicialização explícita de todos os campos
    cpu->programa = NULL;
    cpu->quantidade_instrucao = 0;
    cpu->PC = 0;

    // Aloca registradores
    cpu->registrador1 = (Bloco *)malloc(sizeof(Bloco));
    cpu->registrador2 = (Bloco *)malloc(sizeof(Bloco));
    cpu->registrador3 = (Bloco *)malloc(sizeof(Bloco));

    if (cpu->registrador1 == NULL || cpu->registrador2 == NULL || cpu->registrador3 == NULL)
    {
        fprintf(stderr, "Erro ao alocar registradores\n");
        liberarCPU(cpu);
        return NULL;
    }

    // Inicializa registradores com zeros
    for (int i = 0; i < 4; i++)
    {
        cpu->registrador1->palavras[i] = 0;
        cpu->registrador2->palavras[i] = 0;
        cpu->registrador3->palavras[i] = 0;
    }

    // Inicializa outros campos dos registradores
    cpu->registrador1->endBloco = -1;
    cpu->registrador1->atualizado = false;
    cpu->registrador1->custo = 0;
    cpu->registrador1->cacheHit = 0;
    cpu->registrador1->bloco_chamada = 0;

    cpu->registrador2->endBloco = -1;
    cpu->registrador2->atualizado = false;
    cpu->registrador2->custo = 0;
    cpu->registrador2->cacheHit = 0;
    cpu->registrador2->bloco_chamada = 0;

    cpu->registrador3->endBloco = -1;
    cpu->registrador3->atualizado = false;
    cpu->registrador3->custo = 0;
    cpu->registrador3->cacheHit = 0;
    cpu->registrador3->bloco_chamada = 0;

    // Aloca console
    cpu->console = (DispositivoIO *)malloc(sizeof(DispositivoIO));
    if (cpu->console == NULL)
    {
        fprintf(stderr, "Erro ao alocar console\n");
        liberarCPU(cpu);
        return NULL;
    }

    // Inicializa console
    cpu->console->dado = false;
    cpu->console->pronto = 0;

    // Inicializa DMA
    cpu->dma = NULL;

    // Inicializa estatísticas
    cpu->custo = 0;
    cpu->missC1 = 0;
    cpu->hitC1 = 0;
    cpu->missC2 = 0;
    cpu->hitC2 = 0;
    cpu->missC3 = 0;
    cpu->hitC3 = 0;

    // Inicializa interrupções
    cpu->interrupcao_global_habilitada = true;
    for (int i = 0; i < NUM_TIPOS_INTERRUPCAO; i++)
    {
        cpu->interrupcoes[i].tipo = i;
        cpu->interrupcoes[i].ativa = false;
        cpu->interrupcoes[i].prioridade = 0;
    }

    // Aloca a pilha de contexto
    cpu->pilhaContexto = (int *)malloc(20 * sizeof(int));
    if (cpu->pilhaContexto == NULL)
    {
        fprintf(stderr, "Erro ao alocar pilha de contexto\n");
        liberarCPU(cpu);
        return NULL;
    }

    cpu->pcInterrupcao = 0x000;

    return cpu;
}
void definirPrograma(CPU *cpu, INSTRUCAO **auxiliar, int quantidade_processamento)
{
    if (auxiliar == NULL || cpu == NULL)
    {
        fprintf(stderr, "Erro: CPU ou vetor de instruções é NULL\n");
        return;
    }

    // Libera programa anterior se existir
    if (cpu->programa != NULL)
    {
        for (int i = 0; i < cpu->quantidade_instrucao; i++)
        {
            free(cpu->programa[i].add1);
            free(cpu->programa[i].add2);
            free(cpu->programa[i].add3);
        }
        free(cpu->programa);
        cpu->programa = NULL;
    }

    cpu->quantidade_instrucao = quantidade_processamento;
    cpu->programa = (INSTRUCAO *)malloc(quantidade_processamento * sizeof(INSTRUCAO));

    if (cpu->programa == NULL)
    {
        fprintf(stderr, "Erro ao alocar memória para o programa\n");
        return;
    }

    for (int i = 0; i < quantidade_processamento; i++)
    {
        if (auxiliar[i] == NULL)
        {
            fprintf(stderr, "Erro: auxiliar[%d] é NULL!\n", i);
            continue;
        }

        cpu->programa[i].opcode = auxiliar[i]->opcode;

        // Aloca endereços
        cpu->programa[i].add1 = (ENDERECO *)malloc(sizeof(ENDERECO));
        cpu->programa[i].add2 = (ENDERECO *)malloc(sizeof(ENDERECO));
        cpu->programa[i].add3 = (ENDERECO *)malloc(sizeof(ENDERECO));

        if (cpu->programa[i].add1 == NULL || cpu->programa[i].add2 == NULL || cpu->programa[i].add3 == NULL)
        {
            fprintf(stderr, "Erro ao alocar memória para endereços na instrução %d\n", i);

            // Libera o que foi alocado até agora
            for (int j = 0; j <= i; j++)
            {
                if (j < i)
                {
                    free(cpu->programa[j].add1);
                    free(cpu->programa[j].add2);
                    free(cpu->programa[j].add3);
                }
                else
                {
                    if (cpu->programa[j].add1)
                        free(cpu->programa[j].add1);
                    if (cpu->programa[j].add2)
                        free(cpu->programa[j].add2);
                    if (cpu->programa[j].add3)
                        free(cpu->programa[j].add3);
                }
            }
            free(cpu->programa);
            cpu->programa = NULL;
            return;
        }

        // Copia os dados dos endereços
        if (auxiliar[i]->add1)
            *cpu->programa[i].add1 = *auxiliar[i]->add1;
        else
            memset(cpu->programa[i].add1, 0, sizeof(ENDERECO));

        if (auxiliar[i]->add2)
            *cpu->programa[i].add2 = *auxiliar[i]->add2;
        else
            memset(cpu->programa[i].add2, 0, sizeof(ENDERECO));

        if (auxiliar[i]->add3)
            *cpu->programa[i].add3 = *auxiliar[i]->add3;
        else
            memset(cpu->programa[i].add3, 0, sizeof(ENDERECO));
    }

    // Reinicia estatísticas
    cpu->custo = 0;
    cpu->missC1 = 0;
    cpu->hitC1 = 0;
    cpu->missC2 = 0;
    cpu->hitC2 = 0;
    cpu->missC3 = 0;
    cpu->hitC3 = 0;
    cpu->PC = 0; // Reinicia o contador de programa
}

CACHE *inicializarCache(int tamanho)
{
    CACHE *cache = malloc(sizeof(CACHE));
    if (cache == NULL)
    {
        printf("Erro ao alocar cachce");
        return NULL;
    }
    cache->tamanho = tamanho;
    cache->linhas = (Bloco **)malloc(cache->tamanho * sizeof(Bloco *));
    if (cache->linhas == NULL)
    {
        printf("Erro ao alocar linhas da cache \n");
        free(cache);
        return NULL;
    }

    for (int i = 0; i < cache->tamanho; i++)
    {
        cache->linhas[i] = (Bloco *)malloc(sizeof(Bloco));
        if (cache->linhas[i] == NULL)
        {
            printf("Erro ao alocar bloco de memória na cache! \n");
            for (int k = 0; k < i; k++)
            {
                free(cache->linhas[k]);
            }
            free(cache->linhas);
            free(cache);
            return NULL;
        }

        for (int j = 0; j < 4; j++)
        {
            cache->linhas[i]->palavras[j] = 0;
        }
        cache->linhas[i]->endBloco = -1;
        cache->linhas[i]->atualizado = false;
        cache->linhas[i]->custo = 0;
        cache->linhas[i]->cacheHit = 0;
        cache->linhas[i]->bloco_chamada = 0;
    }
    return cache;
}

void iniciar(CPU *cpu, RAM *ram, int tamanho_cache1, int tamanho_cache2, int tamanho_cache3, HD *hd)
{
    cpu->cache1 = inicializarCache(tamanho_cache1);
    cpu->cache2 = inicializarCache(tamanho_cache2);
    cpu->cache3 = inicializarCache(tamanho_cache3);

    iniciarIO(cpu->console);

    // carrega tratadores na RAM
    carregarTratadorInterrupcao(ram);
    carregarTratadorIO(ram);
    carregarTratadorDMA(ram);

    while (cpu->PC < cpu->quantidade_instrucao && cpu->opcode != -1)
    {
        if (cpu->PC < 0 || cpu->PC >= cpu->quantidade_instrucao)
        {
            fprintf(stderr, "PC inválido: %d\n", cpu->PC);
            break;
        }
        INSTRUCAO instrucao = cpu->programa[cpu->PC];
        cpu->opcode = instrucao.opcode;
        verificarInterrupcao(cpu);
        verificarInterrupcaoIO(cpu);

        if (cpu->interrupcoes[INTERRUPCAO_TIMER].ativa)
        {
            tratarInterrupcao(cpu, ram, hd);

            cpu->interrupcoes[INTERRUPCAO_TIMER].ativa = false;
        }
        if (cpu->interrupcoes[INTERRUPCAO_IO].ativa)
        {
            tratarInterrupcao(cpu, ram, hd);
            cpu->interrupcoes[INTERRUPCAO_IO].ativa = false;
        }

        for (int i = 0; i < 3; i++)
        {
            if (cpu->interrupcoes[i].ativa)
            {
                tratarInterrupcao(cpu, ram, hd);
            }
        }

        if (cpu->opcode != -1)
        {

            cpu->registrador1 = buscarNasMemorias(*instrucao.add1, ram, cpu->cache1, cpu->cache2, cpu->cache3, hd);
            cpu->registrador2 = buscarNasMemorias(*instrucao.add2, ram, cpu->cache1, cpu->cache2, cpu->cache3, hd);
            cpu->registrador3 = buscarNasMemorias(*instrucao.add3, ram, cpu->cache1, cpu->cache2, cpu->cache3, hd);
            if (cpu->registrador1 == NULL)
            {
                printf("Buscando no HD");
                buscarDoHD(ram, hd, cpu->registrador1->endBloco);
            }
            if (cpu->registrador1 == NULL)
            {
                printf("Buscando no HD");
                buscarDoHD(ram, hd, cpu->registrador2->endBloco);
            }
            if (cpu->registrador3 == NULL)
            {
                printf("Buscando no HD");
                buscarDoHD(ram, hd, cpu->registrador3->endBloco);
            }

            cpu->registrador1->bloco_chamada += 1;
            cpu->registrador2->bloco_chamada += 1;
            cpu->registrador3->bloco_chamada += 1;
            switch (cpu->registrador1->cacheHit)
            {
            case 1:
                cpu->hitC1++;
                break;
            case 2:
                cpu->missC1++;
                cpu->hitC2++;
                break;
            case 3:
                cpu->missC1++;
                cpu->missC2++;
                cpu->hitC3++;
                break;
            case 4:
                cpu->missC1++;
                cpu->missC2++;
                cpu->missC3++;
            }
            switch (cpu->registrador2->cacheHit)
            {
            case 1:
                cpu->hitC1++;
                break;
            case 2:
                cpu->missC1++;
                cpu->hitC2++;
                break;
            case 3:
                cpu->missC1++;
                cpu->missC2++;
                cpu->hitC3++;
                break;
            case 4:
                cpu->missC1++;
                cpu->missC2++;
                cpu->missC3++;
            }
            switch (cpu->registrador3->cacheHit)
            {
            case 1:
                cpu->hitC1++;
                break;
            case 2:
                cpu->missC1++;
                cpu->hitC2++;
                break;
            case 3:
                cpu->missC1++;
                cpu->missC2++;
                cpu->hitC3++;
                break;
            case 4:
                cpu->missC1++;
                cpu->missC2++;
                cpu->missC3++;
                break;
            }
            switch (cpu->opcode)
            {
            case -1: // HALT
                printf("Programa finalizado.\n");
                imprimirRAM(ram);
                break;

            case 0x10:
            case 0x11:
                executarIO(cpu, instrucao.opcode);
                break;
            case 0x20:
                executarDMA(cpu, hd, ram);
                break;
            }

            cpu->custo += cpu->registrador1->custo + cpu->registrador2->custo + cpu->registrador3->custo;
            cpu->PC++;
        }
    } // fim do While
} // fim da função

void verificarInterrupcao(CPU *cpu)
{
    if (!cpu->interrupcao_global_habilitada)
        return;

    // Verifica interrupção de timer (exemplo: a cada 100 instruções)
    if (cpu->PC % 100 == 0)
    {
        cpu->interrupcoes[INTERRUPCAO_TIMER].ativa = true;
        cpu->interrupcoes[INTERRUPCAO_TIMER].prioridade = 2;
    }

    // Verifica interrupção de I/O (exemplo: quando dispositivo está pronto)
    if (cpu->console->pronto)
    {
        cpu->interrupcoes[INTERRUPCAO_IO].ativa = true;
        cpu->interrupcoes[INTERRUPCAO_IO].prioridade = 1;
    }
}
bool temInterrupcaoPendente(CPU *cpu)
{
    for (int i = 0; i < NUM_TIPOS_INTERRUPCAO; i++)
    {
        if (cpu->interrupcoes[i].ativa)
        {
            return true;
        }
    }
    return false;
}
void salvarContexto(CPU *cpu)
{
    if (!cpu || !cpu->pilhaContexto || !cpu->registrador1)
    {
        fprintf(stderr, "Erro: Ponteiros inválidos em salvarContexto()\n");
        return;
    }
    cpu->pilhaContexto[0] = cpu->PC;
    // Salva registrador1 (4 palavras)
    for (int j = 0; j < 4; j++)
    {
        cpu->pilhaContexto[1 + j] = cpu->registrador1->palavras[j];
    }

    // Salva registrador2 (4 palavras)
    for (int j = 0; j < 4; j++)
    {
        cpu->pilhaContexto[5 + j] = cpu->registrador2->palavras[j];
    }

    // Salva registrador3 (4 palavras)
    for (int j = 0; j < 4; j++)
    {
        cpu->pilhaContexto[9 + j] = cpu->registrador3->palavras[j];
    }
}
void restaurarContexto(CPU *cpu)
{
    cpu->PC = cpu->pilhaContexto[0];
}
/// @brief
/// @param cpu
/// @param ram
/// @param hd
void tratarInterrupcao(CPU *cpu, RAM *ram, HD *hd)
{
    if (cpu->PC >= cpu->quantidade_instrucao)
    {
        fprintf(stderr, "Erro: PC fora dos limites!\n");
        return;
    }
    salvarContexto(cpu);
    int maior_prioridade = -1;
    int interrupcao_para_tratar = -1;
    int pc_original = cpu->PC;
    cpu->PC = cpu->pcInterrupcao;
    for (int i = 0; i < NUM_TIPOS_INTERRUPCAO; i++)
    {
        if (cpu->interrupcoes[i].ativa &&
            cpu->interrupcoes[i].prioridade > maior_prioridade)
        {
            maior_prioridade = cpu->interrupcoes[i].prioridade;
            interrupcao_para_tratar = i;
        }
    }
    if (interrupcao_para_tratar == -1)
        return;
    cpu->interrupcao_global_habilitada = false;
    salvarContexto(cpu);

    switch (interrupcao_para_tratar)
    {
    case INTERRUPCAO_TIMER:
        cpu->PC = INTERRUPCAO_TIMER;
        break;
    case INTERRUPCAO_IO:
        cpu->PC = INTERRUPCAO_IO;
        break;
    case INTERRUPCAO_DMA:
        cpu->PC = INTERRUPCAO_IO;
        break;
    }
    while (cpu->PC < cpu->quantidade_instrucao && cpu->opcode != -1)
    {
        INSTRUCAO instrucao = cpu->programa[cpu->PC];
        cpu->opcode = instrucao.opcode;
        if (cpu->opcode != -1)
        {

            cpu->registrador1 = buscarNasMemorias(*instrucao.add1, ram, cpu->cache1, cpu->cache2, cpu->cache3, hd);
            cpu->registrador2 = buscarNasMemorias(*instrucao.add2, ram, cpu->cache1, cpu->cache2, cpu->cache3, hd);
            cpu->registrador3 = buscarNasMemorias(*instrucao.add3, ram, cpu->cache1, cpu->cache2, cpu->cache3, hd);
            if (cpu->registrador1 == NULL || cpu->registrador2 == NULL || cpu->registrador3 == NULL)
            {
                printf("Endereço buscado é inexistente \n");
            }
            else
            {
                cpu->registrador1->bloco_chamada += 1;
                cpu->registrador2->bloco_chamada += 1;
                cpu->registrador3->bloco_chamada += 1;
                switch (cpu->registrador1->cacheHit)
                {
                case 1:
                    cpu->hitC1++;
                    break;
                case 2:
                    cpu->missC1++;
                    cpu->hitC2++;
                    break;
                case 3:
                    cpu->missC1++;
                    cpu->missC2++;
                    cpu->hitC3++;
                    break;
                case 4:
                    cpu->missC1++;
                    cpu->missC2++;
                    cpu->missC3++;
                }
                switch (cpu->registrador2->cacheHit)
                {
                case 1:
                    cpu->hitC1++;
                    break;
                case 2:
                    cpu->missC1++;
                    cpu->hitC2++;
                    break;
                case 3:
                    cpu->missC1++;
                    cpu->missC2++;
                    cpu->hitC3++;
                    break;
                case 4:
                    cpu->missC1++;
                    cpu->missC2++;
                    cpu->missC3++;
                }
                switch (cpu->registrador3->cacheHit)
                {
                case 1:
                    cpu->hitC1++;
                    break;
                case 2:
                    cpu->missC1++;
                    cpu->hitC2++;
                    break;
                case 3:
                    cpu->missC1++;
                    cpu->missC2++;
                    cpu->hitC3++;
                    break;
                case 4:
                    cpu->missC1++;
                    cpu->missC2++;
                    cpu->missC3++;
                    break;
                }
                switch (cpu->opcode)
                {
                case -1: // HALT
                    printf("Programa finalizado.\n");
                    cpu->PC = cpu->quantidade_instrucao;
                    imprimirRAM(ram);
                    break;
                case 0:

                    cpu->registrador3->palavras[instrucao.add3->endPalavra] = cpu->registrador1->palavras[instrucao.add1->endPalavra] + cpu->registrador2->palavras[instrucao.add2->endPalavra];
                    cpu->registrador3->atualizado = true;

                    cpu->custo += cpu->registrador1->custo + cpu->registrador2->custo + cpu->registrador3->custo;

                    printf("Instrução soma -> RAM posicao %d com conteudo na cache 1 %d\n", instrucao.add3->endbloco, cpu->registrador3->palavras[instrucao.add3->endPalavra]);
                    printf("Custo ateh o momento.... %d\n", cpu->custo);

                    printf("No momento: Hit C1: %d Miss C1: %d\n", cpu->hitC1, cpu->missC1);
                    printf("No momento: Hit C2: %d Miss C2: %d\n", cpu->hitC2, cpu->missC2);
                    printf("No momento: Hit C3: %d Miss C3: %d\n", cpu->hitC3, cpu->missC3);
                    break;
                case 1:

                    cpu->registrador3->palavras[instrucao.add3->endPalavra] = cpu->registrador1->palavras[instrucao.add1->endPalavra] - cpu->registrador2->palavras[instrucao.add2->endPalavra];
                    cpu->registrador3->atualizado = true;

                    cpu->custo += cpu->registrador1->custo + cpu->registrador2->custo + cpu->registrador3->custo;

                    printf("Instrucao subtracao = RAM posicao %d com conteudo na cache 1 %d\n", instrucao.add3->endbloco, cpu->registrador3->palavras[instrucao.add3->endPalavra]);
                    printf("Custo ate o momento.... %d\n", cpu->custo);

                    printf("subtração: Hit C1: %d Miss C1: %d\n", cpu->hitC1, cpu->missC1);
                    printf("subtração: Hit C2: %d Miss C2: %d\n", cpu->hitC2, cpu->missC2);
                    printf("subtração: Hit C3: %d Miss C3: %d\n", cpu->hitC3, cpu->missC3);

                    break;
                }
                cpu->custo += cpu->registrador1->custo + cpu->registrador2->custo + cpu->registrador3->custo;
                cpu->PC++;
            }
            printf("[INTERRUPCAO] Tratando interrupção tipo %d\n", interrupcao_para_tratar);
            printf("[INTERRUPCAO] Iniciando tratamento (PC=%d)\n", cpu->PC);
        }
    } // fim do While
    restaurarContexto(cpu);
    cpu->interrupcoes[interrupcao_para_tratar].ativa = false;
    cpu->interrupcao_global_habilitada = true;
}

void verificarInterrupcaoIO(CPU *cpu)
{

    if (cpu->console->pronto)
    {
        cpu->interrupcoes[INTERRUPCAO_SOFTWARE].ativa = true;
    }
}

void executarIO(CPU *cpu, int opcode)
{
    switch (opcode)
    {
    case 0x10:
        cpu->registrador1->palavras[0] = lerIO(cpu->console);
        cpu->custo += 50;
        break;
    case 0x11:
        escreverIO(cpu->console, cpu->registrador1->palavras[0]);
        cpu->custo += 50;
        break;
    default:
        fprintf(stderr, "Opcode Io inválido %d\n", opcode);
    }
}

void executarDMA(CPU *cpu, HD *hd, RAM *ram)
{
    if (dmaPronto(cpu->dma))
    {
        cpu->dma->origem = cpu->registrador1->palavras[0];  // Origem do HD
        cpu->dma->destino = cpu->registrador2->palavras[0]; // Destino do HD
        iniciarDMA(cpu->dma, hd, ram, 4);
        cpu->custo += 100;
    }
    else
    {
        fprintf(stderr, "DMA ocupado!\n");
    }
}

CACHE *liberarCache(CACHE *cache)
{
    if (!cache)
        return NULL;

    if (cache->linhas)
    {
        for (int i = 0; i < cache->tamanho; i++)
        {
            free(cache->linhas[i]); // Libera cada bloco na cache
        }
        free(cache->linhas);
    }

    free(cache);
    return NULL;
}

CPU *liberarCPU(CPU *cpu)
{
    if (!cpu)
        return NULL;

    // Libera programa
    if (cpu->programa)
    {
        for (int i = 0; i < cpu->quantidade_instrucao; i++)
        {
            free(cpu->programa[i].add1);
            free(cpu->programa[i].add2);
            free(cpu->programa[i].add3);
        }
        free(cpu->programa);
    }

    free(cpu->registrador1);
    free(cpu->registrador2);
    free(cpu->registrador3);

    // Libera registradores
    // Libera caches
    liberarCache(cpu->cache1);
    liberarCache(cpu->cache2);
    liberarCache(cpu->cache3);

    // Libera console e pilha
    free(cpu->console);
    free(cpu->pilhaContexto);

    free(cpu);
    return NULL;
}