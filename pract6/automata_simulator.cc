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
 * @file automaton_simulator.cc
 * @brief Implementación de EpsilonClosure y Simulate.
 *
 * Implementa la semántica de NFA usando las estructuras públicas del Automaton.
 */

#include "automata_simulator.h"

#include <queue>

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
bool AutomatonSimulator::Simulate(const std::string& input) const {
  // Inicializar conjunto de estados actuales con epsilon-closure del estado inicial
  Automaton::StateSet current;
  current.insert(automaton_.GetStartState());
  current = EpsilonClosure(current);

  // Si la entrada contiene símbolos fuera del alfabeto, rechazar
  for (char c : input) {
    if (!automaton_.IsSymbolInAlphabet(c)) {
      return false;
    }
  }

  // Procesar cada símbolo
  for (char c : input) {
    Automaton::StateSet next; // conjunto de estados siguientes
    // para cada estado actual, obtener transiciones con símbolo c
    for (auto s : current) {
      const auto& trans_map = automaton_.GetTransitionsForState(s);
      auto it_sym = trans_map.find(c);
      // Si no hay transiciones con el símbolo c, continuar
      if (it_sym == trans_map.end()) continue;
      // Añadir estados destino a next
      for (const auto& dest : it_sym->second) {
        next.insert(dest);
      }
    }
    // aplicar epsilon-closure a next
    current = EpsilonClosure(next);
    if (current.empty()) break;  // no quedan estados activos
  }

  // Comprobar si algún estado actual es de aceptación
  const auto& accepting = automaton_.GetAcceptingStates();
  for (auto s : current) {
    if (accepting.find(s) != accepting.end()) return true;
  }
  return false;
}

}