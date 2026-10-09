/*! @file plotter.hpp
Заголовочный файл класса формирования файла данных для gnuplot.
@author Козов А.В.
@date 2025.03.15 */

#ifndef PLOTTER_HPP
#define PLOTTER_HPP

#include <ostream>


namespace so {

  /*! Класс формирования файла данных для gnuplot.
  @details Класс содержит ссылку на поток вывода и метод для формирования блока данных для массива значений
  фазовой переменной (решение уравнения струны для одного временного шага). */
  class Plotter {
  public:

    /*! Создаёт объект вывода по ссылке на поток вывода.
    @param o Ссылка на поток вывода. */
    explicit Plotter(std::ostream& o) :
      _output(o) {};

    /*! Выводит значения из массива в виде пар "x u" в одном блоке данных gnuplot.
    @param u Выводимый массив значений.
    @param s Число узлов в массиве. */
    void plot(const double* u, unsigned long s) {
      const double step = string_length / (s - 1); // интервалов на 1 меньше числа узлов
      for (unsigned i = 0u; i < s; i++) {
        _output << '\n' << i * step << " " << u[i];
      }
      _output << "\n\n";
    }

  protected:

    std::ostream& _output; ///< Ссылка на поток вывода.
  };

} // so namespace

#endif // PLOTTER_HPP
