/**
 * hash_index.h - Módulo central do índice hash extensível
 *
 * Contém as funções principais de inserção, remoção e busca,
 * além da lógica de split de bucket e duplicação de diretório.
 */

#ifndef HASH_INDEX_H
#define HASH_INDEX_H

#include "directory.h"
#include "bucket.h"
#include <string>
#include <fstream>

/**
 * Resultado de uma operação de inserção, contendo informações
 * para gerar a saída no out.txt.
 */
struct InsertResult {
    bool success;
    int globalDepth;     // PG após a inserção
    int localDepth;      // PL do bucket onde a chave foi inserida
    bool dirDoubled;     // Indica se houve duplicação de diretório
    int dupGlobalDepth;  // PG após duplicação (para linha DUP DIR)
    int dupLocalDepth;   // PL do bucket que causou o split (para linha DUP DIR)

    // Suporte a múltiplas duplicações
    int dupCount;        // Número de duplicações ocorridas
    int dupGlobalDepths[32]; // PGs das duplicações (máximo 32 duplicações)
    int dupLocalDepths[32];  // PLs das duplicações
};

/**
 * Resultado de uma operação de remoção.
 */
struct RemoveResult {
    int removedCount;    // 1 se encontrado e removido, 0 se não encontrado
    int globalDepth;     // PG atual
    int localDepth;      // PL do bucket consultado
};

/**
 * Resultado de uma operação de busca.
 */
struct SearchResult {
    int foundCount;      // 1 se encontrado, 0 se não encontrado
    std::string linhaTexto; // Texto da linha (se encontrado)
};

/**
 * Insere um LinhaNum no índice hash extensível.
 * Pode causar split de bucket e/ou duplicação de diretório.
 *
 * @param dir      Ponteiro para o diretório (em memória)
 * @param linhaNum Valor a inserir
 * @return Resultado da inserção com informações para o out.txt
 */
InsertResult insertEntry(Directory* dir, int linhaNum);

/**
 * Remove um LinhaNum do índice hash extensível.
 * Não implementa merge/shrink de buckets.
 *
 * @param dir      Ponteiro para o diretório (em memória)
 * @param linhaNum Valor a remover
 * @return Resultado da remoção com informações para o out.txt
 */
RemoveResult removeEntry(Directory* dir, int linhaNum);

/**
 * Busca um LinhaNum no índice hash extensível.
 * Se encontrado, recupera o LinhaTexto do arquivo CSV.
 *
 * @param dir      Ponteiro para o diretório (em memória)
 * @param linhaNum Valor a buscar
 * @return Resultado da busca com informações para o out.txt
 */
SearchResult searchEntry(Directory* dir, int linhaNum);

#endif // HASH_INDEX_H
