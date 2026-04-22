/**
 * directory.h - Módulo de gerenciamento do diretório do índice hash
 *
 * O diretório contém:
 *   - Profundidade global (PG)
 *   - Vetor de 2^PG referências (IDs) para os buckets
 *
 * O diretório PODE ser mantido inteiramente em memória (exceção à restrição).
 */

#ifndef DIRECTORY_H
#define DIRECTORY_H

/**
 * Estrutura do diretório do índice hash extensível.
 * Mantido inteiramente em memória durante a execução.
 */
struct Directory {
    int globalDepth;    // Profundidade global (PG)
    int size;           // Número de entradas = 2^PG
    int* bucketIds;     // Vetor de IDs dos buckets (índices no diretório)
    int nextBucketId;   // Próximo ID disponível para criar um novo bucket
};

/**
 * Inicializa o diretório com a profundidade global fornecida.
 * Cria 2^PG entradas, cada uma apontando para um bucket distinto.
 * Cria os arquivos de bucket iniciais no disco.
 *
 * @param dir          Ponteiro para o diretório a inicializar
 * @param globalDepth  Profundidade global inicial (lida do in.txt)
 */
void initDirectory(Directory* dir, int globalDepth);

/**
 * Duplica o diretório (directory doubling).
 * PG é incrementado em 1 e o vetor de entradas é duplicado:
 * dir[i] e dir[i + 2^PG_antigo] apontam para o mesmo bucket.
 *
 * @param dir  Ponteiro para o diretório a duplicar
 */
void doubleDirectory(Directory* dir);

/**
 * Salva o diretório em disco (arquivo directory.dat).
 *
 * @param dir  Ponteiro para o diretório a salvar
 * @return true se salvou com sucesso
 */
bool saveDirectory(const Directory* dir);

/**
 * Carrega o diretório do disco (arquivo directory.dat).
 *
 * @param dir  Ponteiro para o diretório onde carregar os dados
 * @return true se carregou com sucesso, false se o arquivo não existe
 */
bool loadDirectory(Directory* dir);

/**
 * Libera a memória alocada pelo diretório.
 *
 * @param dir  Ponteiro para o diretório a liberar
 */
void freeDirectory(Directory* dir);

/**
 * Calcula o hash (PG bits menos significativos) de um valor.
 *
 * @param linhaNum  Valor inteiro a dispersar
 * @param depth     Número de bits a considerar (PG ou PL)
 * @return Valor hash (os 'depth' bits menos significativos de linhaNum)
 */
int hashFunction(int linhaNum, int depth);

#endif // DIRECTORY_H
