/*
schema.hpp - Definição do esquema de uma tabela

O esquema armazena os nomes das colunas e permite
mapear o nome de uma coluna para seu índice no vetor de valores.
*/
#pragma once

#include <string>
#include <vector>
#include <unordered_map>

// Classe que representa o esquema de uma tabela
class Schema {
public:
    // Número de colunas no esquema
    int qtd_cols;

    // Mapa de nome da coluna para seu índice
    std::unordered_map<std::string, int> nome_para_indice;

    // Construtor padrão (esquema vazio)
    Schema();

    // Construtor que recebe os nomes das colunas
    Schema(const std::vector<std::string>& nomes_colunas);

    // Retorna o índice da coluna pelo nome
    int getIndex(const std::string& col_name) const;
};
