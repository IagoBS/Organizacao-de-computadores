#include "./bloco.h"
#include "./ram.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

RAM *criaRAM_vazia(int tamanho)
{
    RAM *ram = (RAM *)malloc(sizeof(RAM));
    if (ram == NULL)
    {
        fprintf(stderr, "Erro ao alocar memória para RAM\n");
        ram->tamanho = 0;
        return NULL;
    }
    ram->bloco = (Bloco *)malloc(tamanho * sizeof(Bloco));
    if (ram->bloco == NULL)
    {
        free(ram);
        fprintf(stderr, "Erro ao alocar bloco de memória");
        return NULL;
    }
    ram->tamanho = tamanho;

    for (int i = 0; i < ram->tamanho; i++)
    {
        ram->bloco[i].endBloco = i;
        for (int j = 0; j < 4; j++)
        {
            ram->bloco[i].palavras[j] = 0;
        }
        ram->bloco[i].atualizado = false;
        ram->bloco[i].cacheHit = 0;
        ram->bloco[i].custo = 0;
        ram->bloco[i].emHD = false;
    }
    return ram;
}

RAM *criaRAM_aleatoria(int tamanho)
{
    RAM *ram = (RAM *)malloc(sizeof(RAM));
    if (ram == NULL)
    {
        fprintf(stderr, "Erro ao alocar memória para RAM\n");
        return NULL;
    }

    ram->bloco = (Bloco *)malloc(tamanho * sizeof(Bloco));
    if (ram->bloco == NULL)
    {
        fprintf(stderr, "Erro ao alocar bloco de memória");
        free(ram);
        return NULL;
    }

    ram->tamanho = tamanho;
    srand(time(NULL));

    for (int i = 0; i < ram->tamanho; i++)
    {
        ram->bloco[i].endBloco = i;
        for (int j = 0; j < 4; j++)
        {
            ram->bloco[i].palavras[j] = rand() % 100;
        }
        ram->bloco[i].atualizado = false;
        ram->bloco[i].cacheHit = 0;
        ram->bloco[i].custo = 0;
        ram->bloco[i].bloco_chamada = 0;
    }
    return ram;
}

void defineValor(RAM *ram, int endereco, Bloco chamada)
{
    // Validação de segurança
    ram->bloco[endereco] = chamada;
}
Bloco visualizarDado(RAM *ram, int endereco)
{
    return ram->bloco[endereco];
}

void imprimirRAM(RAM *ram)
{
    printf("Conteúdo da RAM TP-2, uma introdução a memória cache:\n");
    for (int i = 0; i < ram->tamanho; i++)
    {
        imprimirBloco(ram->bloco[i]);
        printf("\n");
    }
}
void imprimirBloco(Bloco bloco)
{
    for (int i = 0; i < 4; i++)
    {
        printf("%d, ", bloco.palavras[i]);
        printf("%d\n", bloco.palavras[3]);
    }
}

/**
 * Move um bloco da RAM para o HD (swap out).
 * @param ram Ponteiro para a estrutura RAM
 * @param hd Ponteiro para o HD
 * @param endBlocoRam Endereço do bloco na RAM a ser movido
 */
void moverParaHD(RAM *ram, HD *hd, int endBlocoRam)
{
    // Verifica se o endereço é válido
    if (endBlocoRam < 0 || endBlocoRam >= ram->tamanho)
    {
        fprintf(stderr, "Erro: Endereço de RAM inválido (%d)\n", endBlocoRam);
        return;
    }

    // Obtém o bloco da RAM
    Bloco *bloco = &ram->bloco[endBlocoRam];

    // Escreve no HD (arquivo)
    fseek(hd->arquivo, endBlocoRam * sizeof(Bloco), SEEK_SET);
    if (fwrite(bloco, sizeof(Bloco), 1, hd->arquivo) != 1)
    {
        fprintf(stderr, "Erro ao escrever bloco %d no HD\n", endBlocoRam);
        return;
    }
    fflush(hd->arquivo); // Garante que os dados são persistidos

    // Marca o bloco como "presente no HD"
    bloco->emHD = true;

    printf("Bloco %d movido para o HD\n", endBlocoRam);
}

Bloco buscarDoHD(RAM *ram, HD *hd, int endBlocoRam)
{
    // Verifica se o endereço é válido
    if (endBlocoRam < 0 || endBlocoRam >= ram->tamanho)
    {
        fprintf(stderr, "Erro: Endereço de RAM inválido (%d)\n", endBlocoRam);
        return (Bloco){0}; // Retorna um bloco vazio em caso de erro
    }

    // Lê o bloco do HD
    Bloco bloco;
    fseek(hd->arquivo, endBlocoRam * sizeof(Bloco), SEEK_SET);
    if (fread(&bloco, sizeof(Bloco), 1, hd->arquivo) != 1)
    {
        fprintf(stderr, "Erro ao ler bloco %d do HD\n", endBlocoRam);
        return (Bloco){0};
    }

    // Atualiza a RAM
    ram->bloco[endBlocoRam] = bloco;
    ram->bloco[endBlocoRam].emHD = false; // Agora está na RAM

    printf("Bloco %d carregado do HD para a RAM\n", endBlocoRam);
    return bloco;
}

RAM *liberarRAM(RAM *ram)
{
    if (ram == NULL)
        return NULL;

    free(ram->bloco); // Libera o array de blocos
    free(ram);
    return NULL;
}
