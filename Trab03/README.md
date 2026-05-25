# Trabalho 03 – Sort-Merge Join

**Disciplina:** CK0095 – Sistemas de Gerenciamento de Banco de Dados (2026.1)  
**Universidade Federal do Ceará (UFC)**

## Descrição do Projeto

Implementação do operador algébrico **Sort-Merge Join (SMJ)** em C++17, sem uso de bibliotecas externas de SGBD. O programa realiza a junção de duas tabelas (`Grapes` e `Wines`) utilizando ordenação externa com buffer limitado a **B=5 frames** e páginas de **12 tuplas**.

### Esquema das Tabelas

| Tabela  | Colunas                                                   |
|---------|-----------------------------------------------------------|
| Grapes  | `chave_primaria` (PK), `grape_synonym`, `grape_country`   |
| Wines   | `vinho` (PK), `chave_estrangeira` (FK), `origem`, `tipo`, `estilo` |

**Atributo de junção:** `Grapes.chave_primaria = Wines.chave_estrangeira`

### Algoritmo

O algoritmo é dividido em duas fases:

1. **Ordenação Externa (External Sort):**
   - **Passo 0 (Geração de Runs):** Lê até 5 páginas por vez, ordena em memória pela chave de junção, e grava como runs ordenados no disco simulado.
   - **Passo 1+ (Merge K-Way):** Usa 4 frames de entrada e 1 de saída para fazer merge de até 4 runs por vez, utilizando uma fila de prioridade (min-heap). Repete até restar apenas 1 run totalmente ordenado.

2. **Merge-Join:**
   - Percorre as duas tabelas ordenadas simultaneamente, comparando as chaves de junção.
   - Quando as chaves são iguais, coleta todos os grupos de tuplas com mesma chave e emite o produto cartesiano completo.
   - Avança os cursores conforme necessário.

### Modelo de Memória

- **Buffer:** 5 frames (páginas)
- **Página:** 12 tuplas por página
- **Disco Simulado:** Estruturas de dados em memória que simulam persistência em disco

## Como Compilar

```bash
make
```

Ou manualmente:
```bash
g++ -std=c++17 -Wall -Wextra -g -c main.cpp
g++ -std=c++17 -Wall -Wextra -g -c src/schema.cpp
g++ -std=c++17 -Wall -Wextra -g -c src/tuple.cpp
g++ -std=c++17 -Wall -Wextra -g -c src/page.cpp
g++ -std=c++17 -Wall -Wextra -g -c src/table.cpp
g++ -std=c++17 -Wall -Wextra -g -c src/buffer_manager.cpp
g++ -std=c++17 -Wall -Wextra -g -c src/simulated_disk.cpp
g++ -std=c++17 -Wall -Wextra -g -c src/sort_merge_join.cpp
g++ -std=c++17 -Wall -Wextra -g -o sort_merge_join *.o
```

## Como Executar

```bash
make run
```

Ou diretamente:
```bash
./sort_merge_join.exe
```

> **Nota:** Os arquivos `grapes.csv` e `wines.csv` devem estar no mesmo diretório do executável.

## Casos de Teste

| Teste | Descrição                                                    |
|-------|--------------------------------------------------------------|
| TC1   | Tabelas que requerem exatamente um passo de merge (runs ≤ 4) |
| TC2   | Tabelas que requerem dois ou mais passos de merge (runs > 4) |
| TC3   | Chaves com muitas duplicatas (verifica produto cartesiano)   |
| TC4   | Uma tabela com uma página; outra com muitas páginas          |
| TC5   | Resultado vazio (nenhuma chave em comum)                     |

## Estrutura dos Arquivos

```
Trab03/
├── main.cpp                    # Programa principal e casos de teste
├── Makefile                    # Compilação e execução
├── README.md                   # Este arquivo
├── grapes.csv                  # Dados da tabela Grapes
├── wines.csv                   # Dados da tabela Wines
└── src/
    ├── schema.hpp / schema.cpp             # Classe Schema
    ├── tuple.hpp / tuple.cpp               # Classe Tuple
    ├── page.hpp / page.cpp                 # Classe Page
    ├── table.hpp / table.cpp               # Classe Table
    ├── buffer_manager.hpp / buffer_manager.cpp  # Classe BufferManager
    ├── simulated_disk.hpp / simulated_disk.cpp  # Classe SimulatedDisk
    └── sort_merge_join.hpp / sort_merge_join.cpp # Algoritmo Sort-Merge Join
```

## Equipe

- Gabriel Matias de Almeida
- Edileudo Maciel

