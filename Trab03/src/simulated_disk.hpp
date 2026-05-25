/*
simulated_disk.hpp - Disco simulado para armazenamento de tabelas

Simula um disco em memória usando estruturas de dados internas.
Cada tabela é armazenada com um nome (string) como chave,
permitindo operações de leitura, escrita, verificação e remoção.
*/

#pragma once

#include <string>
#include <unordered_map>
#include "table.hpp"

// Classe que simula um disco para armazenar tabelas por nome
class SimulatedDisk {
public:
    // Grava uma tabela no disco simulado
    void write(const std::string& name, const Table& table);

    // Lê uma tabela do disco simulado
    Table read(const std::string& name) const;

    // Verifica se uma tabela existe no disco simulado
    bool exists(const std::string& name) const;

    // Remove uma tabela do disco simulado
    void remove(const std::string& name);

private:
    // Armazena tabelas por nome
    std::unordered_map<std::string, Table> storage;
};
