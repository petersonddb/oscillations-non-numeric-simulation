cmake_minimum_required(VERSION 3.14)
project(OscillationsSimulation)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)
find_package(nlohmann_json REQUIRED)

find_package(VTK REQUIRED 
  ChartsCore
  ChartsCore
  CommonColor
  CommonCore
  CommonDataModel
  InteractionStyle
  RenderingContext2D
  RenderingContextOpenGL2
  RenderingCore
  RenderingFreeType
  RenderingGL2PSOpenGL2
  RenderingOpenGL2
  ViewsContext2D
  FiltersSources
  InteractionStyle
)

add_library(oscillations_simulation oscillations.cpp)
target_include_directories(oscillations_simulation PUBLIC "${PROJECT_BINARY_DIR}" .)

add_executable(simulation main.cpp)
target_link_libraries(simulation PRIVATE oscillations_simulation nlohmann_json::nlohmann_json)

add_executable(plot plot.cpp)
target_link_libraries(plot PRIVATE ${VTK_LIBRARIES})

add_executable(animate animate.cpp)
target_link_libraries(animate PRIVATE ${VTK_LIBRARIES})

vtk_module_autoinit(
  TARGETS plot animate
  MODULES ${VTK_LIBRARIES}
)

enable_testing()

add_executable(test_program test_simulation.cpp test_oscillations.cpp)
target_link_libraries(test_program oscillations_simulation GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(test_program)
