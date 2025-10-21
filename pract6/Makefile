CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -O2
LDLIBS :=

SRC := main.cc automata.cc fa_parser.cc automata_simulator.cc
OBJ := $(SRC:.cc=.o)
TARGET := p06_automata_simulator

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)


Añadir una opcion de traza a las opciones de ejecucion, añadir la palabra "trace" como tercer parametro opcional. 
Queremos que la ejecucion este en modo traza, de forma que informe en todo momento lo que hace la ejecución
En cada iteración con cada símbolo vaya mostrando el estado actual, simbolo de entrada actual, las reglas (transiciones y incluidas & transiciones) 
y el conjunto de estados siguientes en formato {}.
ejemplo: la cadena aa. El estado actual {0}, recibe una a y muestra sus transiciones desde estado 0 con a, incluyendo si hay & transiciones (sería en este
ejemplo la transición (0,a)->1 y hay una & transicion con (1,&)->3), y finalmente el estado siguiente en este caso {1,3}).
