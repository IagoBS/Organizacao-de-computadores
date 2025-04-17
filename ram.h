#ifndef RAM_H
#define RAM_H

#include "./bloco.h"
#include "hd.h"

typedef struct {
    Bloco *bloco;
    int tamanho;
} RAM;

          
RAM *criaRAM_vazia(int tamanho);          
RAM *criaRAM_aleatoria(int tamanho);

void defineValor(RAM *ram, int endereco, Bloco chamada);
Bloco visualizarDado(RAM *ram, int endereco); 

void imprimirRAM(RAM *ram);
void imprimirBloco(Bloco bloco);    
void moverParaHD(RAM *ram, HD *hd, int endBlocoRam);
Bloco buscarDoHD(RAM *ram, HD *hd, int endBlocoRam);

RAM *liberarRAM(RAM *ram);       
        
#endif