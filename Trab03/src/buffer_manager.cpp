/*
buffer_manager.cpp - Implementação do gerenciador de buffer

Gerencia os B=5 frames do buffer pool, permitindo carregar
páginas de tabelas, descarregar frames modificados e limpar
frames para reutilização.
*/

#include "buffer_manager.hpp"

// Definição do membro estático constexpr (necessária para uso por referência em C++14/17)
constexpr int BufferManager::B;

// Construtor: inicializa todos os frames vazios e flags dirty como false
BufferManager::BufferManager() {
    for (int i = 0; i < B; i++) {
        frames[i] = Page();
        dirty[i] = false;
    }
}

// Retorna referência ao frame no índice especificado
Page& BufferManager::getFrame(int frame_idx) {
    return frames[frame_idx];
}

// Descarrega o frame para a tabela destino
// Adiciona como nova página se o frame não estiver vazio e marca dirty como false
void BufferManager::flushFrame(int frame_idx, Table& dest) {
    // Só adiciona à tabela se o frame contiver tuplas
    if (frames[frame_idx].qtd_tuplas_ocup > 0) dest.addPage(frames[frame_idx]);
    
    dirty[frame_idx] = false;
}

// Carrega uma página da tabela origem para o frame especificado
void BufferManager::loadPage(int frame_idx, const Table& src, int page_idx) {
    frames[frame_idx] = src.pages[page_idx];
    dirty[frame_idx] = false;
}

// Limpa o frame e marca dirty como false
void BufferManager::clearFrame(int frame_idx) {
    frames[frame_idx].clear();
    dirty[frame_idx] = false;
}
