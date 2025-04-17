#ifndef IO_H
#define IO_H

#include <stdbool.h>

typedef struct {
    bool pronto;
    int dado;
} DispositivoIO;

void iniciarIO(DispositivoIO *io);
int lerIO(DispositivoIO *io);
void escreverIO(DispositivoIO *io, int dado);

#endif