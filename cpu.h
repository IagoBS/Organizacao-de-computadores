#ifndef CPU_H
#define CPU_H

#include "ram.h"
#include "mmu.h"
#include "instrucao.h"
#include "bloco.h"
#include "cache.h"
#include "io.h"      
#include "dma.h"     
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum {
  INTERRUPCAO_TIMER,
  INTERRUPCAO_IO, 
  INTERRUPCAO_SOFTWARE,
  INTERRUPCAO_DMA,
  NUM_TIPOS_INTERRUPCAO
} TipoInterrupcao;

typedef struct {
  int tipo;
  bool ativa;
  int prioridade;
} INTERRUPCAO;

typedef struct
{
  RAM *ram;
  Bloco *registrador1;
  Bloco *registrador2;
  Bloco *registrador3;
  CACHE *cache1;
  CACHE *cache2;
  CACHE *cache3;
  INSTRUCAO *programa;
  INTERRUPCAO interrupcoes[NUM_TIPOS_INTERRUPCAO];
  bool interrupcao_global_habilitada;
  DispositivoIO *console;
  ControladorDMA *dma;

  int *pilhaContexto;
  int pcInterrupcao;
  int PC;
  int opcode;
  int custo;
  int missC1;
  int hitC1;
  int missC2;
  int hitC2;
  int missC3;
  int hitC3;
  int tamanhoCache1;
  int tamanhoCache2;
  int quantidade_instrucao;
} CPU;

CPU *criarCPU();
CACHE *inicializarCache(int tamanho);
void definirPrograma(CPU *cpu, INSTRUCAO **auxiliar, int quantidade_processamento);
void iniciar(CPU *cpu, RAM *ram, int tamanho_cache1, int tamanho_cache2, int tamanho_cache3, HD *hd);

void verificarInterrupcao(CPU *cpu);
void salvarContexto(CPU *cpu);

void restaurarContexto(CPU *cpu);
void tratarInterrupcao(CPU *cpu, RAM *ram, HD *hd);

void verificarInterrupcaoIO(CPU *cpu);
void executarIO(CPU *cpu, int opcode);

void executarDMA(CPU *cpu, HD *hd, RAM *ram);


CACHE *liberarCache(CACHE *cache);
INSTRUCAO *liberarInstrucao(INSTRUCAO *instrucao);
CPU *liberarCPU(CPU *cpu);

#endif