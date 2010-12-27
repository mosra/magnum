cmake_minimum_required(VERSION 2.6)
project(Magnum)

option(BUILD_EXAMPLES "Build examples (requires GLUT)." OFF)
option(BUILD_TESTS "Build unit tests (requires Qt4)." OFF)

if(BUILD_TESTS)
    find_package(Qt4)

    if(NOT QT4_FOUND)
        message(WARNING "Qt4 is required for building unit tests. No tests will be build.")
        set(BUILD_TESTS OFF)
    else()
        enable_testing()
    endif()

endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${Magnum_SOURCE_DIR}/modules/")
include(MagnumMacros)

add_subdirectory(src)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

if(BUILD_EXAMPLES)
    find_package(GLUT)

    if(NOT GLUT_FOUND)
        message(WARNING "GLUT is required for building examples. No examples will be build.")
        set(BUILD_EXAMPLES OFF)
    else()
        add_subdirectory(examples)
    endif()

endif()
