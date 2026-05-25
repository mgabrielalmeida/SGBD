/*
page.cpp - Implementação de uma página de dados

Gerencia os slots de tuplas dentro de uma página,
permitindo inserção, verificação de capacidade e limpeza.
*/

#include "page.hpp"

// Retorna true se todos os slots estão ocupados
bool Page::isFull() const {
    return qtd_tuplas_ocup == MAX_TUPLAS;
}

// Adiciona uma tupla no próximo slot disponível
void Page::addTuple(const Tuple& t) {
    if (!isFull()) {
        tuples[qtd_tuplas_ocup] = t;
        qtd_tuplas_ocup++;
    }
}

// Limpa a página: reseta o contador e esvazia cada slot
void Page::clear() {
    for (int i = 0; i < qtd_tuplas_ocup; i++) tuples[i].cols.clear();
    
    qtd_tuplas_ocup = 0;
}
