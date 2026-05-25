/*
tuple.hpp - Definição de uma tupla (registro)

Uma tupla armazena os valores de uma linha da tabela
como um vetor de strings, permitindo acesso por índice,
merge com outra tupla e verificação de vazio.
*/

#pragma once

#include <string>
#include <vector>

// Classe que representa uma tupla (linha) de uma tabela
class Tuple {
public:
    // Valores das colunas desta tupla
    std::vector<std::string> cols;

    // Retorna o valor na posição idx (por referência constante)
    const std::string& get(int idx) const;

    // Concatena esta tupla com outra, retornando uma nova tupla combinada
    Tuple merge(const Tuple& other) const;

    // Verifica se a tupla está vazia (sem colunas) 
    bool empty() const;
};
