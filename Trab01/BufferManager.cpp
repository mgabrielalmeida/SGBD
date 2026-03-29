/**
 * =============================================================================
 * Arquivo : BufferManager.cpp
 * Autora  : Kenia
 * Data    : 29/03/2026
 * Descr.  : Implementação completa da classe BufferManager. Contém a lógica de
 *           busca (Fetch), substituição (Evict) com quatro políticas (LRU, FIFO,
 *           CLOCK, MRU), exibição do cache e estatísticas de desempenho.
 * =============================================================================
 */

#include "BufferManager.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <stdexcept>

// =============================================================================
// Construtor
// =============================================================================

BufferManager::BufferManager(const std::string& caminhoArquivo, ReplacementPolicy politica)
    : policy_(politica)
    , globalClock_(0)
    , insertionSeq_(0)
    , clockHand_(0)
    , cacheHit_(0)
    , cacheMiss_(0)
    , rng_(std::random_device{}())
    , dirtyDist_(0.5)
{
    loadFile(caminhoArquivo);
}

// =============================================================================
// Carregamento do arquivo de dados
// =============================================================================

void BufferManager::loadFile(const std::string& caminhoArquivo) {
    std::ifstream arquivo(caminhoArquivo);
    if (!arquivo.is_open()) {
        throw std::runtime_error("Erro: nao foi possivel abrir o arquivo '" + caminhoArquivo + "'.");
    }

    std::string linha;
    while (std::getline(arquivo, linha)) {
        fileLines_.push_back(linha);
    }

    if (fileLines_.empty()) {
        throw std::runtime_error("Erro: o arquivo '" + caminhoArquivo + "' esta vazio.");
    }

    std::cout << "[INFO] Arquivo carregado com sucesso: " << fileLines_.size()
              << " paginas disponiveis.\n" << std::endl;
}

// =============================================================================
// Busca de página no buffer
// =============================================================================

int BufferManager::findInBuffer(int key) const {
    for (size_t i = 0; i < buffer_.size(); ++i) {
        if (buffer_[i].page_id == key) {
            return static_cast<int>(i);
        }
    }
    return -1; // Página não encontrada no buffer
}

// =============================================================================
// Fetch — Operação principal de busca/carregamento de página
// =============================================================================

std::string BufferManager::Fetch(int key) {
    // Incrementa o relógio lógico global a cada operação Fetch
    ++globalClock_;

    // Validação: verifica se a chave está dentro do intervalo válido (1-based)
    if (key < 1 || key > static_cast<int>(fileLines_.size())) {
        std::cerr << "[ERRO] Pagina " << key << " fora do intervalo valido (1-"
                  << fileLines_.size() << ").\n";
        return "";
    }

    // Passo 1: Procura a página no buffer
    int idx = findInBuffer(key);

    if (idx >= 0) {
        // =====================================================================
        // CACHE HIT — Página encontrada no buffer
        // =====================================================================
        ++cacheHit_;
        std::cout << "[HIT]  Pagina " << key << " encontrada no buffer.\n";

        // Atualiza metadados conforme a política ativa
        switch (policy_) {
            case ReplacementPolicy::LRU:
            case ReplacementPolicy::MRU:
                // Atualiza o timestamp para refletir o acesso mais recente
                buffer_[static_cast<size_t>(idx)].timestamp = globalClock_;
                break;

            case ReplacementPolicy::CLOCK:
                // Seta o bit de referência para TRUE (segunda chance)
                buffer_[static_cast<size_t>(idx)].ref_bit = true;
                break;

            case ReplacementPolicy::FIFO:
                // FIFO não atualiza metadados em caso de hit
                break;
        }

        return buffer_[static_cast<size_t>(idx)].content;
    }

    // =========================================================================
    // CACHE MISS — Página não encontrada no buffer
    // =========================================================================
    ++cacheMiss_;
    std::cout << "[MISS] Pagina " << key << " nao encontrada. Carregando do disco...\n";

    // Passo 2: Se o buffer está cheio, realiza a substituição
    if (buffer_.size() >= BUFFER_CAPACITY) {
        Evict();
    }

    // Passo 3: Cria a nova página com os metadados inicializados
    Page novaPagina;
    novaPagina.page_id    = key;
    novaPagina.content    = fileLines_[static_cast<size_t>(key - 1)]; // Índice 0-based no vetor
    novaPagina.dirty      = dirtyDist_(rng_);                         // Flag dirty aleatória (50%)
    novaPagina.ref_bit    = true;                                     // CLOCK: referenciada ao carregar
    novaPagina.timestamp  = globalClock_;                             // LRU/MRU: timestamp atual
    novaPagina.fifo_order = insertionSeq_++;                          // FIFO: ordem de inserção

    // Passo 4: Insere a página no buffer
    buffer_.push_back(novaPagina);

    return novaPagina.content;
}

// =============================================================================
// Evict — Substituição de página
// =============================================================================

void BufferManager::Evict() {
    // Seleciona a vítima conforme a política ativa
    size_t victimIdx = 0;

    switch (policy_) {
        case ReplacementPolicy::LRU:
            victimIdx = selectVictimLRU();
            break;
        case ReplacementPolicy::FIFO:
            victimIdx = selectVictimFIFO();
            break;
        case ReplacementPolicy::CLOCK:
            victimIdx = selectVictimClock();
            break;
        case ReplacementPolicy::MRU:
            victimIdx = selectVictimMRU();
            break;
    }

    // Exibe informações da página evictada
    const Page& vitima = buffer_[victimIdx];
    std::cout << "[EVICT] page#=" << vitima.page_id
              << " | " << vitima.content
              << " | dirty=" << (vitima.dirty ? "W" : "") << "\n";

    // Remove a página do buffer
    buffer_.erase(buffer_.begin() + static_cast<std::ptrdiff_t>(victimIdx));

    // Ajusta o ponteiro do relógio (CLOCK) se necessário
    if (policy_ == ReplacementPolicy::CLOCK) {
        if (buffer_.empty()) {
            clockHand_ = 0;
        } else if (clockHand_ >= buffer_.size()) {
            clockHand_ = 0; // Volta ao início (circular)
        }
    }
}

// =============================================================================
// Seleção de vítima — LRU (Least Recently Used)
// =============================================================================

size_t BufferManager::selectVictimLRU() const {
    // Encontra a página com o menor timestamp (menos recentemente usada)
    size_t victimIdx = 0;
    size_t minTimestamp = buffer_[0].timestamp;

    for (size_t i = 1; i < buffer_.size(); ++i) {
        if (buffer_[i].timestamp < minTimestamp) {
            minTimestamp = buffer_[i].timestamp;
            victimIdx = i;
        }
    }

    return victimIdx;
}

// =============================================================================
// Seleção de vítima — FIFO (First In, First Out)
// =============================================================================

size_t BufferManager::selectVictimFIFO() const {
    // Encontra a página com o menor fifo_order (primeira a entrar)
    size_t victimIdx = 0;
    size_t minOrder = buffer_[0].fifo_order;

    for (size_t i = 1; i < buffer_.size(); ++i) {
        if (buffer_[i].fifo_order < minOrder) {
            minOrder = buffer_[i].fifo_order;
            victimIdx = i;
        }
    }

    return victimIdx;
}

// =============================================================================
// Seleção de vítima — CLOCK (Second-Chance)
// =============================================================================

size_t BufferManager::selectVictimClock() {
    // Percorre circularmente o buffer até encontrar uma página com ref_bit == FALSE
    while (true) {
        if (!buffer_[clockHand_].ref_bit) {
            // Vítima encontrada: página sem referência recente
            size_t victimIdx = clockHand_;
            // Avança o ponteiro para a próxima posição (após remoção)
            clockHand_ = (clockHand_ + 1) % buffer_.size();
            return victimIdx;
        }

        // Segunda chance: limpa o bit de referência e avança
        buffer_[clockHand_].ref_bit = false;
        clockHand_ = (clockHand_ + 1) % buffer_.size();
    }
}

// =============================================================================
// Seleção de vítima — MRU (Most Recently Used)
// =============================================================================

size_t BufferManager::selectVictimMRU() const {
    // Encontra a página com o maior timestamp (mais recentemente usada)
    size_t victimIdx = 0;
    size_t maxTimestamp = buffer_[0].timestamp;

    for (size_t i = 1; i < buffer_.size(); ++i) {
        if (buffer_[i].timestamp > maxTimestamp) {
            maxTimestamp = buffer_[i].timestamp;
            victimIdx = i;
        }
    }

    return victimIdx;
}

// =============================================================================
// DisplayCache — Exibição do conteúdo do buffer
// =============================================================================

void BufferManager::DisplayCache() const {
    std::cout << "\n";
    std::cout << "+==================================================================+\n";
    std::cout << "|              ESTADO ATUAL DO BUFFER (" << policyName() << ")";
    // Preenchimento para alinhar a borda direita
    size_t padding = 64 - 39 - policyName().size();
    for (size_t i = 0; i < padding; ++i) std::cout << " ";
    std::cout << "|\n";
    std::cout << "+==================================================================+\n";

    if (buffer_.empty()) {
        std::cout << "|  (buffer vazio)                                                |\n";
    }

    for (size_t i = 0; i < buffer_.size(); ++i) {
        const Page& p = buffer_[i];
        std::cout << "|  Chave -> " << std::setw(3) << p.page_id
                  << " | Valor -> " << std::left << std::setw(25) << p.content << std::right
                  << " | Atualizacao -> " << (p.dirty ? "TRUE " : "FALSE");

        // Exibe metadados específicos da política
        switch (policy_) {
            case ReplacementPolicy::LRU:
            case ReplacementPolicy::MRU:
                std::cout << " | ts=" << p.timestamp;
                break;
            case ReplacementPolicy::CLOCK:
                std::cout << " | ref=" << (p.ref_bit ? "1" : "0");
                break;
            case ReplacementPolicy::FIFO:
                std::cout << " | ord=" << p.fifo_order;
                break;
        }

        std::cout << "\n";
    }

    std::cout << "+==================================================================+\n\n";
}

// =============================================================================
// DisplayStats — Exibição de estatísticas
// =============================================================================

void BufferManager::DisplayStats() const {
    size_t totalAcessos = cacheHit_ + cacheMiss_;
    double taxaAcerto = (totalAcessos > 0)
        ? (static_cast<double>(cacheHit_) / static_cast<double>(totalAcessos)) * 100.0
        : 0.0;

    std::cout << "+==========================================+\n";
    std::cout << "|        ESTATISTICAS DO BUFFER            |\n";
    std::cout << "+==========================================+\n";
    std::cout << "|  Politica      : " << std::left << std::setw(23) << policyName() << std::right << "|\n";
    std::cout << "|  Cache Hits    : " << std::left << std::setw(23) << cacheHit_     << std::right << "|\n";
    std::cout << "|  Cache Misses  : " << std::left << std::setw(23) << cacheMiss_    << std::right << "|\n";
    std::cout << "|  Total Acessos : " << std::left << std::setw(23) << totalAcessos  << std::right << "|\n";
    std::cout << "|  Taxa de Acerto: " << std::left << std::setw(20) << std::fixed
              << std::setprecision(2) << taxaAcerto << " %" << std::right << "|\n";
    std::cout << "+==========================================+\n\n";
}

// =============================================================================
// Nome da política (para exibição)
// =============================================================================

std::string BufferManager::policyName() const {
    switch (policy_) {
        case ReplacementPolicy::LRU:   return "LRU";
        case ReplacementPolicy::FIFO:  return "FIFO";
        case ReplacementPolicy::CLOCK: return "CLOCK";
        case ReplacementPolicy::MRU:   return "MRU";
    }
    return "DESCONHECIDA";
}
