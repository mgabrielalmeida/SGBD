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
all: $(TARGET)

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

# Executa o programa
run: $(TARGET)
	./$(TARGET)

# Limpa arquivos compilados e dados gerados
clean:
	rm -f $(OBJECTS) $(TARGET) $(TARGET).exe
	rm -f data/directory.dat data/bucket_*.dat
	rm -f test_cases/outputs/out.txt

# Limpa apenas os dados do índice (mantém o executável)
clean-data:
	rm -f data/directory.dat data/bucket_*.dat
	rm -f test_cases/outputs/out.txt

.PHONY: all run clean clean-data
