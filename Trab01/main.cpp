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
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                            ║\n";
    std::cout << "║           GERENCIADOR DE BUFFER — SGBD 2026.1              ║\n";
    std::cout << "║           Universidade Federal do Ceará (UFC)              ║\n";
    std::cout << "║                                                            ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    std::cout << "║  Simulador de Buffer Pool com políticas de substituição:   ║\n";
    std::cout << "║    • LRU  (Least Recently Used)                            ║\n";
    std::cout << "║    • FIFO (First In, First Out)                            ║\n";
    std::cout << "║    • CLOCK (Second-Chance)                                 ║\n";
    std::cout << "║    • MRU  (Most Recently Used)                             ║\n";
    std::cout << "║                                                            ║\n";
    std::cout << "║  Capacidade do buffer: " << BUFFER_CAPACITY << " páginas"
              << "                            ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n\n";
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
        std::cout << "Selecione a política de substituição:\n";
        std::cout << "  1 - LRU  (Least Recently Used)\n";
        std::cout << "  2 - FIFO (First In, First Out)\n";
        std::cout << "  3 - CLOCK (Second-Chance)\n";
        std::cout << "  4 - MRU  (Most Recently Used)\n";
        std::cout << "Opção: ";
        std::cin >> opcao;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "[ERRO] Entrada inválida. Tente novamente.\n\n";
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (opcao) {
            case 1: return ReplacementPolicy::LRU;
            case 2: return ReplacementPolicy::FIFO;
            case 3: return ReplacementPolicy::CLOCK;
            case 4: return ReplacementPolicy::MRU;
            default:
                std::cout << "[ERRO] Opção inválida. Escolha entre 1 e 4.\n\n";
        }
    }
}

/**
 * Exibe o menu principal de operações.
 */
void exibirMenu() {
    std::cout << "────────────────────────────────────────────\n";
    std::cout << "  MENU DE OPERAÇÕES\n";
    std::cout << "────────────────────────────────────────────\n";
    std::cout << "  1 - Fetch (buscar página por chave)\n";
    std::cout << "  2 - Fetch em sequência (lista de chaves)\n";
    std::cout << "  3 - Exibir estado do buffer (DisplayCache)\n";
    std::cout << "  4 - Exibir estatísticas (DisplayStats)\n";
    std::cout << "  5 - Executar sequência de teste padrão\n";
    std::cout << "  0 - Sair\n";
    std::cout << "────────────────────────────────────────────\n";
    std::cout << "Opção: ";
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
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "  EXECUTANDO SEQUÊNCIA DE TESTE PADRÃO\n";
    std::cout << "  Sequência: 1, 2, 3, 4, 5, 1, 6, 2, 7, 3, 1, 8, 4, 9, 5\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

    for (size_t i = 0; i < tamanho; ++i) {
        std::cout << ">>> Fetch(" << sequencia[i] << ") — Acesso #" << (i + 1) << "\n";
        std::string conteudo = bm.Fetch(sequencia[i]);
        std::cout << "    Conteúdo: " << conteudo << "\n\n";

        // Checkpoint 1: após o 5º fetch (página 5)
        if (i == 4) {
            std::cout << "═══ CHECKPOINT 1 (após Fetch #5) ═══\n";
            bm.DisplayCache();
        }

        // Checkpoint 2: após o 8º fetch (página 2)
        if (i == 7) {
            std::cout << "═══ CHECKPOINT 2 (após Fetch #8) ═══\n";
            bm.DisplayCache();
            bm.DisplayStats();
        }

        // Checkpoint 3: após o 15º fetch (página 5) — último
        if (i == 14) {
            std::cout << "═══ CHECKPOINT 3 (após Fetch #15 — final) ═══\n";
            bm.DisplayCache();
            bm.DisplayStats();
        }
    }

    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "  SEQUÊNCIA DE TESTE CONCLUÍDA\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
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
            std::cout << "[ERRO] Entrada inválida.\n\n";
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (opcao) {
            case 1: {
                // Fetch individual
                int chave;
                std::cout << "Informe a chave da página (1-based): ";
                std::cin >> chave;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::string conteudo = bm.Fetch(chave);
                if (!conteudo.empty()) {
                    std::cout << "Conteúdo: " << conteudo << "\n\n";
                }
                break;
            }

            case 2: {
                // Fetch em sequência
                std::cout << "Informe as chaves separadas por vírgula (ex: 1,2,3,4,5): ";
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
                            std::cout << "    Conteúdo: " << conteudo << "\n";
                        }
                    } catch (const std::exception&) {
                        std::cout << "[ERRO] Chave inválida: '" << token << "'\n";
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
                std::cout << "\nEncerrando o Gerenciador de Buffer. Até logo!\n\n";
                break;

            default:
                std::cout << "[ERRO] Opção inválida.\n\n";
        }
    }

    return 0;
}
