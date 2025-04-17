#include <stdlib.h>
#include <stdbool.h>
#include "mmu.h"

Bloco *buscarNasMemorias(ENDERECO endereco, RAM *ram, CACHE *cache1, CACHE *cache2, CACHE *cache3, HD *hd)
{
    int custo;

    for (int i = 0; i < cache3->tamanho; i++)
    {
        if (i < cache1->tamanho && cache1->linhas[i]->endBloco != -1)
        {
            if (cache1->linhas[i]->endBloco == endereco.endbloco)
            {
                custo = 10;
                cache1->linhas[i]->custo = custo;
                cache1->linhas[i]->cacheHit = 1;
                return cache1->linhas[i]; // Cache hit in cache1
            }
        }

        if (i < cache2->tamanho && cache2->linhas[i]->endBloco != -1)
        {
            if (cache2->linhas[i]->endBloco == endereco.endbloco)
            {
                custo = 110;
                cache2->linhas[i]->custo = custo;
                cache2->linhas[i]->cacheHit = 2;

                if (codicaoCache(cache1) == 0)
                { // Cache1 is empty
                    return movCache2Cache1(endereco, 0, 0, 0, ram, cache1, cache2, cache3);
                }
                else if (codicaoCache(cache1) == 1)
                { // Cache1 is partially filled
                    return movCache2Cache1(endereco, CachesVazios(cache1), 0, 0, ram, cache1, cache2, cache3);
                }
                else
                { // Cache1 is full
                    return movCache2Cache1(endereco, seekLeastCalled(cache1), 0, 0, ram, cache1, cache2, cache3);
                }
            }
        }

        if (i < cache3->tamanho && cache3->linhas[i]->endBloco != -1)
        {
            if (cache3->linhas[i]->endBloco == endereco.endbloco)
            {
                custo = 1110;
                cache3->linhas[i]->custo = custo;
                cache3->linhas[i]->cacheHit = 3;

                if (codicaoCache(cache2) == 0)
                { // Cache2 is empty
                    return movCache3Cache2(endereco, 0, 0, 0, ram, cache1, cache2, cache3);
                }
                else if (codicaoCache(cache2) == 1)
                { // Cache2 is partially filled
                    return movCache3Cache2(endereco, 0, CachesVazios(cache2), 0, ram, cache1, cache2, cache3);
                }
                else
                { // Cache2 is full
                    return movCache3Cache2(endereco, 0, seekLeastCalled(cache2), 0, ram, cache1, cache2, cache3);
                }
            }
        }

    }
    custo = 1110;
    ram->bloco[endereco.endbloco].custo = custo;
    ram->bloco[endereco.endbloco].cacheHit = 4;

    if (codicaoCache(cache3) == 0)
    {
        return movRamCache3(endereco, 0, 0, 0, ram, cache1, cache2, cache3);
    }
    else if (codicaoCache(cache3) == 1)
    {
        return movRamCache3(endereco, 0, 0, CachesVazios(cache3), ram, cache1, cache2, cache3);
    }
    else
    {
        return movRamCache3(endereco, 0, 0, seekLeastCalled(cache3), ram, cache1, cache2, cache3);
    }

    return NULL; 
}
Bloco *movRamCache3(ENDERECO endereco, int posicaoCache1, int posicaoCache2, int posicaoCache3, RAM *ram, CACHE *cache1, CACHE *cache2, CACHE *cache3)
{
    if (posicaoCache3 < 0 && posicaoCache3 >= cache3->tamanho)
    {
        return NULL;
    }
    Bloco aux = ram->bloco[endereco.endbloco];
    if (codicaoCache(cache3) == 2)
    {
        if (cache3->linhas[posicaoCache3] != NULL)
        {
            ram->bloco[endereco.endbloco] = *cache3->linhas[posicaoCache3];
            ram->bloco[endereco.endbloco].atualizado = false;
        }
    }

    if (cache3->linhas[posicaoCache3] != NULL)
    {
        *cache3->linhas[posicaoCache3] = aux;
        cache3->linhas[posicaoCache3]->atualizado = false;
    }
    else
    {
        return NULL;
    }
    return movCache3Cache2(endereco, posicaoCache1, posicaoCache2, posicaoCache3, ram, cache1, cache2, cache3);
}

Bloco *movCache3Cache2(ENDERECO endereco, int posicaoCache1, int posicaoCache2, int posicaoCache3, RAM *ram, CACHE *cache1, CACHE *cache2, CACHE *cache3)
{
    int posicaoAuxiliar;
    switch (codicaoCache(cache2))
    {
    case 1:
        posicaoAuxiliar = CachesVazios(cache2);
        trocaLinha(&cache2->linhas[posicaoAuxiliar], &cache3->linhas[posicaoCache3]);
        break;
    case 2:
        posicaoAuxiliar = seekLeastCalled(cache2);
        trocaLinha(&cache2->linhas[posicaoAuxiliar], &cache3->linhas[posicaoCache3]);
        break;
    default:
        posicaoAuxiliar = 0;
        trocaLinha(&cache2->linhas[0], &cache3->linhas[posicaoCache3]);
    }
    return movCache2Cache1(endereco, posicaoCache1, posicaoAuxiliar, posicaoCache3, ram, cache1, cache2, cache3);
}
Bloco *movCache2Cache1(ENDERECO endereco, int posicaoCache1, int posicaoCache2, int posicaoCache3, RAM *ram, CACHE *cache1, CACHE *cache2, CACHE *cache3)
{
    int posicaoAuxiliar;
    switch (codicaoCache(cache1))
    {
    case 1:
        posicaoAuxiliar = CachesVazios(cache1);
        trocaLinha(&cache1->linhas[posicaoAuxiliar], &cache2->linhas[posicaoCache2]);
        break;
    case 2:
        posicaoAuxiliar = seekLeastCalled(cache1);
        trocaLinha(&cache1->linhas[posicaoAuxiliar], &cache2->linhas[posicaoCache2]);
        break;
    default:
        posicaoAuxiliar = 0;
        trocaLinha(&cache1->linhas[0], &cache2->linhas[posicaoCache2]);
    }
    return cache1->linhas[posicaoAuxiliar];
}

int seekLeastCalled(CACHE *cache)
{
    int menor = cache->linhas[0]->bloco_chamada;
    int pos = 0;
    for (int i = 1; i < cache->tamanho; i++)
    {
        // Combina idade e frequência de acesso
        int score = cache->linhas[i]->bloco_chamada * 2 + cache->linhas[i]->custo;
        if (score < menor)
        {
            menor = score;
            pos = i;
        }
    }
    return pos;
}
int codicaoCache(CACHE *cache)
{
    if (cache->linhas[0]->endBloco == -1)
    {
        return 0;
    }
    for (int i = 0; i < cache->tamanho; i++)
    {
        if (cache->linhas[i]->endBloco == -1)
        {
            return 1;
        }
    }
    return 2;
}
int CachesVazios(CACHE *cache)
{
    for (int i = 0; i < cache->tamanho; i++)
    {
        if (cache->linhas[i]->endBloco == -1)
        {
            return i;
        }
    }
    return -1;
}
void trocaLinha(Bloco **blocoUm, Bloco **blocoDois)
{
    Bloco *aux = *blocoUm;
    *blocoUm = *blocoDois;
    *blocoDois = aux;
}