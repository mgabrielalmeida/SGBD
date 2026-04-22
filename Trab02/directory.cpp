/**
 * directory.cpp - Implementação do módulo de gerenciamento do diretório
 *
 * Gerencia o diretório do índice hash extensível em memória e em disco.
 * O diretório pode ficar inteiramente em memória (exceção permitida).
 */

#include "directory.h"
#include "bucket.h"
#include <fstream>
#include <iostream>
#include <cstring>

// Nome do arquivo de diretório em disco
static const char* DIRECTORY_FILE_NAME = "data/directory.dat";

int hashFunction(int linhaNum, int depth) {
    if (depth == 0) return 0;
    // Retorna os 'depth' bits menos significativos
    return linhaNum & ((1 << depth) - 1);
}

void initDirectory(Directory* dir, int globalDepth) {
    dir->globalDepth = globalDepth;
    dir->size = 1 << globalDepth;  // 2^PG
    dir->bucketIds = new int[dir->size];
    dir->nextBucketId = dir->size; // Próximo ID livre

    // Cria um bucket distinto para cada entrada do diretório
    for (int i = 0; i < dir->size; i++) {
        dir->bucketIds[i] = i;

        // Cria o arquivo de bucket no disco
        Bucket bucket;
        initBucket(&bucket, globalDepth);
        createBucket(i, &bucket);
    }

    // Salva o diretório em disco
    saveDirectory(dir);
}

void doubleDirectory(Directory* dir) {
    int oldSize = dir->size;
    dir->globalDepth++;
    dir->size = 1 << dir->globalDepth; // Novo tamanho = 2^(PG+1)

    // Aloca novo vetor de IDs
    int* newBucketIds = new int[dir->size];

    // Copia: dir[i] e dir[i + oldSize] apontam para o mesmo bucket
    for (int i = 0; i < oldSize; i++) {
        newBucketIds[i] = dir->bucketIds[i];
        newBucketIds[i + oldSize] = dir->bucketIds[i];
    }

    // Libera o vetor antigo e atualiza
    delete[] dir->bucketIds;
    dir->bucketIds = newBucketIds;
}

bool saveDirectory(const Directory* dir) {
    std::ofstream file(DIRECTORY_FILE_NAME, std::ios::binary | std::ios::trunc);

    if (!file.is_open()) {
        std::cerr << "Erro: nao foi possivel salvar o diretorio" << std::endl;
        return false;
    }

    // Escreve profundidade global
    file.write(reinterpret_cast<const char*>(&dir->globalDepth), sizeof(int));
    // Escreve tamanho
    file.write(reinterpret_cast<const char*>(&dir->size), sizeof(int));
    // Escreve próximo ID de bucket
    file.write(reinterpret_cast<const char*>(&dir->nextBucketId), sizeof(int));
    // Escreve vetor de IDs dos buckets
    file.write(reinterpret_cast<const char*>(dir->bucketIds), dir->size * sizeof(int));

    file.close();
    return true;
}

bool loadDirectory(Directory* dir) {
    std::ifstream file(DIRECTORY_FILE_NAME, std::ios::binary);

    if (!file.is_open()) {
        return false;
    }

    // Lê profundidade global
    file.read(reinterpret_cast<char*>(&dir->globalDepth), sizeof(int));
    // Lê tamanho
    file.read(reinterpret_cast<char*>(&dir->size), sizeof(int));
    // Lê próximo ID de bucket
    file.read(reinterpret_cast<char*>(&dir->nextBucketId), sizeof(int));

    // Aloca e lê vetor de IDs dos buckets
    dir->bucketIds = new int[dir->size];
    file.read(reinterpret_cast<char*>(dir->bucketIds), dir->size * sizeof(int));

    file.close();
    return true;
}

void freeDirectory(Directory* dir) {
    if (dir->bucketIds != nullptr) {
        delete[] dir->bucketIds;
        dir->bucketIds = nullptr;
    }
    dir->size = 0;
    dir->globalDepth = 0;
    dir->nextBucketId = 0;
}
