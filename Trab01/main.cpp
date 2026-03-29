/**
 * =============================================================================
 * Arquivo : main.cpp
 * Autora  : Kenia
 * Data    : 29/03/2026
 * Descr.  : Programa principal com CLI interativa para o Gerenciador de Buffer.
 *           Permite selecionar a política de substituição, executar sequências
 *           de Fetch, e visualizar o estado do buffer e estatísticas.
 * =============================================================================
 */

#include "BufferManager.h"
#include <iostream>
#include <sstream>
#include <limits>

// =============================================================================
// Funções auxiliares do menu
// =============================================================================

/**
 * Exibe o banner inicial do programa.
 */
void exibirBanner() {
    std::cout << "\n";
    std::cout << "+============================================================+\n";
    std::cout << "|                                                            |\n";
    std::cout << "|           GERENCIADOR DE BUFFER - SGBD 2026.1              |\n";
    std::cout << "|           Universidade Federal do Ceara (UFC)              |\n";
    std::cout << "|                                                            |\n";
    std::cout << "+============================================================+\n";
    std::cout << "|  Simulador de Buffer Pool com politicas de substituicao:   |\n";
    std::cout << "|    * LRU  (Least Recently Used)                            |\n";
    std::cout << "|    * FIFO (First In, First Out)                            |\n";
    std::cout << "|    * CLOCK (Second-Chance)                                 |\n";
    std::cout << "|    * MRU  (Most Recently Used)                             |\n";
    std::cout << "|                                                            |\n";
    std::cout << "|  Capacidade do buffer: " << BUFFER_CAPACITY << " paginas"
              << "                           |\n";
    std::cout << "+============================================================+\n\n";
}

/**
 * Solicita ao usuário o caminho do arquivo de dados.
 * @return Caminho do arquivo informado pelo usuário.
 */
std::string solicitarArquivo() {
    std::string caminho;
    std::cout << "Informe o caminho do arquivo de dados (ex: data.txt): ";
    std::getline(std::cin, caminho);
    return caminho;
}

/**
 * Solicita ao usuário a política de substituição desejada.
 * @return Política selecionada.
 */
ReplacementPolicy solicitarPolitica() {
    int opcao = 0;

    while (true) {
        std::cout << "Selecione a politica de substituicao:\n";
        std::cout << "  1 - LRU  (Least Recently Used)\n";
        std::cout << "  2 - FIFO (First In, First Out)\n";
        std::cout << "  3 - CLOCK (Second-Chance)\n";
        std::cout << "  4 - MRU  (Most Recently Used)\n";
        std::cout << "Opcao: ";
        std::cin >> opcao;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "[ERRO] Entrada invalida. Tente novamente.\n\n";
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (opcao) {
            case 1: return ReplacementPolicy::LRU;
            case 2: return ReplacementPolicy::FIFO;
            case 3: return ReplacementPolicy::CLOCK;
            case 4: return ReplacementPolicy::MRU;
            default:
                std::cout << "[ERRO] Opcao invalida. Escolha entre 1 e 4.\n\n";
        }
    }
}

/**
 * Exibe o menu principal de operações.
 */
void exibirMenu() {
    std::cout << "--------------------------------------------\n";
    std::cout << "  MENU DE OPERACOES\n";
    std::cout << "--------------------------------------------\n";
    std::cout << "  1 - Fetch (buscar pagina por chave)\n";
    std::cout << "  2 - Fetch em sequencia (lista de chaves)\n";
    std::cout << "  3 - Exibir estado do buffer (DisplayCache)\n";
    std::cout << "  4 - Exibir estatisticas (DisplayStats)\n";
    std::cout << "  5 - Executar sequencia de teste padrao\n";
    std::cout << "  0 - Sair\n";
    std::cout << "--------------------------------------------\n";
    std::cout << "Opcao: ";
}

/**
 * Executa a sequência de teste especificada no enunciado do trabalho:
 * Fetch: 1, 2, 3, 4, 5, 1, 6, 2, 7, 3, 1, 8, 4, 9, 5
 * Com checkpoints nos pontos determinados.
 */
void executarTestePadrao(BufferManager& bm) {
    // Sequência completa de teste
    const int sequencia[] = {1, 2, 3, 4, 5, 1, 6, 2, 7, 3, 1, 8, 4, 9, 5};
    const size_t tamanho = sizeof(sequencia) / sizeof(sequencia[0]);

    std::cout << "\n";
    std::cout << "==============================================================\n";
    std::cout << "  EXECUTANDO SEQUENCIA DE TESTE PADRAO\n";
    std::cout << "  Sequencia: 1, 2, 3, 4, 5, 1, 6, 2, 7, 3, 1, 8, 4, 9, 5\n";
    std::cout << "==============================================================\n\n";

    for (size_t i = 0; i < tamanho; ++i) {
        std::cout << ">>> Fetch(" << sequencia[i] << ") - Acesso #" << (i + 1) << "\n";
        std::string conteudo = bm.Fetch(sequencia[i]);
        std::cout << "    Conteudo: " << conteudo << "\n\n";

        // Checkpoint 1: apos o 5o fetch (pagina 5)
        if (i == 4) {
            std::cout << "=== CHECKPOINT 1 (apos Fetch #5) ===\n";
            bm.DisplayCache();
        }

        // Checkpoint 2: apos o 8o fetch (pagina 2)
        if (i == 7) {
            std::cout << "=== CHECKPOINT 2 (apos Fetch #8) ===\n";
            bm.DisplayCache();
            bm.DisplayStats();
        }

        // Checkpoint 3: apos o 15o fetch (pagina 5) - ultimo
        if (i == 14) {
            std::cout << "=== CHECKPOINT 3 (apos Fetch #15 - final) ===\n";
            bm.DisplayCache();
            bm.DisplayStats();
        }
    }

    std::cout << "==============================================================\n";
    std::cout << "  SEQUENCIA DE TESTE CONCLUIDA\n";
    std::cout << "==============================================================\n\n";
}

// =============================================================================
// Função principal
// =============================================================================

int main() {
    exibirBanner();

    // Solicita caminho do arquivo de dados
    std::string caminhoArquivo = solicitarArquivo();
    std::cout << "\n";

    // Solicita política de substituição
    ReplacementPolicy politica = solicitarPolitica();
    std::cout << "\n";

    // Cria o gerenciador de buffer
    BufferManager bm(caminhoArquivo, politica);

    // Loop principal do menu interativo
    int opcao = -1;
    while (opcao != 0) {
        exibirMenu();
        std::cin >> opcao;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "[ERRO] Entrada invalida.\n\n";
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (opcao) {
            case 1: {
                // Fetch individual
                int chave;
                std::cout << "Informe a chave da pagina (1-based): ";
                std::cin >> chave;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::string conteudo = bm.Fetch(chave);
                if (!conteudo.empty()) {
                    std::cout << "Conteudo: " << conteudo << "\n\n";
                }
                break;
            }

            case 2: {
                // Fetch em sequência
                std::cout << "Informe as chaves separadas por virgula (ex: 1,2,3,4,5): ";
                std::string entrada;
                std::getline(std::cin, entrada);

                std::stringstream ss(entrada);
                std::string token;
                while (std::getline(ss, token, ',')) {
                    try {
                        int chave = std::stoi(token);
                        std::cout << "\n>>> Fetch(" << chave << ")\n";
                        std::string conteudo = bm.Fetch(chave);
                        if (!conteudo.empty()) {
                            std::cout << "    Conteudo: " << conteudo << "\n";
                        }
                    } catch (const std::exception&) {
                        std::cout << "[ERRO] Chave invalida: '" << token << "'\n";
                    }
                }
                std::cout << "\n";
                break;
            }

            case 3:
                // Exibir cache
                bm.DisplayCache();
                break;

            case 4:
                // Exibir estatísticas
                bm.DisplayStats();
                break;

            case 5:
                // Sequência de teste padrão
                executarTestePadrao(bm);
                break;

            case 0:
                std::cout << "\nEncerrando o Gerenciador de Buffer. Ate logo!\n\n";
                break;

            default:
                std::cout << "[ERRO] Opcao invalida.\n\n";
        }
    }

    return 0;
}
