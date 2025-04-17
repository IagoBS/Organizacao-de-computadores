// hd.c
#include "hd.h"
#include <stdio.h>

HD *inicializarHD(const char *nome_arquivo, int tamanho)
{
    HD *hd = malloc(sizeof(HD));
    hd->arquivo = fopen(nome_arquivo, "rb+");

    if (!hd->arquivo)
    {
        hd->arquivo = fopen(nome_arquivo, "wb+");
        hd->tamanho = tamanho;
        Bloco bloco_vazio;
        for (int i = 0; i < tamanho; i++)
        {
            fwrite(&bloco_vazio, sizeof(Bloco), 1, hd->arquivo);
        }
    } else {
        fseek(hd->arquivo, 0, SEEK_END);
        hd->tamanho = ftell(hd->arquivo) / sizeof(Bloco);
    }
    return hd;
}

Bloco lerBlocoHD(HD *hd, int endBloco)
{
    Bloco bloco;
    fseek(hd->arquivo, endBloco * sizeof(Bloco), SEEK_SET);
    fread(&bloco, sizeof(Bloco), 1, hd->arquivo);
    return bloco;
}

void escreverBlocoHD(HD *hd, int endBloco, Bloco bloco)
{
    fseek(hd->arquivo, endBloco * sizeof(Bloco), SEEK_SET);
    fwrite(&bloco, sizeof(Bloco), 1, hd->arquivo);
    fflush(hd->arquivo);
}

void liberarHD(HD *hd)
{
    fclose(hd->arquivo);
    free(hd);
}