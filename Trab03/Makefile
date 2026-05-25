# Makefile - Sort-Merge Join (Trab03)
# Disciplina: CK0095 – Sistemas de Gerenciamento de Banco de Dados (2026.1)
# Universidade Federal do Ceará (UFC)
#
# Compilação: make
# Execução:   make run
# Limpeza:    make clean

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
TARGET = sort_merge_join

# Diretório dos arquivos fonte
SRCDIR = src

# Arquivos fonte
SOURCES = main.cpp \
          $(SRCDIR)/schema.cpp \
          $(SRCDIR)/tuple.cpp \
          $(SRCDIR)/page.cpp \
          $(SRCDIR)/table.cpp \
          $(SRCDIR)/buffer_manager.cpp \
          $(SRCDIR)/simulated_disk.cpp \
          $(SRCDIR)/sort_merge_join.cpp

# Arquivos de cabeçalho
HEADERS = $(SRCDIR)/schema.hpp \
          $(SRCDIR)/tuple.hpp \
          $(SRCDIR)/page.hpp \
          $(SRCDIR)/table.hpp \
          $(SRCDIR)/buffer_manager.hpp \
          $(SRCDIR)/simulated_disk.hpp \
          $(SRCDIR)/sort_merge_join.hpp

# Objetos gerados
OBJECTS = main.o \
          schema.o \
          tuple.o \
          page.o \
          table.o \
          buffer_manager.o \
          simulated_disk.o \
          sort_merge_join.o

# Regra principal
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Regras de compilação individuais
main.o: main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c main.cpp

schema.o: $(SRCDIR)/schema.cpp $(SRCDIR)/schema.hpp
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/schema.cpp

tuple.o: $(SRCDIR)/tuple.cpp $(SRCDIR)/tuple.hpp
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/tuple.cpp

page.o: $(SRCDIR)/page.cpp $(SRCDIR)/page.hpp $(SRCDIR)/tuple.hpp
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/page.cpp

table.o: $(SRCDIR)/table.cpp $(SRCDIR)/table.hpp $(SRCDIR)/page.hpp $(SRCDIR)/schema.hpp
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/table.cpp

buffer_manager.o: $(SRCDIR)/buffer_manager.cpp $(SRCDIR)/buffer_manager.hpp $(SRCDIR)/page.hpp $(SRCDIR)/table.hpp
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/buffer_manager.cpp

simulated_disk.o: $(SRCDIR)/simulated_disk.cpp $(SRCDIR)/simulated_disk.hpp $(SRCDIR)/table.hpp
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/simulated_disk.cpp

sort_merge_join.o: $(SRCDIR)/sort_merge_join.cpp $(SRCDIR)/sort_merge_join.hpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/sort_merge_join.cpp

# Executa o programa
run: $(TARGET)
	$(TARGET).exe

# Limpa arquivos compilados
clean:
	-del /f /q *.o $(TARGET).exe 2>nul

.PHONY: all run clean
