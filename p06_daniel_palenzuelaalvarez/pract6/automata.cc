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
 * Archivo: automata.cc: implementación de la clase Automaton.
 *    Contiene la definición de la clase Automaton que representa un autómata finito no determinista (NFA).
 * Referencias:
 *    Transparencias del Tema 2 de la asignatura: Autómatas finitos y lenguajes regulares
 *    https://campusvirtual.ull.es/2526/ingenieriaytecnologia/mod/resource/view.php?id=11856
 * Historial de revisiones
 *    19/10/2025 - Creación (primera versión) del código
 *    19/10/2025 - Documentación y comentarios
*/

/**
 * @file automata.cc
 * @brief Implementación de getters/setters pequeños de Automaton.
 *
 * Mantiene las operaciones sencillas para que el parser sea quien valide la
 * entrada en detalle. Los métodos aquí son cortos y responsables.
 */

#include "automata.h"

#include <utility>

namespace p06 {

// Constructor por defecto: autómata vacío
Automaton::Automaton()
    : num_states_(0), start_state_(0) {
}

// Borra todos los datos del autómata
void Automaton::Clear() {
  alphabet_.clear();
  num_states_ = 0;
  start_state_ = 0;
  accepting_states_.clear();
  transitions_.clear();
}

/**
 * @brief Añade un símbolo al alfabeto.
 *
 * @param symbol Símbolo a añadir (un char)
 * @return true si se añadió correctamente, false si fue inválido (por ejemplo &)
 */
bool Automaton::AddSymbol(Symbol symbol) {
  if (symbol == '&') {
    // & está reservado para epsilon, no se añade al alfabeto.
    return false;
  }
  // Añadir símbolo al alfabeto
  alphabet_.insert(symbol);
  return true;
}

/**
 * @brief Establece el número de estados del autómata.
 *
 * Validaciones:
 *  - num_states debe ser >= 1.
 *  - Si start_state_ estaba fuera de rango, se reajusta a 0.
 *
 * @return true en caso de éxito, false si num_states < 1.
 */
bool Automaton::SetNumStates(int num_states) {
  if (num_states < 1) return false;
  num_states_ = num_states;
  // Aseguramos start_state_ dentro de rango si ya estaba fijado
  if (start_state_ < 0 || start_state_ >= num_states_) start_state_ = 0;
  return true;
}

/**
 * @brief Fija el estado inicial.
 *
 * Comprueba que el estado esté dentro del rango si num_states_ ya fue fijado.
 * Si num_states_ == 0 (no establecido aún), permitimos fijar temporalmente
 * un start_state siempre que sea >= 0; el parser normalmente llamará a
 * SetNumStates antes de SetStartState.
 */
bool Automaton::SetStartState(State state) {
  if (state < 0 || (num_states_ != 0 && state >= num_states_)) return false;
  start_state_ = state;
  return true;
}

/**
 * @brief Marca un estado como de aceptación.
 */
bool Automaton::AddAcceptingState(State state) {
  if (state < 0 || (num_states_ != 0 && state >= num_states_)) return false;
  accepting_states_.insert(state);
  return true;
}

/**
 * @brief Añade una transición (desde -> símbolo -> a).
 *
 * Validaciones:
 *  - from y to deben ser >= 0
 *  - si num_states_ ya fue fijado, comprobamos que from y to estén en rango
 *  - permitimos símbolo == & incluso si & no está en el alfabeto
 */
bool Automaton::AddTransition(State from, Symbol symbol, State to) {
  if (from < 0 || to < 0) return false;
  if (num_states_ != 0) {
    if (from >= num_states_ || to >= num_states_) return false;
  }
  // Añadir la transición
  transitions_[from][symbol].insert(to);
  return true;
}

// Getters
/**
 * @brief GetNumStates: devuelve el número de estados.
 */
int Automaton::GetNumStates() const {
  return num_states_;
}

/**
 * @brief GetStartState: devuelve el estado inicial.
 */
Automaton::State Automaton::GetStartState() const {
  return start_state_;
}

/**
 * @brief Devuelve el conjunto de estados de aceptación.
 */
const Automaton::StateSet& Automaton::GetAcceptingStates() const {
  return accepting_states_;
}

/**
 * @brief Devuelve el alfabeto.
 */
const std::set<Automaton::Symbol>& Automaton::GetAlphabet() const {
  return alphabet_;
}

/**
 * @brief Comprueba si un estado existe en el autómata (básico).
 * @param state Estado a comprobar
 * @return true si num_states_ > 0 y que 0 <= state < num_states_
 */
bool Automaton::HasState(State state) const {
  if (state < 0) return false;
  if (num_states_ == 0) return false;
  return state < num_states_;
}

/**
 * @brief Comprueba si un símbolo pertenece al alfabeto.
 * @param symbol Símbolo a comprobar
 * @return true si el símbolo está en el alfabeto o es &
 */
bool Automaton::IsSymbolInAlphabet(Symbol symbol) const {
  if (symbol == '&') return true;  // permitimos comprobación externa para epsilon
  return alphabet_.find(symbol) != alphabet_.end();
}

/**
 * @brief Devuelve el mapa símbolo, conjunto de estados para un estado dado.
 *
 * Si el estado no tiene transiciones, devuelve un map vacío estático.
 */
const std::unordered_map<Automaton::Symbol, Automaton::StateSet>&
Automaton::GetTransitionsForState(State state) const {
  // Buscar las transiciones para el estado dado
  static const std::unordered_map<Symbol, StateSet> empty_map;
  auto it = transitions_.find(state);
  // Si no hay transiciones, devolver el map vacío
  if (it == transitions_.end()) return empty_map;
  return it->second;
}

}