#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cpu.h"
#include "ram.h"
#include "hd.h"
#include "programas.h"
#include "gerador.h"
#include "io.h"      
#include "dma.h"     


int main() {
    // Inicializa componentes com tamanhos pré-definidos
    const int TAMANHO_RAM = 256;      // 256 blocos na RAM
    const int TAMANHO_HD = 1024;      // 1024 blocos no HD
    const int CACHE_L1 = 16;          // 16 blocos na L1
    const int CACHE_L2 = 32;          // 32 blocos na L2
    const int CACHE_L3 = 64;          // 64 blocos na L3
    const int PROG_SIZE = 100;        // 100 instruções no programa

    // 1. Inicialização do sistema
    printf("Inicializando sistema...\n");
    HD *hd = inicializarHD("hd.bin", TAMANHO_HD);
    RAM *ram = criaRAM_vazia(TAMANHO_RAM);
    CPU *cpu = criarCPU();
    cpu->pcInterrupcao = 0;
    if (!hd || !ram || !cpu) {
        fprintf(stderr, "Falha na inicialização do sistema\n");
        return EXIT_FAILURE;
    }

    // 2. Geração de programas
    printf("Gerando programa de teste...\n");
    gerarInstrucoes();  // Gera programa.txt
    
    // 3. Carregamento de programas
    printf("Carregando programas...\n");
    
    // Programa alternativo (instruções aleatórias)
    programaAleatorio(ram, cpu, PROG_SIZE, TAMANHO_RAM, hd);
    
    // 4. Configuração final da CPU
    printf("Configurando CPU...\n");
    cpu->pcInterrupcao = 0x0000;      // Endereço do tratador de interrupções
    cpu->pilhaContexto = malloc(10 * sizeof(int));

    // 5. Execução do sistema
    printf("\nIniciando execução...\n");
    printf("\n");
    iniciar(cpu, ram, CACHE_L1, CACHE_L2, CACHE_L3, hd);
    printf("\n");
    printf("Execução concluída.\n");

    // 6. Relatório final
    printf("\nRelatório de desempenho:\n");
    printf("------------------------\n");
    printf("Total de ciclos: %d\n", cpu->custo);
    printf("Cache L1: Hits=%d, Misses=%d\n", cpu->hitC1, cpu->missC1);
    printf("Cache L2: Hits=%d, Misses=%d\n", cpu->hitC2, cpu->missC2);
    printf("Cache L3: Hits=%d, Misses=%d\n", cpu->hitC3, cpu->missC3);

    // 7. Liberação de recursos
    printf("\nLiberando recursos...\n");
    liberarHD(hd);
    liberarRAM(ram);
    liberarCPU(cpu);

    printf("Sistema finalizado com sucesso.\n");
    return 0;
}