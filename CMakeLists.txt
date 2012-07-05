# CMake 2.8.8 required for OBJECT library target
cmake_minimum_required(VERSION 2.8.8)
project(Magnum)

option(TARGET_GLES "Build for OpenGL ES 2 instead of desktop OpenGL" OFF)
option(GCC45_COMPATIBILITY "Enable compatibility mode for GCC 4.5 (might disable some features)" OFF)

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

# Populate MAGNUM_*_INSTALL_DIR variables, check for dependencies
set(MAGNUM_INCLUDE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(MAGNUM_LIBRARY      Magnum)
include(FindMagnum)

add_subdirectory(modules)
add_subdirectory(src)

install(DIRECTORY src/ DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}
                 FILES_MATCHING PATTERN "*.h"
                 PATTERN "*/Test" EXCLUDE
                 PATTERN "src/Contexts" EXCLUDE)
