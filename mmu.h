#ifndef MMU_H
#define MMU_H

#include "bloco.h"
#include "endereco.h"
#include "ram.h"
#include "cache.h"
#include "hd.h"
// Função para buscar nas memórias


Bloco *buscarNasMemorias(ENDERECO endereco, RAM *ram, CACHE *cache1, CACHE *cache2, CACHE *cache3, HD *hd);
Bloco *movRamCache3(ENDERECO endereco, int posicaoCache1, int posicaoCache2, int posicaoCache3, RAM *ram, CACHE *cache1, CACHE *cache2, CACHE *cache3);
Bloco *movCache3Cache2(ENDERECO endereco, int posicaoCache1, int posicaoCache2, int posicaoCache3, RAM *ram, CACHE *cache1, CACHE *cache2, CACHE *cache3);
Bloco *movCache2Cache1(ENDERECO endereco, int posicaoCache1, int posicaoCache2, int posicaoCache3, RAM *ram, CACHE *cache1, CACHE *cache2, CACHE *cache3);
int codicaoCache(CACHE *cache);
int CachesVazios(CACHE *cache);
void trocaLinha(Bloco **blocoUm, Bloco **blocoDois);
int seekLeastCalled(CACHE *cache);

#endif // MMU_H