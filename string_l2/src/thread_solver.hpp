/*! @file thread_solver.hpp
Заголовочный файл класса многопоточной реализации метода решения уравнения.
@author Козов А.В.
@date 2025.03.15 */

#ifndef THREAD_SOLVER_HPP
#define THREAD_SOLVER_HPP

#include <chrono>
#include <cmath>
#include <condition_variable>
#include <memory>
#include <thread>
#include <vector>

#include <iostream> // для отладки

#include "abstract_solver.hpp"
#include "parameters.hpp"


namespace so {

  /*! Класс многопоточной реализации метода решения уравнения.
  @details Класс реализует метод конечных разностей с явной вычислительной схемой для решения уравнения струны с
  заданным числом потоков выполнения с их синхронизацией. */
  class ThreadSolver : public AbstractSolver {
  public:

    /*! Выполняет решение методом конечных разностей с явной вычислительной схемой с заданным числом потоков выполнения.
    @warning Предполагается, что значения параметров моделирования корректные.
    @param p Параметры моделирования.
    @param o Объект для вывода решения в файл. */
    void solve(const Parameters& p, Plotter& o) override {
      init(p);
      std::vector<std::thread> threads(_threads_number - 1u);          // рабочие потоки
      const unsigned long per_thread = round(_size / _threads_number); // число узлов на один поток

      std::cout << "  > Nodes number " << _size << ", nodes per thread " << per_thread << ", time steps number "
                << _time_steps_number << std::endl;
      auto begin_time = std::chrono::system_clock::now();

      _ready_thread_counter = 0ul;
      _current_time_step = 1ul;
      // Порождение рабочих потоков.
      for (unsigned i = 0u; i < threads.size(); i++) {
        const unsigned long begin_node = i * per_thread;
        auto work_function = std::bind(&ThreadSolver::calculateNodes, this, begin_node, per_thread, p, o);
        threads[i] = std::thread(work_function);
      }
      // Вычисления узлов в основном потоке до конца струны.
      const unsigned long main_thread_begin_node = per_thread * threads.size();
      calculateNodes(main_thread_begin_node, _size - main_thread_begin_node, p, o);
      // Проверка завершения работы рабочих потоков.
      for (auto& t : threads) {
        t.join();
      }
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
      while (_current_time_step <= _time_steps_number) {
        // Цикл по узлам сетки.
        for (unsigned long i = b; i < b + s; i++) {
          if (i == 0u) {
            _next.get()[i] = calculateLeftBoundary(p.conditions.left);
          } else if (i == _size - 1) {
            _next.get()[i] = calculateRightBoundary(p.conditions.right);
          } else {
            // Расчёт значений по явной вычислительной схеме.
            const double force = p.conditions.force(i * p.spatial_step, _time_step * _current_time_step);
            _next.get()[i] = calculateNode(force, i);
          }
        }
        unsigned long thread_time_step = _current_time_step; // номер временного шага для проверки в условной переменной
        std::unique_lock<std::mutex> lock(_mutex);           // начало критической секции
        _ready_thread_counter++;
        if (_ready_thread_counter == _threads_number) {
          // Сюда попадёт только один поток, дошедший до этой точки последним.
          _ready_thread_counter = 0ul;
          // Для "реального" масштаба времени анимации выводится не более 20 кадров на 1 секунду модельного времени.
          unsigned long plot_step = 0.05 / _time_step;
          if (!p.no_output && _current_time_step % plot_step == 0) {
            o.plot(_next.get(), _size);
          }
          /// @todo Реализовать критическую секцию кода.
          
        }
      }
    }

    /*! Проверяет номер шага по времени на неравенство значению из аргумента (для условной переменной).
    @param l Сравниваемое значение.
    @return Истина, если номер шага по времени увеличился. */
    bool checkTimeStep(unsigned long l) const noexcept {
      return _current_time_step != l;
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
      _threads_number = 4;
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

    double _time_step;                        ///< Временной шаг, с.
    unsigned long _time_steps_number;         ///< Число временных шагов.
    unsigned long _current_time_step;         ///< Номер текущего временного шага для синхронизации потоков.
    unsigned long _ready_thread_counter;      ///< Число потоков, выполнивших расчёт текущего временного шага.
    unsigned _threads_number;                 ///< Число потоков выполнения.
    unsigned long _size;                      ///< Число узлов сетки.
    double _spatial_step;                     ///< Пространственный шаг, м.
    double _phase_velocity;                   ///< Фазовая скорость, м/с.
    std::mutex _mutex;                        ///< Взаимоисключающая блокировка критической секции.
    std::condition_variable _synchronization; ///< Условная переменная синхронизации потоков выполнения.
    std::shared_ptr<double[]> _next;          ///< Решение для временного слоя j + 1 (новый рассчитываемый).
    std::shared_ptr<double[]> _current;       ///< Решение для временного слоя j.
    std::shared_ptr<double[]> _previous;      ///< Решение для временного слоя j - 1.
  };

} // so namespace

#endif // THREAD_SOLVER_HPP
