project(MPAS-Model_tests LANGUAGES Fortran C CXX)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(add_pfunit)
include(add_unity)
enable_testing()

find_package(MPAS REQUIRED)

add_subdirectory(test)
