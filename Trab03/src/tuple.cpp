/*
tuple.cpp - Implementação de uma tupla (registro)

Fornece acesso aos valores das colunas por índice,
merge de duas tuplas e verificação de tupla vazia.
*/

#include "tuple.hpp"

// Retorna o valor na posição idx
const std::string& Tuple::get(int idx) const {
    return cols[idx];
}

// Concatena esta tupla com outra, criando uma nova tupla com as colunas de ambas
Tuple Tuple::merge(const Tuple& other) const {
    Tuple resultado;

    // Copia as colunas desta tupla
    resultado.cols.reserve(cols.size() + other.cols.size());
    resultado.cols.insert(resultado.cols.end(), cols.begin(), cols.end());

    // Anexa as colunas da outra tupla
    resultado.cols.insert(resultado.cols.end(), other.cols.begin(), other.cols.end());

    return resultado;
}

// Verifica se a tupla está vazia (sem colunas) 
bool Tuple::empty() const {
    return cols.empty();
}
