/*
hash_index.cpp - Implementação do módulo central do índice hash extensível

Implementa inserção (com split e duplicação), remoção e busca.
Respeita a restrição de memória: apenas 1 bucket em memória por vez.
*/

#include "hash_index.h"
#include "data_file.h"
#include <iostream>
#include <cstring>

// Tamanho máximo para armazenamento temporário durante redistribuições encadeadas
static const int MAX_TEMP_ENTRIES = 64;

// Estrutura temporária de bucket que permite mais de BUCKET_CAPACITY entradas.
// Usada apenas durante o processo de redistribuição (split).
struct TempBucket {
    int localDepth;
    int count;
    int entries[MAX_TEMP_ENTRIES];
};

// Adiciona uma entrada a um TempBucket (sem limite de capacidade).
static void addToTemp(TempBucket* tb, int val) {
    tb->entries[tb->count++] = val;
}

// Copia um TempBucket para um Bucket real (usado após redistribuição bem-sucedida).
// Assume que tb->count <= BUCKET_CAPACITY.
static void copyTempToBucket(const TempBucket* tb, Bucket* b) {
    b->localDepth = tb->localDepth;
    b->count = tb->count;
    for (int i = 0; i < tb->count; i++) {
        b->entries[i].linhaNum = tb->entries[i];
    }
    for (int i = tb->count; i < BUCKET_CAPACITY; i++) {
        b->entries[i].linhaNum = -1;
    }
}

// Realiza a inserção com tratamento completo de overflow via split iterativo.
static void doInsert(Directory* dir, int linhaNum, InsertResult* result) {
    // Calcula o hash com a profundidade global atual
    int hashKey = hashFunction(linhaNum, dir->globalDepth);
    int bucketId = dir->bucketIds[hashKey];

    // Carrega o bucket destino (única página em memória)
    Bucket bucket;
    loadBucket(bucketId, &bucket);

    // Se o bucket tem espaço, insere diretamente e termina
    if (bucket.count < BUCKET_CAPACITY) {
        addToBucket(&bucket, linhaNum);
        saveBucket(bucketId, &bucket);

        result->success = true;
        result->globalDepth = dir->globalDepth;
        result->localDepth = bucket.localDepth;
        return;
    }

    // --- OVERFLOW: precisa de split ---

    // Coleta todas as entradas do bucket + a nova entrada
    int allEntries[MAX_TEMP_ENTRIES];
    int totalEntries = 0;
    for (int i = 0; i < bucket.count; i++) {
        allEntries[totalEntries++] = bucket.entries[i].linhaNum;
    }
    allEntries[totalEntries++] = linhaNum;

    // Profundidade local atual do bucket que estourou
    int currentLocalDepth = bucket.localDepth;

    // Loop: pode precisar de múltiplos splits consecutivos se todas as entradas
    // forem para o mesmo bucket após a redistribuição
    while (true) {
        // Incrementa profundidade local
        int newLocalDepth = currentLocalDepth + 1;

        // Se PL > PG, duplica o diretório
        if (newLocalDepth > dir->globalDepth) {
            doubleDirectory(dir);

            // Registra a duplicação
            result->dupCount++;
            int dupIdx = result->dupCount - 1;
            result->dupGlobalDepths[dupIdx] = dir->globalDepth;
            result->dupLocalDepths[dupIdx] = newLocalDepth;
            result->dirDoubled = true;
        }

        // Recalcula o bucketId no diretório (pode ter mudado após doubling)
        hashKey = hashFunction(linhaNum, dir->globalDepth);
        bucketId = dir->bucketIds[hashKey];

        // Cria novo bucket irmão
        int newBucketId = dir->nextBucketId++;

        // Atualiza as entradas do diretório:
        // O bit diferenciador é o bit na posição (newLocalDepth - 1).
        int mask = 1 << (newLocalDepth - 1);

        for (int i = 0; i < dir->size; i++) {
            if (dir->bucketIds[i] == bucketId) {
                if (i & mask) {
                    dir->bucketIds[i] = newBucketId;
                }
            }
        }

        // Redistribui usando TempBuckets (sem limite de capacidade)
        TempBucket tbOld, tbNew;
        tbOld.localDepth = newLocalDepth;
        tbOld.count = 0;
        tbNew.localDepth = newLocalDepth;
        tbNew.count = 0;

        for (int i = 0; i < totalEntries; i++) {
            int key = allEntries[i];
            int h = hashFunction(key, dir->globalDepth);
            int targetId = dir->bucketIds[h];

            if (targetId == bucketId) {
                addToTemp(&tbOld, key);
            } else {
                addToTemp(&tbNew, key);
            }
        }

        // Verifica se a redistribuição resolveu o overflow
        if (tbOld.count <= BUCKET_CAPACITY && tbNew.count <= BUCKET_CAPACITY) {
            // Sucesso -> copia para buckets reais e salva em disco
            Bucket bOld, bNew;
            copyTempToBucket(&tbOld, &bOld);
            copyTempToBucket(&tbNew, &bNew);

            saveBucket(bucketId, &bOld);
            saveBucket(newBucketId, &bNew);
            saveDirectory(dir);

            // Descobre em qual bucket o linhaNum ficou e pega o PL
            int finalHash = hashFunction(linhaNum, dir->globalDepth);
            int finalBucketId = dir->bucketIds[finalHash];
            Bucket finalBucket;
            loadBucket(finalBucketId, &finalBucket);

            result->success = true;
            result->globalDepth = dir->globalDepth;
            result->localDepth = finalBucket.localDepth;
            return;
        }

        // Ainda há overflow - todas (ou quase todas) foram para o mesmo bucket.
        // Salva o bucket que NÃO estourou e continua o loop para o que estourou.

        if (tbOld.count > BUCKET_CAPACITY) {
            // O bucket original ainda tem overflow -> salva o novo (pode estar vazio)
            Bucket bNew;
            copyTempToBucket(&tbNew, &bNew);
            saveBucket(newBucketId, &bNew);

            // Prepara para o próximo split com as entradas do bucket original
            totalEntries = tbOld.count;
            for (int i = 0; i < totalEntries; i++) {
                allEntries[i] = tbOld.entries[i];
            }
            currentLocalDepth = newLocalDepth;
            // O bucketId permanece o mesmo - precisa de mais um split
        } 
        else {
            // O novo bucket tem overflow -> salva o original
            Bucket bOld;
            copyTempToBucket(&tbOld, &bOld);
            saveBucket(bucketId, &bOld);

            // Prepara para o próximo split com as entradas do novo bucket
            totalEntries = tbNew.count;
            for (int i = 0; i < totalEntries; i++) {
                allEntries[i] = tbNew.entries[i];
            }
            currentLocalDepth = newLocalDepth;
            // Atualiza bucketId para o novo bucket (que precisa do split)
            bucketId = newBucketId;
        }
    }
}

InsertResult insertEntry(Directory* dir, int linhaNum) {
    InsertResult result;
    result.success = false;
    result.dirDoubled = false;
    result.dupCount = 0;
    memset(result.dupGlobalDepths, 0, sizeof(result.dupGlobalDepths));
    memset(result.dupLocalDepths, 0, sizeof(result.dupLocalDepths));

    doInsert(dir, linhaNum, &result);

    return result;
}

RemoveResult removeEntry(Directory* dir, int linhaNum) {
    RemoveResult result;

    // Calcula o hash
    int hashKey = hashFunction(linhaNum, dir->globalDepth);

    // Obtém o ID do bucket
    int bucketId = dir->bucketIds[hashKey];

    // Carrega o bucket (única página em memória)
    Bucket bucket;
    if (!loadBucket(bucketId, &bucket)) {
        result.removedCount = 0;
        result.globalDepth = dir->globalDepth;
        result.localDepth = 0;
        return result;
    }

    // Tenta remover
    result.globalDepth = dir->globalDepth;
    result.localDepth = bucket.localDepth;

    if (removeFromBucket(&bucket, linhaNum)) {
        saveBucket(bucketId, &bucket);
        result.removedCount = 1;
    } else {
        result.removedCount = 0;
    }

    return result;
}

SearchResult searchEntry(Directory* dir, int linhaNum) {
    SearchResult result;
    result.foundCount = 0;

    // Calcula o hash
    int hashKey = hashFunction(linhaNum, dir->globalDepth);

    // Obtém o ID do bucket
    int bucketId = dir->bucketIds[hashKey];

    // Carrega o bucket (única página em memória)
    Bucket bucket;
    if (!loadBucket(bucketId, &bucket)) {
        return result;
    }

    // Procura no bucket
    int idx = findInBucket(&bucket, linhaNum);

    if (idx >= 0) {
        result.foundCount = 1;
        // Bucket já saiu de escopo - agora carregamos a página do CSV
        getLinhaTexto(linhaNum, result.linhaTexto);
    }

    return result;
}
