/*
directory.h - Módulo de gerenciamento do diretório do índice hash

O diretório contém:
    - Profundidade global (PG)
    - Vetor de 2^PG referências (IDs) para os buckets
*/

#pragma once

// Estrutura do diretório do índice hash extensível.
struct Directory {
    int globalDepth;    // Profundidade global (PG)
    int size;           // Número de entradas = 2^PG
    int* bucketIds;     // Vetor de IDs dos buckets (índices no diretório)
    int nextBucketId;   // Próximo ID disponível para criar um novo bucket
};

/*
Inicializa o diretório com a profundidade global fornecida.
Cria 2^PG entradas, cada uma apontando para um bucket distinto.
Cria os arquivos de bucket iniciais no disco.
*/
void initDirectory(Directory* dir, int globalDepth);

//Duplica o diretório.
void doubleDirectory(Directory* dir);

// Salva o diretório em um .dat.
bool saveDirectory(const Directory* dir);

// Carrega o diretório do .dat.
bool loadDirectory(Directory* dir);

// Libera a memória alocada para o diretório.
void freeDirectory(Directory* dir);

// Calcula o hash (PG bits menos significativos) de um valor.
int hashFunction(int linhaNum, int depth);
