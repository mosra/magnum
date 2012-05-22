# CMake 2.8.8 required for OBJECT library target
cmake_minimum_required(VERSION 2.8.8)
project(Magnum)

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
find_package(Corrade REQUIRED)

set_parent_scope(MAGNUM_INCLUDE_DIR         "${CMAKE_CURRENT_SOURCE_DIR}/src")
set_parent_scope(MAGNUM_LIBRARY             Magnum)
set_parent_scope(MAGNUM_PHYSICS_LIBRARY     MagnumPhysics)
set_parent_scope(MAGNUM_PRIMITIVES_LIBRARY  MagnumPrimitives)
set_parent_scope(MAGNUM_MESHTOOLS_LIBRARY   MagnumMeshTools)
set_parent_scope(MAGNUM_SHADERS_LIBRARY     MagnumShaders)
include(FindMagnum)

add_subdirectory(modules)
add_subdirectory(src)

install(DIRECTORY src/ DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}
                 FILES_MATCHING PATTERN "*.h"
                 PATTERN "*/Test" EXCLUDE)
