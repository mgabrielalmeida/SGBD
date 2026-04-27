/**
bucket.cpp - Implementação do módulo de gerenciamento de buckets

Gerencia a leitura/escrita de buckets individuais em disco.
Cada bucket é um arquivo binário separado.
Garante que apenas 1 bucket esteja em memória por vez.
*/

#include "bucket.h"
#include <fstream>
#include <iostream>
#include <cstring>

std::string getBucketFileName(int bucketId) {
    return "data/bucket_" + std::to_string(bucketId) + ".dat";
}

void initBucket(Bucket* bucket, int localDepth) {
    bucket->localDepth = localDepth;
    bucket->count = 0;
    // Limpa todas as entradas
    for (int i = 0; i < BUCKET_CAPACITY; i++) {
        bucket->entries[i].linhaNum = -1;
    }
}

bool loadBucket(int bucketId, Bucket* bucket) {
    std::string fileName = getBucketFileName(bucketId);
    std::ifstream file(fileName, std::ios::binary);

    if (!file.is_open()) {
        return false;
    }

    // Lê a profundidade local
    file.read(reinterpret_cast<char*>(&bucket->localDepth), sizeof(int));
    // Lê a contagem de entradas
    file.read(reinterpret_cast<char*>(&bucket->count), sizeof(int));
    // Lê as entradas
    for (int i = 0; i < BUCKET_CAPACITY; i++) {
        file.read(reinterpret_cast<char*>(&bucket->entries[i].linhaNum), sizeof(int));
    }

    file.close();
    return true;
}

bool saveBucket(int bucketId, const Bucket* bucket) {
    std::string fileName = getBucketFileName(bucketId);
    std::ofstream file(fileName, std::ios::binary | std::ios::trunc);

    if (!file.is_open()) {
        std::cerr << "Erro: nao foi possivel salvar o bucket " << bucketId << std::endl;
        return false;
    }

    // Escreve a profundidade local
    file.write(reinterpret_cast<const char*>(&bucket->localDepth), sizeof(int));
    // Escreve a contagem de entradas
    file.write(reinterpret_cast<const char*>(&bucket->count), sizeof(int));
    // Escreve as entradas
    for (int i = 0; i < BUCKET_CAPACITY; i++) {
        file.write(reinterpret_cast<const char*>(&bucket->entries[i].linhaNum), sizeof(int));
    }

    file.close();
    return true;
}

bool createBucket(int bucketId, const Bucket* bucket) {
    return saveBucket(bucketId, bucket);
}

int findInBucket(const Bucket* bucket, int linhaNum) {
    for (int i = 0; i < bucket->count; i++) {
        if (bucket->entries[i].linhaNum == linhaNum) {
            return i;
        }
    }
    return -1;
}

bool addToBucket(Bucket* bucket, int linhaNum) {
    if (bucket->count >= BUCKET_CAPACITY) {
        return false; // Bucket cheio
    }
    bucket->entries[bucket->count].linhaNum = linhaNum;
    bucket->count++;
    return true;
}

bool removeFromBucket(Bucket* bucket, int linhaNum) {
    int idx = findInBucket(bucket, linhaNum);
    if (idx == -1) {
        return false; // Não encontrado
    }

    // Move a última entrada para a posição removida (compactação)
    bucket->count--;
    if (idx < bucket->count) {
        bucket->entries[idx] = bucket->entries[bucket->count];
    }
    // Limpa a última posição
    bucket->entries[bucket->count].linhaNum = -1;

    return true;
}
