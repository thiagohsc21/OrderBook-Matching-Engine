# Compilador e flags
CXX = g++
# CXX: define o compilador C++ (neste caso, g++)

CXXFLAGS = -std=c++17 -Wall -Iinclude
# CXXFLAGS: opções de compilação
# -std=c++17: usa a versão C++17
# -Wall: mostra todos os warnings (boas práticas!)
# -Iinclude: diz ao compilador para procurar headers (.hpp) na pasta include/

# Diretório de objetos (arquivos .o)
OBJDIR = build
# OBJDIR: pasta onde vamos salvar os arquivos .o para não poluir src/

# Fontes do projeto
SRC = $(shell find src -name "*.cpp")
# SRC: pega todos os arquivos .cpp dentro de src/ e subpastas (ex: src/core/, src/utils/)

# Objetos correspondentes (.o)
OBJ = $(patsubst src/%.cpp, $(OBJDIR)/%.o, $(SRC))
# OBJ: converte cada arquivo .cpp em um .o equivalente, mas dentro da pasta build/
# Exemplo: src/core/orderbook.cpp → build/core/orderbook.o

# Nome do binário final
TARGET = orderbook
# TARGET: nome do executável final

# Regra principal: 'make' chama isso por padrão
all: $(TARGET)
# Regra padrão: para compilar tudo, precisamos gerar o $(TARGET)

# Linkagem final
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@
# Para gerar o binário (TARGET), usamos todos os objetos (.o)
# $@: representa o nome da meta atual, aqui: 'orderbook'
# $^: seria todos os arquivos de dependência (neste caso, os objetos)

# Compilar cada .cpp em .o dentro de build/
$(OBJDIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
# Essa é a regra genérica para compilar .cpp → .o
# $< é o arquivo de entrada (ex: src/core/orderbook.cpp)
# $@ é o destino (ex: build/core/orderbook.o)
# mkdir -p cria a pasta build/core/ se ela ainda não existir

# Exibir informações úteis
debug:
	@echo "Source files:"  
	@for src in $(SRC); do echo "  $$src"; done
	@echo "Object files:"
	@for obj in $(OBJ); do echo "  $$obj"; done

# Limpeza do projeto
clean:
	rm -rf $(OBJDIR) $(TARGET)
# 'make clean' remove a pasta de objetos e o binário
# Útil para recomeçar uma build do zero

# Declara comandos que não são arquivos
.PHONY: all clean debug
# Isso informa ao Make que 'all' e 'clean' são comandos, não arquivos reais
