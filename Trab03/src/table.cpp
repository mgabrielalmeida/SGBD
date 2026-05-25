/*
table.cpp - Implementação de uma tabela

Gerencia o armazenamento de tuplas em páginas,
criando novas páginas automaticamente quando a
página atual está cheia.
*/

#include "table.hpp"

// Adiciona uma página completa à tabela
void Table::addPage(const Page& p) {
    pages.push_back(p);
    qtd_pags++;
}

// Retorna o total de tuplas somando os slots ocupados de cada página
int Table::totalTuples() const {
    int total = 0;
    for (const auto& page : pages) total += page.qtd_tuplas_ocup;
    
    return total;
}

// Adiciona uma tupla à tabela, criando nova página se necessário
void Table::addTuple(const Tuple& t) {
    // Se não há páginas ou a última página está cheia, cria uma nova
    if (pages.empty() || pages.back().isFull()) {
        Page nova_pagina;
        pages.push_back(nova_pagina);
        qtd_pags++;
    }

    // Insere a tupla na última página
    pages.back().addTuple(t);
}
