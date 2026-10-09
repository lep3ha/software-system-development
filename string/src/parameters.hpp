/*! @file parameters.hpp
Заголовочный файл структуры с параметрами моделирования.
@author Козов А.В.
@date 2025.03.09 */

#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP

#include <functional>
#include <string>


/// Пространство имён приложения моделирования колебаний струны.
namespace so {

  /// Глобальная константа с длиной струны, м.
  const double string_length = 1.;

  /// Глобальная константа с фазовой скоростью, м/с.
  const double phase_velocity = 1.;

  /// Типы граничных условий.
  enum class BoundaryType {
    first, ///< Первого рода.
    second ///< Второго рода.
  };

  /// Структура описания граничного условия.
  struct Boundary {
    BoundaryType type; ///< Тип.
    double value;      ///< Значение.
  };

  /*! Структура с начальным и граничными условиями моделирования. */
  struct Conditions {
    std::function<double(double, double)> force; ///< Внешняя сила p(x, t).
    std::function<double(double)> begin;         ///< Начальные условия.
    Boundary left;                               ///< Граничное условие для левого конца.
    Boundary right;                              ///< Граничное условие для правого конца.
  };

  /*! Служебная структура с параметрами моделирования. */
  struct Parameters {
    double duration;         ///< Продолжительность моделирования.
    double spatial_step;     ///< Пространственный шаг сетки.
    bool no_output;          ///< Флаг служебного режима без файлового вывода.
    std::string output_file; ///< Название файла для вывода результатов моделирования.
    Conditions conditions;   ///< Начальные и граничные условия.
  };

} // so namespace

#endif // PARAMETERS_HPP
