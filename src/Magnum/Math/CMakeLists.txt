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

set(MagnumMath_HEADERS
    Angle.h
    Bezier.h
    BoolVector.h
    Color.h
    Complex.h
    Constants.h
    Dual.h
    DualComplex.h
    DualQuaternion.h
    Frustum.h
    Functions.h
    Half.h
    Math.h
    TypeTraits.h
    Matrix.h
    Matrix3.h
    Matrix4.h
    Quaternion.h
    Packing.h
    Range.h
    RectangularMatrix.h
    Swizzle.h
    Tags.h
    Unit.h
    Vector.h
    Vector2.h
    Vector3.h
    Vector4.h)

# Force IDEs to display all header files in project view
add_custom_target(MagnumMath SOURCES ${MagnumMath_HEADERS})
set_target_properties(MagnumMath PROPERTIES FOLDER "Magnum/Math")

install(FILES ${MagnumMath_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Math)

add_subdirectory(Algorithms)
add_subdirectory(Geometry)

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()
