#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020 Vladimír Vondruš <mosra@centrum.cz>
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

set(MagnumPrimitives_SRCS
    Axis.cpp
    Capsule.cpp
    Circle.cpp
    Crosshair.cpp
    Cone.cpp
    Cube.cpp
    Cylinder.cpp
    Gradient.cpp
    Grid.cpp
    Icosphere.cpp
    Line.cpp
    Plane.cpp
    Square.cpp
    UVSphere.cpp

    Implementation/Spheroid.cpp
    Implementation/WireframeSpheroid.cpp)

set(MagnumPrimitives_HEADERS
    Axis.h
    Capsule.h
    Circle.h
    Crosshair.h
    Cone.h
    Cube.h
    Cylinder.h
    Gradient.h
    Grid.h
    Icosphere.h
    Line.h
    Plane.h
    Square.h
    UVSphere.h

    visibility.h)

# Header files to display in project view of IDEs only
set(MagnumPrimitives_PRIVATE_HEADERS
    Implementation/Spheroid.h
    Implementation/WireframeSpheroid.h)

# Primitives library
add_library(MagnumPrimitives ${SHARED_OR_STATIC}
    ${MagnumPrimitives_SRCS}
    ${MagnumPrimitives_HEADERS}
    ${MagnumPrimitives_PRIVATE_HEADERS})
set_target_properties(MagnumPrimitives PROPERTIES
    DEBUG_POSTFIX "-d"
    FOLDER "Magnum/Primitives")
if(NOT BUILD_STATIC)
    set_target_properties(MagnumPrimitives PROPERTIES VERSION ${MAGNUM_LIBRARY_VERSION} SOVERSION ${MAGNUM_LIBRARY_SOVERSION})
elseif(BUILD_STATIC_PIC)
    set_target_properties(MagnumPrimitives PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumPrimitives PUBLIC
    Magnum
    MagnumMeshTools
    MagnumTrade)

install(TARGETS MagnumPrimitives
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumPrimitives_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Primitives)

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()

# Magnum Primitives target alias for superprojects
add_library(Magnum::Primitives ALIAS MagnumPrimitives)
