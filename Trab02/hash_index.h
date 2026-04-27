/*
hash_index.h - Módulo central do índice hash extensível

Contém as funções principais de inserção, remoção e busca,
da lógica de split de bucket e duplicação de diretório.
*/

#pragma once

#include "directory.h"
#include "bucket.h"
#include <string>
#include <fstream>

// Resultados de uma operação de inserção
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

// Resultados de uma operação de remoção.
struct RemoveResult {
    int removedCount;    // 1 se encontrado e removido, 0 se não encontrado
    int globalDepth;     // PG atual
    int localDepth;      // PL do bucket consultado
};

// Resultados de uma operação de busca.
struct SearchResult {
    int foundCount;      // 1 se encontrado, 0 se não encontrado
    std::string linhaTexto; // Texto da linha (se encontrado)
};

// Insere um LinhaNum no índice hash extensível.
InsertResult insertEntry(Directory* dir, int linhaNum);

// Remove um LinhaNum do índice hash extensível.
RemoveResult removeEntry(Directory* dir, int linhaNum);

// Busca um LinhaNum no índice hash extensível.
SearchResult searchEntry(Directory* dir, int linhaNum);
