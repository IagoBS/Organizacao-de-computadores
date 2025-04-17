#ifndef PROGRAMA_H
#define PROGRAMA_H

#include "ram.h"
#include "cpu.h"

typedef struct programas
{
    RAM *ram;
    CPU *cpu;
} Programas;

Programas *novoPrograma(RAM *ram, CPU *cpu);
void programaAleatorioRepeticoes(RAM *ram, CPU *cpu);
void programaAleatorio(RAM *ram, CPU *cpu, int qtdInstrucoes, int tamanhoRam, HD *hd);

// Declarações dos tratadores
void carregarTratadorInterrupcao(RAM *ram);
void carregarTratadorIO(RAM *ram);
void carregarTratadorDMA(RAM *ram);

#endif