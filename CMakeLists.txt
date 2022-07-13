cmake_minimum_required(VERSION 3.8)
project(lanelet_tutorial)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Eigen3 REQUIRED)
include_directories(SYSTEM
  ${EIGEN3_INCLUDE_DIR}
)
find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()
ament_auto_add_executable(example_01 src/01.cpp)
ament_auto_add_executable(example_02 src/02.cpp)
ament_auto_add_executable(example_03 src/03.cpp)
ament_auto_add_executable(training src/training.cpp)

ament_auto_package()
