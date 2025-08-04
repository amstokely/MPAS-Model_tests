cmake_minimum_required(VERSION 3.20)
project(MPAS-Model_tests LANGUAGES Fortran C CXX)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(add_pfunit)
include(add_unity)
enable_testing()

set(MPAS_DIR CACHE PATH "Path to the MPAS model source directory")
find_package(MPAS REQUIRED)
find_package(MPI REQUIRED COMPONENTS Fortran C)
option(MPAS_ENABLE_SYSTEM_TESTS "Enable system tests" OFF)

add_subdirectory(test)
