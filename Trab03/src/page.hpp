/*
page.hpp - Definição de uma página de dados

Uma página contém até 12 tuplas (MAX_TUPLAS) e simula
uma página de disco em memória, com controle de slots
ocupados e operações de inserção e limpeza.
*/
#pragma once

#include "tuple.hpp"
#include <array>

// Classe que representa uma página de dados
class Page {
public:
    // Capacidade máxima de tuplas por página
    static constexpr int MAX_TUPLAS = 12;

    // Slots de tuplas na página
    std::array<Tuple, MAX_TUPLAS> tuples;

    // Número de slots atualmente ocupados
    int qtd_tuplas_ocup = 0;

    // Verifica se a página está cheia
    bool isFull() const;

    // Adiciona uma tupla no próximo slot vazio
    void addTuple(const Tuple& t);

    // Limpa a página, resetando o contador e as tuplas
    void clear();
};
