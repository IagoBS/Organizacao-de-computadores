#ifndef BLOCO_H
#define BLOCO_H


#include <stdbool.h>



typedef struct
{
    int palavras[4];
    int endBloco;
    bool atualizado;
    int custo;
    int cacheHit;
    int tam;
    int bloco_chamada;
    bool emHD;
} Bloco;

#include "cache.h"
#endif