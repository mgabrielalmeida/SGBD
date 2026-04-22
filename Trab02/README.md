# Índice Hash Extensível — Trabalho 02 (SGBD)

## Descrição

Implementação de um **Índice Hash Extensível** em C++ para um banco de dados que armazena trechos do livro *Dom Casmurro* de Machado de Assis.

O índice utiliza os **PG bits menos significativos** do valor inteiro `LinhaNum` como chave de dispersão (hash). Cada bucket suporta no máximo **5 entradas**. Quando um bucket transborda, ocorre um **split** com possível **duplicação do diretório**.

### Restrição de Memória

Apenas **uma página de dados** pode estar em memória por vez (exceto o diretório, que pode permanecer completamente em memória). A cada acesso a um bucket ou ao arquivo CSV, o dado anterior é liberado antes de carregar o novo.

---

## Estrutura do Projeto

| Arquivo | Descrição |
|---|---|
| `main.cpp` | Ponto de entrada. Lê `in.txt`, processa operações e escreve `out.txt` |
| `hash_index.h/.cpp` | Lógica central: inserção (com split/duplicação), remoção e busca |
| `directory.h/.cpp` | Gerenciamento do diretório (doubling, persistência em disco) |
| `bucket.h/.cpp` | Gerenciamento de buckets (leitura/escrita de arquivos individuais) |
| `data_file.h/.cpp` | Acesso ao arquivo CSV (`bd-trab2-dataset.csv`) |
| `Makefile` | Compilação e execução |

### Arquivos Gerados em Tempo de Execução

| Arquivo | Descrição |
|---|---|
| `data/directory.dat` | Diretório do índice hash (binário) |
| `data/bucket_N.dat` | Arquivo de cada bucket (binário) |
| `test_cases/outputs/out.txt` | Resultado das operações executadas |

---

## Compilação

### Usando Make (Linux/macOS)
```bash
make
```

### Usando g++ diretamente (Windows/Linux)
```bash
g++ -std=c++11 -Wall -o sgbd main.cpp hash_index.cpp directory.cpp bucket.cpp data_file.cpp
```

---

## Execução

1. Coloque os arquivos de entrada (ex: `in.txt`) na pasta `test_cases/inputs/`.
   - Você também pode passar caminhos customizados via terminal: `./sgbd <caminho_entrada> <caminho_saida>`.
2. Coloque o arquivo `bd-trab2-dataset.csv` com os dados no mesmo diretório do arquivo executável principal.
3. Certifique-se de que as pastas `data/`, `test_cases/inputs/` e `test_cases/outputs/` existam. 
4. Execute:

```bash
# Linux/macOS
./sgbd

# Windows
sgbd.exe
```

5. O resultado padrão será gravado em `test_cases/outputs/out.txt` caso nenhum argumento seja passado.

---

## Formato do Arquivo de Entrada (`in.txt`)

```
PG/<profundidade_global_inicial>
INC:x       → Inserir LinhaNum x no índice
REM:x       → Remover LinhaNum x do índice
BUS=:x      → Busca por igualdade de LinhaNum x
```

### Exemplo:
```
PG/4
INC:8
INC:16
BUS=:8
REM:16
```

---

## Formato do Arquivo de Saída (`out.txt`)

```
PG/<profundidade_global_inicial>

INC:x/<PG>,<PL>                          (inserção sem duplicação)
DUP DIR:/<novo_PG>,<PL_do_split>         (antes da inserção que causou)
INC:x/<PG>,<PL>

REM:x/<qtd_removidas>,<PG>,<PL>
BUS:x/<quantidade_encontradas>

P:/<profundidade_global_final>           (última linha, sempre)
```

---

## Algoritmos Implementados

### Inserção (`INC:x`)
1. Calcula hash = `x & ((1 << PG) - 1)`
2. Localiza o bucket via diretório
3. Se há espaço, insere diretamente
4. Se o bucket está cheio, executa **split**

### Split de Bucket
1. Incrementa PL do bucket
2. Se `PL > PG`, duplica o diretório (`PG++`)
3. Cria bucket irmão e redistribui as entradas
4. Atualiza entradas do diretório
5. Caso recursivo: se após o split o bucket ainda transbordar, repete o processo

### Remoção (`REM:x`)
1. Localiza o bucket via hash
2. Remove a entrada se encontrada
3. Não implementa merge/shrink de buckets

### Busca (`BUS=:x`)
1. Localiza o bucket via hash
2. Procura a entrada no bucket
3. Se encontrada, recupera o texto do CSV

---

## Limpeza

```bash
# Remove tudo (executável + dados)
make clean

# Remove apenas os dados gerados
make clean-data
```

---

## Requisitos

- Compilador C++ com suporte a C++11 (g++ 4.8+ ou equivalente)
- Nenhuma dependência externa além da biblioteca padrão do C++
