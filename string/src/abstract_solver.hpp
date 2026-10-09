/*! @file abstract_solver.hpp
Заголовочный файл абстрактного класса реализации метода решения уравнения.
@author Козов А.В.
@date 2025.03.15 */

#ifndef ABSTRACT_SOLVER_HPP
#define ABSTRACT_SOLVER_HPP

#include <memory>

#include "parameters.hpp"
#include "plotter.hpp"


namespace so {

  /*! Абстрактный класс реализации метода решения уравнения.
  @details Класс определяет метод solve для нахождения решения уравнения струны и вывода результата. */
  class AbstractSolver {
  public:

    /*! Выполняет решение уравнения струны с выводом результата.
    @param p Параметры моделирования.
    @param o Объект для вывода решения в файл. */
    virtual void solve(const Parameters& p, Plotter& o) = 0;

    /*! Освобождает память. */
    virtual ~AbstractSolver() = default;
  };

} // namespace so

#endif // ABSTRACT_SOLVER_HPP
