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