/*! @file oscillation.hpp
Заголовочный файл основного класса приложения для моделирования колебаний.
@author Козов А.В., Шам Д.А.
@date 2025.03.09 */

#ifndef OSCILLATION_HPP
#define OSCILLATION_HPP

#include <fstream>
#include <memory>
#include <stdexcept>

#include "abstract_solver.hpp"
#include "parameters.hpp"
#include "plotter.hpp"


namespace so {

  /*! Основной класс приложения для моделирования колебаний.
  @details Класс содержит умный указатель на реализацию метода решения уравнения и основной метод
  @p Oscillation::simulate для моделирования колебаний струны. */
  class Oscillation {
  public:

    /*! Выполняет моделирование с заданными параметрами.
    @warning Предполагается, что значения параметров моделирования корректные.
    @param p Параметры моделирования. */
    void simulate(const Parameters& p);

  protected:

    std::shared_ptr<AbstractSolver> _solver; ///< Умный указатель на реализацию метода решения.
  };

} // namespace so

#endif // OSCILLATION_HPP
