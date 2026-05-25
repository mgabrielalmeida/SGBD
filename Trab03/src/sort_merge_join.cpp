/*
sort_merge_join.cpp - Implementação do Sort-Merge Join

Implementa o operador algébrico Sort-Merge Join em duas fases:
    1. Ordenação Externa (External Sort):
        - Passo 0: Geração de runs ordenados (B=5 páginas por vez)
        - Passo 1+: Merge k-way com 4 frames de entrada e 1 de saída,
          utilizando fila de prioridade (min-heap) - O(N log k) por passo
    2. Merge-Join:
        - Junção por mesclagem das duas tabelas ordenadas
        - Tratamento de duplicatas com produto cartesiano completo

Disciplina: CK0095 – Sistemas de Gerenciamento de Banco de Dados (2026.1)
Universidade Federal do Ceará (UFC)
*/

#include "sort_merge_join.hpp"
#include "schema.hpp"
#include "tuple.hpp"
#include "page.hpp"
#include "table.hpp"
#include "buffer_manager.hpp"
#include "simulated_disk.hpp"

#include <queue>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <functional>

// Estrutura auxiliar para o merge k-way via fila de prioridade

/*
Entrada do heap para o merge k-way.
Armazena a chave de comparação, o índice do run de origem e a tupla completa.
*/
struct HeapEntry {
    std::string key;     // Valor da chave de junção (para comparação)
    int run_idx;         // Índice do run de onde a tupla veio (0..3)
    Tuple tuple;         // A tupla completa

    // Comparador para min-heap (priority_queue em C++ é max-heap por padrão,
    // então invertemos a comparação)
    bool operator>(const HeapEntry& other) const {
        return key > other.key;
    }
};

// ============================================================================
// Fase 1: Ordenação Externa (External Sort)
// ============================================================================

/*
Realiza a ordenação externa de uma tabela usando o algoritmo de duas fases.
 
Parâmetros:
    table      - tabela a ser ordenada (NÃO é modificada)
    join_col   - nome da coluna usada como chave de ordenação
    table_name - nome identificador (para nomear runs no disco)
    disk       - disco simulado para armazenamento intermediário
    buffer     - gerenciador de buffer com B=5 frames
 
Retorna:
    Nome da tabela ordenada armazenada no disco simulado
*/
static std::string externalSort(
    const Table& table,
    const std::string& join_col,
    const std::string& table_name,
    SimulatedDisk& disk,
    BufferManager& buffer
) {
    // Obtém o índice da coluna de junção no esquema da tabela
    int col_idx = table.schema.getIndex(join_col);
    int total_pages = table.qtd_pags;

    // Se a tabela está vazia, retorna uma tabela vazia no disco
    if (total_pages == 0) {
        Table empty_table;
        empty_table.schema = table.schema;
        std::string sorted_name = "sorted_" + table_name;
        disk.write(sorted_name, empty_table);
        return sorted_name;
    }

    // ========================================================================
    // Passo 0: Geração de Runs
    // Lê até B=5 páginas por vez, ordena em memória, e grava como run
    // ========================================================================
    std::vector<std::string> run_names;
    int run_count = 0;

    for (int start_page = 0; start_page < total_pages; start_page += BufferManager::B) {
        // Determina quantas páginas ler neste lote (no máximo B=5)
        int pages_to_read = std::min(BufferManager::B, total_pages - start_page);

        // Carrega as páginas nos frames do buffer
        for (int i = 0; i < pages_to_read; i++) {
            buffer.loadPage(i, table, start_page + i);
        }

        // Coleta todas as tuplas dos frames carregados em um vetor temporário
        std::vector<Tuple> all_tuples;
        for (int i = 0; i < pages_to_read; i++) {
            Page& frame = buffer.getFrame(i);
            for (int j = 0; j < frame.qtd_tuplas_ocup; j++) {
                all_tuples.push_back(frame.tuples[j]);
            }
        }

        // Ordena as tuplas pela coluna de junção (comparação lexicográfica)
        std::sort(all_tuples.begin(), all_tuples.end(),
            [col_idx](const Tuple& a, const Tuple& b) {
                return a.get(col_idx) < b.get(col_idx);
            }
        );

        // Cria a tabela do run ordenado e insere as tuplas ordenadas
        Table run_table;
        run_table.schema = table.schema;
        for (const auto& t : all_tuples) {
            run_table.addTuple(t);
        }

        // Grava o run no disco simulado com nome único
        std::string run_name = "run_" + table_name + "_" + std::to_string(run_count);
        disk.write(run_name, run_table);
        run_names.push_back(run_name);
        run_count++;

        // Limpa os frames utilizados
        for (int i = 0; i < pages_to_read; i++) {
            buffer.clearFrame(i);
        }
    }

    std::cout << "  Gerados " << run_count << " runs para tabela '" << table_name << "'\n";

    // Se gerou apenas 1 run, já está ordenado
    if (run_names.size() == 1) {
        std::string sorted_name = "sorted_" + table_name;
        disk.write(sorted_name, disk.read(run_names[0]));
        disk.remove(run_names[0]);
        return sorted_name;
    }

    // ========================================================================
    // Passo 1+: Merge K-Way
    // Usa 4 frames de entrada e 1 frame de saída (frame[4])
    // Repete até restar apenas 1 run
    // ========================================================================
    int pass_number = 1;
    int merge_run_counter = 0;

    while (run_names.size() > 1) {
        std::vector<std::string> new_run_names;

        std::cout << "  Passo " << pass_number << " de merge: " 
                  << run_names.size() << " runs\n";

        // Processa grupos de até 4 runs por vez
        for (size_t group_start = 0; group_start < run_names.size(); group_start += 4) {
            // Número de runs neste grupo (no máximo 4)
            int k = std::min(4, (int)(run_names.size() - group_start));

            // Se sobrou apenas 1 run no grupo, não precisa fazer merge
            if (k == 1) {
                new_run_names.push_back(run_names[group_start]);
                continue;
            }

            // Carrega os runs deste grupo do disco
            std::vector<Table> runs(k);
            std::vector<int> run_page_cursor(k, 0); // Cursor de página para cada run
            for (int i = 0; i < k; i++) {
                runs[i] = disk.read(run_names[group_start + i]);
            }

            // Tabela de saída do merge
            Table merged_table;
            merged_table.schema = table.schema;

            // Limpa o frame de saída (frame 4)
            buffer.clearFrame(4);

            // Inicializa o min-heap: carrega a primeira página de cada run
            // e insere a primeira tupla de cada run no heap
            std::priority_queue<HeapEntry, std::vector<HeapEntry>, std::greater<HeapEntry>> min_heap;

            for (int i = 0; i < k; i++) {
                if (runs[i].qtd_pags > 0) {
                    // Carrega a primeira página do run no frame i
                    buffer.loadPage(i, runs[i], 0);
                    run_page_cursor[i] = 0;

                    // Insere a primeira tupla do frame no heap
                    Page& frame = buffer.getFrame(i);
                    if (frame.qtd_tuplas_ocup > 0) {
                        HeapEntry entry;
                        entry.key = frame.tuples[0].get(col_idx);
                        entry.run_idx = i;
                        entry.tuple = frame.tuples[0];
                        min_heap.push(entry);
                    }
                }
            }

            // Vetor para rastrear o índice da tupla atual dentro de cada frame
            std::vector<int> tuple_cursor(k, 1); // Começa em 1 pois a tupla 0 já foi para o heap

            // Loop principal do merge k-way
            while (!min_heap.empty()) {
                // Extrai a tupla com menor chave
                HeapEntry top = min_heap.top();
                min_heap.pop();

                // Adiciona a tupla ao frame de saída
                Page& output_frame = buffer.getFrame(4);
                output_frame.addTuple(top.tuple);

                // Se o frame de saída está cheio, descarrega para a tabela de resultado
                if (output_frame.isFull()) {
                    buffer.flushFrame(4, merged_table);
                    buffer.clearFrame(4);
                }

                // Avança o cursor do run de onde veio a tupla
                int ri = top.run_idx;
                Page& input_frame = buffer.getFrame(ri);

                // Verifica se há mais tuplas no frame atual
                if (tuple_cursor[ri] < input_frame.qtd_tuplas_ocup) {
                    // Insere a próxima tupla do mesmo run no heap
                    HeapEntry next_entry;
                    next_entry.key = input_frame.tuples[tuple_cursor[ri]].get(col_idx);
                    next_entry.run_idx = ri;
                    next_entry.tuple = input_frame.tuples[tuple_cursor[ri]];
                    min_heap.push(next_entry);
                    tuple_cursor[ri]++;
                } else {
                    // Frame exaurido: tenta carregar a próxima página do run
                    run_page_cursor[ri]++;
                    if (run_page_cursor[ri] < runs[ri].qtd_pags) {
                        buffer.loadPage(ri, runs[ri], run_page_cursor[ri]);
                        tuple_cursor[ri] = 1; // Reseta cursor de tupla (0 vai para o heap)

                        Page& new_frame = buffer.getFrame(ri);
                        if (new_frame.qtd_tuplas_ocup > 0) {
                            HeapEntry next_entry;
                            next_entry.key = new_frame.tuples[0].get(col_idx);
                            next_entry.run_idx = ri;
                            next_entry.tuple = new_frame.tuples[0];
                            min_heap.push(next_entry);
                        }
                    }
                    // Se não há mais páginas, o run está completamente consumido
                }
            }

            // Descarrega o frame de saída com as tuplas restantes
            if (buffer.getFrame(4).qtd_tuplas_ocup > 0) {
                buffer.flushFrame(4, merged_table);
                buffer.clearFrame(4);
            }

            // Salva o run mesclado no disco
            std::string merged_name = "merge_" + table_name + "_" + std::to_string(merge_run_counter);
            disk.write(merged_name, merged_table);
            new_run_names.push_back(merged_name);
            merge_run_counter++;

            // Remove os runs de entrada que foram consumidos
            for (int i = 0; i < k; i++) {
                disk.remove(run_names[group_start + i]);
            }

            // Limpa todos os frames utilizados
            for (int i = 0; i < k; i++) {
                buffer.clearFrame(i);
            }
            buffer.clearFrame(4);
        }

        run_names = new_run_names;
        pass_number++;
    }

    // Renomeia o run final para o nome de saída
    std::string sorted_name = "sorted_" + table_name;
    disk.write(sorted_name, disk.read(run_names[0]));
    disk.remove(run_names[0]);

    return sorted_name;
}

// ============================================================================
// Funções auxiliares para o Merge-Join
// ============================================================================

/*
Coleta todas as tuplas consecutivas com a mesma chave de junção
a partir da posição atual na tabela, lidando com fronteiras de página.

Parâmetros:
    sorted_table  - tabela ordenada de onde coletar
    col_idx       - índice da coluna de junção
    current_key   - valor da chave atual
    page_idx      - cursor de página (atualizado pela função)
    tuple_idx     - cursor de tupla dentro da página (atualizado)

Retorna:
    Vetor com todas as tuplas que compartilham a mesma chave
*/
static std::vector<Tuple> collectGroup(
    const Table& sorted_table,
    int col_idx,
    const std::string& current_key,
    int& page_idx,
    int& tuple_idx
) {
    std::vector<Tuple> group;

    // Percorre as tuplas consecutivas com a mesma chave
    while (page_idx < sorted_table.qtd_pags) {
        const Page& page = sorted_table.pages[page_idx];

        while (tuple_idx < page.qtd_tuplas_ocup) {
            const Tuple& t = page.tuples[tuple_idx];

            // Se a chave é diferente, o grupo acabou
            if (t.get(col_idx) != current_key) {
                return group;
            }

            group.push_back(t);
            tuple_idx++;
        }

        // Avança para a próxima página
        page_idx++;
        tuple_idx = 0;
    }

    return group;
}

// ============================================================================
// Fase 2: Merge-Join
// ============================================================================

/*
Realiza o merge-join entre duas tabelas ordenadas.
 
Parâmetros:
    sorted_left_name  - nome da tabela esquerda ordenada no disco
    sorted_right_name - nome da tabela direita ordenada no disco
    join_col_left     - nome da coluna de junção na tabela esquerda
    join_col_right    - nome da coluna de junção na tabela direita
    disk              - disco simulado
    buffer            - gerenciador de buffer

Retorna:
    Tabela resultante com a junção (esquema = concatenação dos esquemas)
*/
static Table mergeJoin(
    const std::string& sorted_left_name,
    const std::string& sorted_right_name,
    const std::string& join_col_left,
    const std::string& join_col_right,
    SimulatedDisk& disk,
    BufferManager& buffer
) {
    // Lê as tabelas ordenadas do disco
    Table left = disk.read(sorted_left_name);
    Table right = disk.read(sorted_right_name);

    // Obtém os índices das colunas de junção
    int left_col_idx = left.schema.getIndex(join_col_left);
    int right_col_idx = right.schema.getIndex(join_col_right);

    // Cria o esquema do resultado: concatenação dos dois esquemas
    std::vector<std::string> result_col_names;
    // Adiciona colunas do esquema esquerdo (na ordem dos índices)
    result_col_names.resize(left.schema.qtd_cols + right.schema.qtd_cols);
    for (const auto& pair : left.schema.nome_para_indice) {
        result_col_names[pair.second] = pair.first;
    }
    for (const auto& pair : right.schema.nome_para_indice) {
        result_col_names[left.schema.qtd_cols + pair.second] = pair.first;
    }

    Schema result_schema(result_col_names);
    Table result;
    result.schema = result_schema;

    // Cursores para percorrer as tabelas
    int left_page = 0, left_tuple = 0;   // Cursor da tabela esquerda
    int right_page = 0, right_tuple = 0; // Cursor da tabela direita

    // Limpa o frame de saída (frame 4)
    buffer.clearFrame(4);

    // Loop principal do merge-join
    while (left_page < left.qtd_pags && right_page < right.qtd_pags) {
        // Obtém as tuplas atuais de cada tabela
        const Tuple& left_t = left.pages[left_page].tuples[left_tuple];
        const Tuple& right_t = right.pages[right_page].tuples[right_tuple];

        // Obtém as chaves de junção
        const std::string& left_key = left_t.get(left_col_idx);
        const std::string& right_key = right_t.get(right_col_idx);

        if (left_key < right_key) {
            // Chave esquerda é menor: avança o cursor esquerdo
            left_tuple++;
            if (left_tuple >= left.pages[left_page].qtd_tuplas_ocup) {
                left_page++;
                left_tuple = 0;
            }
        } else if (left_key > right_key) {
            // Chave direita é menor: avança o cursor direito
            right_tuple++;
            if (right_tuple >= right.pages[right_page].qtd_tuplas_ocup) {
                right_page++;
                right_tuple = 0;
            }
        } else {
            // Chaves são iguais: coleta os grupos e emite o produto cartesiano
            std::string match_key = left_key;

            // Coleta todas as tuplas com a mesma chave na tabela esquerda
            std::vector<Tuple> left_group = collectGroup(
                left, left_col_idx, match_key, left_page, left_tuple
            );

            // Coleta todas as tuplas com a mesma chave na tabela direita
            std::vector<Tuple> right_group = collectGroup(
                right, right_col_idx, match_key, right_page, right_tuple
            );

            // Emite o produto cartesiano: para cada par (l, r) nos grupos
            for (const auto& l : left_group) {
                for (const auto& r : right_group) {
                    // Concatena as tuplas
                    Tuple merged = l.merge(r);

                    // Adiciona ao frame de saída
                    Page& output = buffer.getFrame(4);
                    output.addTuple(merged);

                    // Se o frame de saída está cheio, descarrega para o resultado
                    if (output.isFull()) {
                        buffer.flushFrame(4, result);
                        buffer.clearFrame(4);
                    }
                }
            }
        }
    }

    // Descarrega as tuplas restantes do frame de saída
    if (buffer.getFrame(4).qtd_tuplas_ocup > 0) {
        buffer.flushFrame(4, result);
        buffer.clearFrame(4);
    }

    return result;
}

// ============================================================================
// Função principal: sortMergeJoin
// ============================================================================

/*
Implementação completa do Sort-Merge Join.

1. Ordena externamente a tabela de uvas
2. Ordena externamente a tabela de vinhos
3. Realiza o merge-join entre as tabelas ordenadas
*/
Table sortMergeJoin(
    const Table& grapes,
    const Table& wines,
    const std::string& join_col_grapes,
    const std::string& join_col_wines,
    SimulatedDisk& disk,
    BufferManager& buffer
) {
    std::cout << "\n--- Fase 1: Ordenacao Externa ---\n";

    // Ordena a tabela de uvas
    std::cout << "Ordenando tabela Grapes...\n";
    std::string sorted_grapes = externalSort(grapes, join_col_grapes, "grapes", disk, buffer);

    // Ordena a tabela de vinhos
    std::cout << "Ordenando tabela Wines...\n";
    std::string sorted_wines = externalSort(wines, join_col_wines, "wines", disk, buffer);

    std::cout << "\n--- Fase 2: Merge-Join ---\n";

    // Realiza o merge-join
    Table result = mergeJoin(
        sorted_grapes, sorted_wines,
        join_col_grapes, join_col_wines,
        disk, buffer
    );

    std::cout << "Merge-Join concluido.\n";

    return result;
}
