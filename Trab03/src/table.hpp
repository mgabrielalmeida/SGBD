/*
table.hpp - Definição de uma tabela

Uma tabela é composta por um esquema (metadados das colunas)
e um conjunto de páginas que armazenam as tuplas. Fornece
operações para adicionar páginas, tuplas e consultar o total.
*/
#pragma once

#include "schema.hpp"
#include "page.hpp"
#include <vector>

// Classe que representa uma tabela com esquema e páginas de dados
class Table {
public:
    // Vetor de páginas que compõem a tabela
    std::vector<Page> pages;

    // Número de páginas na tabela
    int qtd_pags = 0;

    // Esquema (metadados) da tabela
    Schema schema;

    // Adiciona uma página completa e incrementa o contador
    void addPage(const Page& p);

    // Retorna o total de tuplas somando todas as páginas
    int totalTuples() const;

    // Adiciona uma tupla, criando nova página se necessário
    void addTuple(const Tuple& t);
};
