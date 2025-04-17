#ifndef HD_H
#define HD_H
#include "bloco.h"

typedef struct {
    FILE *arquivo;
    int tamanho;
} HD;

HD *inicializarHD(const char *nome_arquivo, int tamanho);

Bloco lerBlocoHD(HD *hd, int endBloco);
void escreverBlocoHD(HD *hd, int endBloco, Bloco bloco);
void liberarHD(HD *hd);
#endif
