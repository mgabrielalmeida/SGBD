/**
 * main.cpp - Ponto de entrada do programa
 *
 * Trabalho 02 - Índice Hash Extensível
 * Disciplina: Sistemas de Gerenciamento de Banco de Dados (SGBD)
 *
 * Lê o arquivo in.txt com as operações, processa cada uma delas
 * utilizando o índice hash extensível e escreve os resultados em out.txt.
 *
 * Operações suportadas:
 *   INC:x   → Inserir LinhaNum x no índice
 *   REM:x   → Remover LinhaNum x do índice
 *   BUS=:x  → Busca por igualdade de LinhaNum x
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include "hash_index.h"
#include "directory.h"
#include "bucket.h"
#include "data_file.h"

// Nomes dos arquivos de entrada e saída
static const char* INPUT_FILE  = "in.txt";
static const char* OUTPUT_FILE = "out.txt";

/**
 * Remove espaços em branco e caracteres de controle do início e fim da string.
 */
static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

int main() {
    // Abre o arquivo de entrada
    std::ifstream inFile(INPUT_FILE);
    if (!inFile.is_open()) {
        std::cerr << "Erro: nao foi possivel abrir o arquivo " << INPUT_FILE << std::endl;
        return 1;
    }

    // Abre o arquivo de saída
    std::ofstream outFile(OUTPUT_FILE);
    if (!outFile.is_open()) {
        std::cerr << "Erro: nao foi possivel criar o arquivo " << OUTPUT_FILE << std::endl;
        inFile.close();
        return 1;
    }

    // ----------------------------------------------------------------
    // 1. Lê a primeira linha: PG/<profundidade_global_inicial>
    // ----------------------------------------------------------------
    std::string firstLine;
    std::getline(inFile, firstLine);
    firstLine = trim(firstLine);

    int globalDepth = 0;
    if (firstLine.substr(0, 3) == "PG/") {
        globalDepth = std::stoi(firstLine.substr(3));
    } else {
        std::cerr << "Erro: formato invalido na primeira linha do in.txt" << std::endl;
        std::cerr << "Esperado: PG/<profundidade_global>" << std::endl;
        inFile.close();
        outFile.close();
        return 1;
    }

    // Escreve a primeira linha no out.txt (idêntica ao in.txt)
    outFile << firstLine << std::endl;

    // ----------------------------------------------------------------
    // 2. Inicializa o diretório do índice hash
    // ----------------------------------------------------------------
    Directory dir;
    initDirectory(&dir, globalDepth);

    std::cout << "Indice hash extensivel inicializado com PG=" << globalDepth << std::endl;
    std::cout << "Diretorio com " << dir.size << " entradas" << std::endl;

    // ----------------------------------------------------------------
    // 3. Processa cada operação do arquivo de entrada
    // ----------------------------------------------------------------
    std::string line;
    int opCount = 0;

    while (std::getline(inFile, line)) {
        line = trim(line);
        if (line.empty()) continue;

        opCount++;

        // ------- Operação INC:x (Inserção) -------
        if (line.substr(0, 4) == "INC:") {
            int linhaNum = std::stoi(line.substr(4));

            InsertResult result = insertEntry(&dir, linhaNum);

            if (result.success) {
                // Se houve duplicação(ões) de diretório, escreve DUP DIR antes
                for (int d = 0; d < result.dupCount; d++) {
                    outFile << "DUP DIR:/"
                            << result.dupGlobalDepths[d] << ","
                            << result.dupLocalDepths[d] << std::endl;
                }

                // Escreve a linha INC
                outFile << "INC:" << linhaNum << "/"
                        << result.globalDepth << ","
                        << result.localDepth << std::endl;
            } else {
                std::cerr << "Erro ao inserir LinhaNum " << linhaNum << std::endl;
            }
        }
        // ------- Operação REM:x (Remoção) -------
        else if (line.substr(0, 4) == "REM:") {
            int linhaNum = std::stoi(line.substr(4));

            RemoveResult result = removeEntry(&dir, linhaNum);

            outFile << "REM:" << linhaNum << "/"
                    << result.removedCount << ","
                    << result.globalDepth << ","
                    << result.localDepth << std::endl;
        }
        // ------- Operação BUS=:x (Busca por igualdade) -------
        else if (line.substr(0, 5) == "BUS=:") {
            int linhaNum = std::stoi(line.substr(5));

            SearchResult result = searchEntry(&dir, linhaNum);

            outFile << "BUS:" << linhaNum << "/"
                    << result.foundCount << std::endl;
        }
        else {
            std::cerr << "Aviso: operacao desconhecida na linha: " << line << std::endl;
        }
    }

    // ----------------------------------------------------------------
    // 4. Escreve a última linha: P:/<profundidade_global_final>
    // ----------------------------------------------------------------
    outFile << "P:/" << dir.globalDepth << std::endl;

    // ----------------------------------------------------------------
    // 5. Limpeza e finalização
    // ----------------------------------------------------------------
    inFile.close();
    outFile.close();
    freeDirectory(&dir);

    std::cout << "Processamento concluido. " << opCount << " operacoes executadas." << std::endl;
    std::cout << "Resultados gravados em " << OUTPUT_FILE << std::endl;

    return 0;
}
