/**
 * bucket.h - Módulo de gerenciamento de buckets
 *
 * Cada bucket é armazenado em um arquivo separado e contém:
 *   - Profundidade local (PL)
 *   - Número de entradas atuais (count)
 *   - Até 5 entradas de dados (LinhaNum)
 *
 * Restrição: apenas 1 bucket em memória por vez.
 */

#ifndef BUCKET_H
#define BUCKET_H

#include <string>

// Capacidade máxima de entradas por bucket
const int BUCKET_CAPACITY = 5;

/**
 * Estrutura de uma entrada de dados no bucket.
 * Contém apenas o LinhaNum (chave de busca e RID).
 */
struct DataEntry {
    int linhaNum;
};

/**
 * Estrutura de um bucket do índice hash extensível.
 */
struct Bucket {
    int localDepth;                    // Profundidade local (PL)
    int count;                         // Número atual de entradas
    DataEntry entries[BUCKET_CAPACITY]; // Máximo de 5 entradas
};

/**
 * Gera o nome do arquivo para um bucket dado seu ID.
 * Formato: bucket_<id>.dat (ex: bucket_0.dat, bucket_1.dat)
 *
 * @param bucketId  Identificador numérico do bucket
 * @return Nome do arquivo do bucket
 */
std::string getBucketFileName(int bucketId);

/**
 * Inicializa um bucket vazio com a profundidade local especificada.
 *
 * @param bucket     Ponteiro para o bucket a inicializar
 * @param localDepth Profundidade local inicial
 */
void initBucket(Bucket* bucket, int localDepth);

/**
 * Carrega um bucket do disco para a memória.
 * ATENÇÃO: O chamador deve garantir que nenhum outro bucket
 * esteja em memória antes de chamar esta função.
 *
 * @param bucketId  ID do bucket a carregar
 * @param bucket    Ponteiro para o bucket onde carregar os dados
 * @return true se carregou com sucesso, false se o arquivo não existe
 */
bool loadBucket(int bucketId, Bucket* bucket);

/**
 * Salva um bucket da memória para o disco.
 *
 * @param bucketId  ID do bucket a salvar
 * @param bucket    Ponteiro para o bucket com os dados
 * @return true se salvou com sucesso
 */
bool saveBucket(int bucketId, const Bucket* bucket);

/**
 * Cria um novo arquivo de bucket no disco com o bucket fornecido.
 *
 * @param bucketId  ID do novo bucket
 * @param bucket    Ponteiro para o bucket com os dados iniciais
 * @return true se criou com sucesso
 */
bool createBucket(int bucketId, const Bucket* bucket);

/**
 * Verifica se uma entrada com o dado LinhaNum existe no bucket.
 *
 * @param bucket   Ponteiro para o bucket carregado em memória
 * @param linhaNum Valor a procurar
 * @return Índice da entrada se encontrada, -1 caso contrário
 */
int findInBucket(const Bucket* bucket, int linhaNum);

/**
 * Adiciona uma entrada ao bucket (sem verificar overflow).
 * O chamador deve verificar se bucket->count < BUCKET_CAPACITY antes.
 *
 * @param bucket   Ponteiro para o bucket em memória
 * @param linhaNum Valor a inserir
 * @return true se inseriu com sucesso
 */
bool addToBucket(Bucket* bucket, int linhaNum);

/**
 * Remove uma entrada do bucket pelo LinhaNum.
 *
 * @param bucket   Ponteiro para o bucket em memória
 * @param linhaNum Valor a remover
 * @return true se encontrou e removeu, false se não encontrou
 */
bool removeFromBucket(Bucket* bucket, int linhaNum);

#endif // BUCKET_H
