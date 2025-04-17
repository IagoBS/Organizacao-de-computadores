#ifndef INSTRUCAO_H
#define INSTRUCAO_H

#include "./endereco.h"
#include <math.h>
#include <stdbool.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
typedef struct {
   int opcode;
   ENDERECO *add1;
   ENDERECO *add2;
   ENDERECO *add3;
} INSTRUCAO;



     
        
#endif