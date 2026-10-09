cmake_minimum_required(VERSION 3.10)
project(string_oscillation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

## Проверка наличия утилиты gnuplot.
find_program(GNUPLOT gnuplot)
if (NOT GNUPLOT)
  message(WARNING "gnuplot not found!")
endif()

## Исполняемый файл проекта.
add_executable(${PROJECT_NAME}
  src/oscillation.cpp
  src/string_oscillation_main.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE src)
