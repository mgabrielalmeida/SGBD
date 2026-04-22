/**
 * data_file.h - Módulo de acesso ao ArquivoTexto (bd-trab2-dataset.csv)
 *
 * Responsável por recuperar uma linha/tupla específica do arquivo CSV
 * usando LinhaNum como RID. Respeita a restrição de memória: apenas
 * um registro em memória por vez.
 */

#ifndef DATA_FILE_H
#define DATA_FILE_H

#include <string>

// Tamanho máximo de uma linha do arquivo CSV
const int MAX_LINE_LENGTH = 4096;

/**
 * Recupera o texto (LinhaTexto) associado a um LinhaNum no arquivo CSV.
 * Faz busca sequencial pelo arquivo, lendo uma linha por vez.
 *
 * @param linhaNum  O número da linha (RID / chave de busca)
 * @param outTexto  String de saída com o texto encontrado
 * @return true se a linha foi encontrada, false caso contrário
 */
bool getLinhaTexto(int linhaNum, std::string &outTexto);

#endif // DATA_FILE_H
