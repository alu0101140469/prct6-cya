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







Aquí tienes solo las partes de código que he modificado para añadir la opción trace (tercer parámetro opcional) y para que la simulación muestre la traza detallada cuando trace está activado.

Aplica estos tres cambios (en main.cc, automata_simulator.h y automata_simulator.cc).


---

1) main.cc — cambios en comprobación de argumentos y llamada a Simulate

Reemplaza la comprobación de argumentos y la llamada a Simulate por este fragmento (sustituye la parte correspondiente en tu main.cc):

// --- sustitución de la comprobación de argumentos para aceptar opcional "trace"
  // Ahora aceptamos: ./p06_automata_simulator input.fa input.txt [trace]
  if (argc == 1) {
    PrintUsage();
    return 1;
  }
  if (argc == 2) {
    std::string arg = argv[1];
    if (arg == "--help" || arg == "-h") {
      PrintHelp();
      return 0;
    }
    PrintUsage();
    return 1;
  }
  // aceptamos 3 ó 4 argumentos: programa fa_file txt_file [trace]
  if (argc != 3 && argc != 4) {
    PrintUsage();
    return 1;
  }

  std::string fa_file = argv[1];
  std::string txt_file = argv[2];

  bool trace_mode = false;
  if (argc == 4) {
    std::string opt = argv[3];
    if (opt == "trace") {
      trace_mode = true;
    } else {
      std::cerr << "Opción desconocida: " << opt << "\n";
      PrintUsage();
      return 1;
    }
  }

Y en el bucle principal donde llamas a Simulate, cambia la línea:

bool accepted = simulator.Simulate(input);

por:

bool accepted = simulator.Simulate(input, trace_mode);

(El resto del while y la impresión de original --- Accepted/Rejected permanece igual.)


---

2) automata_simulator.h — añadir parámetro trace en la declaración

Modifica la declaración de Simulate para aceptar el parámetro opcional trace (sustituye solo la declaración correspondiente en tu header):

/**
   * @brief Simula la cadena dada sobre el autómata.
   * @param input Cadena de entrada (string vacío representa la cadena epsilon)
   * @param trace Si true, imprime traza detallada de la simulación
   * @return true si la cadena es aceptada, false si es rechazada
   */
  bool Simulate(const std::string& input, bool trace = false) const;


---

3) automata_simulator.cc — implementación de Simulate con traza

Sustituye la implementación anterior de Simulate por la siguiente versión (incluye además los #include extra necesarios al principio del fichero — sustituye/añade en la zona de includes):

#include "automata_simulator.h"

#include <queue>    // std::queue para BFS/cola en epsilon-closure
#include <iostream> // std::cout, std::endl (para traza)
#include <vector>
#include <algorithm>
#include <sstream>

namespace p06 {

Y sustituye la función Simulate por esta (implementa la traza detallada cuando trace == true):

bool AutomatonSimulator::Simulate(const std::string& input, bool trace) const {
  // Helper para imprimir un conjunto en formato {a,b,c}
  auto PrintSet = [](const Automaton::StateSet& s) -> std::string {
    std::vector<Automaton::State> v(s.begin(), s.end());
    std::sort(v.begin(), v.end());
    std::ostringstream oss;
    oss << "{";
    for (size_t i = 0; i < v.size(); ++i) {
      if (i) oss << ",";
      oss << v[i];
    }
    oss << "}";
    return oss.str();
  };

  // 1) Inicialización: estado inicial del NFA es epsilon-closure del start_state
  Automaton::StateSet current;
  current.insert(automaton_.GetStartState());
  current = EpsilonClosure(current);

  if (trace) {
    std::cout << "[TRACE] Inicial (epsilon-closure del start): " << PrintSet(current) << "\n";
  }

  // 2) Validación rápida: si la entrada contiene símbolos que no están en el alfabeto
  for (char c : input) {
    if (!automaton_.IsSymbolInAlphabet(c)) {
      if (trace) {
        std::cout << "[TRACE] Símbolo no válido encontrado en la entrada: '" << c << "' -> Rejected\n";
      }
      return false;
    }
  }

  // 3) Procesamos símbolo a símbolo
  for (char c : input) {
    if (trace) {
      std::cout << "[TRACE] --------------------------------------------------\n";
      std::cout << "[TRACE] Símbolo actual: '" << c << "'\n";
      std::cout << "[TRACE] Estados actuales: " << PrintSet(current) << "\n";
      std::cout << "[TRACE] Reglas (transiciones desde cada estado con '" << c << "'):\n";
    }

    Automaton::StateSet next;  // conjunto temporal de estados alcanzables con 'c'

    // 3.a) Para cada estado activo actualmente, añadimos sus destinos por 'c'
    for (auto s : current) {
      const auto& trans_map = automaton_.GetTransitionsForState(s);
      // transiciones con el símbolo c
      auto it_sym = trans_map.find(c);
      if (it_sym != trans_map.end()) {
        // mostramos la regla (s,c) -> {destinos}
        if (trace) {
          std::cout << "  (" << s << "," << c << ") -> " << PrintSet(it_sym->second) << "\n";
        }
        for (const auto& dest : it_sym->second) {
          next.insert(dest);
        }
      } else {
        if (trace) {
          std::cout << "  (" << s << "," << c << ") -> {}" << "\n";
        }
      }
      // También mostramos si desde s hay transiciones epsilon directas (informativo)
      auto it_eps_from_s = trans_map.find('&');
      if (trace) {
        if (it_eps_from_s != trans_map.end()) {
          std::cout << "  (" << s << ",&) -> " << PrintSet(it_eps_from_s->second) << "  [epsilon desde estado actual]\n";
        }
      }
    }

    // Antes de aplicar epsilon-closure a 'next', mostramos si desde los destinos hay & transiciones
    if (trace) {
      std::cout << "[TRACE] Destinos directos tras consumir '" << c << "': " << PrintSet(next) << "\n";
      std::cout << "[TRACE] Reglas (epsilon desde destinos directos):\n";
      for (auto destState : next) {
        const auto& trans_map_dest = automaton_.GetTransitionsForState(destState);
        auto it_eps = trans_map_dest.find('&');
        if (it_eps != trans_map_dest.end()) {
          std::cout << "  (" << destState << ",&) -> " << PrintSet(it_eps->second) << "\n";
        } else {
          std::cout << "  (" << destState << ",&) -> {}" << "\n";
        }
      }
    }

    // 3.b) Aplicamos epsilon-closure al conjunto 'next' (transiciones epsilon posteriores)
    current = EpsilonClosure(next);

    if (trace) {
      std::cout << "[TRACE] Estados tras epsilon-closure: " << PrintSet(current) << "\n";
    }

    // 3.c) Si no quedan estados activos, la cadena es definitivamente rechazada
    if (current.empty()) {
      if (trace) {
        std::cout << "[TRACE] Conjunto de estados vacío -> terminación temprana (Rejected)\n";
      }
      break;
    }
  }

  // 4) Aceptación: comprobamos si alguno de los estados finales pertenece a accepting_states
  const auto& accepting = automaton_.GetAcceptingStates();
  for (auto s : current) {
    if (accepting.find(s) != accepting.end()) {
      if (trace) {
        std::cout << "[TRACE] Estado de aceptación encontrado en " << s << " -> Accepted\n";
      }
      return true;
    }
  }
  if (trace) {
    std::cout << "[TRACE] Ningún estado de aceptación en el conjunto final -> Rejected\n";
  }
  return false;
}


---

Eso es todo: con estos cambios se activa la traza cuando ejecutas:

./p06_automata_simulator input.fa input.txt trace

Ejemplo de salida en modo traza (esquemático, para una entrada simple aa):

[TRACE] Inicial (epsilon-closure del start): {0}
[TRACE] --------------------------------------------------
[TRACE] Símbolo actual: 'a'
[TRACE] Estados actuales: {0}
[TRACE] Reglas (transiciones desde cada estado con 'a'):
  (0,a) -> {1}
  (0,&) -> {}
[TRACE] Destinos directos tras consumir 'a': {1}
[TRACE] Reglas (epsilon desde destinos directos):
  (1,&) -> {3}
[TRACE] Estados tras epsilon-closure: {1,3}
[TRACE] --------------------------------------------------
[TRACE] Símbolo actual: 'a'
[TRACE] Estados actuales: {1,3}
...
& --- Accepted


---

Si quieres, puedo:

Añadir una opción --trace además de la palabra literal trace (no sensible a mayúsculas).

Mejorar la salida de traza (colores ANSI, nivel de detalle configurables).

Incluir un pequeño ejemplo de ejecución real con tus input2_fixed.fa para ver la traza completa. ¿Quieres que lo haga?


