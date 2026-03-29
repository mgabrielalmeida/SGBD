/*
Declaração da classe BufferManager, responsável por gerenciar um
buffer pool de tamanho fixo com suporte a quatro políticas de
substituição de página: LRU, FIFO, CLOCK e MRU.
*/

#pragma once

#include "Page.h"
#include <vector>
#include <string>
#include <random>

// Capacidade máxima do buffer pool (número de páginas simultâneas)
constexpr size_t BUFFER_CAPACITY = 5;

// Enumeração das políticas de substituição de página disponíveis.
enum class ReplacementPolicy {
    LRU,    // Least Recently Used
    FIFO,   // First In, First Out
    CLOCK,  // Clock / Second-Chance
    MRU     // Most Recently Used
};

// Classe que implementa o Gerenciador de Buffer.
class BufferManager {
public:
    // Construtor.
    BufferManager(const std::string& caminhoArquivo, ReplacementPolicy politica);

    // Busca uma página pelo seu identificador (chave).
    // Se a página estiver no buffer (HIT), retorna seu conteúdo e atualiza metadados.
    // Se não estiver (MISS), carrega do arquivo, possivelmente evictando outra página.
    std::string Fetch(int key);

    // Exibe todas as páginas atualmente no buffer, com seus metadados.
    void DisplayCache() const;

    // Exibe estatísticas de desempenho do buffer (hits, misses, taxa de acerto).
    void DisplayStats() const;

private:
    // --- Dados do buffer ---
    std::vector<Page> buffer_;            // Pool de páginas em memória
    std::vector<std::string> fileLines_;  // Todas as linhas do arquivo (pré-carregadas)
    ReplacementPolicy policy_;            // Política de substituição ativa

    // --- Contadores e relógios ---
    size_t globalClock_;   // Relógio lógico global, incrementado a cada Fetch
    size_t insertionSeq_;  // Contador monotônico de inserção (para FIFO)
    size_t clockHand_;     // Ponteiro do relógio (hand) para a política CLOCK

    // --- Estatísticas ---
    size_t cacheHit_;      // Contador de acertos (hits)
    size_t cacheMiss_;     // Contador de faltas (misses)

    // --- Geração de valores aleatórios ---
    std::mt19937 rng_;                           // Motor de números pseudoaleatórios
    std::bernoulli_distribution dirtyDist_;      // Distribuição para flag dirty (50%)

    // --- Métodos auxiliares ---

    // Carrega todas as linhas do arquivo para o vetor fileLines_.
    void loadFile(const std::string& caminhoArquivo);

    // Procura uma página no buffer pelo page_id.
    // Retorna Índice no vetor buffer_, ou -1 se não encontrada.
    int findInBuffer(int key) const;

    // Seleciona e remove a página vítima do buffer conforme a política ativa.
    void Evict();

    // --- Seleção de vítima por política ---
    size_t selectVictimLRU() const;
    size_t selectVictimFIFO() const;
    size_t selectVictimClock();       // Modifica clockHand_, portanto não é const
    size_t selectVictimMRU() const;

    // Retorna o nome da política como string (para exibição).
    std::string policyName() const;
};

