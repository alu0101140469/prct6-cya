/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Computabilidad y Algoritmia
 * Curso: 2º
 * Práctica 6: Diseño e implementación de un simulador de autómatas finitos.
 * Autor: Daniel Palenzuela Álvarez
 * Correo: alu0101140469@ull.edu.es
 * Fecha: 20/10/2025
 * Archivo: automata_simulator.cc: implementación de la clase AutomatonSimulator.
 *    Contiene la definición de la clase AutomatonSimulator que representa un 
 * simulador de autómatas finitos no deterministas (NFA).
 * Referencias:
 *    Transparencias del Tema 2 de la asignatura: Autómatas finitos y lenguajes regulares
 *    https://campusvirtual.ull.es/2526/ingenieriaytecnologia/mod/resource/view.php?id=11856
 * Historial de revisiones
 *    19/10/2025 - Creación (primera versión) del código
 *    19/10/2025 - Documentación y comentarios
*/

/**
 * @file automata_simulator.cc
 * @brief Implementación de EpsilonClosure y Simulate.
 *
 * Implementa la semántica de NFA usando las estructuras públicas del Automaton.
 */

#include "automata_simulator.h"

#include <queue>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>

namespace p06 {

  /**
  * @brief Constructor: guarda referencia al autómata.
  */
  AutomatonSimulator::AutomatonSimulator(const Automaton& automaton)
      : automaton_(automaton) {
  }

  /**
  * @brief Calcula el epsilon-closure (cierre por &) de un conjunto de estados.
  * @param states Conjunto inicial de estados
  * @return Conjunto resultante tras aplicar cierre por epsilon (&)
  */
  Automaton::StateSet AutomatonSimulator::EpsilonClosure(
      const Automaton::StateSet& states) const {
    // Copiamos los estados iniciales al closure
    Automaton::StateSet closure = states;
    // Cola para procesar estados
    std::queue<Automaton::State> q;
    // Inicializar cola con estados dados
    for (auto s : states) q.push(s);

    // Procesar la cola
    while (!q.empty()) {
      // cur es el estado actual a procesar (current)
      Automaton::State cur = q.front();
      q.pop();

      // Obtener transiciones epsilon desde cur
      const auto& trans_map = automaton_.GetTransitionsForState(cur);
      // Buscar las transiciones con símbolo &
      auto it_eps = trans_map.find('&');
      // Si no hay transiciones epsilon, continuar
      if (it_eps == trans_map.end()) continue;
      // Añadir estados destino al closure si no están ya
      for (const auto& dest : it_eps->second) {
        if (closure.insert(dest).second) {
          q.push(dest);
        }
      }
    }
    // Devolver el conjunto closure
    return closure;
  }

  /**
  * @brief Simula la cadena sobre el autómata.
  *
  * @param input Cadena de entrada (string vacío representa la cadena epsilon)
  * @return true si la cadena es aceptada, false si es rechazada
  */
  // Modif
  bool AutomatonSimulator::Simulate(const std::string& input, bool trace) const {
    // imprimir un conjunto en formato {a,b,c}
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

    // Inicializar conjunto de estados actuales con epsilon-closure del estado inicial
    Automaton::StateSet current;
    current.insert(automaton_.GetStartState());
    current = EpsilonClosure(current);

    if (trace) {
      std::cout << "Estado inicial: " << PrintSet(current) << "\n";
    }

    // Si la entrada contiene símbolos fuera del alfabeto, rechazar
    for (char c : input) {
      if (!automaton_.IsSymbolInAlphabet(c)) {
        if (trace) {
          std::cout << "Símbolo no válido encontrado en la entrada: '" << c << "' -> Rejected\n";
        }
        return false;
      }
    }

    // Procesar cada símbolo
    for (char c : input) {
      if (trace) {
        std::cout << "--------------------------------------------------\n";
        std::cout << "Símbolo actual: '" << c << "'\n";
        std::cout << "Estados actuales: " << PrintSet(current) << "\n";
        std::cout << "Reglas (transiciones desde cada estado con '" << c << "'):\n";
      }

      Automaton::StateSet next;  // conjunto temporal de estados alcanzables con c

      // Para cada estado activo actualmente, añadimos sus destinos por c
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

      // Mostrar si desde los destinos hay & transiciones
      if (trace) {
        std::cout << "Destinos directos tras consumir '" << c << "': " << PrintSet(next) << "\n";
        std::cout << "Reglas (epsilon desde destinos directos):\n";
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

      // Aplicamos epsilon-closure al conjunto next (transiciones epsilon posteriores)
      current = EpsilonClosure(next);

      if (trace) {
        std::cout << "Estados tras epsilon-clausura: " << PrintSet(current) << "\n";
      }

      // Si no quedan estados activos, la cadena es definitivamente rechazada
      if (current.empty()) {
        if (trace) {
          std::cout << "Conjunto de estados vacío por lo que rechazado\n";
        }
        break;
      }
    }

    // Aceptación: comprobamos si alguno de los estados finales pertenece a accepting_states
    const auto& accepting = automaton_.GetAcceptingStates();
    for (auto s : current) {
      if (accepting.find(s) != accepting.end()) {
        if (trace) {
          std::cout << "Estado de aceptación encontrado en " << s << " -> Accepted\n";
        }
        return true;
      }
    }
    if (trace) {
      std::cout << "Ningún estado de aceptación en el conjunto final -> Rejected\n";
    }
    return false;
  }

}