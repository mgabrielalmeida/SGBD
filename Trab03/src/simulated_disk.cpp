/*
simulated_disk.cpp - Implementação do disco simulado

Implementa as operações de leitura, escrita, verificação
e remoção de tabelas em um armazenamento em memória que
simula o comportamento de um disco.
*/

#include "simulated_disk.hpp"
#include <stdexcept>

// Grava uma tabela no disco simulado (insere ou sobrescreve)
void SimulatedDisk::write(const std::string& name, const Table& table) {
    storage[name] = table;
}

// Lê uma tabela do disco simulado
// Lança exceção se a tabela não existir
Table SimulatedDisk::read(const std::string& name) const {
    auto it = storage.find(name);
    if (it == storage.end()) {
        throw std::runtime_error("Tabela não encontrada no disco: " + name);
    }
    return it->second;
}

// Verifica se uma tabela existe no disco simulado
bool SimulatedDisk::exists(const std::string& name) const {
    return storage.find(name) != storage.end();
}

// Remove uma tabela do disco simulado
// Lança exceção se a tabela não existir
void SimulatedDisk::remove(const std::string& name) {
    auto it = storage.find(name);
    if (it == storage.end()) {
        throw std::runtime_error("Tabela não encontrada para remoção: " + name);
    }
    storage.erase(it);
}
