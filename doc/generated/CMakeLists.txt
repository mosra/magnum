#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
#             Vladimír Vondruš <mosra@centrum.cz>
#
#   Permission is hereby granted, free of charge, to any person obtaining a
#   copy of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included
#   in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#   DEALINGS IN THE SOFTWARE.
#

cmake_minimum_required(VERSION 2.8.9)
project(MyApplication)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/../../modules/")

find_package(Magnum REQUIRED
    MeshTools
    Primitives
    Shaders
    Sdl2Application)

if(CORRADE_TARGET_APPLE)
    find_package(Magnum REQUIRED WindowlessCglApplication)
elseif(CORRADE_TARGET_UNIX)
    find_package(Magnum REQUIRED WindowlessGlxApplication)
elseif(CORRADE_TARGET_WINDOWS)
    find_package(Magnum REQUIRED WindowlessWglApplication)
else()
    message(FATAL_ERROR "No windowless application available on this platform")
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CORRADE_CXX_FLAGS}")
include_directories(${MAGNUM_INCLUDE_DIRS}
    ${MAGNUM_APPLICATION_INCLUDE_DIRS}
    ${MAGNUM_WINDOWLESSAPPLICATION_INCLUDE_DIRS})

add_executable(hello hello.cpp)
target_link_libraries(hello
    ${MAGNUM_LIBRARIES}
    ${MAGNUM_APPLICATION_LIBRARIES})

add_executable(shaders shaders.cpp)
target_link_libraries(shaders
    ${MAGNUM_LIBRARIES}
    ${MAGNUM_MESHTOOLS_LIBRARIES}
    ${MAGNUM_PRIMITIVES_LIBRARIES}
    ${MAGNUM_SHADERS_LIBRARIES}
    ${MAGNUM_WINDOWLESSAPPLICATION_LIBRARIES})
