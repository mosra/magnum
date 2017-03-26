#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

set(MagnumShapes_SRCS
    AbstractShape.cpp
    AxisAlignedBox.cpp
    Box.cpp
    Capsule.cpp
    Cylinder.cpp
    Composition.cpp
    Line.cpp
    Plane.cpp
    Point.cpp
    Shape.cpp
    ShapeGroup.cpp
    Sphere.cpp

    shapeImplementation.cpp

    Implementation/CollisionDispatch.cpp)

set(MagnumShapes_HEADERS
    AbstractShape.h
    AxisAlignedBox.h
    Box.h
    Capsule.h
    Cylinder.h
    Collision.h
    Composition.h
    Line.h
    LineSegment.h
    Shape.h
    ShapeGroup.h
    Shapes.h
    Plane.h
    Point.h
    Sphere.h

    shapeImplementation.h
    visibility.h)

# Header files to display in project view of IDEs only
set(MagnumShapes_PRIVATE_HEADERS Implementation/CollisionDispatch.h)

# Shapes library
add_library(MagnumShapes ${SHARED_OR_STATIC}
    ${MagnumShapes_SRCS}
    ${MagnumShapes_HEADERS}
    ${MagnumShapes_PRIVATE_HEADERS})
set_target_properties(MagnumShapes PROPERTIES
    DEBUG_POSTFIX "-d"
    FOLDER "Magnum/Shapes")
if(BUILD_STATIC_PIC)
    set_target_properties(MagnumShapes PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumShapes Magnum MagnumSceneGraph)

install(TARGETS MagnumShapes
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumShapes_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Shapes)

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()

# Magnum Shapes target alias for superprojects
add_library(Magnum::Shapes ALIAS MagnumShapes)
