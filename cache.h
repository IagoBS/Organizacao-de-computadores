#ifndef CACHE_H
#define CACHE_H

#include "bloco.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    Bloco **linhas;
    int tamanho;
} CACHE;

#endif