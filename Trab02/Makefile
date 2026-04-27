# Makefile - Índice Hash Extensível
# Disciplina: Sistemas de Gerenciamento de Banco de Dados (SGBD)
# Trabalho 02
#
# Compilação: make
# Execução:   make run
# Limpeza:    make clean

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g
TARGET = sgbd

# Arquivos fonte
SOURCES = main.cpp hash_index.cpp directory.cpp bucket.cpp data_file.cpp
HEADERS = hash_index.h directory.h bucket.h data_file.h
OBJECTS = $(SOURCES:.cpp=.o)

# Regra principal
all: $(TARGET) debug_dump

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Regras de compilação individuais
main.o: main.cpp hash_index.h directory.h bucket.h data_file.h
	$(CXX) $(CXXFLAGS) -c main.cpp

hash_index.o: hash_index.cpp hash_index.h directory.h bucket.h data_file.h
	$(CXX) $(CXXFLAGS) -c hash_index.cpp

directory.o: directory.cpp directory.h bucket.h
	$(CXX) $(CXXFLAGS) -c directory.cpp

bucket.o: bucket.cpp bucket.h
	$(CXX) $(CXXFLAGS) -c bucket.cpp

data_file.o: data_file.cpp data_file.h
	$(CXX) $(CXXFLAGS) -c data_file.cpp

# Debug dump - mostra o estado do diretório e buckets em disco
debug_dump: debug_dump.cpp directory.o bucket.o
	$(CXX) $(CXXFLAGS) -o debug_dump debug_dump.cpp directory.o bucket.o

# Executa o programa
run: $(TARGET)
	$(TARGET).exe

# Limpa arquivos compilados e dados gerados
clean:
	-del /f /q *.o $(TARGET).exe debug_dump.exe 2>nul
	-del /f /q data\directory.dat data\bucket_*.dat 2>nul
	-del /f /q test_cases\outputs\out.txt 2>nul

# Limpa apenas os dados do índice (mantém o executável)
clean-data:
	-del /f /q data\directory.dat data\bucket_*.dat 2>nul
	-del /f /q test_cases\outputs\out.txt 2>nul

# Executa o debug dump
dump: debug_dump
	debug_dump.exe

.PHONY: all run clean clean-data dump
