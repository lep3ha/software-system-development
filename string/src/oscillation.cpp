/*! @file oscillation.cpp
Файл исходного кода основного класса моделирования колебаний.
@details Подключения заголовочных файлов для разных реализаций методов именно здесь, чтобы обеспечить независимую
сборку файла основной функции (которая в таком случает "знает" только базовый абстрактный класс).
@author Козов А.В.
@date 2025.03.09 */

#include "oscillation.hpp"
//#include "thread_solver.hpp"
#include "simple_solver.hpp"


namespace so {

// Основной метод для моделирования.
void Oscillation::simulate(const Parameters& p) {
  _solver = std::make_shared<SimpleSolver>();
  std::ofstream output;
  if (!p.no_output) {
    output.open(p.output_file);
    if (!output.is_open()) {
      throw std::runtime_error("Oscillation: output file isn't open");
    }
  }
  Plotter plotter(output);
  _solver->solve(p, plotter); // вызов виртуальной функции через указатель на базовый класс
}

} // namespace so
