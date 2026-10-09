/*! @file string_oscillation_main.cpp
Главный файл приложения моделирования колебаний (лабораторная работа №1 по курсу РПС).
@author Козов А.В.
@date 2025.02.15 */

#include <iostream>
#include <string>

#include "oscillation.hpp"
#include "parameter_parser.hpp"


using namespace so;

/*! Главная функция приложения.
@param argc @todo
@param argv @todo
@return @todo */
int main(int argc, char** argv) {
  try {
    ParameterParser parser(argc, argv);
    Parameters parameters = parser.getParameters();
    Oscillation osc;
    osc.simulate(parameters);
  } catch (const std::exception& e) {
    std::cerr << argv[0] << " - " << e.what() << std::endl;
    return 1;
  }
  std::cout << "Complete" << std::endl;
  return 0;
}
