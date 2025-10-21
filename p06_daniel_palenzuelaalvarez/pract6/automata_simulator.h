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
 * Archivo: automata_simulator.h: interfaz de la clase AutomatonSimulator.
 *    Contiene la definición de la clase AutomatonSimulator que representa un simulador de 
 * autómatas finitos no deterministas (NFA).
 * Referencias:
 *    Transparencias del Tema 2 de la asignatura: Autómatas finitos y lenguajes regulares
 *    https://campusvirtual.ull.es/2526/ingenieriaytecnologia/mod/resource/view.php?id=11856
 * Historial de revisiones
 *    19/10/2025 - Creación (primera versión) del código
 *    19/10/2025 - Documentación y comentarios
*/

/**
 * @file automata_simulator.h
 * @brief Interfaz del simulador (epsilon-closure y simulate).
 *
 * El simulador es responsable únicamente de ejecutar el autómata ya validado.
 */

#ifndef P06_SIMULATOR_AUTOMATON_SIMULATOR_H_
#define P06_SIMULATOR_AUTOMATON_SIMULATOR_H_

#include <string>

#include "automata.h"

namespace p06 {

/**
 * @brief Clase que simula un Automaton (NFA).
 *
 * Se construye a partir de una referencia constante a Automaton. Implementa
 * epsilon-closure y Simulate.
 */
class AutomatonSimulator {
 public:
  /**
   * @brief Construye el simulador sobre un autómata ya poblado y validado.
   * @param automaton Referencia al autómata a simular
   */
  explicit AutomatonSimulator(const Automaton& automaton);

  /**
   * @brief Calcula el epsilon-closure (cierre por &) de un conjunto de estados.
   * @param states Conjunto inicial de estados
   * @return Conjunto resultante tras aplicar cierre por epsilon (&)
   */
  Automaton::StateSet EpsilonClosure(const Automaton::StateSet& states) const;

  /**
   * @brief Simula la cadena dada sobre el autómata.
   * @param input Cadena de entrada (string vacío representa la cadena epsilon)
   * @return true si la cadena es aceptada, false si es rechazada
   *
   * Nota: si la cadena contiene símbolos que no pertenecen al alfabeto, se rechaza.
   */
  bool Simulate(const std::string& input) const;

 private:
  const Automaton& automaton_; // Referencia al autómata a simular
};

}

#endif