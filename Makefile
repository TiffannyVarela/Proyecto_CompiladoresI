# Nombre del compilador
CXX = g++
# Bandera para mostrar advertencias y usar C++17
CXXFLAGS = -Wall -std=c++17

# Archivos fuente (todos los .cpp)
SRC = main.cpp \
      lectura.cpp \
      LexerRust.cpp \
      ParserRust.cpp \
	  NodoAST.cpp \
	  Logger.cpp

# Convertir los .cpp en .o
OBJ = $(SRC:.cpp=.o)

# Ejecutable final
OUT = programa

# Regla principal (por defecto)
all: $(OUT)

# Cómo crear el ejecutable: enlaza todos los .o
$(OUT): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regla para compilar cualquier .cpp a .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ejecutar el programa
run: $(OUT)
	./$(OUT)

# Limpiar: elimina ejecutable y objetos
clean:
	rm -f $(OUT) $(OBJ)
