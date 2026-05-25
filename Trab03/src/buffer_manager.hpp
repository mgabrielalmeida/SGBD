/*
buffer_manager.hpp - Gerenciador de buffer com frames de página

Simula o buffer pool de um SGBD com B=5 frames.
Cada frame armazena uma Page e possui um flag dirty
indicando se foi modificado desde o carregamento.

Operações suportadas:
    - Obter referência a um frame
    - Descarregar um frame para uma tabela destino
    - Carregar uma página de uma tabela origem para um frame
    - Limpar um frame
*/

#pragma once

#include <array>
#include "page.hpp"
#include "table.hpp"

// Classe que gerencia o buffer pool com frames de página
class BufferManager {
public:
    // Número total de frames no buffer
    static constexpr int B = 5;

    // Construtor: inicializa todos os frames e flags dirty
    BufferManager();

    // Retorna referência ao frame no índice especificado
    Page& getFrame(int frame_idx);

    // Descarrega o frame para a tabela destino (adiciona como nova página se não estiver vazio)
    // e marca dirty como false
    void flushFrame(int frame_idx, Table& dest);

    // Carrega uma página da tabela origem para o frame especificado
    void loadPage(int frame_idx, const Table& src, int page_idx);

    // Limpa o frame e marca dirty como false
    void clearFrame(int frame_idx);

private:
    // Os 5 frames do buffer
    std::array<Page, B> frames;

    // Indica se o frame está sujo (modificado)
    std::array<bool, B> dirty;
};
