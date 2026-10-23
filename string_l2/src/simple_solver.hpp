/*! @file simple_solver.hpp
Заголовочный файл класса однопоточной реализации метода решения уравнения.
@author Козов А.В., Шам Д.А.
@date 2025.03.15 */

#ifndef SIMPLE_SOLVER_HPP
#define SIMPLE_SOLVER_HPP

#include <chrono>
#include <iostream>
#include <math.h>
#include <memory>

#include "abstract_solver.hpp"
#include "parameters.hpp"


namespace so {

  /*! Класс простой реализации метода решения уравнения.
  @details Класс реализует метод конечных разностей с явной вычислительной схемой для решения уравнения струны в одном
  потоке выполнения. */
  class SimpleSolver : public AbstractSolver {
  public:

    /*! Выполняет решение методом конечных разностей с явной вычислительной схемой в одном потоке выполнения.
    @warning Предполагается, что значения параметров моделирования корректные.
    @param p Параметры моделирования.
    @param o Объект для вывода решения в файл. */
    void solve(const Parameters& p, Plotter& o) override {
      init(p);
      std::cout << "  > Node number " << _size << ", time step number " << _time_steps_number << std::endl;
      auto begin_time = std::chrono::system_clock::now();

      calculateNodes(0ul, _size, p, o);

      auto duration = std::chrono::system_clock::now() - begin_time;
      std::cout << "  > Computation time " << std::chrono::duration<double>(duration).count() << " s" << std::endl;
    }

  protected:

    /*! Рассчитывает значение узла сетки с номером @a n.
    @param f Значение внешней силы в рассчитываемом узле.
    @param n Номер рассчитываемого узла.
    @return Значение узла. */
    inline double calculateNode(double f, unsigned n) const noexcept {
      const double inner_sum = _current.get()[n + 1] - 2. * _current.get()[n] + _current.get()[n - 1];
      const double time_multiplier = pow(_phase_velocity, 2.) * inner_sum / pow(_spatial_step, 2.);
      const double last_term = (time_multiplier + f) * pow(_time_step, 2.);
      const double result = 2. * _current.get()[n] - _previous.get()[n] + last_term;
      return result;
    }

    /*! Рассчитывает значения узлов сетки в заданном диапазоне.
    @param b Начальный узел диапазона.
    @param s Число узлов диапазона.
    @param p Параметры моделирования.
    @param o Объект для вывода решения в файл. */
    void calculateNodes(unsigned long b, unsigned long s, const Parameters& p, Plotter& o) noexcept {
      // Цикл по времени моделирования.
      unsigned long current_time_step = 1u;
      while (current_time_step <= _time_steps_number) {
        // Цикл по узлам сетки.
        for (unsigned long i = b; i < b + s; i++) {
          if (i == 0u) {
            _next.get()[i] = calculateLeftBoundary(p.conditions.left);
          } else if (i == _size - 1) {
            _next.get()[i] = calculateRightBoundary(p.conditions.right);
          } else {
            // Расчёт значений по явной вычислительной схеме.
            const double force = p.conditions.force(i * p.spatial_step, _time_step * current_time_step);
            _next.get()[i] = calculateNode(force, i);
          }
        }
        // Вывод решения.
        unsigned plot_step = 0.05 / _time_step; // для "реального" масштаба времени анимации
        if (!p.no_output && current_time_step % plot_step == 0) {
          o.plot(_next.get(), _size);
        }

        // Обмен указателями для повышения быстродействия.
        _current.swap(_next);  // в last теперь текущий слой, в current теперь слой j
        _previous.swap(_next); // в previous теперь слой j, в current теперь слой j-1

        // Следующий шаг по времени.
        current_time_step++;
      }
    }

    /*! Рассчитывает значение крайнего левого узла струны.
    @param b Граничные условия для левого конца струны.
    @return Значение крайнего левого узла струны. */
    inline double calculateLeftBoundary(Boundary b) {
      if (b.type == BoundaryType::first) {
        return b.value;
      } else {
        /// @todo Реализовать граничное условие второго рода.
        return b.value;
      }
    }

    /*! Рассчитывает значение крайнего правого узла струны.
    @param b Граничные условия для правого конца струны.
    @return Значение крайнего правого узла струны. */
    inline double calculateRightBoundary(Boundary b) {
      if (b.type == BoundaryType::first) {
        return b.value;
      } else {
        /// @todo Реализовать граничное условие второго рода.
        return b.value;
      }
    }

    /*! Инициализирует данные для моделирования.
    @param p Параметры моделирования. */
    inline void init(const Parameters& p) {
      _phase_velocity = phase_velocity;
      _time_step = 0.1 * p.spatial_step / _phase_velocity;
      _spatial_step = p.spatial_step;
      _time_steps_number = round(p.duration / _time_step);
      _size = round(string_length / p.spatial_step) + 1ul; // число узлов сетки
      _next = std::shared_ptr<double[]>(new double[_size]);
      for (unsigned long i = 0lu; i < _size; i++) {
        const double x = i * p.spatial_step;
        _next.get()[i] = p.conditions.begin(x); // начальные условия
      }
      _current = std::shared_ptr<double[]>(new double[_size]);
      _previous = std::shared_ptr<double[]>(new double[_size]);
      for (unsigned long i = 0lu; i < _size; i++) {
        _current.get()[i] = _next.get()[i];
        _previous.get()[i] = _next.get()[i];
      }
    }

    double _time_step;                   ///< Временной шаг, с.
    unsigned long _time_steps_number;    ///< Число временных шагов.
    unsigned long _size;                 ///< Число узлов сетки.
    double _spatial_step;                ///< Пространственный шаг, м.
    double _phase_velocity;              ///< Фазовая скорость, м/с.
    std::shared_ptr<double[]> _next;     ///< Решение для временного слоя j + 1 (текущий рассчитываемый).
    std::shared_ptr<double[]> _current;  ///< Решение для временного слоя j.
    std::shared_ptr<double[]> _previous; ///< Решение для временного слоя j - 1.
  };

} // so namespace

#endif // SIMPLE_SOLVER_HPP
