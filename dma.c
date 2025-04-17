#include "dma.h"
#include <unistd.h>

void iniciarDMA(ControladorDMA *dma, HD *hd, RAM *ram, int tamanho) {
    dma->ocupado = true;
    for (int i = 0; i < tamanho; i++) {
        Bloco bloco = lerBlocoHD(hd, dma->origem + i);
        ram->bloco[dma->destino + i] = bloco;
        usleep(1000);  // Simula latência
    }
    dma->ocupado = false;
}

bool dmaPronto(ControladorDMA *dma) {
    return !dma->ocupado;
}