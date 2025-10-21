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
 * Archivo: fa_parser.cc: implementación del parser para ficheros .fa.
 *    Contiene la implementación de la clase FAParser que se encarga de leer y validar la
 *    especificación del autómata desde un fichero .fa.
 * Referencias:
 *    Transparencias del Tema 2 de la asignatura: Autómatas finitos y lenguajes regulares
 *    https://campusvirtual.ull.es/2526/ingenieriaytecnologia/mod/resource/view.php?id=11856
 * Historial de revisiones
 *    19/10/2025 - Creación (primera versión) del código
 *    19/10/2025 - Documentación y comentarios
*/

/**
 * @file fa_parser.cc
 * @brief Implementación de carga y validación de ficheros .fa.
 *
 * El parser realiza validaciones estrictas del formato del fichero .fa y
 * de la coherencia de los datos para asegurar que el autómata resultante es válido
 */

#include "fa_parser.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace p06 {

/**
 * @brief Convierte un token (string) a char, solo si el token tiene longitud 1.
 *
 * Uso:
 *  -Los símbolos del alfabeto y los símbolos de transición deben ser tokens
 *    de 1 carácter.
 *
 * @param token cadena leída desde el fichero
 * @param out_char salida con el carácter si token.size() == 1
 * @return true si conversion válida y se escribió out_char
 */
static bool TokenToChar(const std::string& token, char& out_char) {
  if (token.size() != 1) return false;
  out_char = token[0];
  return true;
}

/**
 * @brief Parsear un fichero .fa y poblar el autómata pasado por referencia.
 * @param filename Ruta al fichero .fa
 * @param automaton Referencia a Automaton a poblar
 * @param err_msg En caso de error se escribe aquí una descripción
 * @return true si el parseo y la validación fueron correctos
 */
bool FAParser::ParseFile(const std::string& filename,
                         Automaton& automaton,
                         std::string& err_msg) const {
  // Abrimos fichero
  std::ifstream ifs(filename);
  if (!ifs) {
    err_msg = "No se puede abrir fichero: " + filename;
    return false;
  }

  // Limpiamos automaton antes de poblarlo
  automaton.Clear();

  std::string line;

  // Línea 1: alfabeto (símbolos separados por espacios)
  if (!std::getline(ifs, line)) {
    err_msg = "Fichero vacío o formato incorrecto (línea de alfabeto).";
    return false;
  }
  {
    // Tokenizamos la línea por espacios y comprobamos cada símbolo
    std::istringstream iss(line);
    std::string token;
    while (iss >> token) {
      char c;
      // Cada token del alfabeto debe ser exactamente 1 carácter
      if (!TokenToChar(token, c)) {
        err_msg = "Símbolo de alfabeto inválido (debe ser 1 carácter): '" + token + "'";
        return false;
      }
      // & está reservado y no puede ser parte del alfabeto
      if (c == '&') {
        err_msg = "El carácter '&' está reservado para epsilon y no puede formar parte del alfabeto.";
        return false;
      }
      // Añadimos símbolo al alfabeto del Automaton.
      if (!automaton.AddSymbol(c)) {
        // Error inesperado al añadir símbolo
        err_msg = "Error al añadir símbolo al alfabeto: " + std::string(1, c);
        return false;
      }
    }
  }

  // Línea 2: número total de estados
  // Leemos línea
  if (!std::getline(ifs, line)) {
    err_msg = "Formato incorrecto: falta línea con número de estados.";
    return false;
  }
  int num_states = 0;
  {
    // Parseamos número de estados
    std::istringstream iss(line);
    if (!(iss >> num_states) || num_states < 1) {
      err_msg = "Número de estados inválido o menor que 1.";
      return false;
    }
    // SetNumStates aplica una validación mínima y ajusta el autómata
    if (!automaton.SetNumStates(num_states)) {
      err_msg = "No se pudo fijar el número de estados.";
      return false;
    }
  }

  // Línea 3: estado de arranque
  // Leemos línea
  if (!std::getline(ifs, line)) {
    err_msg = "Formato incorrecto: falta línea con estado inicial.";
    return false;
  }
  {
    // Parseamos estado inicial
    std::istringstream iss(line);
    int start;
    if (!(iss >> start)) {
      err_msg = "Estado inicial inválido.";
      return false;
    }
    // Comprobamos rango de estado inicial
    if (start < 0 || start >= num_states) {
      err_msg = "Estado inicial fuera de rango: " + std::to_string(start);
      return false;
    }
    // Fijamos estado inicial
    if (!automaton.SetStartState(start)) {
      err_msg = "Error al fijar estado inicial.";
      return false;
    }
  }

  // Una línea por cada estado (num_states líneas)
  // Formato por línea: "id accept num_trans symbol1 dest1 symbol2 dest2 ..."
  for (int i = 0; i < num_states; ++i) {
    // Leemos línea
    if (!std::getline(ifs, line)) {
      err_msg = "Faltan líneas para la definición de los estados. Se esperaban " +
                std::to_string(num_states) + " líneas (una por estado).";
      return false;
    }
    // Tokenizamos la línea por espacios
    std::istringstream iss(line);
    int state_id;
    int is_accept;
    int n_trans;
    // Leemos id de estado, aceptación y número de transiciones
    if (!(iss >> state_id >> is_accept >> n_trans)) {
      err_msg = "Formato incorrecto en la línea de estado (id accept num_trans). Línea: " + line;
      return false;
    }
    // Validamos datos básicos del estado
    if (state_id < 0 || state_id >= num_states) {
      err_msg = "Identificador de estado fuera de rango: " + std::to_string(state_id);
      return false;
    }
    // Validamos campo de aceptación
    if (is_accept != 0 && is_accept != 1) {
      err_msg = "Campo de aceptación debe ser 0 o 1. Línea: " + line;
      return false;
    }
    // Validamos número de transiciones
    if (n_trans < 0) {
      err_msg = "Número de transiciones negativo en línea: " + line;
      return false;
    }
    // Si es estado de aceptación, lo marcamos
    if (is_accept == 1) {
      if (!automaton.AddAcceptingState(state_id)) {
        err_msg = "Error marcando estado de aceptación: " + std::to_string(state_id);
        return false;
      }
    }

    // Leer transiciones: cada transición es "symbol dest"
    // Ejemplo: "0 1" significa con símbolo '0' va al estado 1
    for (int t = 0; t < n_trans; ++t) {
      std::string sym_token;
      int dest;
      // Leemos símbolo y estado destino
      if (!(iss >> sym_token >> dest)) {
        err_msg = "Faltan datos en transiciones para el estado " + std::to_string(state_id) +
                  ". Línea: " + line;
        return false;
      }
      char c;
      // Validamos símbolo de transición
      if (!TokenToChar(sym_token, c)) {
        err_msg = "Símbolo de transición inválido (debe ser 1 carácter): '" + sym_token + "'";
        return false;
      }
      // Si no es epsilon, debe pertenecer al alfabeto
      if (c != '&' && !automaton.IsSymbolInAlphabet(c)) {
        err_msg = "Símbolo de transición '" + std::string(1, c) +
                  "' no pertenece al alfabeto.";
        return false;
      }
      // Validamos estado destino
      if (dest < 0 || dest >= num_states) {
        err_msg = "Estado destino fuera de rango en transición desde " +
                  std::to_string(state_id) + ". Destino: " + std::to_string(dest);
        return false;
      }
      // Añadimos transición al autómata
      if (!automaton.AddTransition(state_id, c, dest)) {
        // Error inesperado al añadir transición
        err_msg = "Error añadiendo transición: " + std::to_string(state_id) +
                  " -" + std::string(1, c) + "-> " + std::to_string(dest);
        return false;
      }
    }
  }

  // Si llegamos aquí, parseo correcto
  return true;
}

}
