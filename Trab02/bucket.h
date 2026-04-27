/*
bucket.h - Módulo de gerenciamento de buckets

Cada bucket é armazenado em um arquivo separado e contém:
    - Profundidade local (PL)
    - Número de entradas atuais (count)
    - Até 5 entradas de dados (LinhaNum)

Restrição: apenas 1 bucket em memória por vez.
*/

#pragma once
#include <string>

// Capacidade máxima de entradas por bucket
const int BUCKET_CAPACITY = 5;

// Estrutura de uma entrada de dados no bucket
// Contém apenas o linhaNum (chave de busca e RID)
struct DataEntry {
    int linhaNum;
};

// Estrutura de um bucket do índice hash extensível
struct Bucket {
    int localDepth;                     // Profundidade local (PL)
    int count;                          // Número atual de entradas
    DataEntry entries[BUCKET_CAPACITY]; // Máximo de 5 entradas
};

// Gera o nome do arquivo para um bucket dado seu ID.
std::string getBucketFileName(int bucketId);

// Inicializa um bucket vazio com a profundidade local especificada.
void initBucket(Bucket* bucket, int localDepth);

// Carrega um bucket do disco para a memória.
bool loadBucket(int bucketId, Bucket* bucket);

// Salva um bucket da memória para o disco
bool saveBucket(int bucketId, const Bucket* bucket);

// Cria um novo arquivo de bucket no disco com o bucket fornecido.
bool createBucket(int bucketId, const Bucket* bucket);

// Verifica se uma entrada com o dado LinhaNum existe no bucket.
int findInBucket(const Bucket* bucket, int linhaNum);

// Adiciona uma entrada ao bucket (sem verificar overflow).
bool addToBucket(Bucket* bucket, int linhaNum);

// Remove uma entrada do bucket pelo LinhaNum.
bool removeFromBucket(Bucket* bucket, int linhaNum);
