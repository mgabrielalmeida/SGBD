# ==============================================================================
# Arquivo : Makefile
# Autora  : Kenia
# Data    : 29/03/2026
# Descr.  : Regras de compilação para o Gerenciador de Buffer (SGBD 2026.1).
#            Utiliza g++ com padrão C++17 e flags de aviso rigorosas.
# ==============================================================================

CXX       = g++
CXXFLAGS  = -std=c++17 -Wall -Wextra -O2
TARGET    = buffer_manager
SOURCES   = main.cpp BufferManager.cpp
HEADERS   = Page.h BufferManager.h

# Regra principal: compila o executável
all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

# Limpa os arquivos gerados pela compilação
clean:
	rm -f $(TARGET) $(TARGET).exe

# Executa o programa com o arquivo de dados padrão
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
