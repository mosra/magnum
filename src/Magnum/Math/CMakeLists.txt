#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021, 2022, 2023, 2024, 2025
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

# IDE folder in VS, Xcode etc. CMake 3.12+, older versions have only the FOLDER
# property that would have to be set on each target separately.
set(CMAKE_FOLDER "Magnum/Math")

set(MagnumMath_HEADERS
    Angle.h
    Bezier.h
    BitVector.h
    Color.h
    ColorBatch.h
    Complex.h
    Constants.h
    ConfigurationValue.h
    CubicHermite.h
    Distance.h
    Dual.h
    DualComplex.h
    DualQuaternion.h
    Frustum.h
    Functions.h
    FunctionsBatch.h
    Half.h
    Intersection.h
    Math.h
    TypeTraits.h
    Matrix.h
    Matrix3.h
    Matrix4.h
    Quaternion.h
    Packing.h
    PackingBatch.h
    Range.h
    RectangularMatrix.h
    StrictWeakOrdering.h
    Swizzle.h
    Tags.h
    Time.h
    TimeStl.h
    Unit.h
    Vector.h
    Vector2.h
    Vector3.h
    Vector4.h)

if(MAGNUM_BUILD_DEPRECATED)
    list(APPEND MagnumMath_HEADERS BoolVector.h)
endif()

set(MagnumMath_INTERNAL_HEADERS
    Implementation/halfTables.hpp)

# Force IDEs to display all header files in project view
add_custom_target(MagnumMath SOURCES
    ${MagnumMath_HEADERS}
    ${MagnumMath_INTERNAL_HEADERS})

install(FILES ${MagnumMath_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Math)

add_subdirectory(Algorithms)

if(MAGNUM_BUILD_TESTS)
    add_subdirectory(Test ${EXCLUDE_FROM_ALL_IF_TEST_TARGET})
endif()
