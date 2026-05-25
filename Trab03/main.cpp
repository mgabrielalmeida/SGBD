/*
main.cpp - Programa principal do Sort-Merge Join

Lê os arquivos CSV (grapes.csv e wines.csv), carrega-os em tabelas,
e executa o operador Sort-Merge Join para combinar as duas tabelas
pela chave de junção (chave_primaria = chave_estrangeira).
*/

#include "src/schema.hpp"
#include "src/tuple.hpp"
#include "src/page.hpp"
#include "src/table.hpp"
#include "src/buffer_manager.hpp"
#include "src/simulated_disk.hpp"
#include "src/sort_merge_join.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>

// ============================================================================
// Funções auxiliares para leitura de CSV
// ============================================================================

/*
Faz o parsing de uma linha CSV respeitando campos entre aspas duplas.
Campos que contêm vírgulas são delimitados por aspas ("campo, com vírgula").
*/
std::vector<std::string> parseCSVLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool in_quotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];

        if (c == '"') {
            // Alterna o estado de "dentro de aspas"
            in_quotes = !in_quotes;
        } else if (c == ',' && !in_quotes) {
            // Separador de campo encontrado fora de aspas
            fields.push_back(field);
            field.clear();
        } else if (c == '\r') {
            // Ignora retorno de carro (Windows line endings)
            continue;
        } else {
            field += c;
        }
    }
    // Adiciona o último campo
    fields.push_back(field);
    return fields;
}

/*
Carrega um arquivo CSV em uma Table.
A primeira linha do CSV é tratada como cabeçalho (nomes das colunas).
As linhas subsequentes são inseridas como tuplas nas páginas da tabela.
*/
Table loadCSV(const std::string& filename, int expected_cols) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro: não foi possível abrir o arquivo " << filename << std::endl;
        exit(1);
    }

    // Lê a linha de cabeçalho para criar o esquema
    std::string header_line;
    std::getline(file, header_line);
    std::vector<std::string> col_names = parseCSVLine(header_line);

    // Verifica se o número de colunas corresponde ao esperado
    if ((int)col_names.size() != expected_cols) {
        std::cerr << "Erro: esperava " << expected_cols << " colunas, mas encontrou "
                  << col_names.size() << " em " << filename << std::endl;
        exit(1);
    }

    // Cria o esquema e a tabela
    Schema schema(col_names);
    Table table;
    table.schema = schema;

    // Lê cada linha e cria uma tupla
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line == "\r") continue;

        std::vector<std::string> fields = parseCSVLine(line);

        // Garante que o número de campos corresponde ao esquema
        while ((int)fields.size() < expected_cols) {
            fields.push_back("");
        }
        if ((int)fields.size() > expected_cols) {
            fields.resize(expected_cols);
        }

        Tuple t;
        t.cols = fields;
        table.addTuple(t);
    }

    file.close();
    return table;
}

// ============================================================================
// Funções de exibição de resultados
// ============================================================================

/*
Imprime as primeiras N tuplas de uma tabela, formatadas de forma legível.
*/
void printTable(const Table& table, int max_rows = -1) {
    // Imprime cabeçalho
    std::cout << "\n";
    for (int i = 0; i < table.schema.qtd_cols; ++i) {
        for (const auto& pair : table.schema.nome_para_indice) {
            if (pair.second == i) {
                std::cout << pair.first;
                break;
            }
        }
        if (i < table.schema.qtd_cols - 1) std::cout << " | ";
    }
    std::cout << "\n";

    // Linha separadora
    std::cout << std::string(80, '-') << "\n";

    // Imprime tuplas
    int count = 0;
    for (const auto& page : table.pages) {
        for (int i = 0; i < page.qtd_tuplas_ocup; ++i) {
            for (int j = 0; j < (int)page.tuples[i].cols.size(); ++j) {
                std::cout << page.tuples[i].cols[j];
                if (j < (int)page.tuples[i].cols.size() - 1) std::cout << " | ";
            }
            std::cout << "\n";
            count++;
            if (max_rows > 0 && count >= max_rows) {
                std::cout << "... (mostrando " << max_rows << " de "
                          << table.totalTuples() << " tuplas)\n";
                return;
            }
        }
    }
}

// ============================================================================
// Testes
// ============================================================================

/*
TC1: Tabelas que requerem exatamente um passo de merge (runs <= 4).
Tabela pequena com poucas páginas.
*/
void testTC1(SimulatedDisk& disk, BufferManager& buffer) {
    std::cout << "\n========== TC1: Um passo de merge (runs <= 4) ==========\n";

    // Cria tabela esquerda com 3 páginas (36 tuplas) -> 1 run (cabe em 5 frames)
    Schema schemaL({"id", "nome"});
    Table left;
    left.schema = schemaL;
    for (int i = 36; i >= 1; --i) {
        Tuple t;
        char buf[16];
        snprintf(buf, sizeof(buf), "%02d", i);
        t.cols = {std::string(buf), "nome_" + std::to_string(i)};
        left.addTuple(t);
    }

    // Cria tabela direita com 2 páginas (24 tuplas)
    Schema schemaR({"fk", "valor"});
    Table right;
    right.schema = schemaR;
    for (int i = 1; i <= 24; ++i) {
        Tuple t;
        char buf[16];
        snprintf(buf, sizeof(buf), "%02d", i * 2);
        // Somente metade das chaves combinam (pares de 02 a 48)
        t.cols = {std::string(buf), "val_" + std::to_string(i)};
        right.addTuple(t);
    }

    std::cout << "Tabela esquerda: " << left.totalTuples() << " tuplas, "
              << left.qtd_pags << " paginas\n";
    std::cout << "Tabela direita: " << right.totalTuples() << " tuplas, "
              << right.qtd_pags << " paginas\n";

    Table result = sortMergeJoin(left, right, "id", "fk", disk, buffer);
    std::cout << "Resultado: " << result.totalTuples() << " tuplas\n";

    // Chaves pares de 02 a 36 que existem em ambas:
    // Esquerda: 01..36, Direita: 02,04,06,...,48
    // Interseção: 02,04,06,...,36 -> 18 matches
    assert(result.totalTuples() == 18);
    std::cout << "TC1 PASSOU!\n";
}

/*
TC2: Tabelas que requerem dois ou mais passos de merge (runs > 4).
Tabela grande que gera mais de 4 runs.
*/
void testTC2(SimulatedDisk& disk, BufferManager& buffer) {
    std::cout << "\n========== TC2: Múltiplos passos de merge (runs > 4) ==========\n";

    // Cria tabela esquerda com 25 páginas (300 tuplas) -> 5 runs de 5 páginas cada
    Schema schemaL({"id", "info"});
    Table left;
    left.schema = schemaL;
    for (int i = 300; i >= 1; --i) {
        Tuple t;
        // Usa formato com padding para ordenação lexicográfica correta
        char buf[16];
        snprintf(buf, sizeof(buf), "%04d", i);
        t.cols = {std::string(buf), "info_" + std::to_string(i)};
        left.addTuple(t);
    }

    // Cria tabela direita com 10 páginas (120 tuplas)
    Schema schemaR({"fk", "dado"});
    Table right;
    right.schema = schemaR;
    for (int i = 1; i <= 120; ++i) {
        Tuple t;
        char buf[16];
        snprintf(buf, sizeof(buf), "%04d", i * 3);
        t.cols = {std::string(buf), "dado_" + std::to_string(i)};
        right.addTuple(t);
    }

    std::cout << "Tabela esquerda: " << left.totalTuples() << " tuplas, "
              << left.qtd_pags << " paginas\n";
    std::cout << "Tabela direita: " << right.totalTuples() << " tuplas, "
              << right.qtd_pags << " paginas\n";

    Table result = sortMergeJoin(left, right, "id", "fk", disk, buffer);
    std::cout << "Resultado: " << result.totalTuples() << " tuplas\n";

    // Chaves são múltiplos de 3 de 3 a 300 (100 valores) -> 100 matches
    assert(result.totalTuples() == 100);
    std::cout << "TC2 PASSOU!\n";
}

/*
TC3: Chaves de junção com muitas duplicatas para verificar produto cartesiano.
*/
void testTC3(SimulatedDisk& disk, BufferManager& buffer) {
    std::cout << "\n========== TC3: Duplicatas (produto cartesiano) ==========\n";

    Schema schemaL({"id", "nome"});
    Table left;
    left.schema = schemaL;
    // 5 tuplas com chave "AAA", 3 com chave "BBB"
    for (int i = 0; i < 5; ++i) {
        Tuple t;
        t.cols = {"AAA", "left_a_" + std::to_string(i)};
        left.addTuple(t);
    }
    for (int i = 0; i < 3; ++i) {
        Tuple t;
        t.cols = {"BBB", "left_b_" + std::to_string(i)};
        left.addTuple(t);
    }

    Schema schemaR({"fk", "valor"});
    Table right;
    right.schema = schemaR;
    // 4 tuplas com chave "AAA", 2 com chave "BBB"
    for (int i = 0; i < 4; ++i) {
        Tuple t;
        t.cols = {"AAA", "right_a_" + std::to_string(i)};
        right.addTuple(t);
    }
    for (int i = 0; i < 2; ++i) {
        Tuple t;
        t.cols = {"BBB", "right_b_" + std::to_string(i)};
        right.addTuple(t);
    }

    std::cout << "Tabela esquerda: " << left.totalTuples() << " tuplas\n";
    std::cout << "Tabela direita: " << right.totalTuples() << " tuplas\n";

    Table result = sortMergeJoin(left, right, "id", "fk", disk, buffer);
    std::cout << "Resultado: " << result.totalTuples() << " tuplas\n";

    // AAA: 5 * 4 = 20, BBB: 3 * 2 = 6 -> total = 26
    assert(result.totalTuples() == 26);
    std::cout << "TC3 PASSOU!\n";
}

/*
TC4: Uma tabela com uma única página; a outra com muitas páginas.
*/
void testTC4(SimulatedDisk& disk, BufferManager& buffer) {
    std::cout << "\n========== TC4: Uma tabela pequena, outra grande ==========\n";

    // Tabela esquerda: 1 página (5 tuplas)
    Schema schemaL({"id", "descricao"});
    Table left;
    left.schema = schemaL;
    for (int i = 1; i <= 5; ++i) {
        Tuple t;
        char buf[16];
        snprintf(buf, sizeof(buf), "%04d", i * 10);
        t.cols = {std::string(buf), "desc_" + std::to_string(i)};
        left.addTuple(t);
    }

    // Tabela direita: 10 páginas (120 tuplas)
    Schema schemaR({"fk", "dado"});
    Table right;
    right.schema = schemaR;
    for (int i = 1; i <= 120; ++i) {
        Tuple t;
        char buf[16];
        snprintf(buf, sizeof(buf), "%04d", i);
        t.cols = {std::string(buf), "dado_" + std::to_string(i)};
        right.addTuple(t);
    }

    std::cout << "Tabela esquerda: " << left.totalTuples() << " tuplas, "
              << left.qtd_pags << " paginas\n";
    std::cout << "Tabela direita: " << right.totalTuples() << " tuplas, "
              << right.qtd_pags << " paginas\n";

    Table result = sortMergeJoin(left, right, "id", "fk", disk, buffer);
    std::cout << "Resultado: " << result.totalTuples() << " tuplas\n";

    // Chaves da esquerda: 10, 20, 30, 40, 50
    // Direita tem 1..120, logo todos os 5 combinam
    assert(result.totalTuples() == 5);
    std::cout << "TC4 PASSOU!\n";
}

/*
TC5: Resultado vazio (nenhuma chave em comum).
*/
void testTC5(SimulatedDisk& disk, BufferManager& buffer) {
    std::cout << "\n========== TC5: Resultado vazio ==========\n";

    Schema schemaL({"id", "nome"});
    Table left;
    left.schema = schemaL;
    for (int i = 0; i < 10; ++i) {
        Tuple t;
        t.cols = {"LEFT_" + std::to_string(i), "nome_" + std::to_string(i)};
        left.addTuple(t);
    }

    Schema schemaR({"fk", "valor"});
    Table right;
    right.schema = schemaR;
    for (int i = 0; i < 10; ++i) {
        Tuple t;
        t.cols = {"RIGHT_" + std::to_string(i), "valor_" + std::to_string(i)};
        right.addTuple(t);
    }

    Table result = sortMergeJoin(left, right, "id", "fk", disk, buffer);
    std::cout << "Resultado: " << result.totalTuples() << " tuplas\n";

    assert(result.totalTuples() == 0);
    std::cout << "TC5 PASSOU!\n";
}

// ============================================================================
// Função principal
// ============================================================================

int main() {
    std::cout << "=============================================================\n";
    std::cout << "             Trabalho 3 - Sort-Merge Join\n";
    std::cout << "=============================================================\n";

    // Inicializa o disco simulado e o gerenciador de buffer
    SimulatedDisk disk;
    BufferManager buffer;

    // --- Carrega as tabelas dos arquivos CSV ---
    std::cout << "\n--- Carregando tabelas dos arquivos CSV ---\n";

    Table grapes = loadCSV("grapes.csv", 3);
    std::cout << "Grapes: " << grapes.totalTuples() << " tuplas em "
              << grapes.qtd_pags << " paginas\n";

    Table wines = loadCSV("wines.csv", 5);
    std::cout << "Wines:  " << wines.totalTuples() << " tuplas em "
              << wines.qtd_pags << " paginas\n";

    // --- Executa o Sort-Merge Join ---
    std::cout << "\n--- Executando Sort-Merge Join ---\n";
    std::cout << "Juncao: Grapes.chave_primaria = Wines.chave_estrangeira\n";

    Table result = sortMergeJoin(grapes, wines, "chave_primaria", "chave_estrangeira",
                                  disk, buffer);

    std::cout << "\n--- Resultado da Juncao ---\n";
    std::cout << "Total de tuplas no resultado: " << result.totalTuples() << "\n";
    std::cout << "Total de paginas no resultado: " << result.qtd_pags << "\n";

    // Exibe as primeiras 30 tuplas do resultado
    printTable(result, 30);

    // --- Verifica que as tabelas originais não foram modificadas ---
    std::cout << "\n--- Verificacao de integridade ---\n";
    Table grapes_check = loadCSV("grapes.csv", 3);
    Table wines_check = loadCSV("wines.csv", 5);
    assert(grapes.totalTuples() == grapes_check.totalTuples());
    assert(wines.totalTuples() == wines_check.totalTuples());
    std::cout << "Tabelas originais nao foram modificadas. OK!\n";

    // --- Executa testes sintéticos ---
    std::cout << "\n\n=============================================================\n";
    std::cout << "  Executando testes sinteticos\n";
    std::cout << "=============================================================\n";

    testTC1(disk, buffer);
    testTC2(disk, buffer);
    testTC3(disk, buffer);
    testTC4(disk, buffer);
    testTC5(disk, buffer);

    std::cout << "\n=============================================================\n";
    std::cout << "  Todos os testes passaram com sucesso!\n";
    std::cout << "=============================================================\n";

    return 0;
}
