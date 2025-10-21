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
 * Archivo: automata.h: implementación de la clase Automaton.
 *    Contiene la definición de la clase Automaton que representa un autómata finito no determinista (NFA).
 * Referencias:
 *    Transparencias del Tema 2 de la asignatura: Autómatas finitos y lenguajes regulares
 *    https://campusvirtual.ull.es/2526/ingenieriaytecnologia/mod/resource/view.php?id=11856
 * Historial de revisiones
 *    19/10/2025 - Creación (primera versión) del código
 *    19/10/2025 - Documentación y comentarios
*/

/**
 * @file automata.h
 * @brief Interfaz y datos de la clase Automaton.
 *
 * Esta clase representa un autómata finito no determinista (NFA) con operaciones
 * básicas para construirlo y consultarlo. La validación detallada de la
 * entrada se deja al parser.
 * 
 */

#ifndef P06_AUTOMATON_AUTOMATON_H_
#define P06_AUTOMATON_AUTOMATON_H_

#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace p06 {

/**
 * @brief Clase que representa un autómata finito no determinista (NFA).
 *
 * Tipos públicos:
 *  - State: tipo para identificadores de estado (int)
 *  - Symbol: tipo para símbolos de entrada (char)
 *  - StateSet: conjunto de estados (unordered_set<State>)
 */
class Automaton {
 public:
  using State = int; // Tipo para identificadores de estado
  using Symbol = char; // Tipo para símbolos de entrada
  using StateSet = std::unordered_set<State>; // Conjunto de estados

  /**
   * @brief Construye un autómata vacío.
   */
  Automaton();

  /**
   * @brief Borra todos los datos del autómata (vuelve al estado inicial vacío).
   */
  void Clear();

  /**
   * @name Setters (simples)
   * Los setters validan lo mínimo (por ejemplo rango de estados) y devuelven
   * true/false según éxito. El parser es responsable de construir el autómata.
   */
  bool AddSymbol(Symbol symbol); // Añade símbolo al alfabeto (no añade '&')
  bool SetNumStates(int num_states); // Establece número de estados (>= 1)
  bool SetStartState(State state); // Establece estado inicial
  bool AddAcceptingState(State state); // Añade estado de aceptación
  bool AddTransition(State from, Symbol symbol, State to); // Añade transición

  /**
   * @name Getters
   */
  int GetNumStates() const; // Devuelve número de estados
  State GetStartState() const; // Devuelve estado inicial
  const StateSet& GetAcceptingStates() const; // Devuelve estados de aceptación
  const std::set<Symbol>& GetAlphabet() const; // Devuelve el alfabeto
  bool HasState(State state) const; // true si estado está en rango
  bool IsSymbolInAlphabet(Symbol symbol) const;  // true si símbolo está en alfabeto
  /**
   * @brief Devuelve el mapa símbolo, conjunto de estados para un estado dado.
   * @note Si el estado no tiene transiciones, devuelve un map vacío estático.
   */
  const std::unordered_map<Symbol, StateSet>& GetTransitionsForState(State state) const;

 private:
  // Atributos privados
  std::set<Symbol> alphabet_; // Alfabeto del autómata
  int num_states_; // Número de estados
  State start_state_; // Estado inicial
  StateSet accepting_states_; // Conjunto de estados de aceptación

  // transitions_[q][a] = conjunto de estados destino desde q con símbolo a
  // Ejemplo: transitions_[0]['1'] = {1, 2}
  std::unordered_map<State, std::unordered_map<Symbol, StateSet>> transitions_;
};

}

#endif
