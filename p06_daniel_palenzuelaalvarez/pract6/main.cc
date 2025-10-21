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
 * Archivo: main.cc: programa principal.
 *    Contiene la clase main que gestiona la ejecución del programa.
 *    Se encarga de usar FAParser, Automaton y AutomatonSimulator para cargar un autómata
 *    desde un fichero y simularlo con varias cadenas de entrada.
 * Referencias:
 *    Transparencias del Tema 2 de la asignatura: Autómatas finitos y lenguajes regulares
 *    https://campusvirtual.ull.es/2526/ingenieriaytecnologia/mod/resource/view.php?id=11856
 * Historial de revisiones
 *    19/10/2025 - Creación (primera versión) del código
 *    19/10/2025 - Documentación y comentarios
*/

/**
 * @file main.cc
 * @brief Programa principal: usa FAParser, Automaton y AutomatonSimulator.
 *
 * Uso:
 *  ./p06_automata_simulator input.fa input.txt
 *
 * Si se ejecuta sin argumentos, muestra un mensaje de uso.
 */

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "automata_simulator.h"
#include "fa_parser.h"

/**
 * @brief Imprime una línea corta de uso cuando faltan argumentos.
 *
 * Se usa cuando el usuario ejecuta el programa sin la sintaxis correcta.
 */
static void PrintUsage() {
  std::cout << "Modo de empleo: ./p06_automata_simulator input.fa input.txt\n"
            << "Pruebe 'p06_automata_simulator --help' para más información.\n";
}

/**
 * @brief Imprime la ayuda extendida (--help).
 *
 * Describe brevemente el uso y los formatos esperados de los ficheros.
 */
static void PrintHelp() {
  std::cout << "p06_automata_simulator - Simulador de autómatas finitos (NFA)\n\n"
            << "Uso:\n"
            << "  ./p06_automata_simulator input.fa input.txt\n\n"
            << "Formato de input.fa: ver especificación de la práctica.\n"
            << "Formato del fichero.txt: una cadena por línea. Usar & para la cadena vacía.\n";
}

/**
 * @brief Elimina espacios en ambos extremos de una cadena (trim).
 *
 * Devuelve una copia recortada de s. Usado para normalizar líneas leídas
 * desde ficheros (quitamos \r\n y espacios/tabs alrededor).
 */
static inline std::string Trim(const std::string& s) {
  size_t a = s.find_first_not_of(" \t\r\n");
  if (a == std::string::npos) return "";
  size_t b = s.find_last_not_of(" \t\r\n");
  return s.substr(a, b - a + 1);
}

/**
 * @brief Parsea una línea del fichero de cadenas (input.txt).
 *
 * El fichero de cadenas admite dos formatos (N <cadena> o <cadena>).
 *
 * @param line Línea original leída desde el fichero (puede contener espacios)
 * @param original Salida: la línea original trimmed (para imprimirla luego tal cual)
 * @param tokenized_input Salida: la cadena tokenizada que debe simularse ("" para cadena vacía)
 */
static void ParseInputLine(const std::string& line,
                           std::string& original,
                           std::string& tokenized_input) {
  // Normalizamos la entrada quitando espacios extremos
  original = Trim(line);
  if (original.empty()) {
    // Si la línea queda vacía tras trim la interpretamos como cadena vacía
    tokenized_input.clear();
    return;
  }

  // Separamos el primer token para detectar el posible prefijo numérico
  std::istringstream iss(original);
  std::string first;
  if (!(iss >> first)) {
    // No se pudo extraer ningún token
    tokenized_input.clear();
    return;
  }

  // Determinamos si el primer token es un número
  bool first_is_number = !first.empty() &&
                         std::all_of(first.begin(), first.end(), ::isdigit);
  if (first_is_number) {
    // Si el primer token es número, intentamos obtener el resto de la línea
    std::string rest;
    std::getline(iss, rest); // lee el resto de la línea tal cual
    rest = Trim(rest);
    if (!rest.empty()) {
      // Formato "N <cadena>" entonces usamos el resto como cadena
      if (rest == "&") tokenized_input.clear(); // & representa cadena vacía
      else tokenized_input = rest;
    } else {
      // La línea es solo numérica, la tratamos como cadena literal
      if (original == "&") tokenized_input.clear();
      else tokenized_input = original;
    }
  } else {
    // El primer token no es un número: la línea completa (original) es la cadena.
    if (original == "&") tokenized_input.clear();
    else tokenized_input = original;
  }
}

/**
 * @brief main: organiza la ejecución completa.
 *
 * Pasos principales:
 *  -Comprueba argumentos y muestra ayuda si procede.
 *  -Usa FAParser para leer y validar el autómata desde input.fa.
 *  -Crea AutomatonSimulator con el autómata validado.
 *  -Lee el fichero input.txt línea a línea, parsea cada cadena y la simula.
 *  -Imprime por pantalla la línea original y el resultado Accepted/Rejected.
 */
int main(int argc, char* argv[]) {
  // Comprobación de argumentos
  if (argc == 1) {
    PrintUsage();
    return 1;
  }
  if (argc == 2) {
    std::string arg = argv[1]; // el programa debe recibir 2 argumentos
    if (arg == "--help" || arg == "-h") {
      PrintHelp();
      return 0;
    }
    PrintUsage();
    return 1;
  }
  if (argc != 3) {
    PrintUsage();
    return 1;
  }

  // Guardamos las rutas de ficheros recibidas por línea de comandos
  std::string fa_file = argv[1];
  std::string txt_file = argv[2];

  // Creamos las estructuras principales, el autómata y el parser
  p06::Automaton automaton;
  p06::FAParser parser;
  std::string err;

  // Parseo y validación del fichero .fa
  if (!parser.ParseFile(fa_file, automaton, err)) {
    std::cerr << "Error al crear el autómata: " << err << "\n";
    // Salimos con código de error distinto de 0 para indicar fallo en la carga
    return 2;
  }

  // Creamos el simulador con el autómata ya validado
  p06::AutomatonSimulator simulator(automaton);

  // Abrimos el fichero de cadenas (input.txt)
  std::ifstream ifs(txt_file);
  if (!ifs) {
    std::cerr << "No se puede abrir fichero de cadenas: " << txt_file << "\n";
    return 3;
  }

  // Leemos línea a línea, parseamos y simulamos cada cadena
  std::string line;
  while (std::getline(ifs, line)) {
    // original guarda la línea tal cual para imprimirla idéntica
    std::string original, input;
    ParseInputLine(line, original, input);
    // Simulamos la cadena
    bool accepted = simulator.Simulate(input);
    // Salida es "<línea original> --- Accepted/Rejected"
    std::cout << original << " --- " << (accepted ? "Accepted" : "Rejected") << "\n";
  }

  return 0;
}