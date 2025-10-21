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
 * Archivo: fa_parser.h: interfaz del parser para ficheros .fa.
 *    Contiene la clase FAParser que se encarga de leer y validar la
 *    especificación del autómata desde un fichero .fa.
 * Referencias:
 *    Transparencias del Tema 2 de la asignatura: Autómatas finitos y lenguajes regulares
 *    https://campusvirtual.ull.es/2526/ingenieriaytecnologia/mod/resource/view.php?id=11856
 * Historial de revisiones
 *    19/10/2025 - Creación (primera versión) del código
 *    19/10/2025 - Documentación y comentarios
*/

/**
 * @file fa_parser.h
 * @brief Interfaz del parser para ficheros .fa (especificación de automáta).
 *
 * El parser se encarga de validar el fichero .fa y poblar un objeto Automaton.
 * Mantiene la validación centralizada, separando responsabilidades.
 */

#ifndef P06_PARSER_FA_PARSER_H_
#define P06_PARSER_FA_PARSER_H_

#include <string>

#include "automata.h"

namespace p06 {

/**
 * @brief Clase responsable de parsear y validar ficheros .fa.
 */
class FAParser {
 public:
  FAParser() = default;

  /**
   * @brief Parsear un fichero .fa y poblar el autómata pasado por referencia.
   * @param filename Ruta al fichero .fa
   * @param automaton Referencia a Automaton a poblar
   * @param err_msg En caso de error se escribe aquí una descripción
   * @return true si el parseo y la validación fueron correctos
   */
  bool ParseFile(const std::string& filename,
                 Automaton& automaton,
                 std::string& err_msg) const;
};

}

#endif  