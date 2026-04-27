/*
data_file.cpp - Implementação do módulo de acesso ao ArquivoTexto

Lê o arquivo bd-trab2-dataset.csv de forma sequencial, uma linha por vez,
respeitando a restrição de memória (apenas 1 registro em memória por vez).
*/

#include "data_file.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Nome do arquivo CSV com os dados
static const char* DATA_FILE_NAME = "bd-trab2-dataset.csv";

bool getLinhaTexto(int linhaNum, std::string &outTexto) {
    std::ifstream file(DATA_FILE_NAME);
    if (!file.is_open()) {
        std::cerr << "Erro: nao foi possivel abrir o arquivo " << DATA_FILE_NAME << std::endl;
        return false;
    }

    std::string line;
    // Lê linha por linha (uma página por vez)
    while (std::getline(file, line)) {
        // Formato esperado: LinhaNum,LinhaTexto
        // ou LinhaNum;LinhaTexto dependendo do separador
        std::stringstream ss(line);
        std::string numStr;

        // Tenta separador vírgula primeiro
        if (std::getline(ss, numStr, ',')) {
            // Remove espaços em branco
            while (!numStr.empty() && (numStr[0] == ' ' || numStr[0] == '\t'))
                numStr.erase(numStr.begin());

            // Verifica se é um número válido
            bool isNumber = !numStr.empty();
            for (size_t i = 0; i < numStr.size(); i++) {
                if (!isdigit(numStr[i]) && !(i == 0 && numStr[i] == '-')) {
                    isNumber = false;
                    break;
                }
            }

            if (isNumber) {
                int num = std::stoi(numStr);
                if (num == linhaNum) {
                    // Recupera o resto da linha como LinhaTexto
                    std::string texto;
                    std::getline(ss, texto);
                    outTexto = texto;
                    file.close();
                    return true;
                }
            }
        }
        // Página lida e descartada - próxima iteração carrega a próxima
    }

    file.close();
    return false;
}
