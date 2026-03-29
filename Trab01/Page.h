/*
Definição da struct Page, representando uma página no buffer pool.
Cada página armazena seu identificador, conteúdo lido do arquivo,
flag de atualização (dirty), bit de referência (CLOCK), timestamp
lógico (LRU/MRU) e ordem de inserção (FIFO).
*/

#pragma once

#include <string>
#include <cstddef>

/*
Struct que representa uma página carregada no buffer pool.
Contém todos os metadados necessários para as quatro políticas
de substituição (LRU, FIFO, CLOCK, MRU).
*/
struct Page {
    int       page_id;     // Identificador único da página (número da linha no arquivo)
    std::string content;   // Conteúdo textual da página (linha lida do arquivo)
    bool      dirty;       // Flag de atualização: TRUE se a página foi "modificada"
    bool      ref_bit;     // Bit de referência utilizado exclusivamente pela política CLOCK
    size_t    timestamp;   // Relógio lógico — utilizado pelas políticas LRU e MRU
    size_t    fifo_order;  // Contador de ordem de inserção — utilizado pela política FIFO
};

