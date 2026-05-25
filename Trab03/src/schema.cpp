/*
schema.cpp - Implementação do esquema de uma tabela

Gerencia os nomes das colunas e o mapeamento
de nome para índice, permitindo acesso rápido
por nome de coluna.
*/

#include "schema.hpp"
#include <stdexcept>

// Construtor padrão: esquema vazio com zero colunas
Schema::Schema() : qtd_cols(0) {}

// Construtor que recebe um vetor com os nomes das colunas
Schema::Schema(const std::vector<std::string>& nomes_colunas)
    : qtd_cols(static_cast<int>(nomes_colunas.size()))
{
    // Preenche o mapa de nome para índice
    for (int i = 0; i < qtd_cols; i++) {
        nome_para_indice[nomes_colunas[i]] = i;
    }
}

// Retorna o índice da coluna pelo nome e lança exceção se não existir
int Schema::getIndex(const std::string& col_name) const {
    auto it = nome_para_indice.find(col_name);
    if (it == nome_para_indice.end()) {
        throw std::runtime_error("Coluna não encontrada: " + col_name);
    }
    return it->second;
}
