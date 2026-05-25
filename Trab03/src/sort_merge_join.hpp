/*
sort_merge_join.hpp - Junção por ordenação-mesclagem (Sort-Merge Join)

Declara a função principal sortMergeJoin que realiza a junção
de duas tabelas utilizando o algoritmo Sort-Merge Join com
ordenação externa em duas fases (geração de runs e merge k-way).
*/

#pragma once

#include "schema.hpp"
#include "tuple.hpp"
#include "page.hpp"
#include "table.hpp"
#include "buffer_manager.hpp"
#include "simulated_disk.hpp"
#include <string>
 
/*
Executa o Sort-Merge Join entre duas tabelas.

Parâmetros:
    grapes          - tabela da esquerda (uvas)
    wines           - tabela da direita (vinhos)
    join_col_grapes - nome da coluna de junção na tabela de uvas
    join_col_wines  - nome da coluna de junção na tabela de vinhos
    disk            - disco simulado para armazenamento intermediário
    buffer          - gerenciador de buffer com B=5 frames

Retorna:
    Tabela resultante contendo a junção das duas tabelas.
*/
Table sortMergeJoin(
    const Table& grapes,
    const Table& wines,
    const std::string& join_col_grapes,
    const std::string& join_col_wines,
    SimulatedDisk& disk,
    BufferManager& buffer
);
