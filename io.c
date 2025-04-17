#include "io.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void iniciarIO(DispositivoIO *io) {
    io->pronto = true;
    io->dado = 0;
}

int lerIO(DispositivoIO *io) {
    io->pronto = false;
    usleep(100000);  // Simula operação demorada
    io->dado = rand() % 100;
    io->pronto = true;
    return io->dado;
}

void escreverIO(DispositivoIO *io, int dado) {
    io->pronto = false;
    usleep(50000);  // Simula operação demorada
    printf("[I/O] Dado escrito: %d\n", dado);
    io->dado = dado;
    io->pronto = true;
}