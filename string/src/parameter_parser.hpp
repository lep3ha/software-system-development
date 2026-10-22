/*! @file parameter_parser.hpp
Заголовочный файл класса разбора параметров моделирования.
@author Козов А.В.
@date 2025.03.15 */

#ifndef PARAMETER_PARSER_HPP
#define PARAMETER_PARSER_HPP

#include <cstdlib>
#include <math.h>
#include <stdexcept>
#include <string>
#include <unistd.h>

#include "parameters.hpp"


namespace so {

  /// Функция для получения нулевых начальных условий.
  double zeroBeginCondition(double) {
    return 0.;
  }

  /*! Функция для получения ненулевых начальных условий.
  @param x @todo
  @return @todo */
  double nonZeroBeginCondition(double x) {
    double u = 0.1 * x;
    if (x >= string_length / 2.) {
      u = -u + 0.1;
    }
    return u;
  }

  /// Функция "нулевой" внешней силы.
  double zeroForce(double, double) {
    return 0.;
  }

  /*! Функция внешней силы, действующей по гармоническому закону.
  @param x @todo
  @param t @todo
  @return @todo */
  double harmonicForce(double x, double t) {
    double f = 0.;
    if (abs(x - string_length / 2.) < 0.05) {
      f = sin(2. * M_PI * t);
    }
    return f;
  }

  /*! Класс для разбора аргументов запуска программы.
  @details Класс содержит число аргументов запуска и их строковые значения. Основной метод класса @p getParameters
  выполняет разбор аргументов при помощи функции getopt и формирование параметров моделирования в виде объекта
  класса @p Parameters. */
  class ParameterParser {
  public:

    /*! Создаёт объект-анализатор для заданных аргументов запуска программы.
    @param c Число аргументов запуска программы.
    @param v Массив строковых значений аргументов запуска программы. */
    ParameterParser(int c, char** v) :
      _argument_count(c),
      _argument_strings(v) {}

    /*! Разбирает аргументы, переданные при создании, для определения параметров моделирования.
    @throws std::invalid_argument при некорректных значениях параметров.
    @return Структура параметров моделирования. */
    Parameters getParameters() const {
      // Значения по умолчанию.
      double duration = 1.;
      double spatial_step = 0.05;
      bool service_mode = false;
      std::string file_name = "data.txt";
      // Разбор аргументов при помощи getopt.
      int option = 0;
      while ((option = getopt(_argument_count, _argument_strings, "t:x:o:s")) != -1) {
        switch (option) {
        case 't':
          duration = atof(optarg);
          if (duration <= 0.) {
            throw std::invalid_argument("Parameters: duration must be positive");
          }
          break;
        case 'x':
          spatial_step = atof(optarg);
          if (spatial_step <= 0.) {
            throw std::invalid_argument("Parameters: spatial step must be positive");
          }
          break;
        case 'o':
          file_name = optarg;
          break;
        case 's':
          service_mode = true;
          break;
        default:
          break;
        }
      }
      // Создание объекта параметров.
      Parameters parameters;
      // Заполнение полей структуры значениями параметров.
      parameters.output_file = file_name;
      parameters.duration = duration;
      parameters.spatial_step = spatial_step;
      parameters.no_output = service_mode;
      // Начальные и граничные условия.
      parameters.conditions.force = zeroForce;             // harmonicForce
      parameters.conditions.begin = nonZeroBeginCondition; // zeroBeginCondition
      parameters.conditions.left = Boundary{ BoundaryType::first, 0. };
      parameters.conditions.right = Boundary{ BoundaryType::first, 0.01 };
      return parameters;
    }

  private:

    const int _argument_count; ///< Число аргументов запуска программы.
    char** _argument_strings;  ///< Массив строковых значений аргументов запуска программы.
  };

} // namespace so

#endif // PARAMETER_PARSER_HPP
