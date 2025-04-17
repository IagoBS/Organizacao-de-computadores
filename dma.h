#ifndef DMA_H
#define DMA_H

#include "ram.h"
#include "hd.h"

typedef struct {
    int origem;
    int destino;
    bool ocupado;
} ControladorDMA;

void iniciarDMA(ControladorDMA *dma, HD *hd, RAM *ram, int tamanho);
bool dmaPronto(ControladorDMA *dma);

#endif