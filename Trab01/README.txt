================================================================================
                 GERENCIADOR DE BUFFER — SGBD 2026.1
                 Universidade Federal do Ceará (UFC)
================================================================================

DESCRIÇÃO DO PROJETO
--------------------
Este projeto implementa um Gerenciador de Buffer (Buffer Manager) que simula
o comportamento de um SGBD ao gerenciar um pool de páginas em memória de
tamanho fixo (5 slots). O sistema suporta quatro políticas de substituição
de páginas, selecionáveis em tempo de execução:

  1. LRU  (Least Recently Used)  — Substitui a página menos recentemente usada
  2. FIFO (First In, First Out)  — Substitui a página mais antiga no buffer
  3. CLOCK (Second-Chance)       — Algoritmo do relógio com bit de referência
  4. MRU  (Most Recently Used)   — Substitui a página mais recentemente usada


ESTRUTURA DE ARQUIVOS
---------------------
  Page.h            — Definição da struct Page (página do buffer)
  BufferManager.h   — Declaração da classe BufferManager
  BufferManager.cpp — Implementação completa do gerenciador de buffer
  main.cpp          — Interface interativa via linha de comando (CLI)
  Makefile          — Regras de compilação
  README.txt        — Este arquivo


REQUISITOS
----------
  • Compilador g++ com suporte a C++17 (g++ 7+ recomendado)
  • Sistema operacional: Linux, macOS ou Windows (com MinGW/MSYS2)
  • Nenhuma biblioteca externa além da biblioteca padrão do C++


COMPILAÇÃO
----------
Via Makefile (Linux/macOS/MSYS2):

    make

Manualmente:

    g++ -std=c++17 -Wall -Wextra -O2 -o buffer_manager main.cpp BufferManager.cpp


EXECUÇÃO
--------
Após compilar, execute:

    ./buffer_manager          (Linux/macOS)
    buffer_manager.exe        (Windows)

O programa solicitará:
  1. O caminho do arquivo de dados (ex: data.txt)
  2. A política de substituição desejada (1-LRU, 2-FIFO, 3-CLOCK, 4-MRU)

Em seguida, um menu interativo será exibido com as seguintes opções:
  1 — Fetch individual (buscar uma página por chave)
  2 — Fetch em sequência (lista de chaves separadas por vírgula)
  3 — Exibir estado do buffer (DisplayCache)
  4 — Exibir estatísticas (DisplayStats)
  5 — Executar a sequência de teste padrão do trabalho
  0 — Sair


FORMATO DOS DADOS
-----------------
O arquivo de dados é um arquivo texto onde cada linha representa uma página.
As páginas são numeradas a partir de 1 (1-based index). Ou seja:
  - Linha 1 do arquivo = Página 1
  - Linha 2 do arquivo = Página 2
  - ...e assim por diante.


SEQUÊNCIA DE TESTE PADRÃO
--------------------------
A opção 5 do menu executa automaticamente a seguinte sequência de Fetch:

    1, 2, 3, 4, 5, 1, 6, 2, 7, 3, 1, 8, 4, 9, 5

Com checkpoints de exibição nos seguintes pontos:
  • Após o 5º fetch  → DisplayCache()
  • Após o 8º fetch  → DisplayCache() + DisplayStats()
  • Após o 15º fetch → DisplayCache() + DisplayStats()


EXPLICAÇÃO DAS POLÍTICAS
-------------------------

LRU (Least Recently Used):
  Mantém um timestamp lógico (relógio global) que é atualizado a cada acesso
  à página (tanto em hits quanto em misses). Quando o buffer está cheio, a
  página com o menor timestamp (isto é, a que não é acessada há mais tempo)
  é selecionada como vítima para substituição. Esta política é eficiente para
  cargas de trabalho com boa localidade temporal.

FIFO (First In, First Out):
  Cada página recebe um contador de inserção no momento em que é carregada no
  buffer. Este contador nunca é atualizado em caso de cache hit. Quando é
  necessário substituir uma página, a que possui o menor contador de inserção
  (ou seja, a que está no buffer há mais tempo) é escolhida como vítima.
  Simples de implementar, mas pode sofrer da anomalia de Bélády.

CLOCK (Second-Chance / Algoritmo do Relógio):
  Implementa uma variação do FIFO com segunda chance. Um ponteiro (clockHand)
  percorre circularmente os slots do buffer. Cada página possui um bit de
  referência (ref_bit) que é setado para TRUE quando a página é acessada.
  Durante a substituição, o ponteiro verifica o ref_bit de cada página:
    - Se ref_bit = TRUE: limpa o bit (segunda chance) e avança o ponteiro.
    - Se ref_bit = FALSE: seleciona essa página como vítima.
  Oferece um bom equilíbrio entre desempenho e simplicidade.

MRU (Most Recently Used):
  Funciona de forma oposta ao LRU: seleciona para substituição a página com
  o maior timestamp (a mais recentemente acessada). É útil em cenários de
  varredura sequencial (sequential scan), onde a página recém-acessada tem
  menor probabilidade de ser reutilizada.


FLAG DE ATUALIZAÇÃO (DIRTY BIT)
-------------------------------
Ao carregar uma página do disco para o buffer, a flag "dirty" é atribuída
aleatoriamente com probabilidade de 50%, utilizando std::bernoulli_distribution
e std::mt19937 como gerador de números pseudoaleatórios. Se a flag for TRUE,
isso indica que a página foi "modificada" e precisaria ser escrita de volta ao
disco antes de ser removida (simulação). Na saída de evicção, páginas dirty
são marcadas com "W" (Write-back).

================================================================================
